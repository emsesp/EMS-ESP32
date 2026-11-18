// @ts-check
import eslint from '@eslint/js';
import prettierConfig from 'eslint-config-prettier';
import { defineConfig } from 'eslint/config';
import tseslint from 'typescript-eslint';

export default defineConfig(
  eslint.configs.recommended,
  ...tseslint.configs.recommendedTypeChecked,
  prettierConfig,
  {
    languageOptions: {
      parserOptions: {
        project: true
      }
    }
  },
  {
    ignores: [
      'dist/*',
      'dist-recovery/*',
      '*.mjs',
      'build/*',
      '*.js',
      '**/*.cjs',
      'i18n*.*'
    ]
  },
  {
    rules: {
      '@typescript-eslint/no-unsafe-enum-comparison': 'off',
      '@typescript-eslint/no-unused-expressions': 'off',
      '@typescript-eslint/no-unsafe-assignment': 'off',
      '@typescript-eslint/no-unsafe-member-access': 'off',
      '@typescript-eslint/no-unsafe-call': 'off',
      '@typescript-eslint/no-misused-promises': [
        'error',
        {
          checksVoidReturn: false
        }
      ]
    }
  },
  {
    files: ['src/**'],
    rules: {
      'no-restricted-imports': [
        'error',
        {
          patterns: [
            {
              group: [
                '@mui/*',
                '@emotion/*',
                'react-icons',
                'react-icons/*',
                '@table-library/*'
              ],
              message: 'Import UI only from components/ui.'
            }
          ]
        }
      ]
    }
  }
);
