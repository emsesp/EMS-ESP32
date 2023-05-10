import { CssBaseline } from '@mui/material';
import { blueGrey, blue } from '@mui/material/colors';
import { createTheme, responsiveFontSizes, ThemeProvider } from '@mui/material/styles';
import type { FC } from 'react';

import type { RequiredChildrenProps } from 'utils';

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

const CustomTheme: FC<RequiredChildrenProps> = ({ children }) => (
  <ThemeProvider theme={theme}>
    <CssBaseline />
    {children}
  </ThemeProvider>
);

export default CustomTheme;
