import AutoAwesomeMotionIcon from '@mui/icons-material/AutoAwesomeMotion';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import RefreshIcon from '@mui/icons-material/Refresh';
import ReportIcon from '@mui/icons-material/Report';
import SpeakerNotesOffIcon from '@mui/icons-material/SpeakerNotesOff';
import { Avatar, Button, Divider, List, ListItem, ListItemAvatar, ListItemText, useTheme } from '@mui/material';
import type { Theme } from '@mui/material';
import type { FC } from 'react';

import type { MqttStatus } from 'types';
import * as MqttApi from 'api/mqtt';
import { ButtonRow, FormLoader, SectionContent } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { MqttDisconnectReason } from 'types';
import { useRest } from 'utils';

export const mqttStatusHighlight = ({ enabled, connected }: MqttStatus, theme: Theme) => {
  if (!enabled) {
    return theme.palette.info.main;
  }
  if (connected) {
    return theme.palette.success.main;
  }
  return theme.palette.error.main;
};

export const mqttPublishHighlight = ({ mqtt_fails }: MqttStatus, theme: Theme) => {
  if (mqtt_fails === 0) return theme.palette.success.main;
  if (mqtt_fails < 10) return theme.palette.warning.main;

  return theme.palette.error.main;
};

export const mqttQueueHighlight = ({ mqtt_queued }: MqttStatus, theme: Theme) => {
  if (mqtt_queued <= 1) return theme.palette.success.main;

  return theme.palette.warning.main;
};

const MqttStatusForm: FC = () => {
  // TODO replace with   const { data: data, send: loadData, error } = useRequest(APApi.readAPStatus());
  const { loadData, data, errorMessage } = useRest<MqttStatus>({ read: MqttApi.readMqttStatus });

  const { LL } = useI18nContext();

  const theme = useTheme();

  const mqttStatus = ({ enabled, connected, connect_count }: MqttStatus) => {
    if (!enabled) {
      return LL.NOT_ENABLED();
    }
    if (connected) {
      return LL.CONNECTED(0) + (connect_count > 1 ? ' (' + connect_count + ')' : '');
    }
    return LL.DISCONNECTED() + (connect_count > 1 ? ' (' + connect_count + ')' : '');
  };

  const disconnectReason = ({ disconnect_reason }: MqttStatus) => {
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
      case MqttDisconnectReason.ESP8266_NOT_ENOUGH_SPACE:
        return 'Device out of memory';
      case MqttDisconnectReason.TLS_BAD_FINGERPRINT:
        return 'Server fingerprint invalid';
      default:
        return 'Unknown';
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
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
              <ListItemText primary={LL.DISCONNECT_REASON()} secondary={disconnectReason(data)} />
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
      <>
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
        <ButtonRow>
          <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
            {LL.REFRESH()}
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title={LL.STATUS_OF('MQTT')} titleGutter>
      {content()}
    </SectionContent>
  );
};

export default MqttStatusForm;
