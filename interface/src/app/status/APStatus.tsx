import ComputerIcon from '@mui/icons-material/Computer';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
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

import * as APApi from 'api/ap';

import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { APStatusType } from 'types';
import { APNetworkStatus } from 'types';
import { useInterval } from 'utils';

export const apStatusHighlight = ({ status }: APStatusType, theme: Theme) => {
  switch (status) {
    case APNetworkStatus.ACTIVE:
      return theme.palette.success.main;
    case APNetworkStatus.INACTIVE:
      return theme.palette.info.main;
    case APNetworkStatus.LINGERING:
      return theme.palette.warning.main;
    default:
      return theme.palette.warning.main;
  }
};

const APStatus = () => {
  const { data, send: loadData, error } = useRequest(APApi.readAPStatus);

  useInterval(() => {
    void loadData();
  });

  const { LL } = useI18nContext();
  useLayoutTitle(LL.ACCESS_POINT(0));

  const theme = useTheme();

  const apStatus = ({ status }: APStatusType) => {
    switch (status) {
      case APNetworkStatus.ACTIVE:
        return LL.ACTIVE();
      case APNetworkStatus.INACTIVE:
        return LL.INACTIVE(0);
      case APNetworkStatus.LINGERING:
        return 'Lingering until idle';
      default:
        return LL.UNKNOWN();
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: apStatusHighlight(data, theme) }}>
              <SettingsInputAntennaIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary={LL.STATUS_OF('')} secondary={apStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>IP</Avatar>
          </ListItemAvatar>
          <ListItemText primary={LL.ADDRESS_OF('IP')} secondary={data.ip_address} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>
              <DeviceHubIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary={LL.ADDRESS_OF('MAC')}
            secondary={data.mac_address}
          />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>
              <ComputerIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary={LL.AP_CLIENTS()} secondary={data.station_num} />
        </ListItem>
        <Divider variant="inset" component="li" />
      </List>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default APStatus;
