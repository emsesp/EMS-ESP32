import React from 'react';
import { TextValidator, ValidatorForm, SelectValidator } from 'react-material-ui-form-validator';

import { Checkbox, TextField } from '@material-ui/core';
import SaveIcon from '@material-ui/icons/Save';
import MenuItem from '@material-ui/core/MenuItem';

import { RestFormProps, FormActions, FormButton, BlockFormControlLabel, PasswordValidator } from '../components';
import { isIP, isHostname, or } from '../validators';

import { MqttSettings } from './types';

type MqttSettingsFormProps = RestFormProps<MqttSettings>;

class MqttSettingsForm extends React.Component<MqttSettingsFormProps> {

  componentDidMount() {
    ValidatorForm.addValidationRule('isIPOrHostname', or(isIP, isHostname));
  }

  render() {
    const { data, handleValueChange, saveData, loadData } = this.props;
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
          label="Enable MQTT?"
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
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.clean_session}
              onChange={handleValueChange('clean_session')}
              value="clean_session"
            />
          }
          label="Clean Session?"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:1', 'maxNumber:65535']}
          errorMessages={['Max topic length is required', "Must be a number", "Must be greater than 0", "Max value is 65535"]}
          name="max_topic_length"
          label="Max Topic Length"
          fullWidth
          variant="outlined"
          value={data.max_topic_length}
          type="number"
          onChange={handleValueChange('max_topic_length')}
          margin="normal"
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.system_heartbeat}
              onChange={handleValueChange('system_heartbeat')}
              value="system_heartbeat"
            />
          }
          label="MQTT Heartbeat"
        />
        <SelectValidator name="mqtt_format"
          label="MQTT Format"
          value={data.mqtt_format}
          fullWidth
          variant="outlined"
          onChange={handleValueChange('mqtt_format')}
          margin="normal">
          <MenuItem value={1}>Single</MenuItem>
          <MenuItem value={2}>Nested</MenuItem>
          <MenuItem value={3}>Home Assistant</MenuItem>
          <MenuItem value={4}>Custom</MenuItem>
        </SelectValidator>
        <SelectValidator name="mqtt_qos"
          label="MQTT QoS"
          value={data.mqtt_qos}
          fullWidth
          variant="outlined"
          onChange={handleValueChange('mqtt_qos')}
          margin="normal">
          <MenuItem value={0}>0</MenuItem>
          <MenuItem value={1}>1</MenuItem>
          <MenuItem value={2}>2</MenuItem>
        </SelectValidator>
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:1', 'maxNumber:65535']}
          errorMessages={['Publish time is required', "Must be a number", "Must be greater than 0", "Max value is 65535"]}
          name="publish_time"
          label="MQTT Publish Time (seconds)"
          fullWidth
          variant="outlined"
          value={data.publish_time}
          type="number"
          onChange={handleValueChange('publish_time')}
          margin="normal"
        />
        <FormActions>
          <FormButton startIcon={<SaveIcon />} variant="contained" color="primary" type="submit">
            Save
          </FormButton>
          <FormButton variant="contained" color="secondary" onClick={loadData}>
            Reset
          </FormButton>
        </FormActions>
      </ValidatorForm>
    );
  }
}

export default MqttSettingsForm;
