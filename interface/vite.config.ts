import { defineConfig, type PluginOption } from 'vite';
import react from '@vitejs/plugin-react-swc';
import viteTsconfigPaths from 'vite-tsconfig-paths';
import svgrPlugin from 'vite-plugin-svgr';
// import { ViteMinifyPlugin } from 'vite-plugin-minify';
import { visualizer } from 'rollup-plugin-visualizer';
import ProgmemGenerator from './progmem-generator';

export default defineConfig(({ command, mode }) => {
  if (mode === 'hosted') {
    return {
      // hosted, ignore all errors, output to dist
      plugins: [react(), viteTsconfigPaths(), svgrPlugin(), visualizer({ gzipSize: true }) as PluginOption]
    };
  } else {
    // normal build
    return {
      plugins: [
        react(),
        viteTsconfigPaths(),
        svgrPlugin(),
        // ViteMinifyPlugin({
        //   removeAttributeQuotes: true,
        //   minifyCSS: true,
        //   minifyJS: true,
        //   decodeEntities: true,
        //   removeComments: true,
        //   removeEmptyAttributes: true,
        //   removeOptionalTags: true,
        //   removeEmptyElements: true,
        //   removeScriptTypeAttributes: true,
        //   removeTagWhitespace: true,
        //   minifyURLs: true
        // }),
        ProgmemGenerator({ outputPath: '../lib/framework/WWWData.h', bytesPerLine: 20 })
      ],

      build: {
        outDir: 'build',
        chunkSizeWarningLimit: 1024,
        sourcemap: false,
        manifest: false,
        minify: mode === 'development' ? false : 'terser'
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
