import React, { RefObject } from 'react';
import { ValidatorForm } from 'react-material-ui-form-validator';
import {
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  FormHelperText,
  OutlinedInput,
  InputAdornment
} from '@material-ui/core';

import { FormButton } from '../components';
import { Sensor } from './EMSESPtypes';

interface SensorFormProps {
  sensor: Sensor;
  onDoneEditing: () => void;
  onCancelEditing: () => void;
  handleSensorChange: (
    data: keyof Sensor
  ) => (event: React.ChangeEvent<HTMLInputElement>) => void;
}

class SensorForm extends React.Component<SensorFormProps> {
  formRef: RefObject<any> = React.createRef();

  submit = () => {
    this.formRef.current.submit();
  };

  render() {
    const {
      sensor,
      handleSensorChange,
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
            Editing Sensor #{sensor.no}
          </DialogTitle>
          <DialogContent dividers>
            <FormHelperText>Name (no spaces)</FormHelperText>
            <OutlinedInput
              id="id"
              value={sensor.id}
              autoFocus
              fullWidth
              onChange={handleSensorChange('id')}
            />

            <FormHelperText>Custom Offset</FormHelperText>
            <OutlinedInput
              id="offset"
              value={sensor.offset}
              fullWidth
              type="number"
              onChange={handleSensorChange('offset')}
              endAdornment={<InputAdornment position="end">°C</InputAdornment>}
            />
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

export default SensorForm;
