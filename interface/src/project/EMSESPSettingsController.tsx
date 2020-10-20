import React, { Component } from 'react';
import { ValidatorForm, TextValidator, SelectValidator } from 'react-material-ui-form-validator';

import { Checkbox, Typography, Box, Link } from '@material-ui/core';
import SaveIcon from '@material-ui/icons/Save';
import MenuItem from '@material-ui/core/MenuItem';

import { ENDPOINT_ROOT } from '../api';
import { restController, RestControllerProps, RestFormLoader, RestFormProps, FormActions, FormButton, BlockFormControlLabel, SectionContent } from '../components';

import { isIP, isHostname, or, optional } from '../validators';

import { EMSESPSettings } from './EMSESPtypes';

export const EMSESP_SETTINGS_ENDPOINT = ENDPOINT_ROOT + "emsespSettings";

type EMSESPSettingsControllerProps = RestControllerProps<EMSESPSettings>;

class EMSESPSettingsController extends Component<EMSESPSettingsControllerProps> {

    componentDidMount() {
        ValidatorForm.addValidationRule('isIPOrHostname', optional(or(isIP, isHostname)));
        this.props.loadData();
    }

    render() {
        return (
            <SectionContent title='EMS-ESP Settings' titleGutter>
                <RestFormLoader
                    {...this.props}
                    render={props => (
                        <EMSESPSettingsControllerForm {...props} />
                    )}
                />
            </SectionContent>
        )
    }

}

export default restController(EMSESP_SETTINGS_ENDPOINT, EMSESPSettingsController);

type EMSESPSettingsControllerFormProps = RestFormProps<EMSESPSettings>;

