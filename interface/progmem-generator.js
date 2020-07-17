const { resolve, relative, sep } = require('path');
const { readdirSync, existsSync, unlinkSync, readFileSync, createWriteStream } = require('fs');
var zlib = require('zlib');
var mime = require('mime-types');

const ARDUINO_INCLUDES = "#include <Arduino.h>\n\n";

function getFilesSync(dir, files = []) {
  readdirSync(dir, { withFileTypes: true }).forEach(entry => {
    const entryPath = resolve(dir, entry.name);
    if (entry.isDirectory()) {
      getFilesSync(entryPath, files);
    } else {
      files.push(entryPath);
    }
  })
  return files;
}

function coherseToBuffer(input) {
  return Buffer.isBuffer(input) ? input : Buffer.from(input);
}

function cleanAndOpen(path) {
  if (existsSync(path)) {
    unlinkSync(path);
  }
  return createWriteStream(path, { flags: "w+" });
}

class ProgmemGenerator {

  constructor(options = {}) {
    const { outputPath, bytesPerLine = 20, indent = "  ", includes = ARDUINO_INCLUDES } = options;
    this.options = { outputPath, bytesPerLine, indent, includes };
  }

  apply(compiler) {
    compiler.hooks.emit.tapAsync(
      { name: 'ProgmemGenerator' },
      (compilation, callback) => {
        const { outputPath, bytesPerLine, indent, includes } = this.options;
        const fileInfo = [];
        const writeStream = cleanAndOpen(resolve(compilation.options.context, outputPath));
        try {
          const writeIncludes = () => {
            writeStream.write(includes);
          }

          const writeFile = (relativeFilePath, buffer) => {
            const variable = "ESP_REACT_DATA_" + fileInfo.length;
            const mimeType = mime.lookup(relativeFilePath);
            var size = 0;
            writeStream.write("const uint8_t " + variable + "[] PROGMEM = {");
            const zipBuffer = zlib.gzipSync(buffer);
            zipBuffer.forEach((b) => {
              if (!(size % bytesPerLine)) {
                writeStream.write("\n");
                writeStream.write(indent);
              }
              writeStream.write("0x" + ("00" + b.toString(16).toUpperCase()).substr(-2) + ",");
              size++;
            });
            if (size % bytesPerLine) {
              writeStream.write("\n");
            }
            writeStream.write("};\n\n");
            fileInfo.push({
              uri: '/' + relativeFilePath.replace(sep, '/'),
              mimeType,
              variable,
              size
            });
          };

          const writeFiles = () => {
            // process static files
            const buildPath = compilation.options.output.path;
            for (const filePath of getFilesSync(buildPath)) {
              const readStream = readFileSync(filePath);
              const relativeFilePath = relative(buildPath, filePath);
              writeFile(relativeFilePath, readStream);
            }
            // process assets
            const { assets } = compilation;
            Object.keys(assets).forEach((relativeFilePath) => {
              writeFile(relativeFilePath, coherseToBuffer(assets[relativeFilePath].source()));
            });
          }

          const generateWWWClass = () => {
            return `typedef std::function<void(const String& uri, const String& contentType, const uint8_t * content, size_t len)> RouteRegistrationHandler;          

class WWWData {
${indent}public:
${indent.repeat(2)}static void registerRoutes(RouteRegistrationHandler handler) {
${fileInfo.map(file => `${indent.repeat(3)}handler("${file.uri}", "${file.mimeType}", ${file.variable}, ${file.size});`).join('\n')}
${indent.repeat(2)}}
};
`;
          }

          const writeWWWClass = () => {
            writeStream.write(generateWWWClass());
          }

          writeIncludes();
          writeFiles();
          writeWWWClass();

          writeStream.on('finish', () => {
            callback();
          });
        } finally {
          writeStream.end();
        }
      }
    );
  }
}

module.exports = ProgmemGenerator;
