import crypto from 'crypto';
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
var totalSize = 0;

const generateWWWClass = () =>
  `typedef std::function<void(const char * uri, const String & contentType, const uint8_t * content, size_t len, const String & hash)> RouteRegistrationHandler;
// Total size is ${totalSize} bytes

class WWWData {
${indent}public:
${indent.repeat(2)}static void registerRoutes(RouteRegistrationHandler handler) {
${fileInfo.map((file) => `${indent.repeat(3)}handler("${file.uri}", "${file.mimeType}", ${file.variable}, ${file.size}, "${file.hash}");`).join('\n')}
${indent.repeat(2)}}
};
`;

function getFilesSync(dir, files = []) {
  readdirSync(dir, { withFileTypes: true }).forEach((entry) => {
    const entryPath = resolve(dir, entry.name);
    if (entry.isDirectory()) {
      getFilesSync(entryPath, files);
    } else {
      files.push(entryPath);
    }
  });
  return files;
}

function cleanAndOpen(path) {
  if (existsSync(path)) {
    unlinkSync(path);
  }
  return createWriteStream(path, { flags: 'w+' });
}

const writeFile = (relativeFilePath, buffer) => {
  const variable = 'ESP_REACT_DATA_' + fileInfo.length;
  const mimeType = mime.lookup(relativeFilePath);
  var size = 0;
  writeStream.write('const uint8_t ' + variable + '[] = {');
  // const zipBuffer = zlib.brotliCompressSync(buffer, { quality: 1 });
  const zipBuffer = zlib.gzipSync(buffer, { level: 9 });

  // create sha
  const hashSum = crypto.createHash('sha256');
  hashSum.update(zipBuffer);
  const hash = hashSum.digest('hex');

  zipBuffer.forEach((b) => {
    if (!(size % bytesPerLine)) {
      writeStream.write('\n');
      writeStream.write(indent);
    }
    writeStream.write('0x' + ('00' + b.toString(16).toUpperCase()).slice(-2) + ',');
    size++;
  });

  if (size % bytesPerLine) {
    writeStream.write('\n');
  }

  writeStream.write('};\n\n');

  fileInfo.push({
    uri: '/' + relativeFilePath.replace(sep, '/'),
    mimeType,
    variable,
    size,
    hash
  });

  // console.log(relativeFilePath + ' (size ' + size + ' bytes)');
  totalSize += size;
};

// start
console.log('Generating ' + outputPath + ' from ' + sourcePath);
const includes = ARDUINO_INCLUDES;
const indent = INDENT;
const fileInfo = [];
const writeStream = cleanAndOpen(resolve(outputPath));

// includes
writeStream.write(includes);

// process static files
const buildPath = resolve(sourcePath);
for (const filePath of getFilesSync(buildPath)) {
  const readStream = readFileSync(filePath);
  const relativeFilePath = relative(buildPath, filePath);
  writeFile(relativeFilePath, readStream);
}

// add class
writeStream.write(generateWWWClass());

// end
writeStream.end();

console.log('Total size: ' + totalSize / 1000 + ' KB');
