import { useRef, useState } from 'react';

import { callAction } from 'api/app';
import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import { clearAccessToken } from 'components/routing/authentication';
import { Button, Message, ProgressBar } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { SystemStatusCodes } from 'types';
import { CUSTOM_BUILD_URL, useInterval } from 'utils';

import './SystemMonitor.css';

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
    { immediate: false }
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
        setErrorMessage(LL.CHECK_SYSTEM_LOG());
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
      setErrorMessage(String(error.error?.message || LL.ERROR()));
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
  const progress =
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

  const body = () => {
    if (notOfficial && onInstallCustom) {
      return (
        <>
          <Message level="warning">
            {LL.UPLOAD_NOT_OFFICIAL()}.{' '}
            <a href={CUSTOM_BUILD_URL} target="_blank" rel="noreferrer">
              {LL.UPLOAD_NOT_OFFICIAL_HELP()}
            </a>
          </Message>
          <div className="sysmon-actions">
            <Button color="neutral" icon="cancel" onClick={() => void onCancel()}>
              {LL.CANCEL()}
            </Button>
            <Button
              color="warning"
              icon="warning"
              onClick={() => void installCustom()}
            >
              {LL.UPLOAD_INSTALL_ANYWAY()}
            </Button>
          </div>
        </>
      );
    }
    if (errorMessage) {
      return (
        <Message
          level="error"
          actions={
            <Button
              size="sm"
              color="warning"
              icon="restart_alt"
              onClick={() => void onCancel()}
            >
              {LL.RESTART()}
            </Button>
          }
        >
          {errorMessage}
        </Message>
      );
    }
    return (
      <>
        <p className="sysmon-wait">{LL.PLEASE_WAIT()}&hellip;</p>
        {isUploading && (
          <>
            <div className="sysmon-progress">
              <ProgressBar value={progress} label={LL.WAIT_FIRMWARE()} />
              <span className="cap">{progress}%</span>
            </div>
            <div className="sysmon-actions">
              <Button
                color="neutral"
                icon="cancel"
                onClick={() => void onCancel()}
              >
                {LL.CANCEL()}
              </Button>
            </div>
          </>
        )}
      </>
    );
  };

  return (
    <div className="sysmon" role="status" aria-live="polite">
      <div className="sysmon-box">
        <img src="/app/icon.png" alt="" width={40} height={40} />
        <h2 className="sysmon-title">{statusMessage}</h2>
        {body()}
      </div>
    </div>
  );
};

export default SystemMonitor;
