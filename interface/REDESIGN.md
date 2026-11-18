# Web UI redesign

Working plan for moving the web UI to the new design. This file is the source of truth for progress: update it in the same commit as the work, so both machines (and any agent) see the current state after a `git pull`.

- Branch: `webui-redesign` on `origin` (proddy/EMS-ESP32), based on `dev`
- Visual spec: the mockups, hosted at <https://emsesp-mockups.emsesp.org>
- Step IDs (`S2.3` etc.) are used in commit messages and PR titles, for example `webui: status overview (S2.1)`

## Context (read first)

Findings from the analysis, so a new session doesn't have to rediscover them.

**Current UI**

- Preact runs through `preact/compat`. `react` is aliased to `src/preact-react-shim.ts` in `vite.config.ts` (react-router v8 needs `useOptimistic`). Keep that.
- Routes: `src/index.tsx` (react-router v8, `RequireAuthenticated`, `RequireAdmin`). Menu: `components/layout/LayoutMenu.tsx`, separate from the routes today.
- REST: alova in `src/api/endpoints.ts` (bearer token, `PASSWORD_REQUIRED` re-prompt via `passwordPrompt.ts`, MessagePack unpack for `arraybuffer` responses). Calls per area in `src/api/*.ts`.
- Settings forms use `src/utils/useRest.ts`: keeps `origData`, `dirtyFlags`, `useBlocker` for the unsaved-changes guard, and handles the 205 "Reboot required" response. The new save bar should sit on top of this, not replace it.
- List pages (Scheduler, Commands, Custom Entities, Modules, Customizations) keep original values as `o_*` fields added in `src/api/app.ts` transforms. `usePendingList` (S5.1) is meant to replace those.
- Theme: `src/CustomTheme.tsx` (MUI dark theme, font size 13). Shared components: `src/components/` (layout, inputs, loading, routing, toast, upload).
- i18n: typesafe-i18n, `LL.*` keys, `src/i18n/<lang>/index.ts`.
- MUI is imported in 79 files, about 393 `sx` props, MUI icons in 58 files, `@table-library` in 11, `react-icons` in 3.
- Biggest pages: ApplicationSettings 1147 lines, Version 1045, Customizations 955, Devices 834, Recovery Dashboard 630.

**Mockups to real pages**

| Mockup | Real page |
| --- | --- |
| `shell.js` (drawer, breadcrumbs via `parents`, dialog, toast) | `components/layout/*` |
| `system.js` `status`, `status-*` | `app/status/*` |
| `system.js` `settings`, `settings-*`, `FORMS` + `formPage` (prototype for `FormPage`) | `app/settings/*` |
| `a-classic.html` `downloadupload`, `restore`, backup dialog | `app/settings/DownloadUpload.tsx` + new restore page |
| `a-classic.html` `dashboard`, `devices` | `app/main/Dashboard.tsx`, `Devices.tsx` |
| `modules.js` (sensors, customizations, commands, scheduler, customentities, `openForm` = prototype for `EntityDialog`) | `app/main/*` |
| `account.js` (help, user) | `app/main/Help.tsx`, `UserProfile.tsx` |
| `base.css` (all tokens and styles) | `components/ui/tokens.css` + component CSS |
| `data.js` (mock data) | shapes are illustrative; real shapes are in `src/types/*` and `app/main/types.ts` |

Run the mockups with `cd mockups && python3 -m http.server 8765`.

**Icons**

The mockups use Material Symbols Rounded by name (`<span class="ms">name</span>`). Copy each used icon as SVG from `google/material-design-icons` (`symbols/web/<name>/materialsymbolsrounded/<name>_24px.svg`) or the `@material-symbols/svg-400` package, keep only the path data, and register it in `components/ui/icons/`. No font, no CDN.

**Firmware already supports** (no change needed): hashed API keys shown once on create (`/rest/apiKeys`), `auto_fw_check`, `write_limit` / `write_limit_refuse`, GPIO lists in sensor and settings data, `setPartition` / `getVersions` / `uploadURL` / `recovery_version`, `/rest/console`, `/rest/logSettings`, `/rest/time`, product ID per device in `/rest/coreData` (`p`), all export types except `heatingbackup`.

## Goals

- Look and behave like the mockups
- **Smaller flash**: no `@mui/*` or `@emotion/*` at the end, local SVG icons only (no icon fonts, no CDN), fewer third-party libraries
- **Responsive**: phone (360 px and up), tablet and desktop
- No regressions in behaviour: auth and admin gating, unsaved-changes guard, password re-prompt, i18n for every language, MessagePack endpoints
- Keep page URLs unchanged (bookmarks and firmware redirects such as `/fileUpdated` depend on them)

## Rules

- Pages import UI only from `components/ui`, never from `@mui` directly. An ESLint `no-restricted-imports` rule enforces this per migrated folder.
- Icons: one `Icon` component backed by local SVG files in `components/ui/icons/`, only the icons actually used. Material Symbols Rounded, 24 px, copied in as SVG paths.
- Styling: plain CSS with the tokens from `components/ui/tokens.css` (taken from `mockups/base.css`). No CSS-in-JS.
- Every new UI string goes into all `src/i18n/<lang>/index.ts` files.
- Any REST change: update `mock-api/restServer.ts` in the same commit.
- Each step ends with `pnpm lint`, `pnpm build`, a bundle size entry in the table below, and a check on a phone-sized viewport.

