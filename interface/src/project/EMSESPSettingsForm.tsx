import { Component } from 'react';

import {
  ValidatorForm,
  TextValidator,
  SelectValidator
} from 'react-material-ui-form-validator';

import {
  Checkbox,
  Typography,
  Box,
  Link,
  withWidth,
  WithWidthProps,
  Grid
} from '@material-ui/core';

import SaveIcon from '@material-ui/icons/Save';
import MenuItem from '@material-ui/core/MenuItem';

import {
  redirectingAuthorizedFetch,
  withAuthenticatedContext,
  AuthenticatedContextProps
} from '../authentication';

import {
  RestFormProps,
  FormActions,
  FormButton,
  BlockFormControlLabel
} from '../components';

import { isIPv4, optional, isHostname, or } from '../validators';

import { EMSESPSettings } from './EMSESPtypes';

import { boardProfileSelectItems } from './EMSESPBoardProfiles';

import { ENDPOINT_ROOT } from '../api';
export const BOARD_PROFILE_ENDPOINT = ENDPOINT_ROOT + 'boardProfile';

type EMSESPSettingsFormProps = RestFormProps<EMSESPSettings> &
  AuthenticatedContextProps &
  WithWidthProps;

interface EMSESPSettingsFormState {
  processing: boolean;
}

class EMSESPSettingsForm extends Component<EMSESPSettingsFormProps> {
  state: EMSESPSettingsFormState = {
    processing: false
  };

  componentDidMount() {
    ValidatorForm.addValidationRule(
      'isOptionalIPorHost',
      optional(or(isIPv4, isHostname))
    );
  }

  changeBoardProfile = (event: React.ChangeEvent<HTMLSelectElement>) => {
    const { data, setData } = this.props;
    setData({
      ...data,
      board_profile: event.target.value
    });

    if (event.target.value === 'CUSTOM') return;

    this.setState({ processing: true });
    redirectingAuthorizedFetch(BOARD_PROFILE_ENDPOINT, {
      method: 'POST',
      body: JSON.stringify({ code: event.target.value }),
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
        this.props.enqueueSnackbar('Profile loaded', { variant: 'success' });
        setData({
          ...data,
          led_gpio: json.led_gpio,
          dallas_gpio: json.dallas_gpio,
          rx_gpio: json.rx_gpio,
          tx_gpio: json.tx_gpio,
          pbutton_gpio: json.pbutton_gpio,
          aux_gpio: json.aux_gpio,
          board_profile: event.target.value
        });
        this.setState({ processing: false });
      })
      .catch((error) => {
        this.props.enqueueSnackbar(
          error.message || 'Problem fetching board profile',
          { variant: 'warning' }
        );
        this.setState({ processing: false });
      });
  };

