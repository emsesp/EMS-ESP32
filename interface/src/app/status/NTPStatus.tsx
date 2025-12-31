import { useMemo } from 'react';

import AccessTimeIcon from '@mui/icons-material/AccessTime';
import DnsIcon from '@mui/icons-material/Dns';
import SwapVerticalCircleIcon from '@mui/icons-material/SwapVerticalCircle';
import UpdateIcon from '@mui/icons-material/Update';
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

import * as NTPApi from 'api/ntp';

import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { NTPStatusType } from 'types';
import { NTPSyncStatus } from 'types';
import { useInterval } from 'utils';
import { formatDateTime } from 'utils';

// Utility functions
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

const NTPStatus = () => {
  const { data, send: loadData, error } = useRequest(NTPApi.readNTPStatus);

  useInterval(() => {
    void loadData();
  });

  const { LL } = useI18nContext();
  useLayoutTitle('NTP');

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

  const content = useMemo(() => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />;
    }

    return (
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
    );
  }, [data, error, loadData, LL, theme]);

  return <SectionContent>{content}</SectionContent>;
};

export default NTPStatus;
