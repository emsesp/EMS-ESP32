import * as NetworkApi from 'api/network';

import { useRequest } from 'alova/client';
import { Loader } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { NetworkStatusType } from 'types';
import { useInterval } from 'utils';

import StatusPage from './StatusPage';
import { isEthernet, isWiFi, networkIcon, networkInfo } from './status';

interface Address {
  value: string;
  note?: string;
}

// one address per line, with an optional dimmed note such as the IPv6 scope
const AddressList = ({ addresses }: { addresses: Address[] }) => {
  if (addresses.length === 0) {
    return 'none';
  }
  return addresses.map(({ value, note }) => (
    <div key={value}>
      {value}
      {note && <span className="cap"> ({note})</span>}
    </div>
  ));
};

const isSet = (ip?: string): ip is string => !!ip && ip !== '0.0.0.0' && ip !== '::';

const ipAddresses = ({ local_ip, ipv6 }: NetworkStatusType): Address[] => [
  ...(isSet(local_ip) ? [{ value: local_ip }] : []),
  ...(ipv6 ?? []).map(({ address, scope }) => ({ value: address, note: scope }))
];

const gateways = ({ gateway_ip, gateway_ipv6 }: NetworkStatusType): Address[] => [
  ...(isSet(gateway_ip) ? [{ value: gateway_ip }] : []),
  ...(gateway_ipv6 ?? []).filter(isSet).map((value) => ({ value }))
];

const dnsServers = ({ dns }: NetworkStatusType): Address[] =>
  (dns ?? []).filter(isSet).map((value) => ({ value }));

const NetworkStatus = () => {
  const { LL } = useI18nContext();
  const { data, send: loadData, error } = useRequest(NetworkApi.readNetworkStatus);

  useInterval(() => {
    void loadData();
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void loadData()} />;
  }

  const wifi = isWiFi(data.status);
  const connected = wifi || isEthernet(data.status);
  const { state, text } = networkInfo(data.status, LL);

  return (
    <StatusPage
      state={state}
      icon={networkIcon(data.status)}
      title={text}
      text={[
        connected && isSet(data.local_ip) && data.local_ip,
        wifi && `${data.ssid} (${data.rssi} dBm)`
      ]
        .filter(Boolean)
        .join(' · ')}
      settings={{
        to: '/settings/network',
        label: LL.SETTINGS_OF(LL.NETWORK(0)),
        icon: 'settings_ethernet'
      }}
      rows={[
        { label: LL.HOSTNAME(), value: data.hostname },
        wifi && {
          label: 'SSID',
          value: data.ssid,
          extra: `${data.rssi} dBm · ch ${data.channel}`
        },
        !isEthernet(data.status) && {
          label: LL.RECONNECTS(),
          value: data.reconnect_count
        },
        connected && {
          label: LL.ADDRESS_OF('IP'),
          value: <AddressList addresses={ipAddresses(data)} />
        },
        connected && { label: LL.ADDRESS_OF('MAC'), value: data.mac_address },
        connected && { label: LL.NETWORK_SUBNET(), value: data.subnet_mask },
        connected && {
          label: LL.NETWORK_GATEWAY(),
          value: <AddressList addresses={gateways(data)} />
        },
        connected && {
          label: LL.NETWORK_DNS(),
          value: <AddressList addresses={dnsServers(data)} />
        }
      ]}
    />
  );
};

export default NetworkStatus;
