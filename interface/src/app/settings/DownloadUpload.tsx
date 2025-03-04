import { useState } from 'react';
import { toast } from 'react-toastify';

import DownloadIcon from '@mui/icons-material/GetApp';
import { Box, Button, Grid2 as Grid, Typography } from '@mui/material';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';

import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import SystemMonitor from 'app/status/SystemMonitor';
import {
  FormLoader,
  SectionContent,
  SingleUpload,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { saveFile } from 'utils';

const DownloadUpload = () => {
  const { LL } = useI18nContext();

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
      toast.error(error.message);
    });

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  const doRestart = async () => {
    setRestarting(true);
    await sendAPI({ device: 'system', cmd: 'restart', id: 0 }).catch(
      (error: Error) => {
        toast.error(error.message);
      }
    );
  };

  useLayoutTitle(LL.DOWNLOAD_UPLOAD());

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <>
        <Typography sx={{ pb: 2 }} variant="h6" color="primary">
          {LL.DOWNLOAD(0)}
        </Typography>

        <Typography mb={1} variant="body1" color="warning">
          {LL.DOWNLOAD_SETTINGS_TEXT()}.
        </Typography>
        <Grid container spacing={1}>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => sendExportData('settings')}
          >
            {LL.SETTINGS_OF(LL.APPLICATION())}
          </Button>

          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => sendExportData('customizations')}
          >
            {LL.CUSTOMIZATIONS()}
          </Button>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => sendExportData('entities')}
          >
            {LL.CUSTOM_ENTITIES(0)}
          </Button>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => sendExportData('schedule')}
          >
            {LL.SCHEDULE(0)}
          </Button>
        </Grid>
        <Button
          sx={{ ml: 2, mt: 2 }}
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="primary"
          onClick={() => sendExportData('allvalues')}
        >
          {LL.ALLVALUES()}
        </Button>

        <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
          {LL.UPLOAD()}
        </Typography>

        <Box color="warning.main" sx={{ pb: 2 }}>
          <Typography variant="body1">{LL.UPLOAD_TEXT()}.</Typography>
        </Box>

        <SingleUpload text={LL.UPLOAD_DRAG()} doRestart={doRestart} />
      </>
    );
  };

  return (
    <SectionContent>{restarting ? <SystemMonitor /> : content()}</SectionContent>
  );
};

export default DownloadUpload;
