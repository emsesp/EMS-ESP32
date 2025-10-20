// Mock server for development
// Simulates file uploads and EventSource (SSE) for log messages
import formidable from 'formidable';

// Optimized padding function
const pad = (number) => String(number).padStart(2, '0');

// Cached date formatter to avoid prototype pollution
const formatDate = (date) => {
  const year = date.getUTCFullYear();
  const month = pad(date.getUTCMonth() + 1);
  const day = pad(date.getUTCDate());
  const hours = pad(date.getUTCHours());
  const minutes = pad(date.getUTCMinutes());
  const seconds = pad(date.getUTCSeconds());
  const milliseconds = String((date.getUTCMilliseconds() / 1000).toFixed(3)).slice(
    2,
    5
  );

  return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}.${milliseconds}`;
};

export default () => {
  return {
    name: 'vite:mockserver',
    configureServer: async (server) => {
      server.middlewares.use(async (req, res, next) => {
        // Handle file uploads
        if (req.url.startsWith('/rest/uploadFile')) {
          const fileSize = parseInt(req.headers['content-length'] || '0', 10);
          let progress = 0;

          // Track upload progress
          req.on('data', (chunk) => {
            progress += chunk.length;
            if (fileSize > 0) {
              const percentage = Math.round((progress / fileSize) * 100);
              console.log(`Upload progress: ${percentage}%`);
            }
          });

          try {
            const form = formidable({
              maxFileSize: 50 * 1024 * 1024, // 50MB limit
              keepExtensions: true
            });

            const [fields, files] = await form.parse(req);

            if (Object.keys(files).length === 0) {
              res.statusCode = 400;
              res.end('No file uploaded');
              return;
            }

            const uploadedFile = files.file[0];
            const fileName = uploadedFile.originalFilename;
            const fileExtension = fileName
              .substring(fileName.lastIndexOf('.') + 1)
              .toLowerCase();

            console.log(
              `File uploaded: ${fileName} (${fileExtension}, ${fileSize} bytes)`
            );

            // Validate file extension
            const validExtensions = new Set(['bin', 'json', 'md5']);
            if (!validExtensions.has(fileExtension)) {
              res.statusCode = 406;
              res.end('Invalid file extension');
              return;
            }

            // Handle different file types
            if (fileExtension === 'md5') {
              res.setHeader('Content-Type', 'application/json');
              res.end(
                JSON.stringify({
                  md5: 'ef4304fc4d9025a58dcf25d71c882d2c'
                })
              );
            } else {
              console.log('File uploaded successfully!');
              res.end();
            }
          } catch (err) {
            console.error('Upload error:', err.message);
            res.statusCode = err.httpCode || 400;
            res.setHeader('Content-Type', 'text/plain');
            res.end(err.message);
          }
        }

        // Handle Server-Sent Events (SSE) for log streaming
        else if (req.url.startsWith('/es/log')) {
          // Set SSE headers
          res.writeHead(200, {
            Connection: 'keep-alive',
            'Cache-Control': 'no-cache',
            'Content-Type': 'text/event-stream',
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Headers': 'Cache-Control'
          });

          let messageCount = 0;
          const logLevels = [3, 4, 5, 6, 7, 8]; // Different log levels
          const logNames = ['system', 'ems', 'wifi', 'mqtt', 'ntp', 'api'];

          const sendLogMessage = () => {
            const level = logLevels[messageCount % logLevels.length];
            const name = logNames[messageCount % logNames.length];
            let message = `Log message #${messageCount}`;

            // Add long message every 6th message
            if (messageCount % 6 === 1) {
              message +=
                ' - This is a longer message to test text wrapping and truncation behavior in the UI';
            }

            const logData = {
              t: formatDate(new Date()),
              l: level,
              i: messageCount,
              n: name,
              m: message
            };

            res.write(`data: ${JSON.stringify(logData)}\n\n`);
            messageCount++;
          };

          // Send initial message
          sendLogMessage();

          // Set up interval for periodic messages
          const interval = setInterval(sendLogMessage, 1000);

          // Clean up on connection close
          const cleanup = () => {
            console.log('SSE connection closed');
            clearInterval(interval);
            if (!res.destroyed) {
              res.end();
            }
          };

          res.on('close', cleanup);
          res.on('error', cleanup);
        } else {
          next(); // Continue to next middleware
        }
      });
    }
  };
};
