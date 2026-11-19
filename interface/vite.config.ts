import preact from '@preact/preset-vite';
import path from 'path';
import { visualizer } from 'rollup-plugin-visualizer';
import { pathToFileURL } from 'url';
import {
  type BuildEnvironmentOptions,
  type Plugin,
  type PluginOption,
  type UserConfig,
  defineConfig,
  normalizePath
} from 'vite';

const RECOVERY_OUT_DIR = 'dist-recovery';
const ES_TARGET = 'es2022';
const CHUNK_SIZE_WARNING_LIMIT = 1024;

// Common resolve aliases
// `react` points at a local shim (preact/compat + a useOptimistic stub) because
// react-router v8 statically imports useOptimistic from "react". See src/preact-react-shim.ts for details.
const REACT_SHIM = path.resolve(import.meta.dirname, 'src/preact-react-shim.ts');
// `react/jsx-runtime` is listed before `react` so the more specific alias wins
// (a bare `react` alias would otherwise also match `react/jsx-runtime`).
// NOTE: @preact/preset-vite is configured with `reactAliasesEnabled: false`
// so these aliases (not the preset's `react -> preact/compat`) are authoritative.
const RESOLVE_ALIASES = {
  'react/jsx-runtime': 'preact/jsx-runtime',
  'react-dom/test-utils': 'preact/test-utils',
  'react-dom': 'preact/compat',
  react: REACT_SHIM
};

// The recovery app is flashed into the small factory partition. components/ui/Icon.tsx imports
// the full icon registry; in the recovery app it gets src/recovery/icons.ts, which lists only
// the icons the recovery shows.
const ICON_COMPONENT = normalizePath(
  path.resolve(import.meta.dirname, 'src/components/ui/Icon.tsx')
);
const RECOVERY_ICONS = normalizePath(
  path.resolve(import.meta.dirname, 'src/recovery/icons.ts')
);
const recoveryIcons = (): Plugin => ({
  name: 'recovery-icons',
  enforce: 'pre',
  resolveId(source, importer) {
    if (
      source === './icons' &&
      importer &&
      normalizePath(importer.split('?')[0] ?? '') === ICON_COMPONENT
    ) {
      return RECOVERY_ICONS;
    }
    return null;
  }
});

// Common resolve extensions - prioritize TypeScript/React files for Windows compatibility
const RESOLVE_EXTENSIONS = ['.tsx', '.ts', '.jsx', '.js', '.mjs', '.json'];

const createPreactPlugin = (devToolsEnabled: boolean): PluginOption[] => {
  const plugins = preact({
    devToolsEnabled,
    prefreshEnabled: false,
    // Disable the preset's built-in `react -> preact/compat` aliases so our
    // RESOLVE_ALIASES (which routes `react` through the useOptimistic shim) win.
    reactAliasesEnabled: false
  });
  // The preset always registers its devtools-only plugins (`preact:devtools`
  // and `preact:transform-hook-names`); the flag only gates their work, not
  // their presence. When disabled they're per-module no-ops that still run a
  // hook on every module and dominate [PLUGIN_TIMINGS], so drop them entirely.
  const devToolsOnly = new Set(['preact:devtools', 'preact:transform-hook-names']);
  return devToolsEnabled
    ? plugins
    : plugins.filter((p) => !p || !devToolsOnly.has(p.name));
};

const baseConfig = (devToolsEnabled: boolean, plugins: PluginOption[] = []) =>
  ({
    plugins: [...createPreactPlugin(devToolsEnabled), ...plugins],
    resolve: {
      alias: RESOLVE_ALIASES,
      extensions: RESOLVE_EXTENSIONS,
      tsconfigPaths: true
    }
  }) satisfies UserConfig;

const loadMockPlugin = async (file: string): Promise<PluginOption> => {
  const url = pathToFileURL(path.resolve(import.meta.dirname, '../mock-api', file)).href;
  const { default: plugin } = (await import(/* @vite-ignore */ url)) as {
    default: () => PluginOption;
  };
  return plugin();
};

// A separate vendor chunk compresses ~2 KB smaller for the main UI, but is larger for the
// small recovery app, which therefore ships as a single JS file.
const createBuildConfig = (
  aggressive: boolean,
  { input, vendorChunk = true }: { input?: string; vendorChunk?: boolean } = {}
): BuildEnvironmentOptions => ({
  target: ES_TARGET,
  chunkSizeWarningLimit: CHUNK_SIZE_WARNING_LIMIT,
  // progmem-generator.js reports the real (zopfli) sizes that end up in flash
  reportCompressedSize: false,
  minify: 'terser',
  terserOptions: {
    module: true,
    ecma: 2022,
    compress: {
      passes: aggressive ? 3 : 2,
      drop_console: ['log', 'debug', 'info']
    }
  },
  rolldownOptions: {
    ...(input && { input }),
    checks: {
      pluginTimings: false
    },
    treeshake: aggressive
      ? {
          moduleSideEffects: false,
          propertyReadSideEffects: false,
          unknownGlobalSideEffects: false
        }
      : { moduleSideEffects: false },
    ...(vendorChunk && {
      output: {
        codeSplitting: { groups: [{ name: 'vendor', test: /[\\/]node_modules[\\/]/ }] }
      }
    })
  }
});

export default defineConfig(async ({ command, mode }) => {
  if (command === 'serve' && mode === 'recovery') {
    console.log('Preparing Recovery WebUI with mock server');
    return {
      ...baseConfig(true, [recoveryIcons(), await loadMockPlugin('recoveryMock.js')]),
      server: {
        open: '/recovery.html',
        port: 3001
      }
    };
  }

  if (mode === 'recovery') {
    console.log('Preparing for Recovery WebUI, optimized build');
    return {
      ...baseConfig(false, [recoveryIcons()]),
      build: {
        ...createBuildConfig(true, {
          input: path.resolve(import.meta.dirname, 'recovery.html'),
          vendorChunk: false
        }),
        outDir: RECOVERY_OUT_DIR,
        emptyOutDir: true
      }
    };
  }

  if (command === 'serve') {
    console.log(`Preparing for standalone build with server, mode=${mode}`);
    return {
      ...baseConfig(true, [await loadMockPlugin('mockServer.js')]),
      server: {
        open: true,
        port: mode === 'production' ? 4173 : 3000,
        proxy: {
          '/api': {
            target: 'http://localhost:3080',
            changeOrigin: true,
            secure: false
          },
          '/rest': 'http://localhost:3080'
        }
      },
      build: {
        target: ES_TARGET,
        minify: false,
        sourcemap: true
      }
    };
  }

  if (mode === 'hosted') {
    console.log('Preparing for hosted build');
    return {
      ...baseConfig(false),
      build: createBuildConfig(false)
    };
  }

  console.log('Preparing for production, optimized build');
  return {
    ...baseConfig(
      false,
      process.env.ANALYZE
        ? [
            visualizer({
              template: 'treemap',
              open: false,
              gzipSize: true,
              brotliSize: true,
              filename: '../analyse.html'
            })
          ]
        : []
    ),
    build: createBuildConfig(true)
  };
});
