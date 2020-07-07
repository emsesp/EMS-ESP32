import React, { Component, Fragment } from 'react';
import { ValidatorForm, TextValidator, SelectValidator } from 'react-material-ui-form-validator';

import { Checkbox, Typography, Box } from '@material-ui/core';
import SaveIcon from '@material-ui/icons/Save';
import MenuItem from '@material-ui/core/MenuItem';

import { ENDPOINT_ROOT } from '../api';
import { restController, RestControllerProps, RestFormLoader, RestFormProps, FormActions, FormButton, BlockFormControlLabel, SectionContent } from '../components';

import { isIP, isHostname, or } from '../validators';

import { EMSESPSettings } from './types';

export const EMSESP_SETTINGS_ENDPOINT = ENDPOINT_ROOT + "emsespSettings";

type EMSESPSettingsControllerProps = RestControllerProps<EMSESPSettings>;

class EMSESPSettingsController extends Component<EMSESPSettingsControllerProps> {

    componentDidMount() {
        ValidatorForm.addValidationRule('isIPOrHostname', or(isIP, isHostname));
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
    const { data, saveData, loadData, handleValueChange } = props;
    return (
        <ValidatorForm onSubmit={saveData}>
            <Box bgcolor="primary.main" color="primary.contrastText" p={2} mt={2} mb={2}>
                <Typography variant="body1">
                    Customize EMS-ESP by editing the default settings here. Refer to the <a href="https://emsesp.github.io/docs/#/">Wiki</a> for assistance.
                </Typography>
            </Box>
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
                errorMessages={['Keep alive is required', "Must be a number", "Must be greater than 0", "Max value is 65535"]}
                name="publish_time"
                label="MQTT Publish Time (seconds)"
                fullWidth
                variant="outlined"
                value={data.publish_time}
                type="number"
                onChange={handleValueChange('publish_time')}
                margin="normal"
            />
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
            {data.syslog_level !== -1 &&
                <Fragment>
                    <TextValidator
                        validators={['isIPOrHostname']}
                        errorMessages={["Not a valid IP address or hostname"]}
                        name="syslog_host"
                        label="Syslog IP/Host"
                        fullWidth
                        variant="outlined"
                        value={data.syslog_host}
                        onChange={handleValueChange('syslog_host')}
                        margin="normal"
                    />
                    <TextValidator
                        validators={['required', 'isNumber', 'minNumber:0', 'maxNumber:65535']}
                        errorMessages={['Keep alive is required', "Must be a number", "Must be 0 or higher (0=off)", "Max value is 65535"]}
                        name="syslog_mark_interval"
                        label="Syslog Mark Interval (seconds, 0=off)"
                        fullWidth
                        variant="outlined"
                        value={data.syslog_mark_interval}
                        type="number"
                        onChange={handleValueChange('syslog_mark_interval')}
                        margin="normal"
                    />
                </Fragment>
            }
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
