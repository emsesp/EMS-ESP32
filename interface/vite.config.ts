import { defineConfig, type PluginOption } from 'vite';
import react from '@vitejs/plugin-react-swc';
import viteTsconfigPaths from 'vite-tsconfig-paths';
import svgrPlugin from 'vite-plugin-svgr';
import { visualizer } from 'rollup-plugin-visualizer';
import ProgmemGenerator from './progmem-generator';

export default defineConfig(({ command, mode }) => {
  if (mode === 'hosted') {
    return {
      // hosted, ignore all errors, output to dist
      plugins: [react({ plugins: [['@swc/plugin-styled-components', {}]] }), viteTsconfigPaths(), svgrPlugin()]
    };
  } else {
    // normal build
    return {
      plugins: [
        react({ plugins: [['@swc/plugin-styled-components', {}]] }),
        viteTsconfigPaths(),
        svgrPlugin(),
        ProgmemGenerator({ outputPath: '../lib/framework/WWWData.h', bytesPerLine: 20 }),
        visualizer({ gzipSize: true }) as PluginOption
      ],
      build: {
        outDir: 'build',
        chunkSizeWarningLimit: 1024
      },
      server: {
        open: true,
        port: 3000,
        proxy: {
          '/rest': 'http://localhost:3080',
          '/api': {
            target: 'http://localhost:3080',
            changeOrigin: true,
            secure: false,
            ws: true
          }
        }
      }
    };
  }
});
