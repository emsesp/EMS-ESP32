# Web UI translations

How the web UI text is translated, how to add or change a string, and the two helper scripts: `pnpm translate` drafts translations with DeepL, and `pnpm check-layout` finds text that doesn't fit.

The firmware has its own translations (entity names in `src/core/locale_translations.h`). They are not covered here.

## Where the text lives

- One file per language: `src/i18n/<lang>/index.ts`, a flat object of keys like `SIGN_IN: 'Sign In'`. English (`en`) is the source for new text.
- Languages: cs, de, en, fr, hu, it, nl, no, pl, sk, sv, tr.
- [typesafe-i18n](https://github.com/ivanhofer/typesafe-i18n) generates `src/i18n/i18n-types.ts` from the base locale set in `.typesafe-i18n.json` (currently `pl`). Every language must have every key, otherwise TypeScript fails.
- Code uses the keys through `LL`, for example `LL.SIGN_IN()`.

### Placeholders and plurals

- `{0}` or `{name}` is an argument: `LOGGED_IN: 'Logged in as {name}'` is called as `LL.LOGGED_IN({ name })`. Every translation must keep the same placeholders.
- `{{...}}` is plural syntax and depends on the language: English `'{num} day{{s}}'`, German `'{{Wert|Werte}}'`, Polish `'{{N|n|}}iestandardowe…'`. See the typesafe-i18n docs on plural rules.

## Adding or changing a string

1. Add or change the key in `src/i18n/en/index.ts`.
2. Fill in the other languages: write them yourself, or draft them with DeepL (next section).
3. Run `pnpm typesafe-i18n` (or keep `pnpm standalone` running, which watches the files) and check that `npx tsc --noEmit` passes.
4. Run `pnpm check-layout` if the text appears in a menu, button, tab or table header.
5. Ask a native speaker to review machine translations (lines marked `// DeepL`).

Keep labels short. A menu item has about 130 px; long labels wrap to a second line and are cut off with "…" after that.

## Drafting translations with DeepL: `pnpm translate`

`translate-missing.js` sends English strings to the DeepL API and writes the results into the language files, marked `// DeepL` for review. It doesn't replace reviewed community translations unless you ask it to.

### Setup

Get an API key from your DeepL account (Account → API keys). Keys ending in `:fx` use the free API, others the Pro API. Never commit the key; export it in your shell:

```sh
export DEEPL_API_KEY="your-key"
```

### Usage

Run from `interface/`:

| Command                  | What it does                                                   |
| ------------------------ | -------------------------------------------------------------- |
| `pnpm translate --list`  | Lists what would be translated. No DeepL calls, no key needed. |
| `pnpm translate`         | Translates and prints the results; files are not changed.      |
| `pnpm translate --write` | Translates and updates `src/i18n/<lang>/index.ts`.             |

Options (combine with any of the above):

- `--lang it,nl` limits the run to some languages.
- `--keys SETTINGS,LOGGED_IN` (re)translates these keys, even when a translation exists.
- `--since <git ref>` also retranslates keys whose English text changed since that commit, e.g. `--since origin/dev`.

Without `--keys` or `--since`, only keys a language doesn't have yet are translated.

Typical flow after adding new English strings:

```sh
pnpm translate --list            # check the plan
pnpm translate                   # preview the translations
pnpm translate --write           # write them
pnpm typesafe-i18n && npx tsc --noEmit
pnpm check-layout --route /the/page
```

### What the script takes care of

- **Placeholders**: `{name}` and `{0}` are protected from DeepL. If a translation comes back with different placeholders it's rejected and listed under "Not applied".
- **Plurals**: strings with `{{...}}`, and values that aren't plain strings (`STATUS_NAMES`), are never sent. They're listed under "Translate by hand".
- **Consistent terms**: each language's existing translations of core terms (Devices, Entities, Customizations, Dashboard and so on, see `GLOSSARY_KEYS`) become a temporary DeepL glossary, so new text reuses the same words. EMS-ESP, EMS, MQTT, NTP, GPIO, API and Home Assistant are kept as they are. The glossary is deleted after the run.
- **Tone**: formal "you" for German and French, informal for Dutch and Italian, matching the existing files (`FORMALITY` in the script). DeepL ignores this for languages without the distinction.
- **Context**: DeepL is told the text is short UI labels for a heating-system web app, which helps with ambiguous words like "Reset" or "Value".
- **File layout**: an existing line is replaced in place; a missing key is inserted at the same position as in the English file. Prettier formats the changed files.

### Reviewing

Find machine translations with:

```sh
rg -n "// DeepL" src/i18n
```

When a native speaker has checked or corrected a line, remove the `// DeepL` marker. Watch out for heating terms (flow temperature, DHW, heat pump modes): DeepL is good at general UI text but sometimes picks the wrong technical word.

Be careful with `--since` on a large range: during the redesign many English strings were reworded, and retranslating all of them would replace community translations even where only the English phrasing changed. Use `--list` first and pick the keys whose meaning changed with `--keys`.

### Cost

The whole English UI is about 16,000 characters, so translating everything into all 11 languages is about 180,000 characters. Normal use, a few new strings at a time, is a few hundred characters.

## Checking the layout: `pnpm check-layout`

Translations are often longer than English. `check-layout.js` opens every page, plus the "add" dialogs, in every language at desktop (1280 px), tablet (1000 px) and phone (390 px) width, and lists text that is cut off, truncated or pushed off screen.

```sh
pnpm standalone                  # in another terminal: mock API + dev server
pnpm check-layout                # everything, about 6 minutes
pnpm check-layout --lang nl,de --width 390 --route /status/log,/settings
```

It exits with an error and lists each problem by page, width, element and language. Device names that are shortened on purpose are ignored.

It needs Chromium for `playwright-core`; install it once with `pnpm exec playwright-core install chromium`.

It only finds text that doesn't fit, not wrapping that looks awkward, so look at the page too. Not covered: edit and confirm dialogs, dropdown option lists, notifications and the recovery app.

### When something doesn't fit

Fix the component, not the string, so every language benefits. The shared components already handle long text:

- Menu and tab bar labels wrap to two lines (`.nav-label`, `.tab-label` in `Layout.css`).
- Toggle buttons and, on phones, normal buttons wrap their label (`ToggleGroup.css`, `Button.css`).
- Wide tables go in a `<div className="tbl-wrap">` so they scroll inside their card.
- `<html lang>` is set to the current language, so `hyphens: auto` can break long words.

Shorten a translation only when the text is really too long for its purpose, as with Dutch "Aanpassingen van entiteiten" → "Aanpassingen".

## Flash size

At build time the `compactTranslations` plugin in `vite.config.ts` stores each language as a list of values without the keys, which saves about 40 KB of flash. It works on the files as they are; nothing changes for translators. The build fails if a language has keys that English doesn't, or misses keys.
