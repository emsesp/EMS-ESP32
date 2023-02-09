import { FC, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';

import { Button, Checkbox, MenuItem, Grid, Typography, InputAdornment } from '@mui/material';

import WarningIcon from '@mui/icons-material/Warning';
import CancelIcon from '@mui/icons-material/Cancel';

import { createMqttSettingsValidator, validate } from 'validators';
import {
  BlockFormControlLabel,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedPasswordField,
  ValidatedTextField,
  BlockNavigation
} from 'components';
import { MqttSettings } from 'types';
import { numberValue, updateValueDirty, useRest } from 'utils';
import * as MqttApi from 'api/mqtt';

import { useI18nContext } from 'i18n/i18n-react';

const MqttSettingsForm: FC = () => {
  const { loadData, saving, data, setData, origData, dirtyFlags, setDirtyFlags, blocker, saveData, errorMessage } =
    useRest<MqttSettings>({
      read: MqttApi.readMqttSettings,
      update: MqttApi.updateMqttSettings
    });

  const { LL } = useI18nContext();

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValueDirty(origData, dirtyFlags, setDirtyFlags, setData);

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(createMqttSettingsValidator(data), data);
        saveData();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    };

    return (
      <>
        <BlockFormControlLabel
          control={<Checkbox name="enabled" checked={data.enabled} onChange={updateFormValue} />}
          label={LL.ENABLE_MQTT()}
        />
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={6}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="host"
              label={LL.ADDRESS_OF(LL.BROKER())}
              fullWidth
              variant="outlined"
              value={data.host}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="port"
              label="Port"
              fullWidth
              variant="outlined"
              value={numberValue(data.port)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
        </Grid>
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={6}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="base"
              label={LL.BASE_TOPIC()}
              fullWidth
              variant="outlined"
              value={data.base}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6}>
            <ValidatedTextField
              name="client_id"
              label={LL.ID_OF(LL.CLIENT()) + ' (' + LL.OPTIONAL() + ')'}
              fullWidth
              variant="outlined"
              value={data.client_id}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
        </Grid>
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={6}>
            <ValidatedTextField
              name="username"
              label={LL.USERNAME(0)}
              fullWidth
              variant="outlined"
              value={data.username}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6}>
            <ValidatedPasswordField
              name="password"
              label={LL.PASSWORD()}
              fullWidth
              variant="outlined"
              value={data.password}
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
        </Grid>
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={6}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="keep_alive"
              label="Keep Alive"
              InputProps={{
                endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
              }}
              fullWidth
              variant="outlined"
              value={numberValue(data.keep_alive)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6}>
            <ValidatedTextField
              name="mqtt_qos"
              label="QoS"
              value={data.mqtt_qos}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={0}>0</MenuItem>
              <MenuItem value={1}>1</MenuItem>
              <MenuItem value={2}>2</MenuItem>
            </ValidatedTextField>
          </Grid>
        </Grid>
        <BlockFormControlLabel
          control={<Checkbox name="clean_session" checked={data.clean_session} onChange={updateFormValue} />}
          label={LL.MQTT_CLEAN_SESSION()}
        />
        <BlockFormControlLabel
          control={<Checkbox name="mqtt_retain" checked={data.mqtt_retain} onChange={updateFormValue} />}
          label={LL.MQTT_RETAIN_FLAG()}
        />

        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.FORMATTING()}
        </Typography>
        <ValidatedTextField
          name="nested_format"
          label={LL.MQTT_FORMAT()}
          value={data.nested_format}
          fullWidth
          variant="outlined"
          onChange={updateFormValue}
          margin="normal"
          select
        >
          <MenuItem value={1}>{LL.MQTT_NEST_1()}</MenuItem>
          <MenuItem value={2}>{LL.MQTT_NEST_2()}</MenuItem>
        </ValidatedTextField>
        <BlockFormControlLabel
          control={<Checkbox name="send_response" checked={data.send_response} onChange={updateFormValue} />}
          label={LL.MQTT_RESPONSE()}
        />
        {!data.ha_enabled && (
          <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
            <Grid item>
              <BlockFormControlLabel
                control={<Checkbox name="publish_single" checked={data.publish_single} onChange={updateFormValue} />}
                label={LL.MQTT_PUBLISH_TEXT_1()}
              />
            </Grid>
            {data.publish_single && (
              <Grid item>
                <BlockFormControlLabel
                  control={
                    <Checkbox name="publish_single2cmd" checked={data.publish_single2cmd} onChange={updateFormValue} />
                  }
                  label={LL.MQTT_PUBLISH_TEXT_2()}
                />
              </Grid>
            )}
          </Grid>
        )}
        {!data.publish_single && (
          <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
            <Grid item>
              <BlockFormControlLabel
                sx={{ pb: 1 }}
                control={<Checkbox name="ha_enabled" checked={data.ha_enabled} onChange={updateFormValue} />}
                label={LL.MQTT_PUBLISH_TEXT_3()}
              />
            </Grid>
            {data.ha_enabled && (
              <>
                <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
                  <Grid item>
                    <ValidatedTextField
                      name="discovery_prefix"
                      label={LL.MQTT_PUBLISH_TEXT_4()}
                      fullWidth
                      variant="outlined"
                      value={data.discovery_prefix}
                      onChange={updateFormValue}
                      margin="normal"
                    />
                  </Grid>
                  <Grid item>
                    <ValidatedTextField
                      name="entity_format"
                      label={LL.MQTT_ENTITY_FORMAT()}
                      value={data.entity_format}
                      fullWidth
                      variant="outlined"
                      onChange={updateFormValue}
                      margin="normal"
                      select
                    >
                      <MenuItem value={0}>{LL.MQTT_ENTITY_FORMAT_0()}</MenuItem>
                      <MenuItem value={1}>{LL.MQTT_ENTITY_FORMAT_1()}</MenuItem>
                      <MenuItem value={2}>{LL.MQTT_ENTITY_FORMAT_2()}</MenuItem>
                    </ValidatedTextField>
                  </Grid>
                </Grid>
              </>
            )}
          </Grid>
        )}
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.MQTT_PUBLISH_INTERVALS()}&nbsp;(0=auto)
        </Typography>
        <Grid container spacing={1} direction="row" justifyContent="flex-start" alignItems="flex-start">
          <Grid item xs={6} sm={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="publish_time_heartbeat"
              label={LL.MQTT_INT_HEARTBEAT()}
              InputProps={{
                endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
              }}
              fullWidth
              variant="outlined"
              value={numberValue(data.publish_time_heartbeat)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6} sm={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="publish_time_boiler"
              label={LL.MQTT_INT_BOILER()}
              InputProps={{
                endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
              }}
              fullWidth
              variant="outlined"
              value={numberValue(data.publish_time_boiler)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6} sm={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="publish_time_thermostat"
              label={LL.MQTT_INT_THERMOSTATS()}
              InputProps={{
                endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
              }}
              fullWidth
              variant="outlined"
              value={numberValue(data.publish_time_thermostat)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6} sm={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="publish_time_solar"
              label={LL.MQTT_INT_SOLAR()}
              InputProps={{
                endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
              }}
              fullWidth
              variant="outlined"
              value={numberValue(data.publish_time_solar)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6} sm={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="publish_time_mixer"
              label={LL.MQTT_INT_MIXER()}
              InputProps={{
                endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
              }}
              fullWidth
              variant="outlined"
              value={numberValue(data.publish_time_mixer)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6} sm={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="publish_time_sensor"
              label={LL.TEMP_SENSORS()}
              InputProps={{
                endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
              }}
              fullWidth
              variant="outlined"
              value={numberValue(data.publish_time_sensor)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
          <Grid item xs={6} sm={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="publish_time_other"
              InputProps={{
                endAdornment: <InputAdornment position="end">{LL.SECONDS()}</InputAdornment>
              }}
              label={LL.DEFAULT(0)}
              fullWidth
              variant="outlined"
              value={numberValue(data.publish_time_other)}
              type="number"
              onChange={updateFormValue}
              margin="normal"
            />
          </Grid>
        </Grid>

        {dirtyFlags && dirtyFlags.length !== 0 && (
          <ButtonRow>
            <Button
              startIcon={<CancelIcon />}
              disabled={saving}
              variant="outlined"
              color="primary"
              type="submit"
              onClick={() => loadData()}
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
    <SectionContent title={LL.SETTINGS_OF('MQTT')} titleGutter>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {content()}
    </SectionContent>
  );
};

export default MqttSettingsForm;
