import { useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import DownloadIcon from '@mui/icons-material/GetApp';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  Typography
} from '@mui/material';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';

import { dialogStyle } from '@/CustomTheme';
import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import SystemMonitor from 'app/status/SystemMonitor';
import {
  FormLoader,
  SectionContent,
  SingleUpload,
  useLayoutTitle
} from 'components';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';
import { saveFile } from 'utils';

const DownloadUpload = () => {
  const { LL } = useI18nContext();

  const [confirmBackup, setConfirmBackup] = useState<boolean>(false);

  const [restarting, setRestarting] = useState<boolean>(false);

  const { send: sendExportData } = useRequest(
    (type: string) => callAction({ action: 'export', param: type }),
    {
      immediate: false
    }
  )
    .onSuccess((event) => {
      saveFile(event.data, event.args[0], '.json');
      toast.info(LL.DOWNLOAD_SUCCESSFUL());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || 'An error occurred'));
    });

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  const doRestart = async () => {
    setRestarting(true);
    try {
      await sendAPI({ device: 'system', cmd: 'restart', id: 0 });
    } catch (error) {
      toast.error((error as Error).message);
      setRestarting(false);
    }
  };

  useLayoutTitle(LL.DOWNLOAD_UPLOAD());

  const handleCloseBackupDialog = () => {
    setConfirmBackup(false);
  };

  const handleDownload = (type: string) => () => {
    void sendExportData(type);
    setConfirmBackup(false);
  };

  if (restarting) {
    return <SystemMonitor />;
  }

  if (!data) {
    return (
      <SectionContent>
        <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />
      </SectionContent>
    );
  }

  return (
    <SectionContent>
      <Dialog
        sx={dialogStyle}
        open={confirmBackup}
        onClose={handleCloseBackupDialog}
      >
        <DialogTitle>{LL.DOWNLOAD_SYSTEM_BACKUP()}</DialogTitle>
        <DialogContent dividers>
          <WarningIcon color="warning" sx={{ fontSize: 18 }} />
          &nbsp;
          {LL.WARNING_SYSTEM_BACKUP()}
        </DialogContent>
        <DialogActions>
          <Button
            startIcon={<CancelIcon />}
            variant="outlined"
            onClick={handleCloseBackupDialog}
            color="secondary"
          >
            {LL.CANCEL()}
          </Button>
          <Button
            startIcon={<DownloadIcon />}
            variant="outlined"
            onClick={handleDownload('systembackup')}
            color="primary"
          >
            {LL.DOWNLOAD(0)}
          </Button>
        </DialogActions>
      </Dialog>

      <Typography sx={{ pb: 2 }} variant="h6" color="primary">
        {LL.DOWNLOAD(0)}
      </Typography>

      <Grid
        container
        spacing={2}
        sx={{
          alignItems: 'center'
        }}
      >
        <Typography variant="body1" color="warning">
          {LL.DOWNLOAD_SETTINGS_TEXT()}:
        </Typography>
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="primary"
          onClick={() => setConfirmBackup(true)}
        >
          {LL.DOWNLOAD_SYSTEM_BACKUP()}
        </Button>
      </Grid>

      <Grid container spacing={2} sx={{ mt: 2, alignItems: 'center' }}>
        <Typography variant="body1" color="warning">
          {LL.DOWNLOAD_SETTINGS_TEXT2()}:
        </Typography>
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="primary"
          onClick={handleDownload('allvalues')}
        >
          {LL.ALLVALUES()}
        </Button>
      </Grid>

      <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
        {LL.UPLOAD()}
      </Typography>

      <SingleUpload text={LL.UPLOAD_BACKUP_TEXT()} doRestart={doRestart} />
    </SectionContent>
  );
};

export default DownloadUpload;
