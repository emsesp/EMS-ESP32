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

export const mqttStatusHighlight = (
  { enabled, connected }: MqttStatusType,
  theme: Theme
) => {
  if (!enabled) {
    return theme.palette.info.main;
  }
  if (connected) {
    return theme.palette.success.main;
  }
  return theme.palette.error.main;
};

export const mqttPublishHighlight = (
  { mqtt_fails }: MqttStatusType,
  theme: Theme
) => {
  if (mqtt_fails === 0) return theme.palette.success.main;
  if (mqtt_fails < 10) return theme.palette.warning.main;

  return theme.palette.error.main;
};

export const mqttQueueHighlight = (
  { mqtt_queued }: MqttStatusType,
  theme: Theme
) => {
  if (mqtt_queued <= 1) return theme.palette.success.main;

  return theme.palette.warning.main;
};

const MqttStatus = () => {
  const { data, send: loadData, error } = useRequest(MqttApi.readMqttStatus);

  useInterval(() => {
    void loadData();
  });

  const { LL } = useI18nContext();
  useLayoutTitle('MQTT');

  const theme = useTheme();

  const mqttStatus = ({ enabled, connected, connect_count }: MqttStatusType) => {
    if (!enabled) {
      return LL.NOT_ENABLED();
    }
    if (connected) {
      return LL.CONNECTED(0) + ' (' + connect_count + ')';
    }
    return LL.DISCONNECTED() + ' (' + connect_count + ')';
  };

  const disconnectReason = ({ disconnect_reason }: MqttStatusType) => {
    switch (disconnect_reason) {
      case MqttDisconnectReason.TCP_DISCONNECTED:
        return 'TCP disconnected';
      case MqttDisconnectReason.MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
        return 'Unacceptable protocol version';
      case MqttDisconnectReason.MQTT_IDENTIFIER_REJECTED:
        return 'Client ID rejected';
      case MqttDisconnectReason.MQTT_SERVER_UNAVAILABLE:
        return 'Server unavailable';
      case MqttDisconnectReason.MQTT_MALFORMED_CREDENTIALS:
        return 'Malformed credentials';
      case MqttDisconnectReason.MQTT_NOT_AUTHORIZED:
        return 'Not authorized';
      case MqttDisconnectReason.TLS_BAD_FINGERPRINT:
        return 'TLS fingerprint invalid';
      default:
        return 'Unknown';
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    const renderConnectionStatus = () => (
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
                secondary={disconnectReason(data)}
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

    return (
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: mqttStatusHighlight(data, theme) }}>
              <DeviceHubIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary={LL.STATUS_OF('')} secondary={mqttStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        {data.enabled && renderConnectionStatus()}
      </List>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default MqttStatus;
