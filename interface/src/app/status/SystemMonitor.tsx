import { useRef, useState } from 'react';

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

interface SystemMonitorProps {
  // when set the page isn't reloaded when done, e.g. when the firmware was installed without a restart
  onDone?: (success: boolean) => void;
}

const SystemMonitor = ({ onDone }: SystemMonitorProps) => {
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
        if (onDone) {
          onDone(true);
        } else {
          document.location.href = '/';
        }
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

  const status = data?.status;

  const statusMessage =
    status && status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING
      ? LL.WAIT_FIRMWARE()
      : status === SystemStatusCodes.SYSTEM_STATUS_PENDING_RESTART
        ? LL.APPLICATION_RESTARTING()
        : status === SystemStatusCodes.SYSTEM_STATUS_NORMAL
          ? LL.RESTARTING_PRE()
          : status === SystemStatusCodes.SYSTEM_STATUS_ERROR_UPLOAD
            ? 'Upload Failed'
            : LL.RESTARTING_POST();

  const isUploading =
    status !== undefined && status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING;
  const progressValue =
    isUploading && status
      ? Math.round(status - SystemStatusCodes.SYSTEM_STATUS_UPLOADING)
      : 0;

  const onCancel = async () => {
    setErrorMessage(undefined);
    await setSystemStatus(String(SystemStatusCodes.SYSTEM_STATUS_NORMAL));
    if (onDone) {
      onDone(false);
    } else {
      document.location.href = '/';
    }
  };

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
        justifyContent: 'center'
        // backdropFilter: 'blur(8px)'
      }}
    >
      <Box
        sx={{
          width: '30%',
          minWidth: '300px',
          maxWidth: '500px',
          backgroundColor: '#393939',
          border: 2,
          borderColor: '#565656',
          borderRadius: '8px',
          boxShadow: '0 8px 32px rgba(0, 0, 0, 0.3)',
          p: 3
        }}
      >
        <Box sx={{ display: 'flex', alignItems: 'center', flexDirection: 'column' }}>
          <img
            src="/app/icon.png"
            alt="EMS-ESP"
            style={{ width: '40px', height: '40px', marginBottom: '16px' }}
          />
          <Typography
            sx={{ color: 'secondary', fontWeight: 400, textAlign: 'center' }}
            variant="h6"
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
              <Typography
                sx={{ mt: 2, fontWeight: 400, textAlign: 'center' }}
                variant="h6"
              >
                {LL.PLEASE_WAIT()}&hellip;
              </Typography>
              {isUploading && (
                <>
                  <Box sx={{ width: '100%', pl: 2, pr: 2, py: 2 }}>
                    <LinearProgressWithLabel value={progressValue} />
                  </Box>

                  <Button
                    sx={{ ml: 2, mt: 2 }}
                    startIcon={<CancelIcon />}
                    variant="outlined"
                    color="secondary"
                    onClick={onCancel}
                  >
                    {LL.CANCEL()}
                  </Button>
                </>
              )}
            </>
          )}
        </Box>
      </Box>
    </Box>
  );
};

export default SystemMonitor;
