import etag from 'etag';
import {
  createWriteStream,
  existsSync,
  readFileSync,
  readdirSync,
  unlinkSync
} from 'fs';
import mime from 'mime-types';
import { relative, resolve, sep } from 'path';
import zlib from 'zlib';

// optional args: --src <dir> --out <file> --index <html file served as /index.html>
const getArg = (name, fallback) => {
  const i = process.argv.indexOf(name);
  return i !== -1 && process.argv[i + 1] ? process.argv[i + 1] : fallback;
};

const ARDUINO_INCLUDES = '#include <Arduino.h>\n\n';
const INDENT = '  ';
const outputPath = getArg('--out', '../src/ESP32React/WWWData.h');
const sourcePath = getArg('--src', './dist');
const indexFile = getArg('--index', 'index.html');
const bytesPerLine = 20;
let totalSize = 0;
let bundleStats = {
  js: { count: 0, uncompressed: 0, compressed: 0 },
  css: { count: 0, uncompressed: 0, compressed: 0 },
  html: { count: 0, uncompressed: 0, compressed: 0 },
  svg: { count: 0, uncompressed: 0, compressed: 0 },
  other: { count: 0, uncompressed: 0, compressed: 0 }
};

// AsyncWebHandler that performs the lookup.
const generateWWWClass = () => `// Bundle Statistics:
// - Total compressed size: ${(totalSize / 1000).toFixed(1)} KB
// - Total uncompressed size: ${(Object.values(bundleStats).reduce((sum, stat) => sum + stat.uncompressed, 0) / 1000).toFixed(1)} KB
// - Compression ratio: ${(((Object.values(bundleStats).reduce((sum, stat) => sum + stat.uncompressed, 0) - totalSize) / Object.values(bundleStats).reduce((sum, stat) => sum + stat.uncompressed, 0)) * 100).toFixed(1)}%
// - Generated on: ${new Date().toISOString()}

struct WWWAsset {
${INDENT}const char *    uri;
${INDENT}const char *    contentType;
${INDENT}const uint8_t * content;
${INDENT}size_t          len;
${INDENT}const char *    etag; // already includes enclosing double quotes
};

static const WWWAsset WWW_ASSETS[] = {
${fileInfo.map((f) => `${INDENT}{"${f.uri}", "${f.mimeType}", ${f.variable}, ${f.size}, "\\"${f.rawHash}\\""},`).join('\n')}
};

static constexpr size_t WWW_ASSETS_COUNT = sizeof(WWW_ASSETS) / sizeof(WWW_ASSETS[0]);
`;

// Optional locale allow-list, shared with the Vite build via VITE_APP_LOCALES
// (e.g. "en,de,nl"). When set, locale chunks outside the list are NOT embedded
// into firmware flash. `en` is always kept as the fallback. Unset => embed all.
const ALL_LOCALES = [
  'cs',
  'de',
  'en',
  'fr',
  'it',
  'nl',
  'no',
  'pl',
  'sk',
  'sv',
  'tr'
];
const localeAllowList = (process.env.VITE_APP_LOCALES || '')
  .split(',')
  .map((locale) => locale.trim())
  .filter(Boolean);

const isExcludedLocaleChunk = (relativeFilePath) => {
  if (localeAllowList.length === 0) return false;
  const base = relativeFilePath.split(sep).pop();
  const match = /^([a-z]{2})-[A-Za-z0-9_-]+\.js$/.exec(base);
  if (!match) return false;
  const code = match[1];
  // Only treat known locale codes as locale chunks; never drop the en fallback.
  if (!ALL_LOCALES.includes(code) || code === 'en') return false;
  return !localeAllowList.includes(code);
};

const getFilesSync = (dir, files = []) => {
  readdirSync(dir, { withFileTypes: true }).forEach((entry) => {
    const entryPath = resolve(dir, entry.name);
    entry.isDirectory() ? getFilesSync(entryPath, files) : files.push(entryPath);
  });
  return files;
};