  render() {
    const { data, saveData, handleValueChange } = this.props;
    return (
      <ValidatorForm onSubmit={saveData}>
        <Box bgcolor="info.main" p={2} mt={2} mb={2}>
          <Typography variant="body1">
            <i>
              Refer to the
              <Link
                target="_blank"
                href="https://emsesp.github.io/docs/#/Configure-firmware32?id=ems-esp-settings"
                color="primary"
              >
                {' documentation'}
              </Link>
              &nbsp;for information on each setting
            </i>
          </Typography>
        </Box>

        <br></br>
        <Typography variant="h6" color="primary">
          EMS Bus
        </Typography>

        <Grid
          container
          spacing={1}
          direction="row"
          justify="flex-start"
          alignItems="flex-start"
        >
          <Grid item xs={5}>
            <SelectValidator
              name="tx_mode"
              label="Tx Mode"
              value={data.tx_mode}
              fullWidth
              variant="outlined"
              onChange={handleValueChange('tx_mode')}
              margin="normal"
            >
              <MenuItem value={0}>Off</MenuItem>
              <MenuItem value={1}>EMS</MenuItem>
              <MenuItem value={2}>EMS+</MenuItem>
              <MenuItem value={3}>HT3</MenuItem>
              <MenuItem value={4}>Hardware</MenuItem>
            </SelectValidator>
          </Grid>
          <Grid item xs={6}>
            <SelectValidator
              name="ems_bus_id"
              label="Bus ID"
              value={data.ems_bus_id}
              fullWidth
              variant="outlined"
              onChange={handleValueChange('ems_bus_id')}
              margin="normal"
            >
              <MenuItem value={0x0b}>Service Key (0x0B)</MenuItem>
              <MenuItem value={0x0d}>Modem (0x0D)</MenuItem>
              <MenuItem value={0x0a}>Terminal (0x0A)</MenuItem>
              <MenuItem value={0x0f}>Time Module (0x0F)</MenuItem>
              <MenuItem value={0x12}>Alarm Module (0x12)</MenuItem>
            </SelectValidator>
          </Grid>
        </Grid>

        <br></br>
        <Typography variant="h6" color="primary">
          Board Profile
        </Typography>

        <Box color="warning.main" p={0} mt={0} mb={0}>
          <Typography variant="body2">
            <i>
              Select a pre-configured board layout to automatically set the GPIO
              pins. Select "Custom..." to view or manually edit the values.
            </i>
          </Typography>
        </Box>

        <SelectValidator
          name="board_profile"
          label="Board Profile"
          value={data.board_profile}
          fullWidth
          variant="outlined"
          onChange={this.changeBoardProfile}
          margin="normal"
        >
          {boardProfileSelectItems()}
          <MenuItem key={'CUSTOM'} value={'CUSTOM'}>
            Custom...
          </MenuItem>
        </SelectValidator>

        {data.board_profile === 'CUSTOM' && (
          <Grid
            container
            spacing={1}
            direction="row"
            justify="flex-start"
            alignItems="flex-start"
          >
            <Grid item xs={4}>
              <TextValidator
                validators={[
                  'required',
                  'isNumber',
                  'minNumber:0',
                  'maxNumber:40',
                  'matchRegexp:^((?!6|7|8|9|10|11|12|14|15|20|24|28|29|30|31)[0-9]*)$'
                ]}
                errorMessages={[
                  'GPIO is required',
                  'Must be a number',
                  'Must be 0 or higher',
                  'Max value is 40',
                  'Not a valid GPIO'
                ]}
                name="rx_gpio"
                label="Rx GPIO"
                fullWidth
                variant="outlined"
                value={data.rx_gpio}
                type="number"
                onChange={handleValueChange('rx_gpio')}
                margin="normal"
              />
            </Grid>
            <Grid item xs={4}>
              <TextValidator
                validators={[
                  'required',
                  'isNumber',
                  'minNumber:0',
                  'maxNumber:40',
                  'matchRegexp:^((?!6|7|8|9|10|11|12|14|15|20|24|28|29|30|31)[0-9]*)$'
                ]}
                errorMessages={[
                  'GPIO is required',
                  'Must be a number',
                  'Must be 0 or higher',
                  'Max value is 40',
                  'Not a valid GPIO'
                ]}
                name="tx_gpio"
                label="Tx GPIO"
                fullWidth
                variant="outlined"
                value={data.tx_gpio}
                type="number"
                onChange={handleValueChange('tx_gpio')}
                margin="normal"
              />
            </Grid>
            <Grid item xs={4}>
              <TextValidator
                validators={[
                  'required',
                  'isNumber',
                  'minNumber:0',
                  'maxNumber:40',
                  'matchRegexp:^((?!6|7|8|9|10|11|12|14|15|20|24|28|29|30|31)[0-9]*)$'
                ]}
                errorMessages={[
                  'GPIO is required',
                  'Must be a number',
                  'Must be 0 or higher',
                  'Max value is 40',
                  'Not a valid GPIO'
                ]}
                name="pbutton_gpio"
                label="Button GPIO"
                fullWidth
                variant="outlined"
                value={data.pbutton_gpio}
                type="number"
                onChange={handleValueChange('pbutton_gpio')}
                margin="normal"
              />
            </Grid>
            <Grid item xs={4}>
              <TextValidator
                validators={[
                  'required',
                  'isNumber',
                  'minNumber:0',
                  'maxNumber:40',
                  'matchRegexp:^((?!6|7|8|9|10|11|12|14|15|20|24|28|29|30|31)[0-9]*)$'
                ]}
                errorMessages={[
                  'GPIO is required',
                  'Must be a number',
                  'Must be 0 or higher',
                  'Max value is 40',
                  'Not a valid GPIO'
                ]}
                name="dallas_gpio"
                label="Dallas GPIO (0=none)"
                fullWidth
                variant="outlined"
                value={data.dallas_gpio}
                type="number"
                onChange={handleValueChange('dallas_gpio')}
                margin="normal"
              />
            </Grid>
            <Grid item xs={4}>
              <TextValidator
                validators={[
                  'required',
                  'isNumber',
                  'minNumber:0',
                  'maxNumber:40',
                  'matchRegexp:^((?!6|7|8|9|10|11|12|14|15|20|24|28|29|30|31)[0-9]*)$'
                ]}
                errorMessages={[
                  'GPIO is required',
                  'Must be a number',
                  'Must be 0 or higher',
                  'Max value is 40',
                  'Not a valid GPIO'
                ]}
                name="led_gpio"
                label="LED GPIO (0=none)"
                fullWidth
                variant="outlined"
                value={data.led_gpio}
                type="number"
                onChange={handleValueChange('led_gpio')}
                margin="normal"
              />
            </Grid>
            <Grid item xs={4}>
              <TextValidator
                validators={[
                  'required',
                  'isNumber',
                  'minNumber:0',
                  'maxNumber:40',
                  'matchRegexp:^((?!6|7|8|9|10|11|12|14|15|20|24|28|29|30|31)[0-9]*)$'
                ]}
                errorMessages={[
                  'GPIO is required',
                  'Must be a number',
                  'Must be 0 or higher',
                  'Max value is 40',
                  'Not a valid GPIO'
                ]}
                name="aux_gpio"
                label="Aux port GPIO (0=none)"
                fullWidth
                variant="outlined"
                value={data.aux_gpio}
                type="number"
                onChange={handleValueChange('aux_gpio')}
                margin="normal"
              />
            </Grid>
          </Grid>
        )}

        <br></br>
        <Typography variant="h6" color="primary">
          General Options
        </Typography>

        {data.aux_gpio !== 0 && (
          <Grid
            container
            spacing={1}
            direction="row"
            justify="flex-start"
            alignItems="flex-start"
          >
            <Grid item xs={4}>
              <SelectValidator
                name="aux_function"
                value={data.aux_function}
                fullWidth
                variant="outlined"
                onChange={handleValueChange('aux_function')}
                margin="normal"
                label="Auxiliary port function"
              >
                <MenuItem value={0}>None</MenuItem>
                <MenuItem value={1}>Pool circulation pump</MenuItem>
              </SelectValidator>
            </Grid>
            {data.aux_function === 1 && (
              <Grid item xs={4}>
                <TextValidator
                  validators={[
                    'required',
                    'isNumber',
                    'minNumber:0',
                    'maxNumber:600'
                  ]}
                  errorMessages={[
                    'Value is required',
                    'Must be a number',
                    'Must be 0 or higher',
                    'Max value is 1800 (30 min)'
                  ]}
                  name="aux_pump_delay"
                  label="Pool pump stop delay (seconds)"
                  fullWidth
                  variant="outlined"
                  value={data.aux_pump_delay}
                  type="number"
                  onChange={handleValueChange('aux_pump_delay')}
                  margin="normal"
                />
              </Grid>
            )}
          </Grid>
        )}
        {data.led_gpio !== 0 && (
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={data.hide_led}
                onChange={handleValueChange('hide_led')}
                value="hide_led"
              />
            }
            label="Hide LED"
          />
        )}

        {data.dallas_gpio !== 0 && (
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={data.dallas_parasite}
                onChange={handleValueChange('dallas_parasite')}
                value="dallas_parasite"
              />
            }
            label="Use Dallas Sensor parasite power"
          />
        )}

        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.analog_enabled}
              onChange={handleValueChange('analog_enabled')}
              value="analog_enabled"
            />
          }
          label="Enable ADC"
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.low_clock}
              onChange={handleValueChange('low_clock')}
              value="low_clock"
            />
          }
          label="Run at a lower CPU clock speed"
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.notoken_api}
              onChange={handleValueChange('notoken_api')}
              value="notoken_api"
            />
          }
          label="Bypass Access Token authorization on API calls"
        />
        <Grid
          container
          spacing={0}
          direction="row"
          justify="flex-start"
          alignItems="flex-start"
        >
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={data.shower_timer}
                onChange={handleValueChange('shower_timer')}
                value="shower_timer"
              />
            }
            label="Enable Shower Timer"
          />
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={data.shower_alert}
                onChange={handleValueChange('shower_alert')}
                value="shower_alert"
              />
            }
            label="Enable Shower Alert"
          />
        </Grid>

        <br></br>
        <Typography variant="h6" color="primary">
          Formatting Options
        </Typography>

        <Grid
          container
          spacing={1}
          direction="row"
          justify="flex-start"
          alignItems="flex-start"
        >
          <Grid item xs={4}>
            <SelectValidator
              name="bool_format"
              label="Boolean Format"
              value={data.bool_format}
              fullWidth
              variant="outlined"
              onChange={handleValueChange('bool_format')}
              margin="normal"
            >
              <MenuItem value={1}>"on"/"off"</MenuItem>
              <MenuItem value={2}>"ON"/"OFF"</MenuItem>
              <MenuItem value={3}>true/false</MenuItem>
              <MenuItem value={4}>1/0</MenuItem>
            </SelectValidator>
          </Grid>
          <Grid item xs={4}>
            <SelectValidator
              name="enum_format"
              label="Enum Format"
              value={data.enum_format}
              fullWidth
              variant="outlined"
              onChange={handleValueChange('enum_format')}
              margin="normal"
            >
              <MenuItem value={1}>Text</MenuItem>
              <MenuItem value={2}>Number</MenuItem>
            </SelectValidator>
          </Grid>
          <Grid item xs={4}>
            <SelectValidator
              name="dallas_format"
              label="Dallas Sensor Format"
              value={data.dallas_format}
              fullWidth
              variant="outlined"
              onChange={handleValueChange('dallas_format')}
              margin="normal"
            >
              <MenuItem value={1}>ID</MenuItem>
              <MenuItem value={2}>Number</MenuItem>
              <MenuItem value={3}>Name</MenuItem>
            </SelectValidator>
          </Grid>
        </Grid>

        <br></br>
        <Typography variant="h6" color="primary">
          Syslog
        </Typography>

        <BlockFormControlLabel
          control={
            <Checkbox
              checked={data.syslog_enabled}
              onChange={handleValueChange('syslog_enabled')}
              value="syslog_enabled"
            />
          }
          label="Enable Syslog"
        />

        {data.syslog_enabled && (
          <Grid
            container
            spacing={1}
            direction="row"
            justify="flex-start"
            alignItems="flex-start"
          >
            <Grid item xs={5}>
              <TextValidator
                validators={['isOptionalIPorHost']}
                errorMessages={['Not a valid IPv4 address or hostname']}
                name="syslog_host"
                label="Host"
                fullWidth
                variant="outlined"
                value={data.syslog_host}
                onChange={handleValueChange('syslog_host')}
                margin="normal"
              />
            </Grid>
            <Grid item xs={6}>
              <TextValidator
                validators={[
                  'required',
                  'isNumber',
                  'minNumber:0',
                  'maxNumber:65535'
                ]}
                errorMessages={[
                  'Port is required',
                  'Must be a number',
                  'Must be greater than 0 ',
                  'Max value is 65535'
                ]}
                name="syslog_port"
                label="Port"
                fullWidth
                variant="outlined"
                value={data.syslog_port}
                type="number"
                onChange={handleValueChange('syslog_port')}
                margin="normal"
              />
            </Grid>
            <Grid item xs={5}>
              <SelectValidator
                name="syslog_level"
                label="Log Level"
                value={data.syslog_level}
                fullWidth
                variant="outlined"
                onChange={handleValueChange('syslog_level')}
                margin="normal"
              >
                <MenuItem value={-1}>OFF</MenuItem>
                <MenuItem value={3}>ERR</MenuItem>
                <MenuItem value={5}>NOTICE</MenuItem>
                <MenuItem value={6}>INFO</MenuItem>
                <MenuItem value={7}>DEBUG</MenuItem>
                <MenuItem value={8}>ALL</MenuItem>
              </SelectValidator>
            </Grid>
            <Grid item xs={6}>
              <TextValidator
                validators={[
                  'required',
                  'isNumber',
                  'minNumber:0',
                  'maxNumber:65535'
                ]}
                errorMessages={[
                  'Syslog Mark is required',
                  'Must be a number',
                  'Must be 0 or higher',
                  'Max value is 10'
                ]}
                name="syslog_mark_interval"
                label="Mark Interval (seconds, 0=off)"
                fullWidth
                variant="outlined"
                value={data.syslog_mark_interval}
                type="number"
                onChange={handleValueChange('syslog_mark_interval')}
                margin="normal"
              />
            </Grid>
            <BlockFormControlLabel
              control={
                <Checkbox
                  checked={data.trace_raw}
                  onChange={handleValueChange('trace_raw')}
                  value="trace_raw"
                />
              }
              label="Output EMS telegrams as hexadecimal bytes"
            />
          </Grid>
        )}

        <br></br>
        <FormActions>
          <FormButton
            startIcon={<SaveIcon />}
            variant="contained"
            color="primary"
            type="submit"
          >
            Save
          </FormButton>
        </FormActions>
      </ValidatorForm>
    );
  }
}

export default withAuthenticatedContext(withWidth()(EMSESPSettingsForm));
