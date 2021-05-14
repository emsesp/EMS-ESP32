import React, { RefObject } from 'react';
import { TextValidator, ValidatorForm } from 'react-material-ui-form-validator';
import {
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Box,
  Typography
} from '@material-ui/core';
import { FormButton } from '../components';
import { DeviceValue } from './EMSESPtypes';

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

  buildLabel = (devicevalue: DeviceValue) => {
    if (devicevalue.uom === '' || !devicevalue.uom) {
      return 'New value';
    }
    return 'New value (' + devicevalue.uom + ')';
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
          <DialogTitle id="user-form-dialog-title">
            Change the {devicevalue.name}
          </DialogTitle>
          <DialogContent dividers>
            <TextValidator
              validators={['required']}
              errorMessages={['is required']}
              name="data"
              label={this.buildLabel(devicevalue)}
              fullWidth
              variant="outlined"
              value={devicevalue.data}
              margin="normal"
              onChange={handleValueChange('data')}
            />
            <Box color="warning.main" p={1} pl={0} pr={0} mt={0} mb={0}>
              <Typography variant="body2">
                <i>
                  Note: it may take a few seconds before the change is visible.
                  If nothing happens check the logs.
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
