import { useCallback, useMemo, useRef, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import { Box, Button, Typography } from '@mui/material';

import { callAction } from 'api/app';
import { readSystemStatus } from 'api/system';

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
    <Box
      sx={{
        position: 'fixed',
        top: 0,
        left: 0,
        width: '100vw',
        height: '100vh',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        backdropFilter: 'blur(8px)'
      }}
    >
      <Box
        sx={{
          width: '30%',
          minWidth: '300px',
          maxWidth: '500px',
          backgroundColor: '#393939',
          border: 3,
          borderColor: '#565656',
          borderRadius: '8px',
          boxShadow: '0 8px 32px rgba(0, 0, 0, 0.3)',
          p: 3
        }}
      >
        <Box display="flex" alignItems="center" flexDirection="column">
          <img
            src="/app/icon.png"
            alt="EMS-ESP"
            style={{ width: '40px', height: '40px', marginBottom: '16px' }}
          />
          <Typography
            color="secondary"
            variant="h6"
            fontWeight={400}
            textAlign="center"
          >
            {statusMessage}
          </Typography>

          {errorMessage ? (
            <MessageBox level="error" message={errorMessage}>
              <Button
                sx={{ ml: 2 }}
                size="small"
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
      </Box>
    </Box>
  );
};

export default SystemMonitor;
