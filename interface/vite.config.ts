import preact from '@preact/preset-vite';
import fs from 'fs';
import path from 'path';
import { visualizer } from 'rollup-plugin-visualizer';
import { defineConfig } from 'vite';
import { Plugin } from 'vite';
import viteImagemin from 'vite-plugin-imagemin';
import viteTsconfigPaths from 'vite-tsconfig-paths';
import zlib from 'zlib';

// @ts-expect-error - mock server doesn't have type declarations
import mockServer from '../mock-api/mockServer.js';

// Plugin to display bundle size information
const bundleSizeReporter = (): Plugin => {
  return {
    name: 'bundle-size-reporter',
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    writeBundle(options: any, bundle: any) {
      console.log('\n📦 Bundle Size Report:');
      console.log('='.repeat(50));

      let totalSize = 0;
      const files: Array<{ name: string; size: number; gzipSize?: number }> = [];

      for (const [fileName, chunk] of Object.entries(
        bundle as Record<string, unknown>
      )) {
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        if ((chunk as any).type === 'chunk' || (chunk as any).type === 'asset') {
          const filePath = path.join((options.dir as string) || 'dist', fileName);
          let size = 0;
          let gzipSize = 0;

          try {
            const stats = fs.statSync(filePath);
            size = stats.size;
            totalSize += size;

            // Calculate gzip size
            const fileContent = fs.readFileSync(filePath);
            gzipSize = zlib.gzipSync(fileContent).length;

            files.push({
              name: fileName,
              size,
              gzipSize
            });
          } catch (error) {
            console.warn(`Could not read file ${fileName}:`, error);
          }
        }
      }

      // Sort files by size (largest first)
      files.sort((a, b) => b.size - a.size);

      // Display individual file sizes
      files.forEach((file) => {
        const sizeKB = (file.size / 1024).toFixed(2);
        const gzipKB = file.gzipSize ? (file.gzipSize / 1024).toFixed(2) : 'N/A';
        console.log(
          `📄 ${file.name.padEnd(30)} ${sizeKB.padStart(8)} KB (${gzipKB} KB gzipped)`
        );
      });

      console.log('='.repeat(50));
      console.log(`📊 Total Bundle Size: ${(totalSize / 1024).toFixed(2)} KB`);

      // Calculate and display gzip total
      const totalGzipSize = files.reduce(
        (sum, file) => sum + (file.gzipSize || 0),
        0
      );
      console.log(`🗜️  Total Gzipped Size: ${(totalGzipSize / 1024).toFixed(2)} KB`);

      // Show compression ratio
      const compressionRatio = (
        ((totalSize - totalGzipSize) / totalSize) *
        100
      ).toFixed(1);
      console.log(`📈 Compression Ratio: ${compressionRatio}%`);

      console.log('='.repeat(50));
    }
  };
};

