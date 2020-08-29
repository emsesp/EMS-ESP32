import React from 'react';
import { TextValidator, ValidatorComponentProps } from 'react-material-ui-form-validator';

import { withStyles, WithStyles, createStyles } from '@material-ui/core/styles';
import { InputAdornment, IconButton } from '@material-ui/core';
import {Visibility,VisibilityOff } from '@material-ui/icons';

const styles = createStyles({
  input: {
    "&::-ms-reveal": {
      display: "none"
    }
  }
});

type PasswordValidatorProps = WithStyles<typeof styles> & Exclude<ValidatorComponentProps, "type" | "InputProps">;

interface PasswordValidatorState {
  showPassword: boolean;
}

class PasswordValidator extends React.Component<PasswordValidatorProps, PasswordValidatorState> {

  state = {
    showPassword: false
  };

  toggleShowPassword = () => {
    this.setState({
      showPassword: !this.state.showPassword
    });
  }

  render() {
    const { classes, ...rest } = this.props;
    return (
      <TextValidator
        {...rest}
        type={this.state.showPassword ? 'text' : 'password'}
        InputProps={{
          classes,
          endAdornment:
            <InputAdornment position="end">
              <IconButton
                aria-label="Toggle password visibility"
                onClick={this.toggleShowPassword}
              >
                {this.state.showPassword ? <Visibility /> : <VisibilityOff />}
              </IconButton>
            </InputAdornment>
        }}
      />
    );
  }

}

export default withStyles(styles)(PasswordValidator);
