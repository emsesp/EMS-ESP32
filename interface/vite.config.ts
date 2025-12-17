import preact from '@preact/preset-vite';
import fs from 'fs';
import path from 'path';
import { visualizer } from 'rollup-plugin-visualizer';
import { Plugin, defineConfig } from 'vite';
import viteImagemin from 'vite-plugin-imagemin';
import viteTsconfigPaths from 'vite-tsconfig-paths';
import zlib from 'zlib';

// @ts-expect-error - mock server doesn't have type declarations
import mockServer from '../mock-api/mockServer.js';

// Constants
const KB_DIVISOR = 1024;
const REPEAT_CHAR = '=';
const REPEAT_COUNT = 50;
const DEFAULT_OUT_DIR = 'dist';
const ES_TARGET = 'es2020';
const CHUNK_SIZE_WARNING_LIMIT = 512;
const ASSETS_INLINE_LIMIT = 4096;

// Common resolve aliases
const RESOLVE_ALIASES = {
  react: 'preact/compat',
  'react-dom': 'preact/compat',
  'react/jsx-runtime': 'preact/jsx-runtime'
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
const createPreactPlugin = (devToolsEnabled: boolean) =>
  preact({
    devToolsEnabled,
    prefreshEnabled: false
  });

// Common base plugins
const createBasePlugins = (
  devToolsEnabled: boolean,
  includeBundleReporter = true
) => {
  const plugins = [
    createPreactPlugin(devToolsEnabled),
    viteTsconfigPaths({
      projects: ['./tsconfig.json']
    })
  ];
  if (includeBundleReporter) {
    plugins.push(bundleSizeReporter());
  }
  return plugins;
};

// Manual chunk splitting strategy
const createManualChunks = (detailed = false) => {
  return (id: string): string | undefined => {
    if (id.includes('node_modules')) {
      if (id.includes('preact')) return '@preact';
      if (detailed) {
        if (id.includes('react-router')) return '@react-router';
        if (id.includes('@mui/material')) return '@mui-material';
        if (id.includes('@mui/icons-material')) return '@mui-icons';
        if (id.includes('alova')) return '@alova';
        if (id.includes('typesafe-i18n')) return '@i18n';
        if (id.includes('react-toastify')) return '@toastify';
        if (id.includes('@table-library')) return '@table-library';
        if (id.includes('uuid')) return '@uuid';
        if (id.includes('axios') || id.includes('fetch')) return '@http';
        if (id.includes('lodash') || id.includes('ramda')) return '@utils';
      }
      return 'vendor';
    }
    if (detailed) {
      if (id.includes('components/')) return 'components';
      if (id.includes('app/')) return 'app';
      if (id.includes('utils/')) return 'utils';
      if (id.includes('api/')) return 'api';
    }
    return undefined;
  };
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

// Image optimization plugin
const imageOptimizationPlugin = {
  ...viteImagemin({
    verbose: false,
    gifsicle: {
      optimizationLevel: 7,
      interlaced: false
    },
    optipng: {
      optimizationLevel: 7
    },
    mozjpeg: {
      quality: 20
    },
    pngquant: {
      quality: [0.8, 0.9],
      speed: 4
    },
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
  ({ command, mode }: { command: string; mode: string }) => {
    if (command === 'serve') {
      console.log(`Preparing for standalone build with server, mode=${mode}`);
      return {
        plugins: [...createBasePlugins(true, true), mockServer()],
        resolve: {
          alias: RESOLVE_ALIASES,
          extensions: RESOLVE_EXTENSIONS
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
            '/rest': 'http://localhost:3080',
            '/gh': 'http://localhost:3080'
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
          extensions: RESOLVE_EXTENSIONS
        },
        build: {
          ...createBaseBuildConfig(),
          minify: 'terser' as const,
          terserOptions: createHostedTerserOptions(),
          rollupOptions: {
            treeshake: {
              moduleSideEffects: false
            },
            output: {
              manualChunks: createManualChunks(false)
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
        extensions: RESOLVE_EXTENSIONS
      },
      build: {
        ...createBaseBuildConfig(),
        minify: 'terser' as const,
        terserOptions: createProductionTerserOptions(),
        rollupOptions: {
          treeshake: {
            moduleSideEffects: false,
            propertyReadSideEffects: false,
            tryCatchDeoptimization: false,
            unknownGlobalSideEffects: false
          },
          output: {
            chunkFileNames: 'assets/[name]-[hash].js',
            entryFileNames: 'assets/[name]-[hash].js',
            assetFileNames: 'assets/[name]-[hash].[ext]',
            manualChunks: createManualChunks(true),
            sourcemap: false
          }
        }
      }
    };
  }
);
