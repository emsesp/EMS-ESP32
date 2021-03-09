import React from 'react';
import { TextValidator, ValidatorForm, SelectValidator } from 'react-material-ui-form-validator';

import { Checkbox, TextField, Typography } from '@material-ui/core';
import SaveIcon from '@material-ui/icons/Save';
import MenuItem from '@material-ui/core/MenuItem';

import { RestFormProps, FormActions, FormButton, BlockFormControlLabel, PasswordValidator } from '../components';
import { isIP, isHostname, or, isPath } from '../validators';

import { MqttSettings } from './types';

type MqttSettingsFormProps = RestFormProps<MqttSettings>;

class MqttSettingsForm extends React.Component<MqttSettingsFormProps> {

  componentDidMount() {
    ValidatorForm.addValidationRule('isIPOrHostname', or(isIP, isHostname));
    ValidatorForm.addValidationRule('isPath', isPath);
  }

  render() {
    const { data, handleValueChange, saveData } = this.props;
    return (
      <ValidatorForm onSubmit={saveData}>
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.enabled}
              onChange={handleValueChange('enabled')}
              value="enabled"
            />
          }
          label="Enable MQTT"
        />
        <TextValidator
          validators={['required', 'isIPOrHostname']}
          errorMessages={['Host is required', "Not a valid IP address or hostname"]}
          name="host"
          label="Host"
          fullWidth
          variant="outlined"
          value={data.host}
          onChange={handleValueChange('host')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
          errorMessages={['Port is required', "Must be a number", "Must be greater than 0 ", "Max value is 65535"]}
          name="port"
          label="Port"
          fullWidth
          variant="outlined"
          value={data.port}
          type="number"
          onChange={handleValueChange('port')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isPath']}
          errorMessages={['Base is required', "Not a valid Path"]}
          name="base"
          label="Base"
          fullWidth
          variant="outlined"
          value={data.base}
          onChange={handleValueChange('base')}
          margin="normal"
        />
        <TextField
          name="username"
          label="Username"
          fullWidth
          variant="outlined"
          value={data.username}
          onChange={handleValueChange('username')}
          margin="normal"
        />
        <PasswordValidator
          name="password"
          label="Password"
          fullWidth
          variant="outlined"
          value={data.password}
          onChange={handleValueChange('password')}
          margin="normal"
        />
        <TextField
          name="client_id"
          label="Client ID (optional)"
          fullWidth
          variant="outlined"
          value={data.client_id}
          onChange={handleValueChange('client_id')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:1', 'maxNumber:65535']}
          errorMessages={['Keep alive is required', "Must be a number", "Must be greater than 0", "Max value is 65535"]}
          name="keep_alive"
          label="Keep Alive (seconds)"
          fullWidth
          variant="outlined"
          value={data.keep_alive}
          type="number"
          onChange={handleValueChange('keep_alive')}
          margin="normal"
        />
        <SelectValidator name="mqtt_qos"
          label="QoS"
          value={data.mqtt_qos}
          fullWidth
          variant="outlined"
          onChange={handleValueChange('mqtt_qos')}
          margin="normal">
          <MenuItem value={0}>0 (default)</MenuItem>
          <MenuItem value={1}>1</MenuItem>
          <MenuItem value={2}>2</MenuItem>
        </SelectValidator>
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.clean_session}
              onChange={handleValueChange('clean_session')}
              value="clean_session"
            />
          }
          label="Clean Session"
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.mqtt_retain}
              onChange={handleValueChange('mqtt_retain')}
              value="mqtt_retain"
            />
          }
          label="Retain Flag"
        />
        <br></br>
        <Typography variant="h6" color="primary" >
          Formatting
        </Typography>
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.nested_format}
              onChange={handleValueChange('nested_format')}
              value="nested_format"
            />
          }
          label="Nested format (Thermostat & Mixer only)"
        />
        <SelectValidator name="dallas_format"
          label="Dallas Sensor Payload Grouping"
          value={data.dallas_format}
          fullWidth
          variant="outlined"
          onChange={handleValueChange('dallas_format')}
          margin="normal">
          <MenuItem value={1}>by Sensor ID</MenuItem>
          <MenuItem value={2}>by Number</MenuItem>
        </SelectValidator>
        <SelectValidator name="bool_format"
          label="Boolean Format"
          value={data.bool_format}
          fullWidth
          variant="outlined"
          onChange={handleValueChange('bool_format')}
          margin="normal">
          <MenuItem value={1}>"on"/"off"</MenuItem>
          <MenuItem value={2}>true/false</MenuItem>
          <MenuItem value={3}>1/0</MenuItem>
          <MenuItem value={4}>"ON"/"OFF"</MenuItem>
        </SelectValidator>
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.ha_enabled}
              onChange={handleValueChange('ha_enabled')}
              value="ha_enabled"
            />
          }
          label="Home Assistant MQTT Discovery"
        />
        { data.ha_enabled &&
          <SelectValidator name="ha_climate_format"
            label="Thermostat Room Temperature"
            value={data.ha_climate_format}
            fullWidth
            variant="outlined"
            onChange={handleValueChange('ha_climate_format')}
            margin="normal">
            <MenuItem value={1}>use Current temperature (default)</MenuItem>
            <MenuItem value={2}>use Setpoint temperature</MenuItem>
            <MenuItem value={3}>Fix to 0</MenuItem>
          </SelectValidator>
        }
        <br></br>
        <Typography variant="h6" color="primary" >
          Publish Intervals
        </Typography>
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
          errorMessages={['Publish time is required', "Must be a number", "Must be 0 or greater", "Max value is 65535"]}
          name="publish_time_boiler"
          label="Boiler Publish Interval (seconds, 0=on change)"
          fullWidth
          variant="outlined"
          value={data.publish_time_boiler}
          type="number"
          onChange={handleValueChange('publish_time_boiler')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
          errorMessages={['Publish time is required', "Must be a number", "Must be 0 or greater", "Max value is 65535"]}
          name="publish_time_thermostat"
          label="Thermostat Publish Interval (seconds, 0=on change)"
          fullWidth
          variant="outlined"
          value={data.publish_time_thermostat}
          type="number"
          onChange={handleValueChange('publish_time_thermostat')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
          errorMessages={['Publish time is required', "Must be a number", "Must be 0 or greater", "Max value is 65535"]}
          name="publish_time_solar"
          label="Solar Publish Interval (seconds, 0=on change)"
          fullWidth
          variant="outlined"
          value={data.publish_time_solar}
          type="number"
          onChange={handleValueChange('publish_time_solar')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
          errorMessages={['Publish time is required', "Must be a number", "Must be 0 or greater", "Max value is 65535"]}
          name="publish_time_mixer"
          label="Mixer Publish Interval (seconds, 0=on change)"
          fullWidth
          variant="outlined"
          value={data.publish_time_mixer}
          type="number"
          onChange={handleValueChange('publish_time_mixer')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
          errorMessages={['Publish time is required', "Must be a number", "Must be 0 or greater", "Max value is 65535"]}
          name="publish_time_sensor"
          label="Sensors Publish Interval (seconds, 0=on change)"
          fullWidth
          variant="outlined"
          value={data.publish_time_sensor}
          type="number"
          onChange={handleValueChange('publish_time_sensor')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
          errorMessages={['Publish time is required', "Must be a number", "Must be 0 or greater", "Max value is 65535"]}
          name="publish_time_other"
          label="All other Modules Publish Interval (seconds, 0=on change)"
          fullWidth
          variant="outlined"
          value={data.publish_time_other}
          type="number"
          onChange={handleValueChange('publish_time_other')}
          margin="normal"
        />
        <FormActions>
          <FormButton startIcon={<SaveIcon />} variant="contained" color="primary" type="submit">
            Save
          </FormButton>
        </FormActions>
      </ValidatorForm>
    );
  }
}

export default MqttSettingsForm;