export default defineConfig(
  ({ command, mode }: { command: string; mode: string }) => {
    if (command === 'serve') {
      console.log('Preparing for standalone build with server, mode=' + mode);
      return {
        plugins: [
          preact({
            // Keep dev tools enabled for development
            devToolsEnabled: true,
            prefreshEnabled: false
          }),
          viteTsconfigPaths(),
          bundleSizeReporter(), // Add bundle size reporting
          mockServer()
        ],
        resolve: {
          alias: {
            react: 'preact/compat',
            'react-dom': 'preact/compat',
            'react/jsx-runtime': 'preact/jsx-runtime'
          }
        },
        server: {
          open: true,
          port: mode == 'production' ? 4173 : 3000,
          proxy: {
            '/api': {
              target: 'http://localhost:3080',
              changeOrigin: true,
              secure: false
            },
            '/rest': 'http://localhost:3080',
            '/gh': 'http://localhost:3080' // mock for GitHub API
          }
        },
        // Optimize development builds
        build: {
          target: 'es2020',
          minify: false, // Disable minification for faster dev builds
          sourcemap: true // Enable source maps for debugging
        }
      };
    }

    if (mode === 'hosted') {
      console.log('Preparing for hosted build');
      return {
        plugins: [
          preact({
            // Enable Preact optimizations for hosted build
            devToolsEnabled: false,
            prefreshEnabled: false
          }),
          viteTsconfigPaths(),
          bundleSizeReporter() // Add bundle size reporting
        ],
        resolve: {
          alias: {
            react: 'preact/compat',
            'react-dom': 'preact/compat',
            'react/jsx-runtime': 'preact/jsx-runtime'
          }
        },
        build: {
          target: 'es2020',
          chunkSizeWarningLimit: 512,
          minify: 'terser',
          cssMinify: true,
          assetsInlineLimit: 4096,
          terserOptions: {
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
            ecma: 2020
          },
          rollupOptions: {
            treeshake: {
              moduleSideEffects: false
            },
            output: {
              manualChunks(id: string) {
                if (id.includes('node_modules')) {
                  if (id.includes('preact')) {
                    return '@preact';
                  }
                  return 'vendor';
                }
                return undefined;
              }
            }
          }
        }
      };
    }

    console.log('Preparing for production, optimized build');

    return {
      plugins: [
        preact({
          // Enable Preact optimizations
          devToolsEnabled: false,
          prefreshEnabled: false
        }),
        viteTsconfigPaths(),
        // Enable image optimization for size reduction
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
        },
        visualizer({
          template: 'treemap', // or sunburst
          open: false,
          gzipSize: true,
          brotliSize: true,
          filename: '../analyse.html' // will be saved in project's root
        }),
        bundleSizeReporter() // Add bundle size reporting
      ],

      resolve: {
        alias: {
          react: 'preact/compat',
          'react-dom': 'preact/compat',
          'react/jsx-runtime': 'preact/jsx-runtime'
        }
      },

      build: {
        // Target modern browsers for smaller bundles
        target: 'es2020',
        chunkSizeWarningLimit: 512,
        minify: 'terser',
        // Enable CSS minification
        cssMinify: true,
        // Optimize asset handling
        assetsInlineLimit: 4096, // Inline small assets
        terserOptions: {
          compress: {
            passes: 6,
            arrows: true,
            drop_console: true,
            drop_debugger: true,
            sequences: true
            // Additional aggressive compression options
            // dead_code: true,
            // hoist_funs: true,
            // hoist_vars: true,
            // if_return: true,
            // join_vars: true,
            // loops: true,
            // pure_getters: true,
            // reduce_vars: true,
            // side_effects: false,
            // switches: true,
            // unsafe: true,
            // unsafe_arrows: true,
            // unsafe_comps: true,
            // unsafe_Function: true,
            // unsafe_math: true,
            // unsafe_proto: true,
            // unsafe_regexp: true,
            // unsafe_undefined: true,
            // unused: true
          },
          mangle: {
            toplevel: true, // Enable top-level mangling
            module: true // Enable module mangling
            // properties: {
            //   regex: /^_/   // Mangle properties starting with _
            // }
          },
          ecma: 2020, // Updated to modern ECMAScript
          enclose: false,
          keep_classnames: false,
          keep_fnames: false,
          ie8: false,
          module: false,
          safari10: false,
          toplevel: true // Enable top-level optimization
        },

        rollupOptions: {
          // Enable aggressive tree shaking
          treeshake: {
            moduleSideEffects: false,
            propertyReadSideEffects: false,
            tryCatchDeoptimization: false,
            unknownGlobalSideEffects: false
          },
          output: {
            // Optimize chunk naming for better caching
            chunkFileNames: 'assets/[name]-[hash].js',
            entryFileNames: 'assets/[name]-[hash].js',
            assetFileNames: 'assets/[name]-[hash].[ext]',
            manualChunks(id: string) {
              if (id.includes('node_modules')) {
                // More granular chunk splitting for better caching
                if (id.includes('react-router')) {
                  return '@react-router';
                }
                if (id.includes('preact')) {
                  return '@preact';
                }
                if (id.includes('@mui/material')) {
                  return '@mui-material';
                }
                if (id.includes('@mui/icons-material')) {
                  return '@mui-icons';
                }
                if (id.includes('alova')) {
                  return '@alova';
                }
                if (id.includes('typesafe-i18n')) {
                  return '@i18n';
                }
                if (id.includes('react-toastify')) {
                  return '@toastify';
                }
                if (id.includes('@table-library')) {
                  return '@table-library';
                }
                if (id.includes('uuid')) {
                  return '@uuid';
                }
                if (id.includes('axios') || id.includes('fetch')) {
                  return '@http';
                }
                if (id.includes('lodash') || id.includes('ramda')) {
                  return '@utils';
                }
                return 'vendor';
              }
              // Split large application modules
              if (id.includes('components/')) {
                return 'components';
              }
              if (id.includes('app/')) {
                return 'app';
              }
              if (id.includes('utils/')) {
                return 'utils';
              }
              if (id.includes('api/')) {
                return 'api';
              }
              return undefined;
            },
            // Enable source maps for debugging (optional)
            sourcemap: false // Disable for production to save space
          }
        }
      }
    };
  }
);
