import { useContext } from 'react';

import { LinkList, type LinkListItem, Section } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { updateAvailable } from 'utils';

const Settings = () => {
  const { LL } = useI18nContext();
  const { me, versions } = useContext(AuthenticatedContext);

  const upgradeAvailable = versions?.current?.upgradeable ?? false;
  const firmwareText = versions?.current?.version
    ? `v${versions.current.version}${upgradeAvailable ? ` · ${LL.UPDATE_AVAILABLE()}` : ''}`
    : '';

  const items: LinkListItem[] = [
    ...(me.admin
      ? ([
          {
            to: 'application',
            icon: 'settings_applications',
            color: '#134ba2',
            title: LL.APPLICATION(),
            text: LL.APPLICATION_SETTINGS_1()
          },
          {
            to: 'network',
            icon: 'settings_ethernet',
            color: '#40828f',
            title: LL.NETWORK(0),
            text: LL.CONFIGURE(LL.SETTINGS_OF(LL.NETWORK(1)))
          },
          {
            to: 'ap',
            icon: 'wifi_tethering',
            color: '#5f9a5f',
            title: LL.ACCESS_POINT(0),
            text: LL.CONFIGURE(LL.ACCESS_POINT(1))
          },
          {
            to: 'ntp',
            icon: 'schedule',
            color: '#c5572c',
            title: 'NTP',
            text: LL.CONFIGURE(LL.LOCAL_TIME(1))
          },
          {
            to: 'mqtt',
            icon: 'device_hub',
            color: '#68374d',
            title: 'MQTT',
            text: LL.CONFIGURE('MQTT')
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
          },
          {
            to: 'downloadUpload',
            icon: 'import_export',
            color: '#5d89f7',
            title: LL.DOWNLOAD_UPLOAD(),
            text: LL.DOWNLOAD_UPLOAD_1()
          }
        ] satisfies LinkListItem[])
      : []),
    {
      to: 'version',
      icon: 'new_releases',
      color: '#2a7fb0',
      title: 'EMS-ESP Firmware',
      text: firmwareText,
      badge: updateAvailable(versions)
    }
  ];

  return (
    <Section>
      <LinkList items={items} layout="rows" />
    </Section>
  );
};

export default Settings;
