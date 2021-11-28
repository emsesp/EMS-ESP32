import React, { RefObject } from 'react';
import { ValidatorForm, TextValidator } from 'react-material-ui-form-validator';
import {
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions
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
            Editing Sensor #{sensor.n}
          </DialogTitle>
          <DialogContent dividers>
            <TextValidator
              validators={['matchRegexp:^([a-zA-Z0-9_.-]{0,19})$']}
              errorMessages={['Not a valid name']}
              fullWidth
              variant="outlined"
              value={sensor.i}
              onChange={handleSensorChange('i')}
              margin="normal"
              label="Name"
              name="id"
            />
            <TextValidator
              validators={['isFloat', 'minFloat:-5', 'maxFloat:5']}
              errorMessages={[
                'Must be a number',
                'Must be greater than -5',
                'Max value is +5'
              ]}
              label="Custom Offset (°C)"
              name="offset"
              type="number"
              value={sensor.o}
              fullWidth
              variant="outlined"
              InputProps={{ inputProps: { min: '-5', max: '5', step: '0.1' } }}
              margin="normal"
              onChange={handleSensorChange('o')}
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
