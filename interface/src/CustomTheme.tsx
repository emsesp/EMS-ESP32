import { FC } from 'react';

import { CssBaseline } from '@mui/material';
import { createTheme, responsiveFontSizes, ThemeProvider } from '@mui/material/styles';
import { blueGrey, blue } from '@mui/material/colors';

const theme = responsiveFontSizes(
  createTheme({
    typography: {
      fontSize: 13
    },
    palette: {
      mode: 'dark',
      secondary: {
        main: blue[500]
      },
      info: {
        main: blueGrey[500]
      }
    }
  })
);

const CustomTheme: FC = ({ children }) => (
  <ThemeProvider theme={theme}>
    <CssBaseline />
    {children}
  </ThemeProvider>
);

export default CustomTheme;
