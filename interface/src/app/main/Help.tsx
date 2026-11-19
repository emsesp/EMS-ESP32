import { useContext, useState } from 'react';

import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import {
  Button,
  ButtonLink,
  LinkList,
  type LinkListItem,
  Section,
  toast
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { NetworkConnectionStatus, type SystemStatus } from 'types';
import { saveFile } from 'utils';

import { API, callAction, readActivity, readCoreData } from '../../api/app';
import './Help.css';
import {
  type APIcall,
  type Device,
  DeviceType,
  type Stat,
  busConnectionStatus
} from './types';

interface CustomSupport {
  img_url: string | null;
  html: string | null;
}

const DEFAULT_IMAGE_URL = 'https://emsesp.org/media/images/installer.jpeg';
const ISSUE_URL = 'https://github.com/emsesp/EMS-ESP32/issues/new/choose';
const SYSTEM_INFO_API: APIcall = { device: 'system', cmd: 'info', id: 0 };

const BUS_TEXT: Record<number, string> = {
  [busConnectionStatus.BUS_STATUS_CONNECTED]: 'connected',
  [busConnectionStatus.BUS_STATUS_TX_ERRORS]: 'connected with Tx errors',
  [busConnectionStatus.BUS_STATUS_OFFLINE]: 'offline'
};

const duration = (s: number) =>
  `${Math.trunc(s / 86400)}d ${Math.trunc(s / 3600) % 24}h ${Math.trunc(s / 60) % 60}m`;

const TX_MODE_TEXT = ['off', 'EMS', 'EMS+', 'HT3', 'hardware', 'auto'];

const hex = (n: number) => `0x${n.toString(16).toUpperCase().padStart(2, '0')}`;

// stats ids as in /rest/activity: 0 received, 1 Tx reads, 2 Tx writes
const busLine = (s: SystemStatus, stats?: readonly Stat[]) => {
  const rx = stats?.find((st) => st.id === 0);
  const tx = stats?.filter((st) => st.id === 1 || st.id === 2);
  return [
    `EMS bus ${BUS_TEXT[s.bus_status] ?? 'unknown'}`,
    s.tx_mode !== undefined && `Tx mode ${TX_MODE_TEXT[s.tx_mode] ?? s.tx_mode}`,
    s.bus_id !== undefined && `bus ID ${hex(s.bus_id)}`,
    rx && `${rx.q}% quality`,
    tx?.length && `${tx.reduce((n, st) => n + st.f, 0)} Tx fails`,
    `${s.num_sensors} temperature and ${s.num_analogs} analog sensors`
  ]
    .filter(Boolean)
    .join(', ');
};

// the untranslated type name from `url`, so the line stays English
const devicesLine = (s: SystemStatus, devices?: readonly Device[]) => {
  const ems = devices?.filter((d) => d.t !== DeviceType.CUSTOM);
  if (!ems?.length) {
    return `${s.num_devices} devices`;
  }
  return `${ems.length} devices (type/device ID/product ID): ${ems.map((d) => `${d.url ?? d.tn}/${hex(d.d)}/${d.p}`).join(', ')}`;
};

// in English whatever the UI language, it is pasted into GitHub and Discord
const summary = (
  s: SystemStatus,
  devices?: readonly Device[],
  stats?: readonly Stat[]
) =>
  [
    `EMS-ESP v${s.emsesp_version} on ${s.model || s.board || s.esp_platform} (${s.esp_platform}, ${s.partition})`,
    `uptime ${duration(s.uptime)}, free heap ${s.free_heap} KB, max block ${s.max_alloc_heap} KB`,
    busLine(s, stats),
    devicesLine(s, devices),
    `network ${
      s.network_status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED
        ? 'Ethernet'
        : s.network_status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED
          ? `WiFi (${s.wifi_rssi} dBm)`
          : 'not connected'
    }, MQTT ${s.mqtt_status ? 'connected' : 'not connected'}`
  ].join('\n');

const Help = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  const [customSupport, setCustomSupport] = useState<CustomSupport>({
    img_url: null,
    html: null
  });
  const [imgError, setImgError] = useState(false);

  useRequest(callAction({ action: 'getCustomSupport' })).onSuccess((event) => {
    if (event?.data && Object.keys(event.data).length !== 0) {
      const { Support } = event.data as {
        Support: { img_url?: string; html?: string[] };
      };
      setCustomSupport({
        img_url: Support.img_url || null,
        html: Support.html?.join('<br/>') || null
      });
    }
  });

  const { data: status } = useRequest(readSystemStatus);
  const { data: coreData } = useRequest(readCoreData);
  const { data: activity } = useRequest(readActivity);

  const { send: sendAPI, loading: downloading } = useRequest(
    (data: APIcall) => API(data),
    { immediate: false }
  )
    .onSuccess((event) => {
      saveFile(event.data, 'system_info', '.json');
      toast.success(LL.DOWNLOAD_SUCCESSFUL());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || LL.ERROR()));
    });

  const copySummary = (text: string) => {
    navigator.clipboard
      .writeText(text)
      .then(() => toast.success(LL.COPIED()))
      .catch(() => toast.error(LL.COPY_FAILED()));
  };

  const links: LinkListItem[] = [
    {
      href: 'https://emsesp.org',
      icon: 'menu_book',
      color: '#72caf9',
      title: LL.DOCUMENTATION(),
      text: LL.HELP_INFORMATION_1(),
      note: 'emsesp.org'
    },
    {
      href: 'https://discord.gg/GP9DPSgeJq',
      icon: 'forum',
      color: '#5865f2',
      title: LL.COMMUNITY_CHAT(),
      text: LL.HELP_INFORMATION_2(),
      note: 'discord.gg'
    },
    {
      href: ISSUE_URL,
      icon: 'bug_report',
      color: '#c5572c',
      title: LL.REPORT_ISSUE(),
      text: LL.HELP_INFORMATION_3(),
      note: 'github.com/emsesp'
    }
  ];

  const imageSrc =
    imgError || !customSupport.img_url ? DEFAULT_IMAGE_URL : customSupport.img_url;
  const text = status ? summary(status, coreData?.devices, activity?.stats) : '';

  return (
    <>
      {customSupport.html && (
        <Section className="support-box">
          <div
            className="grow support-html"
            dangerouslySetInnerHTML={{ __html: customSupport.html }}
          />
          <img
            className="support-img"
            alt=""
            referrerPolicy="no-referrer"
            src={imageSrc}
            onError={() => setImgError(true)}
          />
        </Section>
      )}

      {me.admin && (
        <Section title={LL.GET_HELP()} icon="live_help">
          <LinkList items={links} />
        </Section>
      )}

      <Section
        desc={LL.HELP_INFORMATION_4()}
        title={LL.REPORT_PROBLEM()}
        icon="bug_report"
      >
        <ol className="help-steps">
          <li>
            <span className="num">1</span>
            <span className="grow">
              {LL.REPORT_STEP_1()}
              <span className="cap">{LL.REPORT_STEP_1_TEXT()}</span>
            </span>
            <Button
              icon="download"
              disabled={downloading}
              onClick={() => void sendAPI(SYSTEM_INFO_API)}
            >
              {LL.SYSTEM_INFORMATION(0)}
            </Button>
          </li>
          {me.admin && (
            <li>
              <span className="num">2</span>
              <span className="grow">
                {LL.REPORT_STEP_2()}
                <span className="cap">{LL.REPORT_STEP_2_TEXT()}</span>
              </span>
              <ButtonLink to="/status/log" color="secondary" icon="list_alt">
                {LL.LOG_OF(LL.SYSTEM(0))}
              </ButtonLink>
            </li>
          )}
          <li>
            <span className="num">{me.admin ? 3 : 2}</span>
            <span className="grow">
              {LL.REPORT_STEP_3()}
              <span className="cap">{LL.REPORT_STEP_3_TEXT()}</span>
            </span>
            <ButtonLink href={ISSUE_URL} color="secondary" icon="open_in_new">
              {LL.NEW_ISSUE()}
            </ButtonLink>
          </li>
        </ol>
        {text && (
          <>
            <div className="sum-head">
              <span className="cap grow">{LL.SHORT_SUMMARY()}</span>
              <Button icon="content_copy" onClick={() => copySummary(text)}>
                {LL.COPY()}
              </Button>
            </div>
            <pre className="summary mono">{text}</pre>
          </>
        )}
      </Section>

      <p className="about">
        EMS-ESP {status ? `v${status.emsesp_version}` : ''} · &copy;{' '}
        <a href="https://emsesp.org" target="_blank" rel="noreferrer">
          emsesp.org
        </a>{' '}
        · {LL.LICENSE_TEXT()}
      </p>
    </>
  );
};

export default Help;
