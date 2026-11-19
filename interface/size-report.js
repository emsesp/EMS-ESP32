// Prints the flash used by the web UI as Markdown, from the asset table in WWWData.h
// (run `pnpm build-webUI` first). Columns match the flash budget table in REDESIGN.md.
// usage: node size-report.js [path to WWWData.h]
import { readFileSync, statSync } from 'fs';

const file = process.argv[2] ?? '../src/ESP32React/WWWData.h';
const header = readFileSync(file, 'utf8');

// e.g.  {"/assets/index-C_BDjahD.js", "text/javascript", ESP_REACT_DATA_8, 62495, "\"f41f-...\""},
const assets = [
  ...header.matchAll(/^\s*\{"([^"]+)", "[^"]+", ESP_REACT_DATA_\d+, (\d+),/gm)
].map(([, uri, size]) => ({ uri, size: Number(size) }));
if (!assets.length) {
  console.error(`No assets found in ${file}`);
  process.exit(1);
}

const groupOf = (uri) => {
  if (/^\/assets\/vendor-.*\.js$/.test(uri)) return 'vendor js';
  if (/^\/assets\/index-.*\.js$/.test(uri)) return 'app js';
  if (/^\/assets\/[a-z]{2}-.*\.js$/.test(uri)) return 'locales';
  if (uri.endsWith('.css')) return 'css';
  return 'other';
};
const groups = ['vendor js', 'app js', 'locales', 'css', 'other'];
const totals = Object.fromEntries(groups.map((g) => [g, 0]));
for (const a of assets) totals[groupOf(a.uri)] += a.size;
const total = assets.reduce((sum, a) => sum + a.size, 0);

const n = (v) => v.toLocaleString('en-US');
const row = (cells) => `| ${cells.join(' | ')} |`;

console.log('### Web UI flash size\n');
console.log('Zopfli-compressed sizes as stored in `WWWData.h`, in bytes.\n');
console.log(row([...groups, 'total in flash', 'WWWData.h']));
console.log(row([...groups, 'total', 'file'].map(() => '---')));
console.log(
  row([...groups.map((g) => n(totals[g])), n(total), n(statSync(file).size)])
);
console.log('\n<details><summary>All assets</summary>\n');
console.log(row(['asset', 'bytes']));
console.log(row(['---', '---']));
for (const a of [...assets].sort((x, y) => y.size - x.size)) {
  console.log(row([`\`${a.uri}\``, n(a.size)]));
}
console.log('\n</details>');
