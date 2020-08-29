import React, { FC } from "react";
import { FormControlLabel, FormControlLabelProps } from "@material-ui/core";

const BlockFormControlLabel: FC<FormControlLabelProps> = (props) => (
  <div>
    <FormControlLabel {...props} />
  </div>
)

export default BlockFormControlLabel;
