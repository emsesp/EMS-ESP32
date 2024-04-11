import express from 'express';

const rest_server = express();

const port = 3081;

const ES_ENDPOINT_ROOT = '/es/';
const ES_LOG_ENDPOINT = ES_ENDPOINT_ROOT + 'log';

const INTERVAL = 2000;

function pad(number) {
  var r = String(number);
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

rest_server.get(ES_LOG_ENDPOINT, (_req, res) => {
  res.writeHead(200, {
    Connection: 'keep-alive',
    'Cache-Control': 'no-cache',
    'Content-Type': 'text/event-stream'
  });

  let count = 0;
  const interval = setInterval(() => {
    const data = {
      t: new Date().toISOString(),
      l: 3, // error
      i: count,
      n: 'system',
      m: 'message #' + count++
    };
    res.write(`data: ${JSON.stringify(data)}\n\n`);
  }, INTERVAL);

  // if client closes connection
  res.on('close', () => {
    console.log('Closing ES connection');
    clearInterval(interval);
    res.end();
  });
});

// start eventsource server
rest_server.listen(port, () => console.log(`EMS-ESP EventSource mock server running on http://localhost:${port}/`));
