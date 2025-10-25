import { memo } from 'react';
import type { FC } from 'react';

import { CssBaseline, ThemeProvider, responsiveFontSizes } from '@mui/material';
import { createTheme } from '@mui/material/styles';

import type { RequiredChildrenProps } from 'utils';

// Memoize dialog style to prevent recreation
export const dialogStyle = {
  '& .MuiDialog-paper': {
    borderRadius: '8px',
    borderColor: '#565656',
    borderStyle: 'solid',
    borderWidth: '1px'
  }
} as const;

// Memoize theme creation to prevent recreation
const theme = responsiveFontSizes(
  createTheme({
    typography: {
      fontSize: 13
    },
    palette: {
      mode: 'dark',
      secondary: {
        main: '#2196f3' // blue[500]
      },
      info: {
        main: '#607d8b' // blueGrey[500]
      },
      text: {
        disabled: '#eee' // white
      }
    },
    components: {
      MuiListItemText: {
        styleOverrides: {
          primary: {
            fontSize: 14
          },
          secondary: {
            color: '#9e9e9e' // grey[500]
          }
        }
      }
    }
  })
);

const CustomTheme: FC<RequiredChildrenProps> = memo(({ children }) => (
  <ThemeProvider theme={theme}>
    <CssBaseline />
    {children}
  </ThemeProvider>
));

export default CustomTheme;
