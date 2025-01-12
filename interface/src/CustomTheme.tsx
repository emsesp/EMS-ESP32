import type { FC } from 'react';

import { CssBaseline, ThemeProvider, responsiveFontSizes } from '@mui/material';
import { createTheme } from '@mui/material/styles';

import type { RequiredChildrenProps } from 'utils';

export const dialogStyle = {
  '& .MuiDialog-paper': {
    borderRadius: '8px',
    borderColor: '#565656',
    borderStyle: 'solid',
    borderWidth: '1px'
  }
};

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
    }
  })
);

const CustomTheme: FC<RequiredChildrenProps> = ({ children }) => (
  <ThemeProvider theme={theme}>
    <CssBaseline />
    {children}
  </ThemeProvider>
);

export default CustomTheme;