const cleanAndOpen = (path) => {
  existsSync(path) && unlinkSync(path);
  return createWriteStream(path, { flags: 'w+' });
};

const getFileType = (filePath) => {
  const ext = filePath.split('.').pop().toLowerCase();
  if (ext === 'js') return 'js';
  if (ext === 'css') return 'css';
  if (ext === 'html') return 'html';
  if (ext === 'svg') return 'svg';
  return 'other';
};

const writeFile = (relativeFilePath, buffer) => {
  const variable = `ESP_REACT_DATA_${fileInfo.length}`;
  const mimeType = mime.lookup(relativeFilePath);
  const fileType = getFileType(relativeFilePath);
  let size = 0;
  writeStream.write(`const uint8_t ${variable}[] = {`);

  const zipBuffer = zlib.gzipSync(buffer, { level: 9 });
  // const hash = crypto.createHash('sha256').update(zipBuffer).digest('hex');
  const hash = etag(zipBuffer); // use smaller md5 instead of sha256
  const rawHash = hash.replace(/^"|"$/g, '');

  zipBuffer.forEach((b) => {
    if (!(size % bytesPerLine)) {
      writeStream.write('\n' + INDENT);
    }
    writeStream.write('0x' + b.toString(16).toUpperCase().padStart(2, '0') + ',');
    size++;
  });

  size % bytesPerLine && writeStream.write('\n');
  writeStream.write('};\n\n');

  // Update bundle statistics
  bundleStats[fileType].count++;
  bundleStats[fileType].uncompressed += buffer.length;
  bundleStats[fileType].compressed += zipBuffer.length;

  fileInfo.push({
    uri:
      relativeFilePath === indexFile
        ? '/index.html'
        : '/' + relativeFilePath.replace(sep, '/'),
    mimeType,
    variable,
    size,
    hash,
    rawHash
  });

  totalSize += size;
};

console.log(`Generating ${outputPath} from ${sourcePath}`);
const fileInfo = [];
const writeStream = cleanAndOpen(resolve(outputPath));

writeStream.write(ARDUINO_INCLUDES);

const buildPath = resolve(sourcePath);
for (const filePath of getFilesSync(buildPath)) {
  const relativeFilePath = relative(buildPath, filePath);
  if (isExcludedLocaleChunk(relativeFilePath)) {
    console.log(`Skipping locale (not in VITE_APP_LOCALES): ${relativeFilePath}`);
    continue;
  }
  writeFile(relativeFilePath, readFileSync(filePath));
}

writeStream.write(generateWWWClass());
writeStream.end();

// Calculate and display bundle statistics
const totalUncompressed = Object.values(bundleStats).reduce(
  (sum, stat) => sum + stat.uncompressed,
  0
);
const totalCompressed = Object.values(bundleStats).reduce(
  (sum, stat) => sum + stat.compressed,
  0
);
const compressionRatio = (
  ((totalUncompressed - totalCompressed) / totalUncompressed) *
  100
).toFixed(1);

console.log('\n📊 Bundle Size Analysis:');
console.log('='.repeat(50));
console.log(`Total compressed size: ${(totalSize / 1000).toFixed(1)} KB`);
console.log(`Total uncompressed size: ${(totalUncompressed / 1000).toFixed(1)} KB`);
console.log(`Compression ratio: ${compressionRatio}%`);
console.log('\n📁 File Type Breakdown:');
Object.entries(bundleStats).forEach(([type, stats]) => {
  if (stats.count > 0) {
    const ratio = (
      ((stats.uncompressed - stats.compressed) / stats.uncompressed) *
      100
    ).toFixed(1);
    console.log(
      `${type.toUpperCase().padEnd(4)}: ${stats.count} files, ${(stats.uncompressed / 1000).toFixed(1)} KB → ${(stats.compressed / 1000).toFixed(1)} KB (${ratio}% compression)`
    );
  }
});
console.log('='.repeat(50));
