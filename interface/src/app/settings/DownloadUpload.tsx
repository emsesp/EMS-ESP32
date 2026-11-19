import { useState } from 'react';
import { useBlocker } from 'react-router';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';
import { askPassword, passwordCancelledMessage } from 'api/passwordPrompt';
import { readPasswordConfirmed } from 'api/security';

import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import SystemMonitor from 'app/status/SystemMonitor';
import BlockNavigation from 'components/routing/BlockNavigation';
import {
  Button,
  ButtonLink,
  Checkbox,
  ConfirmDialog,
  FileUpload,
  Section,
  toast
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { saveBlob, saveFile } from 'utils';

import './DownloadUpload.css';
import HeatingBackupDialog from './HeatingBackupDialog';

const uploadPercent = ({ loaded, total }: { loaded: number; total: number }) =>
  total === 0 || loaded === 0
    ? 0
    : loaded <= total
      ? Math.round((loaded * 100) / total)
      : Math.round((total * 100) / loaded);

const DownloadUpload = () => {
  const { LL } = useI18nContext();

  const [confirmBackup, setConfirmBackup] = useState(false);
  const [includeSecrets, setIncludeSecrets] = useState(false);
  const [heatingBackup, setHeatingBackup] = useState(false);
  const [restarting, setRestarting] = useState(false);

  const { send: sendExportData } = useRequest(
    (type: string, secrets = false) =>
      callAction({ action: 'export', param: type, secrets }),
    { immediate: false }
  )
    .onSuccess((event) => {
      saveFile(event.data, event.args[0], '.json');
      toast.success(LL.DOWNLOAD_SUCCESSFUL());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || LL.ERROR()));
    });

  const { send: sendCoreDump } = useRequest(SystemApi.readCoreDump, {
    immediate: false
  })
    .onSuccess((event) => {
      saveBlob(event.data, 'emsesp_coredump.bin');
      toast.success(LL.DOWNLOAD_SUCCESSFUL());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || LL.ERROR()));
    });

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  const { data } = useRequest(SystemApi.readSystemStatus);

  const {
    loading: uploading,
    uploading: uploadProgress,
    send: sendUpload,
    abort: cancelUpload
  } = useRequest(SystemApi.uploadFile, { immediate: false });
  const blocker = useBlocker(uploading);

  const doRestart = async () => {
    setRestarting(true);
    try {
      await sendAPI({ device: 'system', cmd: 'restart', id: 0 });
    } catch (error) {
      toast.error((error as Error).message);
      setRestarting(false);
    }
  };

  const upload = async (file: File) => {
    // asked before sending, otherwise the whole file is sent first
    const confirmed = await readPasswordConfirmed()
      .then((state) => state.confirmed || askPassword())
      .catch(() => false);
    if (!confirmed) {
      toast.warning(passwordCancelledMessage());
      return;
    }
    try {
      await sendUpload(file);
      toast.success(LL.UPLOAD_SUCCESSFUL());
      await doRestart();
    } catch (error) {
      const message = (error as Error).message;
      if (message.includes('The user aborted a request')) {
        toast.warning(LL.UPLOAD_ABORTED());
      } else {
        toast.error(message);
      }
    }
  };

  const closeBackup = () => {
    setConfirmBackup(false);
    setIncludeSecrets(false);
  };

  const downloadBackup = () => {
    void sendExportData('systembackup', includeSecrets);
    closeBackup();
  };

  if (restarting) {
    return <SystemMonitor />;
  }

  return (
    <>
      <Section
        desc={LL.DU_SETTINGS_TEXT()}
        title={LL.DU_SETTINGS()}
        icon="settings_applications"
      >
        <div className="du-actions">
          <Button icon="download" onClick={() => setConfirmBackup(true)}>
            {LL.DOWNLOAD_SYSTEM_BACKUP()}
          </Button>
        </div>
        <div className="du-upload">
          <FileUpload
            text={LL.UPLOAD_BACKUP_TEXT()}
            accept={['.json']}
            onUpload={(file) => void upload(file)}
            progress={uploading ? uploadPercent(uploadProgress) : undefined}
            onCancel={cancelUpload}
          />
        </div>
      </Section>

      <Section desc={LL.HB_TEXT()} title={LL.HB_TITLE()} icon="thermostat">
        <div className="du-actions">
          <Button icon="download" onClick={() => setHeatingBackup(true)}>
            {LL.HB_BACKUP()}
          </Button>
          <ButtonLink
            to="/settings/downloadUpload/restore"
            color="secondary"
            icon="settings_backup_restore"
          >
            {LL.HB_RESTORE()}
          </ButtonLink>
        </div>
      </Section>

      <Section
        desc={LL.DATA_EXPORT_TEXT()}
        title={LL.DATA_EXPORT()}
        icon="data_object"
      >
        <div className="du-actions">
          <Button icon="download" onClick={() => void sendExportData('allvalues')}>
            {LL.ALLVALUES()}
          </Button>
          {data?.coredump && (
            <Button icon="download" onClick={() => void sendCoreDump()}>
              {LL.COREDUMP()}
            </Button>
          )}
        </div>
      </Section>

      <ConfirmDialog
        open={confirmBackup}
        title={LL.DOWNLOAD_SYSTEM_BACKUP()}
        label={LL.DOWNLOAD(0)}
        icon="download"
        onClose={closeBackup}
        onConfirm={downloadBackup}
      >
        <p>{LL.WARNING_SYSTEM_BACKUP()}</p>
        <Checkbox
          className="du-secrets"
          label={LL.BACKUP_INCLUDE_SECRETS()}
          checked={includeSecrets}
          onChange={setIncludeSecrets}
        />
      </ConfirmDialog>

      <HeatingBackupDialog
        open={heatingBackup}
        onClose={() => setHeatingBackup(false)}
      />

      <BlockNavigation blocker={blocker} busy />
    </>
  );
};

export default DownloadUpload;
