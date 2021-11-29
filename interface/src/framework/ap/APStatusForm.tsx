import { FC } from 'react';
import { Avatar, Button, Divider, List, ListItem, ListItemAvatar, ListItemText, Theme, useTheme } from '@mui/material';

import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import ComputerIcon from '@mui/icons-material/Computer';
import RefreshIcon from '@mui/icons-material/Refresh';

import { ButtonRow, FormLoader, SectionContent } from '../../components';
import { APNetworkStatus, APStatus } from '../../types';
import * as APApi from '../../api/ap';
import { useRest } from '../../utils';

// TODO - Move some of the below to 'types' as required

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

export const apStatus = ({ status }: APStatus) => {
  switch (status) {
    case APNetworkStatus.ACTIVE:
      return 'Active';
    case APNetworkStatus.INACTIVE:
      return 'Inactive';
    case APNetworkStatus.LINGERING:
      return 'Lingering until idle';
    default:
      return 'Unknown';
  }
};

const APStatusForm: FC = () => {
  const { loadData, data, errorMessage } = useRest<APStatus>({ read: APApi.readAPStatus });

  const theme = useTheme();

  const content = () => {
    if (!data) {
      return <FormLoader loadData={loadData} errorMessage={errorMessage} />;
    }

    return (
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: apStatusHighlight(data, theme) }}>
              <SettingsInputAntennaIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Status" secondary={apStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>IP</Avatar>
          </ListItemAvatar>
          <ListItemText primary="IP Address" secondary={data.ip_address} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>
              <DeviceHubIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="MAC Address" secondary={data.mac_address} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>
              <ComputerIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="AP Clients" secondary={data.station_num} />
        </ListItem>
        <Divider variant="inset" component="li" />
      </List>
    );
  };

  return (
    <SectionContent title="Access Point Status" titleGutter>
      {content()}
      <ButtonRow>
        <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
          Refresh
        </Button>
      </ButtonRow>
    </SectionContent>
  );
};

export default APStatusForm;
