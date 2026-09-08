import preact from '@preact/preset-vite';
import fs from 'fs';
import path from 'path';
import { visualizer } from 'rollup-plugin-visualizer';
import { pathToFileURL } from 'url';
import { Plugin, PluginOption, defineConfig } from 'vite';
import viteImagemin from 'vite-plugin-imagemin';
import zlib from 'zlib';

// Constants
const KB_DIVISOR = 1024;
const REPEAT_CHAR = '=';
const REPEAT_COUNT = 50;
const DEFAULT_OUT_DIR = 'dist';
const ES_TARGET = 'es2020';
const CHUNK_SIZE_WARNING_LIMIT = 1024;
const ASSETS_INLINE_LIMIT = 4096;

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

// Common resolve extensions - prioritize TypeScript/React files for Windows compatibility
const RESOLVE_EXTENSIONS = ['.tsx', '.ts', '.jsx', '.js', '.mjs', '.json'];

// Bundle file interface
interface BundleFile {
  name: string;
  size: number;
  gzipSize: number;
}

// Plugin to display bundle size information
const bundleSizeReporter = (): Plugin => {
  return {
    name: 'bundle-size-reporter',
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    writeBundle(options: any, bundle: any) {
      console.log('\n📦 Bundle Size Report:');
      console.log(REPEAT_CHAR.repeat(REPEAT_COUNT));

      const files: BundleFile[] = [];
      const outDir = options.dir || DEFAULT_OUT_DIR;

      // eslint-disable-next-line @typescript-eslint/no-explicit-any, @typescript-eslint/no-unsafe-argument
      const bundleEntries: Array<[string, any]> = Object.entries(bundle);
      for (const [fileName, chunk] of bundleEntries) {
        if (chunk?.type === 'chunk' || chunk?.type === 'asset') {
          // eslint-disable-next-line @typescript-eslint/no-unsafe-argument
          const filePath = path.join(outDir, fileName);
          try {
            const stats = fs.statSync(filePath);
            const size = stats.size;
            const fileContent = fs.readFileSync(filePath);
            const gzipSize = zlib.gzipSync(fileContent).length;

            files.push({ name: fileName, size, gzipSize });
          } catch (error) {
            console.warn(`Could not read file ${fileName}:`, error);
          }
        }
      }

      files.sort((a, b) => b.size - a.size);

      // files.forEach((file) => {
      //   const sizeKB = (file.size / KB_DIVISOR).toFixed(2);
      //   const gzipKB = (file.gzipSize / KB_DIVISOR).toFixed(2);
      //   console.log(
      //     `📄 ${file.name.padEnd(30)} ${sizeKB.padStart(8)} KB (${gzipKB} KB gzipped)`
      //   );
      // });

      const totalSize = files.reduce((sum, file) => sum + file.size, 0);
      const totalGzipSize = files.reduce((sum, file) => sum + file.gzipSize, 0);
      const compressionRatio = ((totalSize - totalGzipSize) / totalSize) * 100;

      console.log(REPEAT_CHAR.repeat(REPEAT_COUNT));
      console.log(`📊 Total Bundle Size: ${(totalSize / KB_DIVISOR).toFixed(2)} KB`);
      console.log(
        `🗜️  Total Gzipped Size: ${(totalGzipSize / KB_DIVISOR).toFixed(2)} KB`
      );
      console.log(`📈 Compression Ratio: ${compressionRatio.toFixed(1)}%`);
      console.log(REPEAT_CHAR.repeat(REPEAT_COUNT));
    }
  };
};

// Common preact plugin config
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

// Common base plugins
const createBasePlugins = (
  devToolsEnabled: boolean,
  includeBundleReporter = true
): PluginOption[] => {
  const plugins: PluginOption[] = [...createPreactPlugin(devToolsEnabled)];
  if (includeBundleReporter) {
    plugins.push(bundleSizeReporter());
  }
  return plugins;
};

const manualChunks = (id: string): string | undefined => {
  if (id.includes('node_modules')) return 'vendor';
  return undefined;
};

