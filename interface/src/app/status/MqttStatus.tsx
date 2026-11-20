import * as MqttApi from 'api/mqtt';

import { useRequest } from 'alova/client';
import { Loader, Stat, StatRow } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { TranslationFunctions } from 'i18n/i18n-types';
import { MqttDisconnectReason } from 'types';
import { formatDateTimeMinutes, formatTimeAgo, useInterval } from 'utils';

import StatusPage from './StatusPage';
import { mqttInfo, nf } from './status';

const DISCONNECT_REASONS: Record<
  MqttDisconnectReason,
  (LL: TranslationFunctions) => string
> = {
  [MqttDisconnectReason.USER_OK]: (LL) => LL.MQTT_R_USER(),
  [MqttDisconnectReason.TCP_DISCONNECTED]: (LL) => LL.MQTT_R_TCP(),
  [MqttDisconnectReason.MQTT_UNACCEPTABLE_PROTOCOL_VERSION]: (LL) =>
    LL.MQTT_R_PROTOCOL(),
  [MqttDisconnectReason.MQTT_IDENTIFIER_REJECTED]: (LL) => LL.MQTT_R_ID(),
  [MqttDisconnectReason.MQTT_SERVER_UNAVAILABLE]: (LL) => LL.MQTT_R_SERVER(),
  [MqttDisconnectReason.MQTT_MALFORMED_CREDENTIALS]: (LL) => LL.MQTT_R_CREDENTIALS(),
  [MqttDisconnectReason.MQTT_NOT_AUTHORIZED]: (LL) => LL.MQTT_R_AUTH(),
  [MqttDisconnectReason.TLS_BAD_FINGERPRINT]: (LL) => LL.MQTT_R_FINGERPRINT()
};

const secondsAgo = (seconds: number) => new Date(Date.now() - seconds * 1000);

const MqttStatus = () => {
  const { LL, locale } = useI18nContext();
  const { data, send: loadData, error } = useRequest(MqttApi.readMqttStatus);

  useInterval(() => {
    void loadData();
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void loadData()} />;
  }

  const { state, text } = mqttInfo(data, LL);
  const reasonText = (reason: MqttDisconnectReason) =>
    DISCONNECT_REASONS[reason]?.(LL) ?? LL.UNKNOWN();
  const disconnectAgo =
    data.last_disconnect_ago !== undefined &&
    formatTimeAgo(locale, secondsAgo(data.last_disconnect_ago));

  return (
    <StatusPage
      state={state}
      icon="device_hub"
      title={`MQTT ${text}`}
      text={data.host && `${data.host}:${data.port}`}
      settings={{ to: '/settings/mqtt', label: LL.MQTT_SETTINGS() }}
      rows={
        data.enabled
          ? [
              { label: LL.CLIENT_ID(), value: data.client_id },
              data.base !== undefined && {
                label: LL.MQTT_BASE_TOPIC(),
                value: <span className="mono">{data.base}/</span>
              },
              !data.connected && {
                label: LL.DISCONNECT_REASON(),
                value: reasonText(data.disconnect_reason),
                extra: disconnectAgo
              },
              data.connected &&
                !!disconnectAgo &&
                data.last_disconnect_reason !== undefined && {
                  label: LL.LAST_DISCONNECT(),
                  value: reasonText(data.last_disconnect_reason),
                  extra: disconnectAgo
                },
              data.connected_for !== undefined && {
                label: LL.CONNECTED_SINCE(),
                value: formatDateTimeMinutes(secondsAgo(data.connected_for))
              }
            ]
          : []
      }
    >
      {data.enabled && (
        <StatRow>
          <Stat value={nf.format(data.mqtt_queued)} label={LL.MQTT_QUEUE()} />
          <Stat
            value={nf.format(data.mqtt_fails)}
            label={LL.MQTT_ERRORS()}
            warn={data.mqtt_fails > 0}
          />
          <Stat value={nf.format(data.connect_count)} label={LL.CONNECTS()} />
        </StatRow>
      )}
    </StatusPage>
  );
};

export default MqttStatus;
