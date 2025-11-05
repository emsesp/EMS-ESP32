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

const ARDUINO_INCLUDES = '#include <Arduino.h>\n\n';
const INDENT = '  ';
const outputPath = '../src/ESP32React/WWWData.h';
const sourcePath = './dist';
const bytesPerLine = 20;
let totalSize = 0;
let bundleStats = {
  js: { count: 0, uncompressed: 0, compressed: 0 },
  css: { count: 0, uncompressed: 0, compressed: 0 },
  html: { count: 0, uncompressed: 0, compressed: 0 },
  svg: { count: 0, uncompressed: 0, compressed: 0 },
  other: { count: 0, uncompressed: 0, compressed: 0 }
};

const generateWWWClass =
  () => `typedef std::function<void(const char * uri, const String & contentType, const uint8_t * content, size_t len, const String & hash)> RouteRegistrationHandler;
// Bundle Statistics:
// - Total compressed size: ${(totalSize / 1000).toFixed(1)} KB
// - Total uncompressed size: ${(Object.values(bundleStats).reduce((sum, stat) => sum + stat.uncompressed, 0) / 1000).toFixed(1)} KB
// - Compression ratio: ${(((Object.values(bundleStats).reduce((sum, stat) => sum + stat.uncompressed, 0) - totalSize) / Object.values(bundleStats).reduce((sum, stat) => sum + stat.uncompressed, 0)) * 100).toFixed(1)}%
// - Generated on: ${new Date().toISOString()}

class WWWData {
${INDENT}public:
${INDENT.repeat(2)}static void registerRoutes(RouteRegistrationHandler handler) {
${fileInfo.map((f) => `${INDENT.repeat(3)}handler("${f.uri}", "${f.mimeType}", ${f.variable}, ${f.size}, ${f.hash});`).join('\n')}
${INDENT.repeat(2)}}
};
`;

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
    uri: '/' + relativeFilePath.replace(sep, '/'),
    mimeType,
    variable,
    size,
    hash
  });

  totalSize += size;
};

console.log(`Generating ${outputPath} from ${sourcePath}`);
const fileInfo = [];
const writeStream = cleanAndOpen(resolve(outputPath));

writeStream.write(ARDUINO_INCLUDES);

const buildPath = resolve(sourcePath);
for (const filePath of getFilesSync(buildPath)) {
  writeFile(relative(buildPath, filePath), readFileSync(filePath));
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
