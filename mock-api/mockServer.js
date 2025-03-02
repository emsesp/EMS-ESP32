// used to simulate
//  - file uploads
//  - EventSource (SSE) for log messages
import formidable from 'formidable';

function pad(number) {
  let r = String(number);
  if (r.length === 1) {
    r = '0' + r;
  }
  return r;
}

// e.g. 2024-03-29 07:02:37.856
Date.prototype.toISOString = function () {
  return (
    this.getUTCFullYear() +
    '-' +
    pad(this.getUTCMonth() + 1) +
    '-' +
    pad(this.getUTCDate()) +
    ' ' +
    pad(this.getUTCHours()) +
    ':' +
    pad(this.getUTCMinutes()) +
    ':' +
    pad(this.getUTCSeconds()) +
    '.' +
    String((this.getUTCMilliseconds() / 1000).toFixed(3)).slice(2, 5)
  );
};

export default () => {
  return {
    name: 'vite:mockserver',
    configureServer: async (server) => {
      server.middlewares.use(async (req, res, next) => {
        // catch any file uploads
        if (req.url.startsWith('/rest/uploadFile')) {
          // show progress
          let progress = 0;
          const file_size = req.headers['content-length'];
          console.log('File size: ' + file_size);
          req.on('data', async (chunk) => {
            progress += chunk.length;
            const percentage = (progress / file_size) * 100;
            console.log(`Progress: ${Math.round(percentage)}%`);
            // await new Promise((resolve) => setTimeout(() => resolve(), 3000)); // slow it down
          });

          const form = formidable({});
          let fields;
          let files;
          try {
            [fields, files] = await form.parse(req);
          } catch (err) {
            console.error('Not json form content');
            res.writeHead(err.httpCode || 400, {
              'Content-Type': 'text/plain'
            });
            res.end(String(err));
            return;
          }

          // only process when we have a file
          if (Object.keys(files).length > 0) {
            const uploaded_file = files.file[0];
            const file_name = uploaded_file.originalFilename;
            const file_extension = file_name.substring(
              file_name.lastIndexOf('.') + 1
            );

            console.log('Filename: ' + file_name);
            console.log('Extension: ' + file_extension);
            console.log('File size: ' + file_size);

            if (file_extension === 'bin' || file_extension === 'json') {
              console.log('File uploaded successfully!');
            } else if (file_extension === 'md5') {
              console.log('MD5 hash generated successfully!');
              res.end(
                JSON.stringify({
                  md5: 'ef4304fc4d9025a58dcf25d71c882d2c'
                })
              );
            } else {
              res.statusCode = 406;
              console.log('Invalid file extension!');
            }
          }

          res.end();
        }

        // SSE Eventsource
        else if (req.url.startsWith('/es/log')) {
          res.writeHead(200, {
            Connection: 'keep-alive',
            'Cache-Control': 'no-cache',
            'Content-Type': 'text/event-stream'
          });

          let count = 0;
          const interval = setInterval(() => {
            let message = 'message #' + count;
            if (count % 6 === 1) {
              message +=
                ' that is a long message that will be wrapped, to test if it gets truncated';
            }
            const data = {
              t: new Date().toISOString(),
              l: 3 + (count % 6),
              i: count,
              n: 'system',
              m: message
            };
            count++;
            res.write(`data: ${JSON.stringify(data)}\n\n`);
          }, 1000);

          // if client closes connection
          res.on('close', () => {
            console.log('Closing ES connection');
            clearInterval(interval);
            res.end();
          });
        } else {
          next(); // move on to the next middleware function in chain
        }
      });
    }
  };
};
