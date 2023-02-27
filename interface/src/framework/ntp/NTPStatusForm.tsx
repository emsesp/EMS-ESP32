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
  useTheme,
  Typography
} from '@mui/material';
import RefreshIcon from '@mui/icons-material/Refresh';
import AccessTimeIcon from '@mui/icons-material/AccessTime';
import SwapVerticalCircleIcon from '@mui/icons-material/SwapVerticalCircle';
import UpdateIcon from '@mui/icons-material/Update';
import DnsIcon from '@mui/icons-material/Dns';
import CancelIcon from '@mui/icons-material/Cancel';

import * as NTPApi from 'api/ntp';
import { NTPStatus, NTPSyncStatus } from 'types';
import { ButtonRow, FormLoader, SectionContent } from 'components';
import { extractErrorMessage, formatDateTime, formatLocalDateTime, useRest } from 'utils';
import { AuthenticatedContext } from 'contexts/authentication';

import { useI18nContext } from 'i18n/i18n-react';

export const isNtpActive = ({ status }: NTPStatus) => status === NTPSyncStatus.NTP_ACTIVE;
export const isNtpEnabled = ({ status }: NTPStatus) => status !== NTPSyncStatus.NTP_DISABLED;

export const ntpStatusHighlight = ({ status }: NTPStatus, theme: Theme) => {
  switch (status) {
    case NTPSyncStatus.NTP_DISABLED:
      return theme.palette.info.main;
    case NTPSyncStatus.NTP_INACTIVE:
      return theme.palette.error.main;
    case NTPSyncStatus.NTP_ACTIVE:
      return theme.palette.success.main;
    default:
      return theme.palette.error.main;
  }
};

const NTPStatusForm: FC = () => {
  const { loadData, data, errorMessage } = useRest<NTPStatus>({ read: NTPApi.readNTPStatus });
  const [localTime, setLocalTime] = useState<string>('');
  const [settingTime, setSettingTime] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);
  const { enqueueSnackbar } = useSnackbar();
  const { me } = useContext(AuthenticatedContext);

  const { LL } = useI18nContext();

  const updateLocalTime = (event: React.ChangeEvent<HTMLInputElement>) => setLocalTime(event.target.value);

  const openSetTime = () => {
    setLocalTime(formatLocalDateTime(new Date()));
    setSettingTime(true);
  };

  const theme = useTheme();

  const ntpStatus = ({ status }: NTPStatus) => {
    switch (status) {
      case NTPSyncStatus.NTP_DISABLED:
        return LL.NOT_ENABLED();
      case NTPSyncStatus.NTP_INACTIVE:
        return LL.INACTIVE(0);
      case NTPSyncStatus.NTP_ACTIVE:
        return LL.ACTIVE();
      default:
        return LL.UNKNOWN();
    }
  };

  const configureTime = async () => {
    setProcessing(true);
    try {
      await NTPApi.updateTime({
        local_time: formatLocalDateTime(new Date(localTime))
      });
      enqueueSnackbar(LL.TIME_SET(), { variant: 'success' });
      setSettingTime(false);
      loadData();
    } catch (error) {
      enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
    } finally {
      setProcessing(false);
    }
  };

  const renderSetTimeDialog = () => (
    <Dialog open={settingTime} onClose={() => setSettingTime(false)}>
      <DialogTitle>{LL.SET_TIME(1)}</DialogTitle>
      <DialogContent dividers>
        <Box color="warning.main" p={0} pl={0} pr={0} mt={0} mb={2}>
          <Typography variant="body2">{LL.SET_TIME_TEXT()}</Typography>
        </Box>
        <TextField
          label={LL.LOCAL_TIME()}
          type="datetime-local"
          value={localTime}
          onChange={updateLocalTime}
          disabled={processing}
          fullWidth
          InputLabelProps={{
            shrink: true
          }}
        />
      </DialogContent>
      <DialogActions>
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => setSettingTime(false)} color="secondary">
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<AccessTimeIcon />}
          variant="outlined"
          onClick={configureTime}
          disabled={processing}
          color="primary"
        >
          {LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );

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
            <ListItemText primary={LL.STATUS_OF('')} secondary={ntpStatus(data)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          {isNtpEnabled(data) && (
            <>
              <ListItem>
                <ListItemAvatar>
                  <Avatar>
                    <DnsIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary={LL.NTP_SERVER()} secondary={data.server} />
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
            <ListItemText primary={LL.LOCAL_TIME()} secondary={formatDateTime(data.local_time)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <SwapVerticalCircleIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.UTC_TIME()} secondary={formatDateTime(data.utc_time)} />
          </ListItem>
          <Divider variant="inset" component="li" />
        </List>
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1}>
            <ButtonRow>
              <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
                {LL.REFRESH()}
              </Button>
            </ButtonRow>
          </Box>
          {me.admin && data && !isNtpActive(data) && (
            <Box flexWrap="nowrap" whiteSpace="nowrap">
              <ButtonRow>
                <Button onClick={openSetTime} variant="outlined" color="primary" startIcon={<AccessTimeIcon />}>
                  {LL.SET_TIME(0)}
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
    <SectionContent title={LL.STATUS_OF('NTP')} titleGutter>
      {content()}
    </SectionContent>
  );
};

export default NTPStatusForm;
