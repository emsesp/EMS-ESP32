import { Component } from 'react';

import {
  ValidatorForm,
  SelectValidator
} from 'react-material-ui-form-validator';

import MenuItem from '@material-ui/core/MenuItem';

import {
  redirectingAuthorizedFetch,
  withAuthenticatedContext,
  AuthenticatedContextProps
} from '../authentication';

import { RestFormProps } from '../components';
import { LogSettings } from './types';

import { ENDPOINT_ROOT } from '../api';
export const LOG_SETTINGS_ENDPOINT = ENDPOINT_ROOT + 'logSettings';

type LogEventFormProps = AuthenticatedContextProps & RestFormProps<LogSettings>;

class LogEventForm extends Component<LogEventFormProps> {
  changeLevel = (event: React.ChangeEvent<HTMLSelectElement>) => {
    const { data, setData } = this.props;
    setData({
      ...data,
      level: parseInt(event.target.value)
    });

    redirectingAuthorizedFetch(LOG_SETTINGS_ENDPOINT, {
      method: 'POST',
      body: JSON.stringify({ level: event.target.value }),
      headers: {
        'Content-Type': 'application/json'
      }
    })
      .then((response) => {
        if (response.status === 200) {
          return response.json();
        }
        throw Error('Unexpected response code: ' + response.status);
      })
      .then((json) => {
        this.props.enqueueSnackbar('Log settings changed', {
          variant: 'success'
        });
        setData({
          ...data,
          level: json.level
        });
      })
      .catch((error) => {
        this.props.enqueueSnackbar(
          error.message || 'Problem changing log settings',
          { variant: 'warning' }
        );
      });
  };

  render() {
    const { data, saveData } = this.props;
    return (
      <ValidatorForm onSubmit={saveData}>
        <SelectValidator
          name="level"
          label="Log Level"
          value={data.level}
          variant="outlined"
          onChange={this.changeLevel}
          margin="normal"
        >
          <MenuItem value={-1}>OFF</MenuItem>
          <MenuItem value={3}>ERROR</MenuItem>
          <MenuItem value={4}>WARNING</MenuItem>
          <MenuItem value={5}>NOTICE</MenuItem>
          <MenuItem value={6}>INFO</MenuItem>
          <MenuItem value={7}>DEBUG</MenuItem>
          <MenuItem value={8}>TRACE</MenuItem>
        </SelectValidator>
      </ValidatorForm>
    );
  }
}

export default withAuthenticatedContext(LogEventForm);
