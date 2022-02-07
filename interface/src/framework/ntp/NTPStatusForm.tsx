import { FC, useContext, useState } from 'react';
import { useSnackbar } from 'notistack';

import {
  Avatar,
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  TextField,
  Theme,
  useTheme
} from '@mui/material';
import RefreshIcon from '@mui/icons-material/Refresh';
import AccessTimeIcon from '@mui/icons-material/AccessTime';
import SwapVerticalCircleIcon from '@mui/icons-material/SwapVerticalCircle';
import UpdateIcon from '@mui/icons-material/Update';
import DnsIcon from '@mui/icons-material/Dns';
import CancelIcon from '@mui/icons-material/Cancel';

import * as NTPApi from '../../api/ntp';
import { NTPStatus, NTPSyncStatus } from '../../types';
import { ButtonRow, FormLoader, SectionContent } from '../../components';
import { extractErrorMessage, formatDateTime, formatLocalDateTime, useRest } from '../../utils';
import { AuthenticatedContext } from '../../contexts/authentication';

export const isNtpActive = ({ status }: NTPStatus) => status === NTPSyncStatus.NTP_ACTIVE;

export const ntpStatusHighlight = ({ status }: NTPStatus, theme: Theme) => {
  switch (status) {
    case NTPSyncStatus.NTP_INACTIVE:
      return theme.palette.info.main;
    case NTPSyncStatus.NTP_ACTIVE:
      return theme.palette.success.main;
    default:
      return theme.palette.error.main;
  }
};

export const ntpStatus = ({ status }: NTPStatus) => {
  switch (status) {
    case NTPSyncStatus.NTP_INACTIVE:
      return 'Inactive';
    case NTPSyncStatus.NTP_ACTIVE:
      return 'Active';
    default:
      return 'Unknown';
  }
};

const NTPStatusForm: FC = () => {
  const { loadData, data, errorMessage } = useRest<NTPStatus>({ read: NTPApi.readNTPStatus });
  const [localTime, setLocalTime] = useState<string>('');
  const [settingTime, setSettingTime] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);
  const { enqueueSnackbar } = useSnackbar();
  const { me } = useContext(AuthenticatedContext);

  const updateLocalTime = (event: React.ChangeEvent<HTMLInputElement>) => setLocalTime(event.target.value);

  const openSetTime = () => {
    setLocalTime(formatLocalDateTime(new Date()));
    setSettingTime(true);
  };

  const theme = useTheme();

  const configureTime = async () => {
    setProcessing(true);
    try {
      await NTPApi.updateTime({
        local_time: formatLocalDateTime(new Date(localTime))
      });
      enqueueSnackbar('Time set', { variant: 'success' });
      setSettingTime(false);
      loadData();
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem updating time'), { variant: 'error' });
    } finally {
      setProcessing(false);
    }
  };

  const renderSetTimeDialog = () => {
    return (
      <Dialog open={settingTime} onClose={() => setSettingTime(false)}>
        <DialogTitle>Set Time</DialogTitle>
        <DialogContent dividers>
          <Box mb={2}>Enter local date and time below to set the device's time.</Box>
          <TextField
            label="Local Time"
            type="datetime-local"
            value={localTime}
            onChange={updateLocalTime}
            disabled={processing}
            variant="outlined"
            fullWidth
            InputLabelProps={{
              shrink: true
            }}
          />
        </DialogContent>
        <DialogActions>
          <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => setSettingTime(false)} color="secondary">
            Cancel
          </Button>
          <Button
            startIcon={<AccessTimeIcon />}
            variant="outlined"
            onClick={configureTime}
            disabled={processing}
            color="primary"
            autoFocus
          >
            Set Time
          </Button>
        </DialogActions>
      </Dialog>
    );
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
              <Avatar sx={{ bgcolor: ntpStatusHighlight(data, theme) }}>
                <UpdateIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="Status" secondary={ntpStatus(data)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          {isNtpActive(data) && (
            <>
              <ListItem>
                <ListItemAvatar>
                  <Avatar>
                    <DnsIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary="NTP Server" secondary={data.server} />
              </ListItem>
              <Divider variant="inset" component="li" />
            </>
          )}
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <AccessTimeIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="Local Time" secondary={formatDateTime(data.local_time)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <SwapVerticalCircleIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="UTC Time" secondary={formatDateTime(data.utc_time)} />
          </ListItem>
          <Divider variant="inset" component="li" />
        </List>
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1}>
            <ButtonRow>
              <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
                Refresh
              </Button>
            </ButtonRow>
          </Box>
          {me.admin && data && !isNtpActive(data) && (
            <Box flexWrap="nowrap" whiteSpace="nowrap">
              <ButtonRow>
                <Button onClick={openSetTime} variant="outlined" color="primary" startIcon={<AccessTimeIcon />}>
                  Set Time
                </Button>
              </ButtonRow>
            </Box>
          )}
        </Box>
        {renderSetTimeDialog()}
      </>
    );
  };

  return (
    <SectionContent title="NTP Status" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default NTPStatusForm;
