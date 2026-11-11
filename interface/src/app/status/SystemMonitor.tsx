import { useRef, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import { Box, Button, Typography } from '@mui/material';

import { callAction } from 'api/app';
import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import MessageBox from 'components/MessageBox';
import { clearAccessToken } from 'components/routing/authentication';
import CustomBuildWarning from 'components/upload/CustomBuildWarning';
import { useI18nContext } from 'i18n/i18n-react';
import { SystemStatusCodes } from 'types';
import { useInterval } from 'utils';

import { LinearProgressWithLabel } from '../../components/upload/LinearProgressWithLabel';

interface SystemMonitorProps {
  // when set the page isn't reloaded when done, e.g. when the firmware was installed without a restart
  onDone?: (success: boolean) => void;
  // installs the same firmware from its URL again, as a custom build
  onInstallCustom?: () => Promise<unknown>;
}

const SystemMonitor = ({ onDone, onInstallCustom }: SystemMonitorProps) => {
  const [errorMessage, setErrorMessage] = useState<string>();
  const [notOfficial, setNotOfficial] = useState(false);
  const hasInitialized = useRef(false);
  // the device reports an upload error once, then goes back to normal
  const uploadFailed = useRef(false);
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
      if (uploadFailed.current) {
        return;
      }
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
        uploadFailed.current = true;
        setErrorMessage('Please check system logs for possible causes');
      } else if (
        event.data.status === SystemStatusCodes.SYSTEM_STATUS_ERROR_NOT_OFFICIAL
      ) {
        uploadFailed.current = true;
        setErrorMessage(LL.UPLOAD_NOT_OFFICIAL());
        setNotOfficial(true);
      }
    })
    .onError((error) => {
      // the device is back but the sign-in is no longer valid, e.g. after a factory reset
      if (error.error?.message === 'Unauthorized') {
        clearAccessToken();
        document.location.href = '/';
        return;
      }
      setErrorMessage(String(error.error?.message || 'An error occurred'));
    });

  useInterval(() => {
    void send();
  }, 1000); // check every 1 second

  const status = data?.status;

  const statusMessage = uploadFailed.current
    ? 'Upload Failed'
    : status && status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING
      ? LL.WAIT_FIRMWARE()
      : status === SystemStatusCodes.SYSTEM_STATUS_PENDING_RESTART
        ? LL.APPLICATION_RESTARTING()
        : status === SystemStatusCodes.SYSTEM_STATUS_NORMAL
          ? LL.RESTARTING_PRE()
          : status === SystemStatusCodes.SYSTEM_STATUS_ERROR_UPLOAD ||
              status === SystemStatusCodes.SYSTEM_STATUS_ERROR_NOT_OFFICIAL
            ? 'Upload Failed'
            : LL.RESTARTING_POST();

  const isUploading =
    status !== undefined && status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING;
  const progressValue =
    isUploading && status
      ? Math.round(status - SystemStatusCodes.SYSTEM_STATUS_UPLOADING)
      : 0;

  const installCustom = async () => {
    if (!onInstallCustom) {
      return;
    }
    setErrorMessage(undefined);
    setNotOfficial(false);
    uploadFailed.current = false;
    await onInstallCustom().catch((error: Error) => {
      uploadFailed.current = true;
      setErrorMessage(error.message);
    });
  };

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

          {notOfficial && onInstallCustom ? (
            <>
              <CustomBuildWarning
                message={LL.UPLOAD_NOT_OFFICIAL()}
                linkText={LL.UPLOAD_NOT_OFFICIAL_HELP()}
                installText={LL.UPLOAD_INSTALL_ANYWAY()}
                onInstall={() => void installCustom()}
              />
              <Button
                sx={{ mt: 2 }}
                size="small"
                startIcon={<CancelIcon />}
                variant="outlined"
                color="secondary"
                onClick={onCancel}
              >
                {LL.CANCEL()}
              </Button>
            </>
          ) : errorMessage ? (
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
