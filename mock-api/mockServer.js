// EMS-ESP standalone mode to simulate
//  a REST server for file uploads
//  an EventSource (Server-Sent Events) for log messages in EMS-ESP standalone mode
const ONE_SECOND_MS = 1000;

// padding function
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

const MOCK_REST_ORIGIN = 'http://localhost:3080';
const UPLOAD_SIMULATED_KB_PER_SEC = 256;
const UPLOAD_SLICE_BYTES = 16 * 1024;

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const readRequestThrottled = (req) =>
  new Promise((resolve, reject) => {
    const chunks = [];
    let ended = false;
    let settled = false;

    const settle = (settler, value) => {
      if (settled) {
        return;
      }
      settled = true;
      settler(value);
    };

    req.on('error', (error) => settle(reject, error));
    req.on('end', () => {
      ended = true;
      void step();
    });

    const step = async () => {
      if (settled) {
        return;
      }
      try {
        const chunk = req.read(UPLOAD_SLICE_BYTES);
        if (chunk) {
          chunks.push(chunk);
          await sleep((chunk.length / (UPLOAD_SIMULATED_KB_PER_SEC * 1024)) * 1000);
          void step();
          return;
        }
        if (ended || req.readableEnded) {
          settle(resolve, Buffer.concat(chunks));
          return;
        }
        req.once('readable', () => {
          void step();
        });
      } catch (error) {
        settle(reject, error);
      }
    };

    void step();
  });

const forwardThrottledUpload = async (req, res) => {
  const startedAt = Date.now();
  try {
    const body = await readRequestThrottled(req);
    console.log(
      `Upload received: ${body.length} bytes in ${((Date.now() - startedAt) / 1000).toFixed(1)}s`
    );
    const headers = {};
    if (req.headers['content-type']) {
      headers['content-type'] = req.headers['content-type'];
    }
    if (req.headers.authorization) {
      headers.authorization = req.headers.authorization;
    }
    headers['content-length'] = String(body.length);

    const bunRes = await fetch(`${MOCK_REST_ORIGIN}${req.url}`, {
      method: 'POST',
      headers,
      body
    });
    const buf = Buffer.from(await bunRes.arrayBuffer());
    res.statusCode = bunRes.status;
    bunRes.headers.forEach((value, key) => {
      if (key !== 'transfer-encoding' && key !== 'content-encoding') {
        res.setHeader(key, value);
      }
    });
    res.end(buf);
  } catch (error) {
    if (error?.code === 'ECONNRESET' || req.destroyed) {
      console.log('Upload cancelled by the client');
    } else {
      console.error('Throttled upload proxy failed', error);
    }
    if (!res.headersSent) {
      res.statusCode = 502;
    }
    if (!res.writableEnded) {
      res.end();
    }
  }
};

export default () => {
  return {
    name: 'vite:mockServer',
    configureServer: async (server) => {
      server.middlewares.use(async (req, res, next) => {
        if (req.url.startsWith('/rest/uploadFile') && req.method === 'POST') {
          await forwardThrottledUpload(req, res);
          return;
        }

        // Handle Server-Sent Events (SSE) for log streaming
        if (req.url.startsWith('/es/log')) {
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
          next(); // Continue to next middleware (Vite proxy → restServer)
        }
      });
    }
  };
};
