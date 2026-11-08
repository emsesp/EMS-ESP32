/// <reference types="vite/client" />

interface ImportMetaEnv {
  // Optional comma-separated allow-list of locales to ship (e.g. "en,de,nl").
  // Unset => all locales are bundled and embedded. `en` is always kept as the
  // fallback. Consumed by App.tsx (UI language list) and progmem-generator.js
  // (which locale chunks get embedded into firmware flash).
  readonly VITE_APP_LOCALES?: string;
}

interface ImportMeta {
  readonly env: ImportMetaEnv;
}