// Common build base configuration
const createBaseBuildConfig = () => ({
  target: ES_TARGET,
  chunkSizeWarningLimit: CHUNK_SIZE_WARNING_LIMIT,
  cssMinify: true,
  assetsInlineLimit: ASSETS_INLINE_LIMIT
});

// Terser options for hosted builds
const createHostedTerserOptions = () => ({
  compress: {
    passes: 3,
    drop_console: true,
    drop_debugger: true,
    dead_code: true,
    unused: true
  },
  mangle: {
    toplevel: true
  },
  ecma: 2020 as const
});

// Terser options for production builds
const createProductionTerserOptions = () => ({
  compress: {
    passes: 6,
    arrows: true,
    drop_console: true,
    drop_debugger: true,
    sequences: true
  },
  mangle: {
    toplevel: true,
    module: true
  },
  ecma: 2020 as const,
  enclose: false,
  keep_classnames: false,
  keep_fnames: false,
  ie8: false,
  module: false,
  safari10: false,
  toplevel: true
});

// Image optimization plugin.
// Only svgo runs here: the raster optimizers (optipng, pngquant, gifsicle,
// jpegtran, mozjpeg) are native binaries whose install scripts are blocked by
// `allowBuilds` in pnpm-workspace.yaml, so they would fail on every build.
// Raster assets are committed pre-optimized instead.
const imageOptimizationPlugin = {
  ...viteImagemin({
    verbose: false,
    filter: /\.svg$/i,
    gifsicle: false,
    jpegTran: false,
    mozjpeg: false,
    optipng: false,
    pngquant: false,
    svgo: {
      plugins: [
        { name: 'removeViewBox' },
        { name: 'removeEmptyAttrs', active: false }
      ]
    }
  }),
  enforce: 'pre' as const
};

export default defineConfig(
  async ({ command, mode }: { command: string; mode: string }) => {
    if (command === 'serve') {
      console.log(`Preparing for standalone build with server, mode=${mode}`);
      const mockServerUrl = pathToFileURL(
        path.resolve(import.meta.dirname, '../mock-api/mockServer.js')
      ).href;
      const { default: mockServer } = (await import(
        /* @vite-ignore */ mockServerUrl
      )) as { default: () => PluginOption };
      return {
        plugins: [...createBasePlugins(true, true), mockServer()],
        resolve: {
          alias: RESOLVE_ALIASES,
          extensions: RESOLVE_EXTENSIONS,
          tsconfigPaths: true
        },
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
        plugins: createBasePlugins(false, true),
        resolve: {
          alias: RESOLVE_ALIASES,
          extensions: RESOLVE_EXTENSIONS,
          tsconfigPaths: true
        },
        build: {
          ...createBaseBuildConfig(),
          minify: 'terser' as const,
          terserOptions: createHostedTerserOptions(),
          rollupOptions: {
            checks: {
              pluginTimings: false
            },
            treeshake: {
              moduleSideEffects: false
            },
            output: {
              manualChunks
            }
          }
        }
      };
    }

    console.log('Preparing for production, optimized build');

    return {
      plugins: [
        ...createBasePlugins(false, true),
        imageOptimizationPlugin,
        visualizer({
          template: 'treemap',
          open: false,
          gzipSize: true,
          brotliSize: true,
          filename: '../analyse.html'
        })
      ],
      resolve: {
        alias: RESOLVE_ALIASES,
        extensions: RESOLVE_EXTENSIONS,
        tsconfigPaths: true
      },
      build: {
        ...createBaseBuildConfig(),
        minify: 'terser' as const,
        terserOptions: createProductionTerserOptions(),
        rollupOptions: {
          checks: {
            pluginTimings: false
          },
          treeshake: {
            moduleSideEffects: false,
            propertyReadSideEffects: false as const,
            unknownGlobalSideEffects: false
          },
          output: {
            chunkFileNames: 'assets/[name]-[hash].js',
            entryFileNames: 'assets/[name]-[hash].js',
            assetFileNames: 'assets/[name]-[hash].[ext]',
            manualChunks,
            sourcemap: false
          }
        }
      }
    };
  }
);
