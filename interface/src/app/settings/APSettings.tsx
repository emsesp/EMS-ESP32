import * as APApi from 'api/ap';

import { FormSection, Segmented, Select, Switch, TextField } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { APSettingsType } from 'types';
import { APProvisionMode } from 'types';
import { createAPSettingsValidator } from 'validators';

import FormPage, { useFormPage } from './FormPage';

export const isAPEnabled = ({ provision_mode }: APSettingsType) =>
  provision_mode === APProvisionMode.AP_MODE_DISCONNECTED;

const range = (from: number, to: number) =>
  Array.from({ length: to - from + 1 }, (_, i) => {
    const v = String(from + i);
    return { value: v, label: v };
  });

const CHANNELS = range(1, 14);
const MAX_CLIENTS = range(1, 9);

const APSettings = () => {
  const { LL } = useI18nContext();
  const form = useFormPage<APSettingsType>({
    read: APApi.readAPSettings,
    update: APApi.updateAPSettings,
    validator: createAPSettingsValidator
  });

  return (
    <FormPage form={form}>
      {(data) => (
        <>
          <FormSection icon="wifi_tethering" title={LL.AP_PROVIDE()}>
            <Segmented
              label={LL.AP_PROVIDE()}
              hideLabel
              fullWidth
              options={[
                {
                  value: String(APProvisionMode.AP_MODE_DISCONNECTED),
                  label: LL.AP_PROVIDE_TEXT_2()
                },
                {
                  value: String(APProvisionMode.AP_NEVER),
                  label: LL.AP_PROVIDE_TEXT_3()
                }
              ]}
              {...form.segmented('provision_mode')}
            />
          </FormSection>
          {isAPEnabled(data) && (
            <>
              <FormSection icon="wifi" title={LL.ACCESS_POINT(0)}>
                <TextField label="SSID" {...form.text('ssid')} />
                <TextField
                  label={LL.PASSWORD()}
                  type="password"
                  autoComplete="new-password"
                  {...form.text('password')}
                />
                <Select
                  label={LL.AP_PREFERRED_CHANNEL()}
                  options={CHANNELS}
                  {...form.select('channel')}
                />
                <Select
                  label={LL.AP_MAX_CLIENTS()}
                  options={MAX_CLIENTS}
                  {...form.select('max_clients')}
                />
                <Switch label={LL.AP_HIDE_SSID()} {...form.check('ssid_hidden')} />
              </FormSection>
              <FormSection icon="router" title={LL.ADDRESSES()}>
                <TextField
                  label={LL.AP_LOCAL_IP()}
                  mono
                  {...form.text('local_ip')}
                />
                <TextField
                  label={LL.NETWORK_GATEWAY()}
                  mono
                  {...form.text('gateway_ip')}
                />
                <TextField
                  label={LL.NETWORK_SUBNET()}
                  mono
                  {...form.text('subnet_mask')}
                />
              </FormSection>
            </>
          )}
        </>
      )}
    </FormPage>
  );
};

export default APSettings;