function EMSESPSettingsControllerForm(props: EMSESPSettingsControllerFormProps) {
    const { data, saveData, handleValueChange } = props;
    return (
        <ValidatorForm onSubmit={saveData}>
            <Box bgcolor="info.main" p={2} mt={2} mb={2}>
                <Typography variant="body1">
                    Change the default settings on this page. For help click <Link target="_blank" href="https://emsesp.github.io/docs/#/Configure-firmware?id=settings" color="primary">{'here'}</Link>.
                </Typography>
            </Box>
            <br></br>
            <Typography variant="h6" color="primary" >
                EMS Bus
            </Typography>
            <TextValidator
                validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:255']}
                errorMessages={['TX mode is required', "Must be a number", "Must be 0 or higher", "Max value is 255"]}
                name="tx_mode"
                label="Tx Mode (0=off)"
                fullWidth
                variant="outlined"
                value={data.tx_mode}
                type="number"
                onChange={handleValueChange('tx_mode')}
                margin="normal"
            />
            <SelectValidator name="ems_bus_id"
                label="Bus ID"
                value={data.ems_bus_id}
                fullWidth
                variant="outlined"
                onChange={handleValueChange('ems_bus_id')}
                margin="normal">
                <MenuItem value={0x0B}>Service Key (0x0B)</MenuItem>
                <MenuItem value={0x0D}>Modem (0x0D)</MenuItem>
                <MenuItem value={0x0A}>Terminal (0x0A)</MenuItem>
                <MenuItem value={0x0F}>Time Module (0x0F)</MenuItem>
                <MenuItem value={0x12}>Alarm Module (0x12)</MenuItem>
            </SelectValidator>
            <TextValidator
                validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:40']}
                errorMessages={['Rx GPIO is required', "Must be a number", "Must be 0 or higher", "Max value is 255"]}
                name="rx_gpio"
                label="Rx GPIO pin"
                fullWidth
                variant="outlined"
                value={data.rx_gpio}
                type="number"
                onChange={handleValueChange('rx_gpio')}
                margin="normal"
            />
            <TextValidator
                validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:40']}
                errorMessages={['Tx GPIO is required', "Must be a number", "Must be 0 or higher", "Max value is 255"]}
                name="tx_gpio"
                label="Tx GPIO pin"
                fullWidth
                variant="outlined"
                value={data.tx_gpio}
                type="number"
                onChange={handleValueChange('tx_gpio')}
                margin="normal"
            />
            <br></br>
            <Typography variant="h6" color="primary" >
                Dallas Sensor
            </Typography>
            <TextValidator
                validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:40']}
                errorMessages={['Dallas GPIO is required', "Must be a number", "Must be 0 or higher", "Max value is 255"]}
                name="dallas_gpio"
                label="Dallas GPIO pin (0=none)"
                fullWidth
                variant="outlined"
                value={data.dallas_gpio}
                type="number"
                onChange={handleValueChange('dallas_gpio')}
                margin="normal"
            />
            <BlockFormControlLabel
                control={
                    <Checkbox
                        checked={data.dallas_parasite}
                        onChange={handleValueChange('dallas_parasite')}
                        value="dallas_parasite"
                    />
                }
                label="Dallas Parasite Mode"
            />
            <br></br>
            <Typography variant="h6" color="primary" >
                LED
            </Typography>
            <TextValidator
                validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:40']}
                errorMessages={['LED GPIO is required', "Must be a number", "Must be 0 or higher", "Max value is 255"]}
                name="led_gpio"
                label="LED GPIO pin (0=none)"
                fullWidth
                variant="outlined"
                value={data.led_gpio}
                type="number"
                onChange={handleValueChange('led_gpio')}
                margin="normal"
            />
            <BlockFormControlLabel
                control={
                    <Checkbox
                        checked={data.hide_led}
                        onChange={handleValueChange('hide_led')}
                        value="hide_led"
                    />
                }
                label="Invert/Hide LED"
            />
            <br></br>
            <Typography variant="h6" color="primary" >
                Shower
            </Typography>
            <BlockFormControlLabel
                control={
                    <Checkbox
                        checked={data.shower_timer}
                        onChange={handleValueChange('shower_timer')}
                        value="shower_timer"
                    />
                }
                label="Shower Timer"
            />
            <BlockFormControlLabel
                control={
                    <Checkbox
                        checked={data.shower_alert}
                        onChange={handleValueChange('shower_alert')}
                        value="shower_alert"
                    />
                }
                label="Shower Alert"
            />
            <br></br>
            <Typography variant="h6" color="primary" >
                API
            </Typography>
            <BlockFormControlLabel
                control={
                    <Checkbox
                        checked={data.api_enabled}
                        onChange={handleValueChange('api_enabled')}
                        value="api_enabled"
                    />
                }
                label="Enable WEB API (for write commands)"
            />
            <SelectValidator name="bool_format"
                label="Boolean Format"
                value={data.bool_format}
                fullWidth
                variant="outlined"
                onChange={handleValueChange('bool_format')}
                margin="normal">
                <MenuItem value={1}>on/off</MenuItem>
                <MenuItem value={2}>true/false</MenuItem>
                <MenuItem value={3}>1/0</MenuItem>
            </SelectValidator>
            <br></br>
            <Typography variant="h6" color="primary" >
                Syslog
            </Typography>
            <TextValidator
                validators={['isIPOrHostname']}
                errorMessages={["Not a valid IP address or hostname"]}
                name="syslog_host"
                label="Syslog IP/Host (optional)"
                fullWidth
                variant="outlined"
                value={data.syslog_host}
                onChange={handleValueChange('syslog_host')}
                margin="normal"
            />
            <SelectValidator name="syslog_level"
                label="Syslog Log Level"
                value={data.syslog_level}
                fullWidth
                variant="outlined"
                onChange={handleValueChange('syslog_level')}
                margin="normal">
                <MenuItem value={-1}>OFF</MenuItem>
                <MenuItem value={3}>ERR</MenuItem>
                <MenuItem value={6}>INFO</MenuItem>
                <MenuItem value={7}>DEBUG</MenuItem>
            </SelectValidator>
            <TextValidator
                validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
                errorMessages={['Syslog Mark is required', "Must be a number", "Must be 0 or higher (0=off)", "Max value is 65535"]}
                name="syslog_mark_interval"
                label="Syslog Mark Interval (seconds, 0=off)"
                fullWidth
                variant="outlined"
                value={data.syslog_mark_interval}
                type="number"
                onChange={handleValueChange('syslog_mark_interval')}
                margin="normal"
            />
            <br></br>
            <Typography variant="h6" color="primary" >
                Analog Input
            </Typography>
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
            <br></br>
            <FormActions>
                <FormButton startIcon={<SaveIcon />} variant="contained" color="primary" type="submit">
                    Save
                </FormButton>
            </FormActions>
        </ValidatorForm>
    );
}
