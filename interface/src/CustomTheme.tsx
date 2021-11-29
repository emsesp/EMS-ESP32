import { FC } from 'react';

import { CssBaseline } from '@mui/material';
import { createTheme, responsiveFontSizes, ThemeProvider } from '@mui/material/styles';
import { blueGrey, blue } from '@mui/material/colors';

const theme = responsiveFontSizes(
  createTheme({
    typography: {
      fontFamily: `"Roboto", "Helvetica", "Arial", sans-serif`,
      fontSize: 13,
      fontWeightLight: 300,
      fontWeightRegular: 400,
      fontWeightMedium: 500
    },
    palette: {
      mode: 'dark',
      // background: {
      //   default: grey[900], // #212121
      //   // paper: grey[800]
      // },
      // primary: {
      //   main: '#33bfff'
      // },
      secondary: {
        main: blue[500] // in buttons
      },
      info: {
        main: blueGrey[500] // used in icons
      }
      // warning: {
      //   main: orange[500]
      // },
      // error: {
      //   main: red[200]
      // },
      // success: {
      //   main: green[700]
      // }
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