## Responsive rules

| Width | Layout |
| --- | --- |
| < 600 px (phone) | Drawer is an overlay opened from the app bar. Tables with many columns turn into stacked cards. Dialogs are full screen. Save bar spans the full width at the bottom. Form fields in one column. |
| 600-1199 px (tablet) | Drawer is an overlay, or collapses to icons from 900 px. Two-column forms and tile grids. |
| >= 1200 px (desktop) | Fixed drawer, as in the mockups. |

Touch targets at least 40 px, no hover-only actions, inputs at least 16 px font on iOS (avoids zoom on focus).

## Flash budget

Measured with `pnpm build` (gzipped sizes, bytes).

| Step | vendor js | app js | WWWData.h | Notes |
| --- | --- | --- | --- | --- |
| Baseline (dev 2d80f70) | 183,780 | 63,792 | 1,813,258 | MUI 9, emotion, table-library, react-icons |

## Firmware / REST changes

All additive unless marked. Keep this list current; it feeds the changelog and the PR descriptions.

| Endpoint | Change | Step | Status |
| --- | --- | --- | --- |
| `GET /rest/systemStatus` | add network type + IP, MQTT broker host:port, log error/warning counts since boot | S2.1 | todo |
| `POST /rest/action` export `heatingbackup` | new export type for heating settings backup | S4.9 | todo |
| UI route `/status/version` | removed, firmware page only at `/settings/version` (UI only, breaking for bookmarks) | S4.8 | todo |

## Steps

Status: `[ ]` todo, `[~]` in progress, `[x]` done. Add the PR link when one exists.

### S0 Groundwork (no visible change)

- [ ] S0.1 `components/ui/` folder, `tokens.css`, base styles, `Icon` with local SVGs
- [ ] S0.2 Dev-only gallery route `/ui` showing every component with mock data (not in production build)
- [ ] S0.3 Route table (path, title key, parent, icon, admin, badge) used by router, drawer and breadcrumbs
- [ ] S0.4 ESLint rule blocking `@mui` imports in migrated folders
- [ ] S0.5 Record baseline sizes (done above), check the bundle report runs in CI

### S1 App shell

- [ ] S1.1 Drawer (fixed / overlay), app bar with breadcrumbs from the route table
- [ ] S1.2 Toasts, dialogs and the save bar slot in the new style
- [ ] S1.3 Sign-in page

### S2 Status pages (read-only)

- [ ] S2.1 Status overview: hero, tiles, memory meters, restart / factory reset
- [ ] S2.2 Hardware
- [ ] S2.3 Network, MQTT, NTP, Access Point (shared key/value page)
- [ ] S2.4 Data Traffic (activity)
- [ ] S2.5 System Log: levels, filter, compact, auto-scroll, pause, download, console line

### S3 Small pages

- [ ] S3.1 Help
- [ ] S3.2 User Profile

### S4 Settings

- [ ] S4.1 Settings list
- [ ] S4.2 `FormPage` + save bar + restart hint, first used by Access Point
- [ ] S4.3 NTP
- [ ] S4.4 MQTT (topic preview)
- [ ] S4.5 Network + Wi-Fi scan tab
- [ ] S4.6 Application (search, group jump buttons)
- [ ] S4.7 Security: users, API keys; Modules
- [ ] S4.8 Firmware (test partition switch, install, recovery, upload on real boards incl. 4 MB)
- [ ] S4.9 Download/Upload + heating settings backup and restore (firmware change, discussion #2707)

### S5 Module pages

- [ ] S5.1 `usePendingList` hook (replaces the `o_*` copies in `api/app.ts`) + `EntityDialog`
- [ ] S5.2 Commands
- [ ] S5.3 Custom Entities
- [ ] S5.4 Scheduler
- [ ] S5.5 Sensors
- [ ] S5.6 Customizations (bulk write-disable)

### S6 Main pages

- [ ] S6.1 Dashboard
- [ ] S6.2 Devices

### S7 Clean-up

- [ ] S7.1 Replace remaining MUI-backed components (Dialog, Select, Menu, Tooltip)
- [ ] S7.2 Replace `@table-library` if a plain table + sort/filter helper is enough
- [ ] S7.3 Remove `@mui/*`, `@emotion/*`, `react-icons`; final size comparison
- [ ] S7.4 Recovery UI (`src/recovery`) on the same components
- [ ] S7.5 Changelog entries, remove `mockups/` and `.cursor/rules/webui-redesign.mdc` from the branch, PR to upstream `dev`

## Decisions

Short log of choices made along the way, newest last.

- 2026-11-18: Migrate inside `interface/` page by page, no separate Preact port of the mockups.
- 2026-11-18: Own component library with plain CSS and local SVG icons; MUI only as a temporary backend for complex components.
- 2026-11-18: Cursor rule `.cursor/rules/webui-redesign.mdc` (force-added, `.cursor` is gitignored) makes every session read this file and record decisions. Branch-only.

## Working on two machines

1. `git pull` before starting, `pnpm install` in `interface/` if `pnpm-lock.yaml` changed.
2. Work, then tick the step here and add sizes to the flash budget table.
3. Commit with the step ID in the message and push before switching machines. Push work-in-progress too (`wip: ... (S2.3)`), and squash it later if needed.
