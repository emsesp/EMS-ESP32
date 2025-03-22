import { useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Button,
  Checkbox,
  Grid2 as Grid,
  InputAdornment,
  MenuItem,
  TextField,
  Typography
} from '@mui/material';

import * as MqttApi from 'api/mqtt';

import type { ValidateFieldsError } from 'async-validator';
import {
  BlockFormControlLabel,
  BlockNavigation,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedPasswordField,
  ValidatedTextField,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { MqttSettingsType } from 'types';
import { numberValue, updateValueDirty, useRest } from 'utils';
import { createMqttSettingsValidator, validate } from 'validators';

const MqttSettings = () => {
  const {
    loadData,
    saving,
    data,
    updateDataValue,
    origData,
    dirtyFlags,
    setDirtyFlags,
    blocker,
    saveData,
    errorMessage
  } = useRest<MqttSettingsType>({
    read: MqttApi.readMqttSettings,
    update: MqttApi.updateMqttSettings
  });

  const { LL } = useI18nContext();
  useLayoutTitle('MQTT');

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValueDirty(
    origData,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue
  );

  const SecondsInputProps = {
    endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(createMqttSettingsValidator(data), data);
        await saveData();
      } catch (error) {
        setFieldErrors(error as ValidateFieldsError);
      }
    };

    return (
      <>
        <BlockFormControlLabel
          control={
            <Checkbox
              name="enabled"
              checked={data.enabled}
              onChange={updateFormValue}
            />
          }
          label={LL.ENABLE_MQTT()}
        />
        <Grid container spacing={2} rowSpacing={0}>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="host"
              label={LL.ADDRESS_OF(LL.BROKER())}
              multiline
              variant="outlined"
              value={data.host}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="port"
              label="Port"
              variant="outlined"
              value={numberValue(data.port)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="base"
              label={LL.BASE_TOPIC()}
              variant="outlined"
              value={data.base}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <TextField
              name="client_id"
              label={LL.ID_OF(LL.CLIENT()) + ' (' + LL.OPTIONAL() + ')'}
              variant="outlined"
              value={data.client_id}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <TextField
              name="username"
              label={LL.USERNAME(0)}
              variant="outlined"
              value={data.username}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedPasswordField
              name="password"
              label={LL.PASSWORD()}
              variant="outlined"
              value={data.password}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="keep_alive"
              label="Keep Alive"
              slotProps={{
                input: SecondsInputProps
              }}
              variant="outlined"
              value={numberValue(data.keep_alive)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <TextField
              name="mqtt_qos"
              label="QoS"
              value={data.mqtt_qos}
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={0}>0</MenuItem>
              <MenuItem value={1}>1</MenuItem>
              <MenuItem value={2}>2</MenuItem>
            </TextField>
          </Grid>
        </Grid>
        {data.enableTLS !== undefined && (
          <BlockFormControlLabel
            control={
              <Checkbox
                name="enableTLS"
                checked={data.enableTLS}
                onChange={updateFormValue}
              />
            }
            label={LL.ENABLE_TLS()}
          />
        )}
        {data.enableTLS === true && (
          <ValidatedPasswordField
            name="rootCA"
            label={LL.CERT()}
            variant="outlined"
            value={data.rootCA}
            onChange={updateFormValue}
            margin="normal"
          />
        )}
        <BlockFormControlLabel
          control={
            <Checkbox
              name="clean_session"
              checked={data.clean_session}
              onChange={updateFormValue}
            />
          }
          label={LL.MQTT_CLEAN_SESSION()}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              name="mqtt_retain"
              checked={data.mqtt_retain}
              onChange={updateFormValue}
            />
          }
          label={LL.MQTT_RETAIN_FLAG()}
        />
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.FORMATTING()}
        </Typography>
        <TextField
          name="nested_format"
          label={LL.MQTT_FORMAT()}
          value={data.nested_format}
          variant="outlined"
          onChange={updateFormValue}
          margin="normal"
          select
        >
          <MenuItem value={1}>{LL.MQTT_NEST_1()}</MenuItem>
          <MenuItem value={2}>{LL.MQTT_NEST_2()}</MenuItem>
        </TextField>
        <BlockFormControlLabel
          control={
            <Checkbox
              name="send_response"
              checked={data.send_response}
              onChange={updateFormValue}
            />
          }
          label={LL.MQTT_RESPONSE()}
        />
        {!data.ha_enabled && (
          <Grid container spacing={2} rowSpacing={0}>
            <Grid>
              <BlockFormControlLabel
                control={
                  <Checkbox
                    name="publish_single"
                    checked={data.publish_single}
                    onChange={updateFormValue}
                  />
                }
                label={LL.MQTT_PUBLISH_TEXT_1()}
              />
            </Grid>
            {data.publish_single && (
              <Grid>
                <BlockFormControlLabel
                  control={
                    <Checkbox
                      name="publish_single2cmd"
                      checked={data.publish_single2cmd}
                      onChange={updateFormValue}
                    />
                  }
                  label={LL.MQTT_PUBLISH_TEXT_2()}
                />
              </Grid>
            )}
          </Grid>
        )}
        {!data.publish_single && (
          <Grid container spacing={2} rowSpacing={0}>
            <Grid>
              <BlockFormControlLabel
                control={
                  <Checkbox
                    name="ha_enabled"
                    checked={data.ha_enabled}
                    onChange={updateFormValue}
                  />
                }
                label={LL.MQTT_PUBLISH_TEXT_3()}
              />
            </Grid>
            {data.ha_enabled && (
              <Grid container spacing={2} rowSpacing={0}>
                <Grid>
                  <TextField
                    name="discovery_type"
                    label={LL.MQTT_PUBLISH_TEXT_5()}
                    value={data.discovery_type}
                    variant="outlined"
                    onChange={updateFormValue}
                    margin="normal"
                    select
                  >
                    <MenuItem value={0}>Home Assistant</MenuItem>
                    <MenuItem value={1}>Domoticz</MenuItem>
                    <MenuItem value={2}>Domoticz (latest)</MenuItem>
                  </TextField>
                </Grid>
                <Grid>
                  <TextField
                    name="discovery_prefix"
                    label={LL.MQTT_PUBLISH_TEXT_4()}
                    variant="outlined"
                    value={data.discovery_prefix}
                    onChange={updateFormValue}
                    margin="normal"
                  />
                </Grid>
                <Grid>
                  <TextField
                    name="entity_format"
                    label={LL.MQTT_ENTITY_FORMAT()}
                    value={data.entity_format}
                    variant="outlined"
                    onChange={updateFormValue}
                    margin="normal"
                    select
                  >
                    <MenuItem value={0}>{LL.MQTT_ENTITY_FORMAT_0()}</MenuItem>
                    <MenuItem value={3}>
                      {LL.MQTT_ENTITY_FORMAT_1()}&nbsp;(v3.6)
                    </MenuItem>
                    <MenuItem value={4}>
                      {LL.MQTT_ENTITY_FORMAT_2()}&nbsp;(v3.6)
                    </MenuItem>
                    <MenuItem value={1}>{LL.MQTT_ENTITY_FORMAT_1()}</MenuItem>
                    <MenuItem value={2}>{LL.MQTT_ENTITY_FORMAT_2()}</MenuItem>
                  </TextField>
                </Grid>
              </Grid>
            )}
          </Grid>
        )}
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.MQTT_PUBLISH_INTERVALS()}&nbsp;(0=auto)
        </Typography>
        <Grid container spacing={2} rowSpacing={0}>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="publish_time_heartbeat"
              label="Heartbeat"
              slotProps={{
                input: SecondsInputProps
              }}
              variant="outlined"
              value={numberValue(data.publish_time_heartbeat)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid>
            <TextField
              name="publish_time_boiler"
              label={LL.MQTT_INT_BOILER()}
              variant="outlined"
              value={numberValue(data.publish_time_boiler)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
              slotProps={{
                input: SecondsInputProps
              }}
            />
          </Grid>
          <Grid>
            <TextField
              name="publish_time_thermostat"
              label={LL.MQTT_INT_THERMOSTATS()}
              variant="outlined"
              value={numberValue(data.publish_time_thermostat)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
              slotProps={{
                input: SecondsInputProps
              }}
            />
          </Grid>
          <Grid>
            <TextField
              name="publish_time_solar"
              label={LL.MQTT_INT_SOLAR()}
              variant="outlined"
              value={numberValue(data.publish_time_solar)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
              slotProps={{
                input: SecondsInputProps
              }}
            />
          </Grid>
          <Grid>
            <TextField
              name="publish_time_mixer"
              label={LL.MQTT_INT_MIXER()}
              variant="outlined"
              value={numberValue(data.publish_time_mixer)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
              slotProps={{
                input: SecondsInputProps
              }}
            />
          </Grid>
          <Grid>
            <TextField
              name="publish_time_water"
              label={LL.MQTT_INT_WATER()}
              variant="outlined"
              value={numberValue(data.publish_time_water)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
              slotProps={{
                input: SecondsInputProps
              }}
            />
          </Grid>
          <Grid>
            <TextField
              name="publish_time_sensor"
              label={LL.TEMP_SENSORS()}
              variant="outlined"
              value={numberValue(data.publish_time_sensor)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
              slotProps={{
                input: SecondsInputProps
              }}
            />
          </Grid>
          <Grid>
            <TextField
              name="publish_time_other"
              label={LL.DEFAULT(0)}
              variant="outlined"
              value={numberValue(data.publish_time_other)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
              slotProps={{
                input: SecondsInputProps
              }}
            />
          </Grid>
        </Grid>
        {dirtyFlags && dirtyFlags.length !== 0 && (
          <ButtonRow>
            <Button
              startIcon={<CancelIcon />}
              disabled={saving}
              variant="outlined"
              color="secondary"
              type="submit"
              onClick={loadData}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<WarningIcon color="warning" />}
              disabled={saving}
              variant="contained"
              color="info"
              type="submit"
              onClick={validateAndSubmit}
            >
              {LL.APPLY_CHANGES(dirtyFlags.length)}
            </Button>
          </ButtonRow>
        )}
      </>
    );
  };

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {content()}
    </SectionContent>
  );
};

export default MqttSettings;
