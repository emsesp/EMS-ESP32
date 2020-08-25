import React from 'react';

import { makeStyles, Theme, createStyles } from '@material-ui/core/styles';
import { Button, LinearProgress, Typography } from '@material-ui/core';

import { RestControllerProps } from '.';

const useStyles = makeStyles((theme: Theme) =>
  createStyles({
    loadingSettings: {
      margin: theme.spacing(0.5),
    },
    loadingSettingsDetails: {
      margin: theme.spacing(4),
      textAlign: "center"
    },
    button: {
      marginRight: theme.spacing(2),
      marginTop: theme.spacing(2),
    }
  })
);

export type RestFormProps<D> = Omit<RestControllerProps<D>, "loading" | "errorMessage"> & { data: D };

interface RestFormLoaderProps<D> extends RestControllerProps<D> {
  render: (props: RestFormProps<D>) => JSX.Element;
}

export default function RestFormLoader<D>(props: RestFormLoaderProps<D>) {
  const { loading, errorMessage, loadData, render, data, ...rest } = props;
  const classes = useStyles();
  if (loading || !data) {
    return (
      <div className={classes.loadingSettings}>
        <LinearProgress className={classes.loadingSettingsDetails} />
        <Typography variant="h6" className={classes.loadingSettingsDetails}>
          Loading&hellip;
        </Typography>
      </div>
    );
  }
  if (errorMessage) {
    return (
      <div className={classes.loadingSettings}>
        <Typography variant="h6" className={classes.loadingSettingsDetails}>
          {errorMessage}
        </Typography>
        <Button variant="contained" color="secondary" className={classes.button} onClick={loadData}>
          Retry
        </Button>
      </div>
    );
  }
  return render({ ...rest, loadData, data });
}
