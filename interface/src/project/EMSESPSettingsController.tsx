import { FC } from 'react';

import {
  ValidatorForm,
  TextValidator,
  SelectValidator
} from 'react-material-ui-form-validator';

import { Checkbox, Typography, Box, Link, Grid } from '@material-ui/core';

import MenuItem from '@material-ui/core/MenuItem';
import SaveIcon from '@material-ui/icons/Save';

import {
  FormActions,
  FormButton,
  SectionContent,
  BlockFormControlLabel,
  FormLoader
} from '../components';

import { useAuthorizedRest } from '../hooks';
import { updateValue } from '../utils';

import { EMSESPSettings } from './EMSESPtypes';

import { ENDPOINT_ROOT } from '../api';
export const EMSESP_SETTINGS_ENDPOINT = ENDPOINT_ROOT + 'emsespSettings';
export const BOARD_PROFILE_ENDPOINT = ENDPOINT_ROOT + 'boardProfile';

import { isIPv4, optional, isHostname, or } from '../validators';

import { boardProfileSelectItems } from './EMSESPBoardProfiles';

const EMSESPSettingsController: FC = () => {
  const { loadData, saveData, saving, setData, data, errorMessage } =
    useAuthorizedRest<EMSESPSettings>({ endpoint: EMSESP_SETTINGS_ENDPOINT });

  const updateFormValue = updateValue(setData);

  ValidatorForm.addValidationRule(
    'isOptionalIPorHost',
    optional(or(isIPv4, isHostname))
  );

  const content = () => {
    if (!data) {
      return <FormLoader loadData={loadData} errorMessage={errorMessage} />;
    }

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
          justifyContent="flex-start"
          alignItems="flex-start"
        >
          <Grid item xs={5}>
            <SelectValidator
              name="tx_mode"
              disabled={saving}
              label="Tx Mode"
              value={data.tx_mode}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
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
              disabled={saving}
              label="Bus ID"
              value={data.ems_bus_id}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
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
          disabled={saving}
          label="Board Profile"
          value={data.board_profile}
          fullWidth
          variant="outlined"
          onChange={updateFormValue}
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
            justifyContent="flex-start"
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
                disabled={saving}
                label="Rx GPIO"
                fullWidth
                variant="outlined"
                value={data.rx_gpio}
                type="number"
                onChange={updateFormValue}
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
                disabled={saving}
                label="Tx GPIO"
                fullWidth
                variant="outlined"
                value={data.tx_gpio}
                type="number"
                onChange={updateFormValue}
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
                disabled={saving}
                label="Button GPIO"
                fullWidth
                variant="outlined"
                value={data.pbutton_gpio}
                type="number"
                onChange={updateFormValue}
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
                disabled={saving}
                label="Dallas GPIO (0=none)"
                fullWidth
                variant="outlined"
                value={data.dallas_gpio}
                type="number"
                onChange={updateFormValue}
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
                disabled={saving}
                label="LED GPIO (0=none)"
                fullWidth
                variant="outlined"
                value={data.led_gpio}
                type="number"
                onChange={updateFormValue}
                margin="normal"
              />
            </Grid>
          </Grid>
        )}

        <br></br>
        <Typography variant="h6" color="primary">
          General Options
        </Typography>

        {data.led_gpio !== 0 && (
          <BlockFormControlLabel
            control={
              <Checkbox
                name="hide_led"
                checked={data.hide_led}
                disabled={saving}
                onChange={updateFormValue}
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
                name="dallas_parasite"
                checked={data.dallas_parasite}
                disabled={saving}
                onChange={updateFormValue}
                value="dallas_parasite"
              />
            }
            label="Use Dallas Sensor parasite power"
          />
        )}

        <BlockFormControlLabel
          control={
            <Checkbox
              name="analog_enabled"
              disabled={saving}
              checked={data.analog_enabled}
              onChange={updateFormValue}
              color="primary"
            />
          }
          label="Enable ADC"
        />

        <BlockFormControlLabel
          control={
            <Checkbox
              name="low_clock"
              disabled={saving}
              checked={data.low_clock}
              onChange={updateFormValue}
              value="low_clock"
            />
          }
          label="Run at a lower CPU clock speed"
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              name="notoken_api"
              disabled={saving}
              checked={data.notoken_api}
              onChange={updateFormValue}
              value="notoken_api"
            />
          }
          label="Bypass Access Token authorization on API calls"
        />

        <Grid
          container
          spacing={0}
          direction="row"
          justifyContent="flex-start"
          alignItems="flex-start"
        >
          <BlockFormControlLabel
            control={
              <Checkbox
                name="shower_timer"
                disabled={saving}
                checked={data.shower_timer}
                onChange={updateFormValue}
                value="shower_timer"
              />
            }
            label="Enable Shower Timer"
          />
          <BlockFormControlLabel
            control={
              <Checkbox
                name="shower_alert"
                disabled={saving}
                checked={data.shower_alert}
                onChange={updateFormValue}
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
          justifyContent="flex-start"
          alignItems="flex-start"
        >
          <Grid item xs={4}>
            <SelectValidator
              name="bool_format"
              disabled={saving}
              label="Boolean Format"
              value={data.bool_format}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
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
              disabled={saving}
              label="Enum Format"
              value={data.enum_format}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
            >
              <MenuItem value={1}>Text</MenuItem>
              <MenuItem value={2}>Number</MenuItem>
            </SelectValidator>
          </Grid>
          <Grid item xs={4}>
            <SelectValidator
              name="dallas_format"
              disabled={saving}
              label="Dallas Sensor Format"
              value={data.dallas_format}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
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
              name="syslog_enabled"
              disabled={saving}
              checked={data.syslog_enabled}
              onChange={updateFormValue}
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
            justifyContent="flex-start"
            alignItems="flex-start"
          >
            <Grid item xs={5}>
              <TextValidator
                disabled={saving}
                validators={['isOptionalIPorHost']}
                errorMessages={['Not a valid IPv4 address or hostname']}
                name="syslog_host"
                label="Host"
                fullWidth
                variant="outlined"
                value={data.syslog_host}
                onChange={updateFormValue}
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
                disabled={saving}
                label="Port"
                fullWidth
                variant="outlined"
                value={data.syslog_port}
                type="number"
                onChange={updateFormValue}
                margin="normal"
              />
            </Grid>
            <Grid item xs={5}>
              <SelectValidator
                name="syslog_level"
                disabled={saving}
                label="Log Level"
                value={data.syslog_level}
                fullWidth
                variant="outlined"
                onChange={updateFormValue}
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
                disabled={saving}
                label="Mark Interval (seconds, 0=off)"
                fullWidth
                variant="outlined"
                value={data.syslog_mark_interval}
                type="number"
                onChange={updateFormValue}
                margin="normal"
              />
            </Grid>

            <BlockFormControlLabel
              control={
                <Checkbox
                  name="trace_raw"
                  disabled={saving}
                  checked={data.trace_raw}
                  onChange={updateFormValue}
                  value="trace_raw"
                />
              }
              label="Output EMS telegrams as hexadecimal bytes (raw)"
            />
          </Grid>
        )}

        <FormActions>
          <FormButton
            startIcon={<SaveIcon />}
            disabled={saving}
            variant="contained"
            color="primary"
            type="submit"
          >
            Save
          </FormButton>
        </FormActions>
      </ValidatorForm>
    );
  };

  return (
    <SectionContent title="REST Controller" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default EMSESPSettingsController;

/*

type EMSESPSettingsControllerProps = RestControllerProps<EMSESPSettings>;

class EMSESPSettingsController extends Component<EMSESPSettingsControllerProps> {
  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="" titleGutter>
        <RestFormLoader
          {...this.props}
          render={(formProps) => <EMSESPSettingsForm {...formProps} />}
        />
      </SectionContent>
    );
  }
}

export default restController(
  EMSESP_SETTINGS_ENDPOINT,
  EMSESPSettingsController
);

*/
