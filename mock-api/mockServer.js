// Mock server for development
// Simulates file uploads and EventSource (SSE) for log messages
import formidable from 'formidable';

// Constants reused across requests
const VALID_EXTENSIONS = new Set(['bin', 'json', 'md5']);
const ONE_SECOND_MS = 1000;
const TEN_PERCENT = 10;

// padding function
const pad = (number) => String(number).padStart(2, '0');

// Simple throttle helper (time-based)
const throttle = (fn, intervalMs) => {
  let last = 0;
  return (...args) => {
    const now = Date.now();
    if (now - last >= intervalMs) {
      last = now;
      fn(...args);
    }
  };
};

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
          // CORS preflight support
          if (req.method === 'OPTIONS') {
            res.writeHead(204, {
              'Access-Control-Allow-Origin': '*',
              'Access-Control-Allow-Methods': 'POST, OPTIONS',
              'Access-Control-Allow-Headers': 'Content-Type, Cache-Control',
              'Access-Control-Max-Age': '600'
            });
            res.end();
            return;
          }

          if (req.method !== 'POST') {
            res.statusCode = 405;
            res.setHeader('Allow', 'POST, OPTIONS');
            res.end('Method Not Allowed');
            return;
          }

          const fileSize = parseInt(req.headers['content-length'] || '0', 10);
          let progress = 0;

          // Track upload progress
          const logThrottled = throttle((percentage) => {
            console.log(`Upload progress: ${percentage}%`);
          }, ONE_SECOND_MS);

          req.on('data', (chunk) => {
            progress += chunk.length;
            if (fileSize > 0) {
              const percentage = Math.round((progress / fileSize) * 100);
              // Only log every ~1s and for meaningful changes (>=10%)
              if (percentage % TEN_PERCENT === 0) {
                logThrottled(percentage);
              }
            }
          });

          try {
            const form = formidable({
              maxFileSize: 50 * 1024 * 1024, // 50MB limit
              keepExtensions: true,
              multiples: false,
              allowEmptyFiles: false
            });

            const [fields, files] = await form.parse(req);

            if (Object.keys(files).length === 0) {
              res.statusCode = 400;
              res.end('No file uploaded');
              return;
            }

            const uploadedFile = Array.isArray(files.file)
              ? files.file[0]
              : files.file;
            const fileName = uploadedFile.originalFilename;
            const fileExtension = fileName
              .substring(fileName.lastIndexOf('.') + 1)
              .toLowerCase();

            console.log(
              `File uploaded: ${fileName} (${fileExtension}, ${fileSize} bytes)`
            );

            // Validate file extension
            if (!VALID_EXTENSIONS.has(fileExtension)) {
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
            console.error('Upload error:', err && err.message ? err.message : err);
            res.statusCode = err.httpCode || 400;
            res.setHeader('Content-Type', 'text/plain');
            res.end(err && err.message ? err.message : 'Upload error');
          }
        }

        // Handle Server-Sent Events (SSE) for log streaming
        else if (req.url.startsWith('/es/log')) {
          // Set SSE headers
          res.writeHead(200, {
            Connection: 'keep-alive',
            'Cache-Control': 'no-cache, no-transform',
            'Content-Type': 'text/event-stream',
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Headers': 'Cache-Control',
            'X-Accel-Buffering': 'no' // disable proxy buffering (nginx, etc.)
          });

          // Flush headers early when supported
          if (typeof res.flushHeaders === 'function') {
            res.flushHeaders();
          }

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
          res.write(`retry: 2000\n\n`); // client reconnection delay
          sendLogMessage();

          // Set up interval for periodic messages
          const messageInterval = setInterval(sendLogMessage, 500);
          if (typeof messageInterval.unref === 'function') messageInterval.unref();

          // Heartbeat to keep connections alive through proxies
          const heartbeat = setInterval(() => {
            res.write(`:keep-alive ${Date.now()}\n\n`);
          }, 15 * ONE_SECOND_MS);
          if (typeof heartbeat.unref === 'function') heartbeat.unref();

          // Clean up on connection close
          const cleanup = () => {
            console.log('SSE connection closed');
            clearInterval(messageInterval);
            clearInterval(heartbeat);
            if (!res.destroyed) {
              res.end();
            }
          };

          res.on('close', cleanup);
          res.on('error', cleanup);
          res.on('finish', cleanup);
        } else {
          next(); // Continue to next middleware
        }
      });
    }
  };
};
