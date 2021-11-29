import { FC } from "react";
import { FormControlLabel, FormControlLabelProps } from "@mui/material";

const BlockFormControlLabel: FC<FormControlLabelProps> = (props) => (
  <div>
    <FormControlLabel {...props} />
  </div>
);

export default BlockFormControlLabel;
