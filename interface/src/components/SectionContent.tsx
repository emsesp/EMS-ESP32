import React from 'react';

import { Typography, Paper } from '@material-ui/core';
import { createStyles, Theme, makeStyles } from '@material-ui/core/styles';

const useStyles = makeStyles((theme: Theme) =>
  createStyles({
    content: {
      padding: theme.spacing(2),
      margin: theme.spacing(3),
    }
  })
);

interface SectionContentProps {
  title: string;
  titleGutter?: boolean;
}

const SectionContent: React.FC<SectionContentProps> = (props) => {
  const { children, title, titleGutter } = props;
  const classes = useStyles();
  return (
    <Paper className={classes.content}>
      <Typography variant="h6" gutterBottom={titleGutter}>
        {title}
      </Typography>
      {children}
    </Paper>
  );
};

export default SectionContent;
