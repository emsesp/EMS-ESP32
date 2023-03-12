import { FC } from 'react';

import { Avatar, Button, Divider, List, ListItem, ListItemAvatar, ListItemText, Theme, useTheme } from '@mui/material';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import ComputerIcon from '@mui/icons-material/Computer';
import RefreshIcon from '@mui/icons-material/Refresh';

import * as APApi from 'api/ap';
import { APNetworkStatus, APStatus } from 'types';
import { ButtonRow, FormLoader, SectionContent } from 'components';
import { useRest } from 'utils';

import { useI18nContext } from 'i18n/i18n-react';

export const apStatusHighlight = ({ status }: APStatus, theme: Theme) => {
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

const APStatusForm: FC = () => {
  const { loadData, data, errorMessage } = useRest<APStatus>({ read: APApi.readAPStatus });

  const { LL } = useI18nContext();

  const theme = useTheme();

  const apStatus = ({ status }: APStatus) => {
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
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
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
            <ListItemText primary={LL.ADDRESS_OF('MAC')} secondary={data.mac_address} />
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
        <ButtonRow>
          <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
            {LL.REFRESH()}
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title={LL.STATUS_OF(LL.ACCESS_POINT(1))} titleGutter>
      {content()}
    </SectionContent>
  );
};

export default APStatusForm;
