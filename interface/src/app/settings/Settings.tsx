import { useContext } from 'react';

import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import { boardProfileName } from 'app/main/types';
import { mqttInfo, networkInfo, ntpInfo } from 'app/status/status';
import { LinkList, type LinkListItem, Section } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { NetworkConnectionStatus } from 'types';
import { updateAvailable } from 'utils';

import './Settings.css';

const Settings = () => {
  const { LL } = useI18nContext();
  const { me, versions } = useContext(AuthenticatedContext);

  // only admins see the rows that show live values
  const { data } = useRequest(readSystemStatus, { immediate: me.admin });

  const update = updateAvailable(versions);
  const firmware: LinkListItem = {
    to: 'version',
    icon: 'new_releases',
    color: '#2a7fb0',
    title: 'EMS-ESP Firmware',
    text: LL.FIRMWARE_DESC(),
    value: versions?.current?.version && (
      <>
        v{versions.current.version}
        {update && ` · ${LL.UPDATE_AVAILABLE()}`}
      </>
    ),
    state: update ? 'warn' : undefined
  };

  const network = data && networkInfo(data.network_status, LL);
  const mqtt =
    data &&
    mqttInfo(
      { enabled: !!data.mqtt_host || data.mqtt_status, connected: data.mqtt_status },
      LL
    );
  const ntp = data && ntpInfo(data.ntp_status, LL);
  const networkValue =
    data && network?.state === 'ok'
      ? [
          data.network_status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED
            ? 'Ethernet'
            : 'WiFi',
          data.network_ip
        ]
          .filter(Boolean)
          .join(' · ')
      : network?.text;

  const groups: { title: string; items: LinkListItem[] }[] = me.admin
    ? [
        {
          title: LL.CONNECTIVITY(),
          items: [
            {
              to: 'network',
              icon: 'settings_ethernet',
              color: '#40828f',
              title: LL.NETWORK(0),
              text: LL.NETWORK_DESC(),
              value: networkValue,
              state: network?.state
            },
            {
              to: 'ap',
              icon: 'wifi_tethering',
              color: '#5f9a5f',
              title: LL.ACCESS_POINT(0),
              text: LL.AP_DESC(),
              value: data && (data.ap_status ? LL.ACTIVE() : LL.INACTIVE(0)),
              state: data && (data.ap_status ? 'ok' : 'off')
            },
            {
              to: 'mqtt',
              icon: 'device_hub',
              color: '#68374d',
              title: 'MQTT',
              text: LL.MQTT_DESC(),
              value: mqtt?.text,
              state: mqtt?.state
            }
          ]
        },
        {
          title: LL.SYSTEM(0),
          items: [
            {
              to: 'application',
              icon: 'settings_applications',
              color: '#134ba2',
              title: LL.APPLICATION(),
              text: LL.APPLICATION_DESC(),
              value: data && (data.model || boardProfileName(data.board))
            },
            {
              to: 'ntp',
              icon: 'schedule',
              color: '#c5572c',
              title: 'NTP',
              text: LL.NTP_DESC(),
              value: ntp?.text,
              state: ntp?.state
            },
            {
              to: 'security',
              icon: 'lock',
              color: '#757575',
              title: LL.SECURITY(0),
              text: LL.SECURITY_1()
            },
            {
              to: 'modules',
              icon: 'extension',
              color: '#b28a1f',
              title: LL.MODULES(),
              text: LL.MODULES_1()
            }
          ]
        },
        {
          title: LL.MAINTENANCE(),
          items: [
            {
              to: 'downloadUpload',
              icon: 'import_export',
              color: '#5d89f7',
              title: LL.DOWNLOAD_UPLOAD(),
              text: LL.DOWNLOAD_UPLOAD_1()
            },
            firmware
          ]
        }
      ]
    : [{ title: LL.MAINTENANCE(), items: [firmware] }];

  return (
    <div className="set-groups">
      {groups.map(({ title, items }) => (
        <section key={title} className="set-group" aria-label={title}>
          <h2 className="set-gtitle">{title}</h2>
          <Section>
            <LinkList items={items} layout="rows" />
          </Section>
        </section>
      ))}
    </div>
  );
};

export default Settings;
