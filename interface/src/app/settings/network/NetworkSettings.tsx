import { useContext } from 'react';

import { isEthernet } from 'app/status/status';
import {
  ButtonLink,
  FormSection,
  Listbox,
  Message,
  Switch,
  TextField
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import './Network.css';
import { WiFiConnectionContext } from './WiFiConnectionContext';

const TX_POWER = [
  ...[78, 76, 74, 68, 60, 52, 44, 34, 28, 20, 8].map((v) => ({
    value: String(v),
    label: `${v / 4} dBm`
  }))
];

const NetworkSettings = () => {
  const { LL } = useI18nContext();
  const { form, status } = useContext(WiFiConnectionContext);
  const { data } = form;

  return (
    <>
      {status && isEthernet(status.network_status) && (
        <Message level="info" className="net-msg">
          {LL.NETWORK_ETHERNET_INFO({ ip: status.network_ip ?? '' })}
        </Message>
      )}
      <FormSection icon="wifi" title="Wi-Fi">
        <TextField
          label="SSID"
          {...form.text('ssid')}
          action={
            <ButtonLink to="/settings/network/scan" size="sm" icon="wifi_find">
              {LL.SCAN()}
            </ButtonLink>
          }
        />
        <TextField
          label={LL.PASSWORD()}
          type="password"
          autoComplete="new-password"
          {...form.text('password')}
        />
        <TextField
          label="BSSID"
          mono
          placeholder={LL.NETWORK_BLANK_BSSID()}
          {...form.text('bssid')}
        />
        <Listbox
          label={LL.TX_POWER()}
          options={[{ value: '0', label: LL.AUTO() }, ...TX_POWER]}
          {...form.select('tx_power')}
        />
        <Switch
          label={LL.NETWORK_DISABLE_SLEEP()}
          helperText={LL.NETWORK_DISABLE_SLEEP_HELP()}
          {...form.check('nosleep')}
        />
        <Switch label={LL.NETWORK_LOW_BAND()} {...form.check('bandwidth20')} />
      </FormSection>
      {status?.esp_platform === 'ESP32' && (
        <FormSection icon="settings_ethernet" title="Ethernet">
          <Switch
            label={LL.NETWORK_ETH_10MBIT()}
            helperText={LL.NETWORK_ETH_10MBIT_HELP()}
            {...form.check('eth_10mbit')}
          />
        </FormSection>
      )}
      <FormSection icon="dns" title={LL.HOSTNAME_ACCESS()}>
        <TextField
          label={LL.HOSTNAME()}
          mono
          helperText={
            data.enableMDNS && data.hostname
              ? LL.NETWORK_MDNS_HELP({ host: data.hostname })
              : undefined
          }
          {...form.text('hostname')}
        />
        <Switch label={LL.NETWORK_USE_DNS()} {...form.check('enableMDNS')} />
        <Switch
          label={LL.NETWORK_ENABLE_CORS()}
          helperText={LL.NETWORK_CORS_HELP()}
          {...form.check('enableCORS')}
        />
        {data.enableCORS && (
          <TextField
            label={LL.NETWORK_CORS_ORIGIN()}
            mono
            fullWidth
            helperText={LL.NETWORK_CORS_ORIGIN_HELP()}
            {...form.text('CORSOrigin')}
          />
        )}
        {data.enableCORS && data.CORSOrigin === '*' && (
          <Message level="warning" className="full">
            {LL.NETWORK_CORS_WILDCARD()}
          </Message>
        )}
        <TextField
          label={LL.NETWORK_ALLOWED_HOSTS()}
          mono
          fullWidth
          helperText={LL.NETWORK_ALLOWED_HOSTS_HELP()}
          {...form.text('allowed_hosts')}
        />
      </FormSection>
      <FormSection icon="router" title={LL.IP_ADDRESS()}>
        <Switch
          label={LL.NETWORK_FIXED_IP()}
          helperText={LL.NETWORK_FIXED_IP_HELP()}
          {...form.check('static_ip_config')}
        />
        {data.static_ip_config && (
          <>
            <TextField
              label={LL.AP_LOCAL_IP()}
              mono
              placeholder="192.168.1.42"
              {...form.text('local_ip')}
            />
            <TextField
              label={LL.NETWORK_GATEWAY()}
              mono
              placeholder="192.168.1.1"
              {...form.text('gateway_ip')}
            />
            <TextField
              label={LL.NETWORK_SUBNET()}
              mono
              placeholder="255.255.255.0"
              {...form.text('subnet_mask')}
            />
          </>
        )}
        <TextField label="DNS #1" mono {...form.text('dns_ip_1')} />
        <TextField label="DNS #2" mono {...form.text('dns_ip_2')} />
      </FormSection>
    </>
  );
};

export default NetworkSettings;
