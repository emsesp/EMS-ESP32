import { Avatar, makeStyles } from "@material-ui/core";
import React, { FC } from "react";

interface HighlightAvatarProps {
  color: string;
}

const useStyles = makeStyles({
  root: (props: HighlightAvatarProps) => ({
    backgroundColor: props.color
  })
});

const HighlightAvatar: FC<HighlightAvatarProps> = (props) => {
  const classes = useStyles(props);
  return (
    <Avatar className={classes.root}>
      {props.children}
    </Avatar>
  );
}

export default HighlightAvatar;
