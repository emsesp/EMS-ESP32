import { Theme } from '@material-ui/core';
import { EMSESPStatus, busConnectionStatus } from './EMSESPtypes';

export const isConnected = ({ status }: EMSESPStatus) => status !== busConnectionStatus.BUS_STATUS_OFFLINE;

export const busStatusHighlight = ({ status }: EMSESPStatus, theme: Theme) => {

  switch (status) {
    case busConnectionStatus.BUS_STATUS_TX_ERRORS:
      return theme.palette.warning.main;
    case busConnectionStatus.BUS_STATUS_CONNECTED:
      return theme.palette.success.main;
    case busConnectionStatus.BUS_STATUS_OFFLINE:
      return theme.palette.error.main;
    default:
      return theme.palette.warning.main;
  }
}

export const busStatus = ({ status }: EMSESPStatus) => {
  switch (status) {
    case busConnectionStatus.BUS_STATUS_CONNECTED:
      return "Connected";
    case busConnectionStatus.BUS_STATUS_TX_ERRORS:
      return "Tx Errors";
    case busConnectionStatus.BUS_STATUS_OFFLINE:
      return "Disconnected";
    default:
      return "Unknown";
  }
}

export const qualityHighlight = ( value: number, theme: Theme) => {
  if (value >= 95) {
    return theme.palette.success.main;
  }

  return theme.palette.error.main;
}

