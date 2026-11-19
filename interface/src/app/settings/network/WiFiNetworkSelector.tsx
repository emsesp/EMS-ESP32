import { useContext } from 'react';

import {
  Button,
  Icon,
  Message,
  SortHeader,
  Table,
  Tag,
  byNumber,
  byText,
  cx,
  useTableSort
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { WiFiNetwork, WiFiNetworkList } from 'types';
import { WiFiEncryptionType } from 'types';

import './Network.css';
import { WiFiConnectionContext } from './WiFiConnectionContext';

export const isNetworkOpen = ({ encryption_type }: WiFiNetwork) =>
  encryption_type === WiFiEncryptionType.WIFI_AUTH_OPEN;

export const networkSecurityMode = ({ encryption_type }: WiFiNetwork) => {
  switch (encryption_type) {
    case WiFiEncryptionType.WIFI_AUTH_WEP:
      return 'WEP';
    case WiFiEncryptionType.WIFI_AUTH_WPA_PSK:
      return 'WPA';
    case WiFiEncryptionType.WIFI_AUTH_WPA2_PSK:
      return 'WPA2';
    case WiFiEncryptionType.WIFI_AUTH_WPA_WPA2_PSK:
      return 'WPA/WPA2';
    case WiFiEncryptionType.WIFI_AUTH_WPA2_ENTERPRISE:
      return 'WPA2 Enterprise';
    case WiFiEncryptionType.WIFI_AUTH_OPEN:
      return 'None';
    case WiFiEncryptionType.WIFI_AUTH_WPA3_PSK:
      return 'WPA3';
    case WiFiEncryptionType.WIFI_AUTH_WPA2_WPA3_PSK:
      return 'WPA2/WPA3';
    default:
      return 'Unknown: ' + String(encryption_type);
  }
};

const bars = (rssi: number) =>
  rssi > -60 ? 4 : rssi > -70 ? 3 : rssi > -80 ? 2 : 1;

const quality = (rssi: number) =>
  rssi <= -85 ? 'err' : rssi <= -75 ? 'warn' : 'ok';

const Signal = ({ rssi }: { rssi: number }) => (
  <span className={cx('net-sig', `s${bars(rssi)}`, quality(rssi))} aria-hidden>
    <i />
    <i />
    <i />
    <i />
  </span>
);

const WiFiNetworkSelector = ({ networkList }: { networkList: WiFiNetworkList }) => {
  const { LL } = useI18nContext();
  const { form, selectNetwork } = useContext(WiFiConnectionContext);
  const sort = useTableSort(networkList.networks, {
    ssid: byText((n) => n.ssid),
    signal: byNumber((n) => n.rssi),
    channel: byNumber((n) => n.channel),
    security: byText((n) => (isNetworkOpen(n) ? '' : networkSecurityMode(n)))
  });

  if (networkList.networks.length === 0) {
    return <Message level="info">{LL.NETWORK_NO_WIFI()}</Message>;
  }

  return (
    <Table className="stack net-tbl">
      <thead>
        <tr>
          <SortHeader {...sort.header('ssid')}>SSID</SortHeader>
          <SortHeader {...sort.header('signal')}>{LL.SIGNAL()}</SortHeader>
          <SortHeader {...sort.header('channel')}>{LL.CHANNEL()}</SortHeader>
          <SortHeader {...sort.header('security')}>{LL.SECURITY(0)}</SortHeader>
          <th />
        </tr>
      </thead>
      <tbody>
        {sort.rows.map((n) => (
          <tr key={n.bssid} className="click" onClick={() => selectNetwork(n)}>
            <td>
              <div>
                <span>{n.ssid}</span>
                {n.ssid === form.origData?.ssid && (
                  <Tag kind="mod">{LL.WIFI_SAVED()}</Tag>
                )}
              </div>
              <div className="cap mono">{n.bssid}</div>
            </td>
            <td className="num" data-label={LL.SIGNAL()}>
              <span>
                <Signal rssi={n.rssi} /> <span className="cap">{n.rssi} dBm</span>
              </span>
            </td>
            <td className="num" data-label={LL.CHANNEL()}>
              {n.channel}
            </td>
            <td data-label={LL.SECURITY(0)}>
              {isNetworkOpen(n) ? (
                <span className="net-sec warnc">
                  <Icon name="lock_open" size="xs" />
                  {LL.WIFI_OPEN()}
                </span>
              ) : (
                <span className="net-sec">
                  <Icon name="lock" size="xs" />
                  {networkSecurityMode(n)}
                </span>
              )}
            </td>
            <td className="act">
              <Button icon="check" size="sm">
                {LL.USE()}
              </Button>
            </td>
          </tr>
        ))}
      </tbody>
    </Table>
  );
};

export default WiFiNetworkSelector;
