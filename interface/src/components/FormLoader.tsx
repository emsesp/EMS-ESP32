import { FC } from 'react';

import { makeStyles, Theme, createStyles } from '@material-ui/core/styles';
import { Button, LinearProgress, Typography } from '@material-ui/core';

const useStyles = makeStyles((theme: Theme) =>
  createStyles({
    loadingSettings: {
      margin: theme.spacing(0.5)
    },
    loadingSettingsDetails: {
      margin: theme.spacing(4),
      textAlign: 'center'
    },
    button: {
      marginRight: theme.spacing(2),
      marginTop: theme.spacing(2)
    }
  })
);

interface FormLoaderProps {
  errorMessage?: string;
  loadData: () => void;
}

const FormLoader: FC<FormLoaderProps> = ({ errorMessage, loadData }) => {
  const classes = useStyles();
  if (errorMessage) {
    return (
      <div className={classes.loadingSettings}>
        <Typography variant="h6" className={classes.loadingSettingsDetails}>
          {errorMessage}
        </Typography>
        <Button
          variant="contained"
          color="secondary"
          className={classes.button}
          onClick={loadData}
        >
          Retry
        </Button>
      </div>
    );
  }
  return (
    <div className={classes.loadingSettings}>
      <LinearProgress className={classes.loadingSettingsDetails} />
      <Typography variant="h6" className={classes.loadingSettingsDetails}>
        Loading&hellip;
      </Typography>
    </div>
  );
};

export default FormLoader;
