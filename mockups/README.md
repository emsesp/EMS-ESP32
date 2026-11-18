# EMS-ESP Web UI mockups

Static HTML mockups of a redesigned web UI, using mock data. They are not part of the firmware or the `interface/` build.

## Running

The pages are plain HTML, CSS and JS, so any static file server works. From this folder:

```bash
cd mockups
python3 -m http.server 8765
```

Then open <http://localhost:8765/>. Stop the server with Ctrl+C.

If port 8765 is already in use, check what holds it with `lsof -nP -iTCP:8765 -sTCP:LISTEN`, or pick another port.

Opening `index.html` straight from disk also mostly works, but some browsers restrict scripts on `file://` pages, so the server is the reliable option.

## Hosting on Cloudflare

The mockup is published as a static-assets Cloudflare Worker (no Worker script, just files) at <https://emsesp-mockups.emsesp.org> (also <https://emsesp-mockups.derbyshire.workers.dev>). Everything needed lives in this folder: `wrangler.jsonc` (Worker config), `.assetsignore` (files not uploaded), `package.json` and `pnpm-workspace.yaml` (pins Wrangler).

```bash
cd mockups
pnpm install
pnpm exec wrangler login   # once, opens the browser
pnpm run deploy            # uploads the files and prints the URL
```

`pnpm run dev` serves the same thing locally through Wrangler. The site is public; anyone with the link can open it.

The custom domain is set in `routes` in `wrangler.jsonc`; Cloudflare creates the DNS record and certificate on deploy. It must be on a zone in the same Cloudflare account.

## Files

- `index.html` - landing page linking to the mockup
- `a-classic.html` - the mockup page: Dashboard, Devices, Download/Upload and Restore Heating Settings
- `shell.js` - shared app shell: drawer, app bar with breadcrumbs, hash router, dialogs, toasts
- `data.js` - mock data (`window.MOCK`): system, devices and entities, sensors, schedules, settings, users, firmware, log
- `modules.js` - Sensors, Customizations, Commands, Scheduler, Custom Entities
- `account.js` - Help and User Profile
- `system.js` - Status and Settings pages
- `base.css` - styles
- `assets/` - icon and font
- `wrangler.jsonc`, `.assetsignore`, `package.json`, `pnpm-workspace.yaml` - Cloudflare hosting

Pages are addressed by hash, for example `a-classic.html#status-log` or `a-classic.html#settings-application`. Changes made in the mockup only live in memory and are reset on reload.
