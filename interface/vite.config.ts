import { defineConfig, type PluginOption } from 'vite';
import viteTsconfigPaths from 'vite-tsconfig-paths';
import svgrPlugin from 'vite-plugin-svgr';
import { visualizer } from 'rollup-plugin-visualizer';
import ProgmemGenerator from './progmem-generator';
import preact from '@preact/preset-vite';
// import prefresh from '@prefresh/vite';

export default defineConfig(({ command, mode }) => {
  if (mode === 'hosted') {
    return {
      // hosted, ignore all errors, output to dist
      plugins: [preact(), viteTsconfigPaths(), svgrPlugin(), visualizer({ gzipSize: true }) as PluginOption]
    };
  } else {
    // normal build
    return {
      plugins: [
        preact(),
        viteTsconfigPaths(),
        svgrPlugin()
        // prefresh()
        // ProgmemGenerator({ outputPath: '../lib/framework/WWWData.h', bytesPerLine: 20 })
      ],

      build: {
        outDir: 'build',
        chunkSizeWarningLimit: 1024,
        sourcemap: false,
        manifest: false,
        minify: mode === 'development' ? false : 'terser',
        rollupOptions: {
          /**
           * Ignore "use client" waning since we are not using SSR
           */
          onwarn(warning, warn) {
            if (warning.code === 'MODULE_LEVEL_DIRECTIVE' && warning.message.includes(`"use client"`)) {
              return;
            }
            warn(warning);
          }
        }
      },

      onwarn(warning, warn) {
        if (warning.code === 'MODULE_LEVEL_DIRECTIVE') {
          return;
        }
        warn(warning);
      },

      server: {
        open: true,
        port: 3000,
        proxy: {
          '/rest': 'http://localhost:3080',
          '/api': {
            target: 'http://localhost:3080',
            changeOrigin: true,
            secure: false
          },
          '/es': {
            target: 'http://localhost:3080',
            changeOrigin: true,
            secure: false
          }
        }
      }
    };
  }
});
function prefresh(): PluginOption {
  throw new Error('Function not implemented.');
}
