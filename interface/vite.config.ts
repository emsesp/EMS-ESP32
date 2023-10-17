import { defineConfig } from 'vite';
import viteTsconfigPaths from 'vite-tsconfig-paths';
import preact from '@preact/preset-vite';
import viteImagemin from 'vite-plugin-imagemin';
import { visualizer } from 'rollup-plugin-visualizer';

export default defineConfig(({ command, mode }) => {
  // standalone build for development - runs the server
  if (command === 'serve') {
    console.log('Building for standalone');
    return {
      plugins: [preact(), viteTsconfigPaths(), visualizer()],
      server: {
        open: true,
        port: mode === 'production' ? 4173 : 3000,
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

  // production build, both for hosted and building the firmware
  if (command === 'build') {
    console.log('Building for production, mode ' + mode);
    return {
      plugins: [
        preact(),
        viteTsconfigPaths(),
        {
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
                {
                  name: 'removeViewBox'
                },
                {
                  name: 'removeEmptyAttrs',
                  active: false
                }
              ]
            }
          }),
          enforce: 'pre'
        }
      ],

      build: {
        outDir: mode === 'hosted' ? 'dist' : 'build',
        reportCompressedSize: false,
        chunkSizeWarningLimit: 1024,
        sourcemap: false,
        manifest: false,
        minify: 'terser',
        terserOptions: {
          parse: {
            // parse options
          },
          compress: {
            // compress options
            passes: 4,
            arrows: true,
            drop_console: true,
            sequences: true
          },
          mangle: {
            // mangle options
            eval: true,
            properties: {
              // mangle property options
            }
          },
          format: {
            // format options (can also use `output` for backwards compatibility)
          },
          sourceMap: {
            // source map options
          },
          ecma: 5,
          enclose: false,
          keep_classnames: false,
          keep_fnames: false,
          ie8: false,
          module: false,
          nameCache: null,
          safari10: false,
          toplevel: false
        },
        rollupOptions: {
          // Ignore "use client" waning since we are not using SSR
          onwarn(warning, warn) {
            if (warning.code === 'MODULE_LEVEL_DIRECTIVE' && warning.message.includes(`"use client"`)) {
              return;
            }
            warn(warning);
          }
        }
      }
    };
  }
});
