import { useEffect, useState } from 'react';

import * as MqttApi from 'api/mqtt';
import { callAction } from 'api/app';

import {
  Button,
  Dialog,
  FormSection,
  IconButton,
  Listbox,
  Message,
  Switch,
  TextArea,
  TextField,
  toast
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { TranslationFunctions } from 'i18n/i18n-types';
import type { MqttSettingsType } from 'types';
import { createMqttSettingsValidator } from 'validators';
import { type TlsCheck, tlsCheckOf } from 'validators/mqtt';

import FormPage, { useFormPage } from './FormPage';
import './MqttSettings.css';

const DISCOVERY_TYPES = ['Home Assistant', 'Domoticz', 'Domoticz (latest)'];

const opts = (labels: string[]) =>
  labels.map((label, i) => ({ value: String(i), label }));

// an illustration of the topics, not the exact payloads
const mqttPreview = (d: MqttSettingsType, LL: TranslationFunctions) => {
  const b = d.base;
  const lines = [
    `${b}/heartbeat   {"bus_status":"connected","uptime":"…"}`,
    `${b}/boiler_data   {"curflowtemp":58.4,"selflowtemp":60,…}`,
    d.nested_format === 1
      ? `${b}/thermostat_data   {"hc1":{"seltemp":21.5,"mode":"auto"},"hc2":{…}}`
      : `${b}/thermostat_data_hc1   {"seltemp":21.5,"mode":"auto"}`
  ];
  if (d.publish_single) lines.push(`${b}/boiler/curflowtemp   58.4`);
  if (d.mqtt_commands)
    lines.push(`${b}/thermostat/hc1/seltemp   ← ${LL.MQTT_PREVIEW_CMD()}`);
  if (d.ha_enabled)
    lines.push(
      `${d.discovery_prefix}/climate/${b}/thermostat_hc1/config   ← ${DISCOVERY_TYPES[d.discovery_type] ?? ''}`
    );
  if (d.send_response) lines.push(`${b}/response   ← ${LL.MQTT_PREVIEW_RESPONSE()}`);
  return lines.join('\n');
};

const MqttSettings = () => {
  const { LL } = useI18nContext();
  // chosen in the form, otherwise what the stored rootCA holds
  const [chosenTlsCheck, setChosenTlsCheck] = useState<TlsCheck>();
  const [previewOpen, setPreviewOpen] = useState(false);
  const form = useFormPage<MqttSettingsType>({
    read: MqttApi.readMqttSettings,
    update: MqttApi.updateMqttSettings,
    validator: (d) =>
      createMqttSettingsValidator(d, chosenTlsCheck ?? tlsCheckOf(d.rootCA))
  });
  const tlsCheck = chosenTlsCheck ?? tlsCheckOf(form.data?.rootCA);

  // back to the stored check after Cancel or Save
  useEffect(() => {
    if (form.data === form.origData) setChosenTlsCheck(undefined);
  }, [form.data, form.origData]);

  const changeTlsCheck = (next: TlsCheck) => {
    setChosenTlsCheck(next);
    const current = form.data?.rootCA ?? '';
    form.set({
      rootCA:
        next === 'none' ? 'insecure' : tlsCheckOf(current) === next ? current : ''
    });
  };

  const sendResetMQTT = () => {
    void callAction({ action: 'resetMQTT' })
      .then(() => {
        toast.success(LL.MQTT_REFRESHED());
      })
      .catch((error: { error?: { message?: string } }) => {
        toast.error(String(error.error?.message || LL.ERROR()));
      });
  };

  const seconds = (name: keyof MqttSettingsType, label: string) => (
    <TextField key={name} label={label} unit="s" min={0} {...form.number(name)} />
  );

  return (
    <FormPage form={form}>
      {(data) => (
        <>
          <Dialog
            open={previewOpen && data.enabled}
            title={LL.MQTT_PREVIEW()}
            size="md"
            onClose={() => setPreviewOpen(false)}
            actions={
              <Button
                icon="close"
                color="neutral"
                onClick={() => setPreviewOpen(false)}
              >
                {LL.CLOSE()}
              </Button>
            }
          >
            <pre className="summary mono mq-preview">{mqttPreview(data, LL)}</pre>
          </Dialog>

          <FormSection
            icon="dns"
            title={LL.BROKER()}
            actions={
              data.enabled && (
                <Button color="secondary" icon="refresh" onClick={sendResetMQTT}>
                  {LL.MQTT_REFRESH()}
                </Button>
              )
            }
          >
            <Switch
              label={LL.ENABLE_MQTT()}
              adornment={
                data.enabled && (
                  <IconButton
                    icon="info"
                    label={LL.MQTT_PREVIEW()}
                    plain
                    onClick={() => setPreviewOpen(true)}
                  />
                )
              }
              {...form.check('enabled')}
            />
            {data.enabled && (
              <>
                <TextField
                  label={LL.ADDRESS_OF(LL.BROKER())}
                  mono
                  {...form.text('host')}
                />
                <TextField
                  label={LL.PORT()}
                  min={0}
                  max={65535}
                  {...form.number('port')}
                />
                <TextField
                  label={LL.USERNAME(0)}
                  autoComplete="off"
                  {...form.text('username')}
                />
                <TextField
                  label={LL.PASSWORD()}
                  type="password"
                  autoComplete="new-password"
                  {...form.text('password')}
                />
                <TextField
                  label={`${LL.CLIENT_ID()} (${LL.OPTIONAL()})`}
                  mono
                  {...form.text('client_id')}
                />
                <TextField label={LL.BASE_TOPIC()} mono {...form.text('base')} />
                <TextField
                  label={LL.KEEP_ALIVE()}
                  unit="s"
                  min={1}
                  {...form.number('keep_alive')}
                />
                <Listbox
                  label="QoS"
                  options={opts(['0', '1', '2'])}
                  {...form.select('mqtt_qos')}
                />
                {data.enableTLS !== undefined && (
                  <Switch label={LL.ENABLE_TLS()} {...form.check('enableTLS')} />
                )}
                {data.enableTLS === true && (
                  <>
                    <Listbox
                      label={LL.TLS_CHECK()}
                      name="tlsCheck"
                      fullWidth
                      value={tlsCheck}
                      changed={
                        chosenTlsCheck !== undefined &&
                        chosenTlsCheck !== tlsCheckOf(form.origData?.rootCA)
                      }
                      options={[
                        { value: 'none', label: LL.TLS_CHECK_NONE() },
                        { value: 'ca', label: LL.TLS_CHECK_CA() },
                        { value: 'fingerprint', label: LL.TLS_CHECK_FINGERPRINT() }
                      ]}
                      onChange={(v) => changeTlsCheck(v as TlsCheck)}
                    />
                    {tlsCheck === 'ca' && (
                      <>
                        <TextArea
                          label={LL.CERT()}
                          rows={6}
                          placeholder="-----BEGIN CERTIFICATE-----"
                          {...form.text('rootCA')}
                        />
                        <Message level="info" className="full">
                          {LL.TLS_CA_NEEDS_TIME()}
                        </Message>
                      </>
                    )}
                    {tlsCheck === 'fingerprint' && (
                      <TextField
                        label={LL.TLS_CHECK_FINGERPRINT()}
                        mono
                        fullWidth
                        placeholder="AB:CD:EF:..."
                        {...form.text('rootCA')}
                      />
                    )}
                    {tlsCheck === 'none' && (
                      <Message level="warning" className="full">
                        {LL.TLS_NOT_CHECKED()}
                      </Message>
                    )}
                  </>
                )}
              </>
            )}
          </FormSection>

          {data.enabled && (
            <>
              <FormSection icon="tune" title={LL.MQTT_BEHAVIOUR()}>
                <Listbox
                  label={LL.MQTT_COMMANDS()}
                  fullWidth
                  options={opts([
                    LL.MQTT_COMMANDS_0(),
                    LL.MQTT_COMMANDS_1(),
                    LL.MQTT_COMMANDS_2()
                  ])}
                  {...form.select('mqtt_commands')}
                />
                <Switch
                  label={LL.MQTT_CLEAN_SESSION()}
                  {...form.check('clean_session')}
                />
                <Switch
                  label={LL.MQTT_RETAIN_FLAG()}
                  {...form.check('mqtt_retain')}
                />
                <Switch
                  label={LL.MQTT_RESPONSE()}
                  {...form.check('send_response')}
                />
              </FormSection>

              <FormSection icon="data_object" title={LL.MQTT_TOPICS()}>
                <Listbox
                  label={LL.MQTT_FORMAT()}
                  fullWidth
                  options={[
                    { value: '1', label: LL.MQTT_NEST_1() },
                    { value: '2', label: LL.MQTT_NEST_2() }
                  ]}
                  {...form.select('nested_format')}
                />
                <Switch
                  label={LL.MQTT_PUBLISH_TEXT_1()}
                  disabled={data.ha_enabled}
                  helperText={
                    data.ha_enabled ? LL.MQTT_SINGLE_NO_DISCOVERY() : undefined
                  }
                  {...form.check('publish_single')}
                />
                {data.publish_single && (
                  <Switch
                    label={LL.MQTT_PUBLISH_TEXT_2()}
                    {...form.check('publish_single2cmd')}
                  />
                )}
              </FormSection>

              <FormSection icon="travel_explore" title={LL.MQTT_DISCOVERY()}>
                <Switch
                  label={LL.MQTT_PUBLISH_TEXT_3()}
                  disabled={data.publish_single}
                  helperText={
                    data.publish_single
                      ? LL.MQTT_DISCOVERY_NO_SINGLE()
                      : LL.MQTT_DISCOVERY_HELP()
                  }
                  {...form.check('ha_enabled')}
                />
                {data.ha_enabled && (
                  <>
                    <Listbox
                      label={LL.MQTT_PUBLISH_TEXT_5()}
                      options={opts(DISCOVERY_TYPES)}
                      {...form.select('discovery_type')}
                    />
                    <TextField
                      label={LL.MQTT_PUBLISH_TEXT_4()}
                      mono
                      {...form.text('discovery_prefix')}
                    />
                    <Listbox
                      label={LL.MQTT_ENTITY_FORMAT()}
                      helperText={
                        <span className="warnc">{LL.MQTT_ENTITY_FORMAT_WARN()}</span>
                      }
                      options={[
                        { value: '0', label: LL.MQTT_ENTITY_FORMAT_0() },
                        { value: '3', label: `${LL.MQTT_ENTITY_FORMAT_1()} (v3.5)` },
                        { value: '4', label: `${LL.MQTT_ENTITY_FORMAT_2()} (v3.5)` },
                        {
                          value: '1',
                          label: `${LL.MQTT_ENTITY_FORMAT_1()} (latest)`
                        },
                        {
                          value: '2',
                          label: `${LL.MQTT_ENTITY_FORMAT_2()} (latest)`
                        }
                      ]}
                      {...form.select('entity_format')}
                    />
                    {data.discovery_type === 0 && (
                      <Listbox
                        label={LL.MQTT_INPUT_NUMBER_FORMAT()}
                        options={opts(['Box', 'Slider'])}
                        {...form.select('ha_number_mode')}
                      />
                    )}
                  </>
                )}
              </FormSection>

              <FormSection
                icon="timer"
                title={LL.MQTT_PUBLISH_INTERVALS()}
                desc={LL.MQTT_INTERVALS_HELP()}
              >
                {seconds('publish_time_boiler', LL.MQTT_INT_BOILER())}
                {seconds('publish_time_thermostat', LL.MQTT_INT_THERMOSTATS())}
                {seconds('publish_time_solar', LL.MQTT_INT_SOLAR())}
                {seconds('publish_time_mixer', LL.MQTT_INT_MIXER())}
                {seconds('publish_time_water', LL.MQTT_INT_WATER())}
                {seconds('publish_time_sensor', LL.SENSORS())}
                {seconds('publish_time_other', LL.DEFAULT(0))}
                {seconds('publish_time_heartbeat', 'Heartbeat')}
              </FormSection>
            </>
          )}
        </>
      )}
    </FormPage>
  );
};

export default MqttSettings;
