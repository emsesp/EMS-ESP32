import { useState } from 'react';
import { toast } from 'react-toastify';

import AccessTimeIcon from '@mui/icons-material/AccessTime';
import CancelIcon from '@mui/icons-material/Cancel';
import DnsIcon from '@mui/icons-material/Dns';
import SwapVerticalCircleIcon from '@mui/icons-material/SwapVerticalCircle';
import UpdateIcon from '@mui/icons-material/Update';
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
  Typography,
  useTheme
} from '@mui/material';
import type { Theme } from '@mui/material';

import * as NTPApi from 'api/ntp';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import { ButtonRow, FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { NTPStatusType, Time } from 'types';
import { NTPSyncStatus } from 'types';
import { useInterval } from 'utils';
import { formatDateTime, formatLocalDateTime } from 'utils';

const NTPStatus = () => {
  const { data, send: loadData, error } = useRequest(NTPApi.readNTPStatus);

  useInterval(() => {
    void loadData();
  });

  const [localTime, setLocalTime] = useState<string>('');
  const [settingTime, setSettingTime] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);

  const { LL } = useI18nContext();
  useLayoutTitle('NTP');

  const { send: updateTime } = useRequest(
    (local_time: Time) => NTPApi.updateTime(local_time),
    {
      immediate: false
    }
  );

  NTPApi.updateTime;

  const isNtpActive = ({ status }: NTPStatusType) =>
    status === NTPSyncStatus.NTP_ACTIVE;
  const isNtpEnabled = ({ status }: NTPStatusType) =>
    status !== NTPSyncStatus.NTP_DISABLED;

  const ntpStatusHighlight = ({ status }: NTPStatusType, theme: Theme) => {
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

  const updateLocalTime = (event: React.ChangeEvent<HTMLInputElement>) =>
    setLocalTime(event.target.value);

  const openSetTime = () => {
    setLocalTime(formatLocalDateTime(new Date()));
    setSettingTime(true);
  };

  const theme = useTheme();

  const ntpStatus = ({ status }: NTPStatusType) => {
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

    await updateTime({ local_time: formatLocalDateTime(new Date(localTime)) })
      .then(async () => {
        toast.success(LL.TIME_SET());
        setSettingTime(false);
        await loadData();
      })
      .catch(() => {
        toast.error(LL.PROBLEM_UPDATING());
      })
      .finally(() => {
        setProcessing(false);
      });
  };

  const renderSetTimeDialog = () => (
    <Dialog
      sx={dialogStyle}
      open={settingTime}
      onClose={() => setSettingTime(false)}
    >
      <DialogTitle>{LL.SET_TIME(1)}</DialogTitle>
      <DialogContent dividers>
        <Box color="warning.main" p={0} pl={0} pr={0} mt={0} mb={2}>
          <Typography variant="body2">{LL.SET_TIME_TEXT()}</Typography>
        </Box>
        <TextField
          label={LL.LOCAL_TIME(0)}
          type="datetime-local"
          value={localTime}
          onChange={updateLocalTime}
          disabled={processing}
          fullWidth
          slotProps={{
            inputLabel: {
              shrink: true
            }
          }}
        />
      </DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={() => setSettingTime(false)}
          color="secondary"
        >
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
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
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
            <ListItemText
              primary={LL.LOCAL_TIME(0)}
              secondary={formatDateTime(data.local_time)}
            />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <SwapVerticalCircleIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary={LL.UTC_TIME()}
              secondary={formatDateTime(data.utc_time)}
            />
          </ListItem>
          <Divider variant="inset" component="li" />
        </List>
        <Box display="flex" flexWrap="wrap">
          {data && !isNtpActive(data) && (
            <Box flexWrap="nowrap" whiteSpace="nowrap">
              <ButtonRow>
                <Button
                  onClick={openSetTime}
                  variant="outlined"
                  color="primary"
                  startIcon={<AccessTimeIcon />}
                >
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

  return <SectionContent>{content()}</SectionContent>;
};

export default NTPStatus;
