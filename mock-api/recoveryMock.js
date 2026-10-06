// Vite plugin that mocks the EMS-ESP Recovery firmware REST API, used by `pnpm recovery`
// Sign in with admin/admin. Uploading any *.bin >= 512KB "flashes" it into the next OTA partition.
// It starts as if an update in app1 failed to start and the bootloader fell back to the recovery.

const MIN_FIRMWARE_SIZE = 512 * 1024;
const RESTART_DOWNTIME_MS = 4000;
const TOKEN = 'mock-recovery-token';
const MB = 1048576;

const state = {
  token: null,
  boot: 'boot',
  downUntil: 0,
  md5: null,
  partitions: [
    {
      label: 'boot',
      factory: true,
      address: 0x10000,
      size: 0x480000,
      valid: true,
      image_size: 0.8 * MB,
      version: '1.0.0',
      install_date: 0,
      ota_state: ''
    },
    {
      label: 'app0',
      factory: false,
      address: 0x490000,
      size: 0x490000,
      valid: true,
      image_size: 1.86 * MB,
      version: '3.8.1',
      install_date: 1767225600,
      ota_state: 'valid'
    },
    {
      label: 'app1',
      factory: false,
      address: 0x920000,
      size: 0x490000,
      valid: true,
      image_size: 1.91 * MB,
      version: '3.9.0-dev.16',
      install_date: 1790000000,
      ota_state: 'aborted'
    }
  ]
};

const failedToStart = (p) => p.ota_state === 'aborted' || p.ota_state === 'invalid';

const started = Date.now();

// same rule as the firmware: an empty partition, then a failed one, otherwise the oldest install date
const nextUploadTarget = () => {
  const ota = state.partitions.filter((p) => !p.factory);
  const rank = (p) => (!p.valid ? 0 : failedToStart(p) ? 1 : 2);
  return ota.reduce((a, b) =>
    rank(b) < rank(a) || (rank(b) === rank(a) && b.install_date < a.install_date)
      ? b
      : a
  ).label;
};

const status = (authenticated) => ({
  recovery_version: '1.0.0',
  authenticated,
  hostname: 'ems-esp',
  chip: 'ESP32', // a wired E32V2 with a static IP, WiFi is not used
  flash_size: 16 * MB,
  psram_size: 8 * MB,
  free_heap: 245760,
  mac: '24:0A:C4:00:00:01',
  uptime: Math.floor((Date.now() - started) / 1000),
  reason: '',
  reset_reason: 'panic',
  ap_ssid: 'ems-esp-recovery',
  ap_ip: '192.168.4.1',
  ap_clients: 1,
  wifi_ssid: 'MyHomeWiFi',
  wifi_started: false,
  wifi_connected: false,
  static_ip: true,
  eth_state: 'connected',
  eth_ip: '192.168.1.50',
  running: 'boot',
  boot: state.boot,
  upload_target: nextUploadTarget(),
  partitions: state.partitions
});

const readBody = (req) =>
  new Promise((resolve, reject) => {
    const chunks = [];
    req.on('data', (c) => chunks.push(c));
    req.on('end', () => resolve(Buffer.concat(chunks)));
    req.on('error', reject);
  });

const send = (res, code, body) => {
  res.statusCode = code;
  if (body !== undefined) {
    res.setHeader('Content-Type', 'application/json');
    res.end(JSON.stringify(body));
  } else {
    res.end();
  }
};

const isAuthorized = (req) =>
  state.token && req.headers.authorization === `Bearer ${state.token}`;

const restart = () => {
  state.token = null;
  state.downUntil = Date.now() + RESTART_DOWNTIME_MS;
};

const uploadedFilename = (body) => {
  const match = /filename="([^"]+)"/.exec(body.subarray(0, 1024).toString());
  return match ? match[1] : '';
};

export default function recoveryMock() {
  return {
    name: 'recovery-mock',
    configureServer(server) {
      server.middlewares.use(async (req, res, next) => {
        const [url, query] = req.url.split('?');
        const params = new URLSearchParams(query ?? '');
        if (!url.startsWith('/rest/')) {
          return next();
        }

        if (Date.now() < state.downUntil) {
          return send(res, 503);
        }

        if (url === '/rest/recovery/status' && req.method === 'GET') {
          return send(res, 200, status(!!isAuthorized(req)));
        }

        if (url === '/rest/signIn' && req.method === 'POST') {
          const { username, password } = JSON.parse(
            (await readBody(req)).toString() || '{}'
          );
          if (username === 'admin' && password === 'admin') {
            state.token = TOKEN;
            return send(res, 200, { access_token: TOKEN });
          }
          return send(res, 401);
        }

        if (!isAuthorized(req)) {
          await readBody(req);
          return send(res, 401);
        }

        if (url === '/rest/recovery/restart' && req.method === 'POST') {
          send(res, 200);
          return restart();
        }

        if (url === '/rest/recovery/boot' && req.method === 'POST') {
          const { partition } = JSON.parse((await readBody(req)).toString() || '{}');
          const p = state.partitions.find((x) => x.label === partition);
          if (!p) return send(res, 404);
          if (!p.valid) return send(res, 400);
          if (!p.factory) p.ota_state = 'new';
          state.boot = partition;
          send(res, 200);
          return restart();
        }

        if (url === '/rest/uploadFile' && req.method === 'POST') {
          const body = await readBody(req);
          const filename = uploadedFilename(body);
          if (filename.endsWith('.md5')) {
            state.md5 = '0123456789abcdef0123456789abcdef';
            return send(res, 200, { md5: state.md5 });
          }
          if (
            !filename.endsWith('.bin') ||
            filename.endsWith('littlefs.bin') ||
            body.length < MIN_FIRMWARE_SIZE
          ) {
            return send(res, 406);
          }
          if (filename.startsWith('EMS-ESP-Recovery')) {
            return send(res, 409);
          }
          const target = state.partitions.find(
            (x) => x.label === (params.get('partition') ?? nextUploadTarget())
          );
          if (!target || target.factory) return send(res, 400);
          Object.assign(target, {
            valid: true,
            image_size: body.length,
            version: '',
            install_date: Math.floor(Date.now() / 1000),
            ota_state: 'new'
          });
          state.boot = target.label;
          const md5_ok = !!state.md5;
          state.md5 = null;
          return md5_ok ? send(res, 200, { md5_ok }) : send(res, 200);
        }

        return send(res, 404);
      });
    }
  };
}
