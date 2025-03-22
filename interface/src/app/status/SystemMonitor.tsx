import { useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import { Box, Button, Dialog, DialogContent, Typography } from '@mui/material';

import { callAction } from 'api/app';
import { readSystemStatus } from 'api/system';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import MessageBox from 'components/MessageBox';
import { useI18nContext } from 'i18n/i18n-react';
import { SystemStatusCodes } from 'types';
import { useInterval } from 'utils';

import { LinearProgressWithLabel } from '../../components/upload/LinearProgressWithLabel';

const SystemMonitor = () => {
  const [errorMessage, setErrorMessage] = useState<string>();

  const { LL } = useI18nContext();

  let count = 0;

  const { send: setSystemStatus } = useRequest(
    (status: string) => callAction({ action: 'systemStatus', param: status }),
    {
      immediate: false
    }
  );

  const { data, send } = useRequest(readSystemStatus, {
    force: true,
    async middleware(_, next) {
      if (count++ >= 1) {
        // skip first request (1 second) to allow AsyncWS to send its response
        await next();
      }
    }
  })
    .onSuccess((event) => {
      if (
        event.data.status === SystemStatusCodes.SYSTEM_STATUS_NORMAL ||
        event.data.status === undefined
      ) {
        document.location.href = '/';
      } else if (
        event.data.status === SystemStatusCodes.SYSTEM_STATUS_ERROR_UPLOAD
      ) {
        setErrorMessage('Please check system logs for possible causes');
      }
    })
    .onError((error) => {
      setErrorMessage(error.message);
    });

  useInterval(() => {
    void send();
  }, 1000); // check every 1 second

  const onCancel = async () => {
    setErrorMessage(undefined);
    await setSystemStatus(
      SystemStatusCodes.SYSTEM_STATUS_NORMAL as unknown as string
    );
    document.location.href = '/';
  };

  return (
    <Dialog fullWidth={true} sx={dialogStyle} open={true}>
      <DialogContent dividers>
        <Box m={0} py={0} display="flex" alignItems="center" flexDirection="column">
          <Typography
            color="secondary"
            variant="h6"
            fontWeight={400}
            textAlign="center"
          >
            {data?.status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING
              ? LL.WAIT_FIRMWARE()
              : data?.status === SystemStatusCodes.SYSTEM_STATUS_PENDING_RESTART
                ? LL.APPLICATION_RESTARTING()
                : data?.status === SystemStatusCodes.SYSTEM_STATUS_NORMAL
                  ? LL.RESTARTING_PRE()
                  : data?.status === SystemStatusCodes.SYSTEM_STATUS_ERROR_UPLOAD
                    ? 'Upload Failed'
                    : LL.RESTARTING_POST()}
          </Typography>

          {errorMessage ? (
            <MessageBox my={2} level="error" message={errorMessage}>
              <Button
                size="small"
                sx={{ ml: 2 }}
                startIcon={<CancelIcon />}
                variant="contained"
                color="error"
                onClick={onCancel}
              >
                {LL.RESET(0)}
              </Button>
            </MessageBox>
          ) : (
            <>
              <Typography mt={2} variant="h6" fontWeight={400} textAlign="center">
                {LL.PLEASE_WAIT()}&hellip;
              </Typography>
              {data && data.status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING && (
                <Box width="100%" pl={2} pr={2} py={2}>
                  <LinearProgressWithLabel
                    value={Math.round(
                      data?.status - SystemStatusCodes.SYSTEM_STATUS_UPLOADING
                    )}
                  />
                </Box>
              )}
            </>
          )}
        </Box>
      </DialogContent>
    </Dialog>
  );
};

export default SystemMonitor;
