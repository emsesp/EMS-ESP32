import { useCallback, useMemo, useRef, useState } from 'react';

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
  const hasInitialized = useRef(false);
  const { LL } = useI18nContext();

  const { send: setSystemStatus } = useRequest(
    (status: string) => callAction({ action: 'systemStatus', param: status }),
    {
      immediate: false
    }
  );

  const { data, send } = useRequest(readSystemStatus, {
    force: true,
    async middleware(_, next) {
      // Skip first request to allow AsyncWS to send its response
      if (!hasInitialized.current) {
        hasInitialized.current = true;
        return; // Don't await next() on first call
      }
      await next();
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
      setErrorMessage(String(error.error?.message || 'An error occurred'));
    });

  useInterval(() => {
    void send();
  }, 1000); // check every 1 second

  const { statusMessage, isUploading, progressValue } = useMemo(() => {
    const status = data?.status;

    let message = '';
    if (status && status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING) {
      message = LL.WAIT_FIRMWARE();
    } else if (status === SystemStatusCodes.SYSTEM_STATUS_PENDING_RESTART) {
      message = LL.APPLICATION_RESTARTING();
    } else if (status === SystemStatusCodes.SYSTEM_STATUS_NORMAL) {
      message = LL.RESTARTING_PRE();
    } else if (status === SystemStatusCodes.SYSTEM_STATUS_ERROR_UPLOAD) {
      message = 'Upload Failed';
    } else {
      message = LL.RESTARTING_POST();
    }

    const uploading =
      status !== undefined && status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING;
    const progress =
      uploading && status
        ? Math.round(status - SystemStatusCodes.SYSTEM_STATUS_UPLOADING)
        : 0;

    return {
      statusMessage: message,
      isUploading: uploading,
      progressValue: progress
    };
  }, [data?.status, LL]);

  const onCancel = useCallback(async () => {
    setErrorMessage(undefined);
    await setSystemStatus(String(SystemStatusCodes.SYSTEM_STATUS_NORMAL));
    document.location.href = '/';
  }, [setSystemStatus]);

  return (
    <Dialog
      fullWidth={false}
      maxWidth="sm"
      sx={{ ...dialogStyle, '& .MuiDialog-paper': { width: '30%' } }}
      open={true}
    >
      <DialogContent dividers>
        <Box m={0} py={0} display="flex" alignItems="center" flexDirection="column">
          <Typography
            color="secondary"
            variant="h6"
            fontWeight={400}
            textAlign="center"
          >
            {statusMessage}
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
                {LL.RESTART()}
              </Button>
            </MessageBox>
          ) : (
            <>
              <Typography mt={2} variant="h6" fontWeight={400} textAlign="center">
                {LL.PLEASE_WAIT()}&hellip;
              </Typography>
              {isUploading && (
                <Box width="100%" pl={2} pr={2} py={2}>
                  <LinearProgressWithLabel value={progressValue} />
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
