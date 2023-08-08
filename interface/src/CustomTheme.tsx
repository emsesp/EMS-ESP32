import { CssBaseline } from '@mui/material';
import { createTheme, responsiveFontSizes, ThemeProvider } from '@mui/material/styles';
import type { FC } from 'react';

import type { RequiredChildrenProps } from 'utils';

export const dialogStyle = {
  '& .MuiDialog-paper': {
    borderRadius: '8px',
    borderColor: '#565656',
    borderStyle: 'solid',
    borderWidth: '1px'
  },
  backdropFilter: 'blur(1px)'
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
