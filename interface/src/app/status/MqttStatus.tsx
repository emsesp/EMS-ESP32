import { type FC, memo, useMemo } from 'react';

import AutoAwesomeMotionIcon from '@mui/icons-material/AutoAwesomeMotion';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import ReportIcon from '@mui/icons-material/Report';
import SpeakerNotesOffIcon from '@mui/icons-material/SpeakerNotesOff';
import {
  Avatar,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  useTheme
} from '@mui/material';
import type { Theme } from '@mui/material';

import * as MqttApi from 'api/mqtt';

import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { MqttStatusType } from 'types';
import { MqttDisconnectReason } from 'types';
import { useInterval } from 'utils';

// Disconnect reason lookup table - created once, reused across renders
const DISCONNECT_REASONS: Record<MqttDisconnectReason, string> = {
  [MqttDisconnectReason.USER_OK]: 'User disconnected',
  [MqttDisconnectReason.TCP_DISCONNECTED]: 'TCP disconnected',
  [MqttDisconnectReason.MQTT_UNACCEPTABLE_PROTOCOL_VERSION]:
    'Unacceptable protocol version',
  [MqttDisconnectReason.MQTT_IDENTIFIER_REJECTED]: 'Client ID rejected',
  [MqttDisconnectReason.MQTT_SERVER_UNAVAILABLE]: 'Server unavailable',
  [MqttDisconnectReason.MQTT_MALFORMED_CREDENTIALS]: 'Malformed credentials',
  [MqttDisconnectReason.MQTT_NOT_AUTHORIZED]: 'Not authorized',
  [MqttDisconnectReason.TLS_BAD_FINGERPRINT]: 'TLS fingerprint invalid'
};

const getDisconnectReason = (disconnect_reason: MqttDisconnectReason): string =>
  DISCONNECT_REASONS[disconnect_reason] ?? 'Unknown';

export const mqttStatusHighlight = (
  { enabled, connected }: MqttStatusType,
  theme: Theme
) => {
  if (!enabled) return theme.palette.info.main;
  return connected ? theme.palette.success.main : theme.palette.error.main;
};

export const mqttPublishHighlight = (
  { mqtt_fails }: MqttStatusType,
  theme: Theme
) => {
  if (mqtt_fails === 0) return theme.palette.success.main;
  if (mqtt_fails < 10) return theme.palette.warning.main;
  return theme.palette.error.main;
};

export const mqttQueueHighlight = ({ mqtt_queued }: MqttStatusType, theme: Theme) =>
  mqtt_queued <= 1 ? theme.palette.success.main : theme.palette.warning.main;

interface ConnectionStatusProps {
  data: MqttStatusType;
  theme: Theme;
}

// Memoized component to prevent unnecessary re-renders when parent updates
const ConnectionStatus: FC<ConnectionStatusProps> = memo(({ data, theme }) => {
  const { LL } = useI18nContext();

  return (
    <>
      {!data.connected && (
        <>
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <ReportIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary={LL.DISCONNECT_REASON()}
              secondary={getDisconnectReason(data.disconnect_reason)}
            />
          </ListItem>
          <Divider variant="inset" component="li" />
        </>
      )}
      <ListItem>
        <ListItemAvatar>
          <Avatar>#</Avatar>
        </ListItemAvatar>
        <ListItemText primary={LL.ID_OF(LL.CLIENT())} secondary={data.client_id} />
      </ListItem>
      <Divider variant="inset" component="li" />
      <ListItem>
        <ListItemAvatar>
          <Avatar sx={{ bgcolor: mqttQueueHighlight(data, theme) }}>
            <AutoAwesomeMotionIcon />
          </Avatar>
        </ListItemAvatar>
        <ListItemText primary={LL.MQTT_QUEUE()} secondary={data.mqtt_queued} />
      </ListItem>
      <Divider variant="inset" component="li" />
      <ListItem>
        <ListItemAvatar>
          <Avatar sx={{ bgcolor: mqttPublishHighlight(data, theme) }}>
            <SpeakerNotesOffIcon />
          </Avatar>
        </ListItemAvatar>
        <ListItemText primary={LL.ERRORS_OF('MQTT')} secondary={data.mqtt_fails} />
      </ListItem>
      <Divider variant="inset" component="li" />
    </>
  );
});

const MqttStatus = () => {
  const { data, send: loadData, error } = useRequest(MqttApi.readMqttStatus);
  const { LL } = useI18nContext();
  const theme = useTheme();

  useLayoutTitle('MQTT');

  useInterval(() => {
    void loadData();
  });

  // Memoize error message separately to avoid re-renders on error object changes
  const errorMessage = error?.message || '';

  const mqttStatusText = useMemo(() => {
    if (!data) return '';
    if (!data.enabled) return LL.NOT_ENABLED();
    return data.connected
      ? `${LL.CONNECTED(0)} (${data.connect_count})`
      : `${LL.DISCONNECTED()} (${data.connect_count})`;
  }, [data, LL]);

  if (!data) {
    return (
      <SectionContent>
        <FormLoader onRetry={loadData} errorMessage={errorMessage} />
      </SectionContent>
    );
  }

  return (
    <SectionContent>
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: mqttStatusHighlight(data, theme) }}>
              <DeviceHubIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary={LL.STATUS_OF('')} secondary={mqttStatusText} />
        </ListItem>
        <Divider variant="inset" component="li" />
        {data.enabled && <ConnectionStatus data={data} theme={theme} />}
      </List>
    </SectionContent>
  );
};

export default MqttStatus;
