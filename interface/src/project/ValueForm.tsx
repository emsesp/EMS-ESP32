import React, { RefObject } from 'react';
import { ValidatorForm } from 'react-material-ui-form-validator';
import {
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Box,
  Typography,
  FormHelperText,
  OutlinedInput,
  InputAdornment,
  TextField,
  MenuItem
} from '@material-ui/core';

import { FormButton } from '../components';
import { DeviceValue, DeviceValueUOM_s } from './EMSESPtypes';

interface ValueFormProps {
  devicevalue: DeviceValue;
  onDoneEditing: () => void;
  onCancelEditing: () => void;
  handleValueChange: (
    data: keyof DeviceValue
  ) => (event: React.ChangeEvent<HTMLInputElement>) => void;
}
class ValueForm extends React.Component<ValueFormProps> {
  formRef: RefObject<any> = React.createRef();

  submit = () => {
    this.formRef.current.submit();
  };

  render() {
    const {
      devicevalue,
      handleValueChange,
      onDoneEditing,
      onCancelEditing
    } = this.props;

    return (
      <ValidatorForm onSubmit={onDoneEditing} ref={this.formRef}>
        <Dialog
          maxWidth="xs"
          onClose={onCancelEditing}
          aria-labelledby="user-form-dialog-title"
          open
        >
          <DialogTitle id="user-form-dialog-title">Change Value</DialogTitle>
          <DialogContent dividers>
            {devicevalue.l && (
              <TextField
                id="outlined-select-value"
                select
                value={devicevalue.v}
                autoFocus
                fullWidth
                onChange={handleValueChange('v')}
                variant="outlined"
              >
                {devicevalue.l.map((val) => (
                  <MenuItem value={val}>{val}</MenuItem>
                ))}
              </TextField>
            )}
            {!devicevalue.l && (
              <OutlinedInput
                id="value"
                value={devicevalue.v}
                autoFocus
                fullWidth
                onChange={handleValueChange('v')}
                endAdornment={
                  <InputAdornment position="end">
                    {DeviceValueUOM_s[devicevalue.u]}
                  </InputAdornment>
                }
              />
            )}
            <FormHelperText>{devicevalue.n}</FormHelperText>
            <Box color="warning.main" p={0} pl={0} pr={0} mt={4} mb={0}>
              <Typography variant="body2">
                <i>
                  Note: it may take a few seconds before the change is
                  registered with the EMS device.
                </i>
              </Typography>
            </Box>
          </DialogContent>
          <DialogActions>
            <FormButton
              variant="contained"
              color="secondary"
              onClick={onCancelEditing}
            >
              Cancel
            </FormButton>
            <FormButton
              variant="contained"
              color="primary"
              type="submit"
              onClick={this.submit}
            >
              Done
            </FormButton>
          </DialogActions>
        </Dialog>
      </ValidatorForm>
    );
  }
}

export default ValueForm;
