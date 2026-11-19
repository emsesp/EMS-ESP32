// Opens every page (and the "add" dialogs) in every language at desktop, tablet and phone
// width and lists translated text that is clipped, truncated or pushed off screen.
// Needs the mock server running (`pnpm standalone`) and Chromium (`pnpm exec playwright-core install chromium`).
// usage: node check-layout.js [--url http://localhost:3000] [--lang nl,de] [--width 390] [--route /status/log]
import { readFileSync } from 'fs';
import { chromium } from 'playwright-core';
import { parseArgs } from 'util';

const { values: args } = parseArgs({
  options: {
    url: { type: 'string', default: 'http://localhost:3000' },
    lang: { type: 'string' },
    width: { type: 'string' },
    route: { type: 'string' },
    concurrency: { type: 'string', default: '6' }
  }
});

const LANGS = [
  'en',
  'cs',
  'de',
  'fr',
  'hu',
  'it',
  'nl',
  'no',
  'pl',
  'sk',
  'sv',
  'tr'
];
// full drawer, icon rail, phone
const WIDTHS = [1280, 1000, 390];
// pages with nested routes are listed by their default child
const ROUTES = [
  ...readFileSync('src/app/routes.tsx', 'utf8').matchAll(/path: '([^']+)'/g)
].map(
  ([, p]) =>
    ({
      '/settings/network': '/settings/network/settings',
      '/settings/security': '/settings/security/users'
    })[p] ?? p
);
// the heat pump has the longest entity list in the mock data
ROUTES.push(
  '/devices?device=8',
  '/settings/network/scan',
  '/settings/security/apikeys'
);
const DIALOG_ROUTES = [
  '/sensors',
  '/commands',
  '/scheduler',
  '/customentities',
  '/settings/security/users'
];
const ADD_ICON = / d="([^"]+)"/.exec(
  readFileSync('src/components/ui/icons/add.svg', 'utf8')
)[1];
// deliberately truncated device data, not translations
const IGNORE = ['.db-name', '.dv-dname', '.linklist .val .txt', '.sr-only', 'svg'];

const pick = (arg, all) => (arg ? arg.split(',') : all);
const langs = pick(args.lang, LANGS);
const widths = pick(args.width, WIDTHS).map(Number);
const routes = pick(args.route, ROUTES);

// runs in the page: text that overflows its own box, is cut off by an ancestor, or ends off screen
const scanPage = ({ rootSelector, ignore }) => {
  const root = rootSelector ? document.querySelector(rootSelector) : document.body;
  const found = [];
  if (!root) return found;
  if (!rootSelector && document.documentElement.scrollWidth > innerWidth + 1) {
    found.push({
      el: 'page',
      text: '(page scrolls sideways)',
      why: `${document.documentElement.scrollWidth - innerWidth}px`
    });
  }
  const scrolls = (s) => s.overflowX === 'auto' || s.overflowX === 'scroll';
  for (const el of root.querySelectorAll('*')) {
    if (ignore.some((sel) => el.closest(sel))) continue;
    const text = [...el.childNodes]
      .filter((n) => n.nodeType === Node.TEXT_NODE)
      .map((n) => n.textContent)
      .join('')
      .trim();
    if (!text) continue;
    const r = el.getBoundingClientRect();
    const style = getComputedStyle(el);
    if (!r.width || !r.height || style.visibility === 'hidden') continue;
    let why = null;
    const over = el.scrollWidth - el.clientWidth;
    if (over > 1 && !scrolls(style)) {
      why = `${style.textOverflow === 'ellipsis' ? 'truncated' : 'clipped'} ${over}px`;
    } else {
      let a = el.parentElement;
      let inScroller = false;
      for (; a && a !== document.body; a = a.parentElement) {
        const s = getComputedStyle(a);
        if (scrolls(s)) {
          inScroller = true;
          break;
        }
        if (s.overflowX !== 'visible') {
          const cut = Math.round(r.right - a.getBoundingClientRect().right);
          if (cut > 1)
            why = `cut off by .${String(a.className).split(' ')[0]} ${cut}px`;
          break;
        }
      }
      if (!why && !inScroller && r.right > innerWidth + 1) {
        why = `off screen ${Math.round(r.right - innerWidth)}px`;
      }
    }
    if (why) {
      found.push({
        el: `${el.tagName.toLowerCase()}.${String(el.className).split(' ')[0]}`,
        text: text.slice(0, 60),
        why
      });
    }
  }
  return found;
};

const browser = await chromium.launch();
const signIn = await fetch(`${args.url}/rest/signIn`, {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ username: 'admin', password: 'admin' })
}).catch(() => null);
if (!signIn?.ok) {
  console.error(`Cannot sign in at ${args.url}. Is \`pnpm standalone\` running?`);
  await browser.close();
  process.exit(2);
}
const { access_token } = await signIn.json();

const pages = [
  ...routes.map((route) => ({ route, dialog: false })),
  ...DIALOG_ROUTES.filter((r) => routes.includes(r)).map((route) => ({
    route,
    dialog: true
  }))
];
const runs = langs.flatMap((lang) => widths.map((width) => ({ lang, width })));

const issues = new Map();
const report = (key, entry) => {
  if (!issues.has(key)) issues.set(key, []);
  issues.get(key).push(entry);
};

// one browser context per language and width, so the dev server's modules stay cached
const run = async ({ lang, width }) => {
  const context = await browser.newContext({ viewport: { width, height: 900 } });
  await context.addInitScript(
    ([token, l]) => {
      localStorage.setItem('access_token', token);
      localStorage.setItem('lang', l);
    },
    [access_token, lang]
  );
  const page = await context.newPage();
  for (const { route, dialog } of pages) {
    const where = `${route}${dialog ? ' [add dialog]' : ''} @${width}px`;
    try {
      // not 'load' or 'networkidle': the log page keeps an event stream open
      await page.goto(args.url + route, { waitUntil: 'commit' });
      await page.locator('.main').waitFor();
      await page.waitForTimeout(800);
      let rootSelector = null;
      if (dialog) {
        await page.locator(`button:has(path[d="${ADD_ICON}"])`).first().click();
        await page.locator('.dialog-box').waitFor();
        rootSelector = '.dialog-box';
      }
      for (const f of await page.evaluate(scanPage, {
        rootSelector,
        ignore: IGNORE
      })) {
        report(`${where}  ${f.el}  "${f.text}"`, `${lang} (${f.why})`);
      }
    } catch (e) {
      report(`${where}  could not be checked: ${e.message.split('\n')[0]}`, lang);
    }
  }
  await context.close();
};

let next = 0;
await Promise.all(
  Array.from({ length: Number(args.concurrency) }, async () => {
    while (next < runs.length) await run(runs[next++]);
  })
);
await browser.close();

console.log(
  `Checked ${routes.length} pages in ${langs.length} languages at ${widths.join(', ')}px`
);
if (!issues.size) {
  console.log('No clipped or overflowing text found');
} else {
  for (const [key, where] of [...issues].sort(([a], [b]) => a.localeCompare(b))) {
    console.log(`\n${key}\n  ${where.join(', ')}`);
  }
  process.exitCode = 1;
}
