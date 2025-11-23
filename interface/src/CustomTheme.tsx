import { memo } from 'react';
import type { FC } from 'react';

import {
  CssBaseline,
  ThemeProvider,
  responsiveFontSizes,
  tooltipClasses
} from '@mui/material';
import { createTheme } from '@mui/material/styles';

import type { RequiredChildrenProps } from 'utils';

export const dialogStyle = {
  '& .MuiDialog-paper': {
    borderRadius: '8px',
    borderColor: '#565656',
    borderStyle: 'solid',
    borderWidth: '2px'
  }
} as const;

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
      },
      MuiTooltip: {
        defaultProps: {
          placement: 'top',
          arrow: true
        },
        styleOverrides: {
          tooltip: {
            padding: '4px 8px',
            fontSize: 10,
            color: 'rgba(0, 0, 0, 0.87)',
            backgroundColor: '#4caf50', // MUI success.main default color
            boxShadow: '0px 2px 8px rgba(0, 0, 0, 0.15)',
            [`& .${tooltipClasses.arrow}`]: {
              color: '#4caf50'
            }
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
