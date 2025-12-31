import { useCallback, useMemo, useState } from 'react';
import { toast } from 'react-toastify';

import DownloadIcon from '@mui/icons-material/GetApp';
import { Box, Button, Grid, Typography } from '@mui/material';

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

interface DownloadButton {
  key: string;
  type: string;
  label: string | number;
  isGridButton: boolean;
}

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
      toast.error(String(error.error?.message || 'An error occurred'));
    });

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  const doRestart = useCallback(async () => {
    setRestarting(true);
    try {
      await sendAPI({ device: 'system', cmd: 'restart', id: 0 });
    } catch (error) {
      toast.error((error as Error).message);
      setRestarting(false);
    }
  }, [sendAPI]);

  useLayoutTitle(LL.DOWNLOAD_UPLOAD());

  const downloadButtons: DownloadButton[] = useMemo(
    () => [
      {
        key: 'settings',
        type: 'settings',
        label: LL.SETTINGS_OF(LL.APPLICATION()),
        isGridButton: true
      },
      {
        key: 'customizations',
        type: 'customizations',
        label: LL.CUSTOMIZATIONS(),
        isGridButton: true
      },
      {
        key: 'entities',
        type: 'entities',
        label: LL.CUSTOM_ENTITIES(0),
        isGridButton: true
      },
      {
        key: 'schedule',
        type: 'schedule',
        label: LL.SCHEDULE(0),
        isGridButton: true
      },
      {
        key: 'allvalues',
        type: 'allvalues',
        label: LL.ALLVALUES(),
        isGridButton: false
      }
    ],
    [LL]
  );

  const handleDownload = useCallback(
    (type: string) => () => {
      void sendExportData(type);
    },
    [sendExportData]
  );

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

  const gridButtons = downloadButtons.filter((btn) => btn.isGridButton);
  const standaloneButton = downloadButtons.find((btn) => !btn.isGridButton);

  return (
    <SectionContent>
      <Typography sx={{ pb: 2 }} variant="h6" color="primary">
        {LL.DOWNLOAD(0)}
      </Typography>

      <Typography mb={1} variant="body1" color="warning">
        {LL.DOWNLOAD_SETTINGS_TEXT()}.
      </Typography>

      <Grid container spacing={2}>
        {gridButtons.map((button) => (
          <Grid key={button.key}>
            <Button
              startIcon={<DownloadIcon />}
              variant="outlined"
              color="primary"
              onClick={handleDownload(button.type)}
            >
              {button.label}
            </Button>
          </Grid>
        ))}
      </Grid>

      <Typography mt={2} mb={1} variant="body1" color="warning">
        {LL.DOWNLOAD_SETTINGS_TEXT2()}.
      </Typography>

      {standaloneButton && (
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="primary"
          onClick={handleDownload(standaloneButton.type)}
        >
          {standaloneButton.label}
        </Button>
      )}

      <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
        {LL.UPLOAD()}
      </Typography>

      <Box color="warning.main" sx={{ pb: 2 }}>
        <Typography variant="body1">{LL.UPLOAD_TEXT()}.</Typography>
      </Box>

      <SingleUpload text={LL.UPLOAD_DRAG()} doRestart={doRestart} />
    </SectionContent>
  );
};

export default DownloadUpload;
