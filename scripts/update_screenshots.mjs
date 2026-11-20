// Regenerates the WebUI screenshots in media/ that are shown in README.md.
//
// Usage:
//   1. Start the standalone WebUI with the mock API (serves on http://localhost:3000):
//        cd interface && pnpm standalone
//   2. In another terminal, from the repo root:
//        node scripts/update_screenshots.mjs              # all screenshots
//        node scripts/update_screenshots.mjs web_log      # only the named ones
//   3. Check the images in media/ and commit them.
//
// Uses playwright-core from interface/node_modules. If Chromium is missing, install it with:
//   cd interface && npx playwright-core install chromium
//
// Options (environment variables):
//   BASE_URL  WebUI address (default http://localhost:3000)
//   OUT_DIR   output folder (default media/)
//
// Screenshots are 1280x800 in dark mode, signed in as admin/admin.
// console0.png and ha_lovelace.png are not covered, they come from telnet and Home Assistant.

import { createRequire } from 'module';
import path from 'path';

const ROOT = path.resolve(import.meta.dirname, '..');
const require = createRequire(path.join(ROOT, 'interface', 'package.json'));
const { chromium } = require('playwright-core');

const BASE = process.env.BASE_URL || 'http://localhost:3000';
const OUT = process.env.OUT_DIR || path.join(ROOT, 'media');
const only = process.argv.slice(2);

const openBoiler = async (page) => {
  await page.getByText('Buderus GB125', { exact: true }).first().click();
  await page.waitForTimeout(1500);
};

const SHOTS = [
  { name: 'web_settings', path: '/settings' },
  { name: 'web_status', path: '/status' },
  { name: 'web_devices', path: '/devices', prep: openBoiler },
  {
    name: 'web_edit',
    path: '/devices',
    prep: async (page) => {
      await openBoiler(page);
      await page.locator('tr', { hasText: 'selected flow temperature' }).first().click();
      await page.waitForTimeout(1000);
    }
  },
  { name: 'web_mqtt', path: '/settings/mqtt' },
  // the mock streams a log line every 0.5s, ~7s fills the panel without scrolling
  { name: 'web_log', path: '/status/log', prep: (page) => page.waitForTimeout(7000) }
];

const browser = await chromium.launch();
const page = await browser.newPage({ viewport: { width: 1280, height: 800 }, colorScheme: 'dark' });

try {
  await page.goto(BASE + '/');
  await page.waitForLoadState('networkidle');
  const password = page.locator('input[type="password"]');
  if (await password.count()) {
    await page.locator('input').first().fill('admin');
    await password.fill('admin');
    await password.press('Enter');
    await password.waitFor({ state: 'detached' });
  }

  for (const shot of SHOTS) {
    if (only.length && !only.includes(shot.name)) continue;
    await page.goto(BASE + shot.path);
    // pages with an event stream (e.g. the log) never go idle
    await page.waitForLoadState('networkidle', { timeout: 5000 }).catch(() => {});
    await page.waitForTimeout(1500);
    if (shot.prep) {
      try {
        await shot.prep(page);
      } catch (e) {
        const failed = path.join(OUT, `${shot.name}_failed.png`);
        await page.screenshot({ path: failed });
        console.error(`${shot.name} failed, see ${failed}`);
        throw e;
      }
    }
    await page.mouse.move(1279, 799);
    await page.waitForTimeout(500);
    const file = path.join(OUT, `${shot.name}.png`);
    await page.screenshot({ path: file });
    console.log('saved', file);
  }
} finally {
  await browser.close();
}
