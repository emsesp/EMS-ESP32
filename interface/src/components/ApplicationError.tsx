import React, { FC } from 'react';
import { makeStyles } from '@material-ui/styles';
import { Paper, Typography, Box, CssBaseline } from "@material-ui/core";
import WarningIcon from "@material-ui/icons/Warning"

const styles = makeStyles(
  {
    siteErrorPage: {
      display: "flex",
      height: "100vh",
      justifyContent: "center",
      flexDirection: "column"
    },
    siteErrorPagePanel: {
      textAlign: "center",
      padding: "280px 0 40px 0",
      backgroundImage: 'url("/app/icon.png")',
      backgroundRepeat: "no-repeat",
      backgroundPosition: "50% 40px",
      backgroundSize: "200px auto",
      width: "100%",
    }
  }
);

interface ApplicationErrorProps {
  error?: string;
}

const ApplicationError: FC<ApplicationErrorProps> = ({ error }) => {
  const classes = styles();
  return (
    <div className={classes.siteErrorPage}>
      <CssBaseline />
      <Paper className={classes.siteErrorPagePanel} elevation={10}>
        <Box display="flex" flexDirection="row" justifyContent="center" alignItems="center" mb={2}>
          <WarningIcon fontSize="large" color="error" />
          <Box ml={2}>
            <Typography variant="h4">
              Application error
            </Typography>
          </Box>
        </Box>
        <Typography variant="subtitle1" gutterBottom>
          Failed to configure the application, please refresh to try again.
        </Typography>
        {error &&
          (
            <Typography variant="subtitle2" gutterBottom>
              Error: {error}
            </Typography>
          )
        }
      </Paper>
    </div>
  );
}

export default ApplicationError;
