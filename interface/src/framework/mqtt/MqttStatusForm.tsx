import { FC } from 'react';
import { Avatar, Button, Divider, List, ListItem, ListItemAvatar, ListItemText, Theme, useTheme } from '@mui/material';

import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import RefreshIcon from '@mui/icons-material/Refresh';
import ReportIcon from '@mui/icons-material/Report';
import SpeakerNotesOffIcon from '@mui/icons-material/SpeakerNotesOff';

import { ButtonRow, FormLoader, SectionContent } from '../../components';
import { MqttStatus, MqttDisconnectReason } from '../../types';
import * as MqttApi from '../../api/mqtt';
import { useRest } from '../../utils';

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

export const mqttStatus = ({ enabled, connected }: MqttStatus) => {
  if (!enabled) {
    return 'Not enabled';
  }
  if (connected) {
    return 'Connected';
  }
  return 'Disconnected';
};

export const disconnectReason = ({ disconnect_reason }: MqttStatus) => {
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

const MqttStatusForm: FC = () => {
  const { loadData, data, errorMessage } = useRest<MqttStatus>({ read: MqttApi.readMqttStatus });

  const theme = useTheme();

  const content = () => {
    if (!data) {
      return <FormLoader loadData={loadData} errorMessage={errorMessage} />;
    }

    const renderConnectionStatus = () => {
      if (data.connected) {
        return (
          <>
            <ListItem>
              <ListItemAvatar>
                <Avatar>#</Avatar>
              </ListItemAvatar>
              <ListItemText primary="Client ID" secondary={data.client_id} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar sx={{ bgcolor: mqttPublishHighlight(data, theme) }}>
                  <SpeakerNotesOffIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText primary="MQTT Publish Errors" secondary={data.mqtt_fails} />
            </ListItem>
          </>
        );
      }
      return (
        <>
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <ReportIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="Disconnect Reason" secondary={disconnectReason(data)} />
          </ListItem>
          <Divider variant="inset" component="li" />
        </>
      );
    };

    return (
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: mqttStatusHighlight(data, theme) }}>
              <DeviceHubIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Status" secondary={mqttStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        {data.enabled && renderConnectionStatus()}
      </List>
    );
  };

  return (
    <SectionContent title="MQTT Status" titleGutter>
      {content()}
      <ButtonRow>
        <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
          Refresh
        </Button>
      </ButtonRow>
    </SectionContent>
  );
};

export default MqttStatusForm;
