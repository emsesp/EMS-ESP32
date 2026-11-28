import { useContext, useState } from 'react';

import { API } from 'api/app';
import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import { type APIcall, boardProfileName } from 'app/main/types';
import {
  Hero,
  IconButton,
  Loader,
  Meter,
  Section,
  Tile,
  Tiles,
  toast
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { NTPSyncStatus } from 'types';
import { formatDateTime, updateAvailable, useInterval } from 'utils';

import './Status.css';
import SystemConfirmDialog from './SystemConfirmDialog';
import SystemMonitor from './SystemMonitor';
import {
  STATE_COLORS,
  busInfo,
  formatDuration,
  formatTemperature,
  mqttInfo,
  networkIcon,
  networkInfo,
  nf,
  ntpInfo,
  systemInfo
} from './status';

const SystemStatus = () => {
  const { LL } = useI18nContext();
  const { me, versions } = useContext(AuthenticatedContext);
  const [confirm, setConfirm] = useState<'restart'>();
  const [restarting, setRestarting] = useState(false);

  const { data, send: loadData, error } = useRequest(readSystemStatus);
  const { send: sendAPI, loading: sending } = useRequest(
    (call: APIcall) => API(call),
    { immediate: false }
  );

  useInterval(() => {
    if (!restarting) void loadData();
  });

  if (restarting) {
    return <SystemMonitor />;
  }

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void loadData()} />;
  }

  const run = async (cmd: 'restart' | 'format') => {
    try {
      await sendAPI({ device: 'system', cmd, id: 0 });
      setConfirm(undefined);
      setRestarting(true);
    } catch (e) {
      toast.error((e as Error).message);
    }
  };

  const system = systemInfo(data.status, LL);
  const bus = busInfo(data.bus_status, data.bus_uptime, LL);
  const network = networkInfo(data.network_status, LL);
  const mqtt = mqttInfo(
    { enabled: !!data.mqtt_host || data.mqtt_status, connected: data.mqtt_status },
    LL
  );
  const ntp = ntpInfo(data.ntp_status, LL);
  const update = updateAvailable(versions);
  const errors = data.log_errors ?? 0;
  const warnings = data.log_warnings ?? 0;
  const hasLogCounts = data.log_errors !== undefined;
  const join = (...parts: (string | false | undefined)[]) =>
    parts.filter(Boolean).join(' · ');

  return (
    <>
      <Hero
        className="st-hero"
        state={system.state}
        icon={system.state === 'ok' ? 'check_circle' : 'warning'}
        title={system.text}
        text={join(
          data.model || boardProfileName(data.board),
          `${LL.UPTIME()} ${formatDuration(data.uptime, LL)}`,
          !!data.reset_reason && `${LL.LAST_RESTART()}: ${data.reset_reason}`
        )}
        actions={
          me.admin && (
            <IconButton
              icon="power_settings_new"
              label={LL.RESTART()}
              onClick={() => setConfirm('restart')}
            />
          )
        }
      />

      <Section>
        <Tiles>
          <Tile
            to="/settings/version"
            icon="new_releases"
            color="#c5572c"
            title="EMS-ESP Firmware"
            text={join(
              versions?.current?.version && `v${versions.current.version}`,
              update && LL.UPDATE_AVAILABLE()
            )}
            state={update ? 'warn' : 'ok'}
          />
          <Tile
            to="/status/hardwarestatus"
            icon="memory"
            color="#72caf9"
            title={LL.HARDWARE()}
            text={join(
              `${nf.format(data.free_heap)} KB ${LL.FREE_MEMORY()}`,
              data.temperature !== undefined && formatTemperature(data.temperature)
            )}
            state="ok"
          />
          <Tile
            to="/status/activity"
            icon="swap_horiz"
            color="#68374d"
            title={LL.DATA_TRAFFIC()}
            text={bus.text}
            state={bus.state}
          />
          <Tile
            to="/status/network"
            icon={networkIcon(data.network_status)}
            color={STATE_COLORS[network.state]}
            title={LL.NETWORK(1)}
            text={join(
              network.text,
              data.network_ip,
              data.wifi_rssi < 0 && `${data.wifi_rssi} dBm`
            )}
            state={network.state}
          />
          <Tile
            to="/status/mqtt"
            icon="device_hub"
            color={STATE_COLORS[mqtt.state]}
            title="MQTT"
            text={join(
              mqtt.text,
              data.mqtt_host && `${data.mqtt_host}:${data.mqtt_port}`
            )}
            state={mqtt.state}
          />
          <Tile
            to="/status/ntp"
            icon="schedule"
            color={STATE_COLORS[ntp.state]}
            title="NTP"
            text={join(
              ntp.text,
              data.ntp_status === NTPSyncStatus.NTP_ACTIVE &&
                data.ntp_time &&
                formatDateTime(data.ntp_time)
            )}
            state={ntp.state}
          />
          <Tile
            to="/status/ap"
            icon={data.ap_status ? 'wifi_tethering' : 'wifi_tethering_off'}
            color={STATE_COLORS[data.ap_status ? 'ok' : 'off']}
            title={LL.ACCESS_POINT(0)}
            text={data.ap_status ? LL.ACTIVE() : LL.INACTIVE(0)}
            state={data.ap_status ? 'ok' : 'off'}
          />
          <Tile
            to="/status/log"
            icon="list_alt"
            color="#40828f"
            title={LL.SYSTEM_LOG()}
            text={
              hasLogCounts
                ? join(
                    LL.NUM_ERRORS({ num: errors }),
                    LL.NUM_WARNINGS({ num: warnings })
                  )
                : LL.VIEW_LOG()
            }
            state={
              !hasLogCounts ? undefined : errors ? 'err' : warnings ? 'warn' : 'ok'
            }
          />
        </Tiles>
      </Section>

      <Section>
        <div className="meters">
          {data.heap_size !== undefined && (
            <Meter
              label={LL.HEAP()}
              used={data.heap_size - data.free_heap}
              total={data.heap_size}
              unit="KB"
            />
          )}
          {data.psram_size !== undefined && data.free_psram !== undefined && (
            <Meter
              label="PSRAM"
              used={data.psram_size - data.free_psram}
              total={data.psram_size}
              unit="KB"
            />
          )}
          <Meter
            label={LL.FILE_SYSTEM()}
            used={data.fs_used}
            total={data.fs_used + data.fs_free}
            unit="KB"
          />
        </div>
      </Section>

      <SystemConfirmDialog
        confirm={confirm}
        disabled={sending}
        onClose={() => setConfirm(undefined)}
        onConfirm={run}
      />
    </>
  );
};

export default SystemStatus;
