import { useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import DownloadIcon from '@mui/icons-material/GetApp';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Link,
  Typography
} from '@mui/material';
import Grid from '@mui/material/Grid2';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';
import { getDevVersion, getStableVersion } from 'api/system';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import RestartMonitor from 'app/status/RestartMonitor';
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
  const [openDialog, setOpenDialog] = useState<boolean>(false);
  const [useDev, setUseDev] = useState<boolean>(false);
  const [upgradeAvailable, setUpgradeAvailable] = useState<boolean>(false);

  const { send: sendCheckUpgrade } = useRequest(
    (version: string) => callAction({ action: 'checkUpgrade', param: version }),
    {
      immediate: false
    }
  ).onSuccess((event) => {
    setUpgradeAvailable((event.data as { upgradeable: boolean }).upgradeable);
  });

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

  const { send: sendUploadURL } = useRequest(
    (url: string) => callAction({ action: 'uploadURL', param: url }),
    {
      immediate: false
    }
  );

  const doRestart = async () => {
    setRestarting(true);
    await sendAPI({ device: 'system', cmd: 'restart', id: 0 }).catch(
      (error: Error) => {
        toast.error(error.message);
      }
    );
  };

  // called immediately to get the latest version, on page load
  const { data: latestVersion } = useRequest(getStableVersion, {
    // uncomment next 2 lines for testing, uses https://github.com/emsesp/EMS-ESP32/releases/download/v3.6.5/EMS-ESP-3_6_5-ESP32-16MB+.bin
    // immediate: false,
    // initialData: '3.6.5'
  });

  // called immediately to get the latest version, on page load, then check for upgrade
  const { data: latestDevVersion } = useRequest(getDevVersion, {
    // uncomment next 2 lines for testing, uses https://github.com/emsesp/EMS-ESP32/releases/download/latest/EMS-ESP-3_7_0-dev_31-ESP32-16MB+.bin
    // immediate: false,
    // initialData: '3.7.0-dev.32'
  }).onSuccess((event) => {
    void sendCheckUpgrade(event.data);
  });

  const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
  const STABLE_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';

  const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';
  const DEV_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';

  const getBinURL = (useDevVersion: boolean) => {
    if (!latestVersion || !latestDevVersion) {
      return '';
    }
    const filename =
      'EMS-ESP-' +
      (useDevVersion ? latestDevVersion : latestVersion).replaceAll('.', '_') +
      '-' +
      getPlatform() +
      '.bin';
    return useDevVersion
      ? DEV_URL + filename
      : STABLE_URL + 'v' + latestVersion + '/' + filename;
  };

  const getPlatform = () => {
    return (
      [data.esp_platform, data.flash_chip_size >= 16384 ? '16MB' : '4MB'].join('-') +
      (data.psram ? '+' : '')
    );
  };

  const installFirmwareURL = async (url: string) => {
    await sendUploadURL(url).catch((error: Error) => {
      toast.error(error.message);
    });
    setRestarting(true);
  };

  useLayoutTitle(LL.DOWNLOAD_UPLOAD());

  const internet_live =
    latestDevVersion !== undefined && latestVersion !== undefined;

  const renderUploadDialog = () => {
    if (!internet_live) {
      return null;
    }

    return (
      <Dialog
        sx={dialogStyle}
        open={openDialog}
        onClose={() => setOpenDialog(false)}
      >
        <DialogTitle>
          {LL.INSTALL('') +
            ' ' +
            (useDev ? LL.DEVELOPMENT() : LL.STABLE()) +
            ' Firmware'}
        </DialogTitle>
        <DialogContent dividers>
          <Typography mb={2}>
            {LL.INSTALL_VERSION(useDev ? latestDevVersion : latestVersion)}
          </Typography>
          <Link
            target="_blank"
            href={useDev ? DEV_RELNOTES_URL : STABLE_RELNOTES_URL}
            color="primary"
          >
            {LL.RELEASE_NOTES()}
          </Link>
          &nbsp;|&nbsp;
          <Link target="_blank" href={getBinURL(useDev)} color="primary">
            {LL.DOWNLOAD(1)}
          </Link>
        </DialogContent>
        <DialogActions>
          <Button
            startIcon={<CancelIcon />}
            variant="outlined"
            onClick={() => setOpenDialog(false)}
            color="secondary"
          >
            {LL.CANCEL()}
          </Button>
          <Button
            startIcon={<WarningIcon color="warning" />}
            variant="outlined"
            onClick={() => installFirmwareURL(getBinURL(useDev))}
            color="primary"
          >
            {LL.INSTALL('')}
          </Button>
        </DialogActions>
      </Dialog>
    );
  };

  // useDevVersion = true to force using the dev version
  const showFirmwareDialog = (useDevVersion: boolean) => {
    if (useDevVersion || data.emsesp_version.includes('dev')) {
      setUseDev(true);
    }
    setOpenDialog(true);
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    const isDev = data.emsesp_version.includes('dev');

    return (
      <>
        <Typography sx={{ pb: 2 }} variant="h6" color="primary">
          {LL.DOWNLOAD(0)}
        </Typography>

        <Typography mb={1} variant="body1" color="warning">
          {LL.DOWNLOAD_SETTINGS_TEXT()}
        </Typography>
        <Grid container spacing={1}>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => sendExportData('settings')}
          >
            {LL.DOWNLOAD(1)}&nbsp;{LL.SETTINGS_OF(LL.APPLICATION())}
          </Button>

          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => sendExportData('customizations')}
          >
            {LL.DOWNLOAD(1)}&nbsp;{LL.CUSTOMIZATIONS()}
          </Button>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => sendExportData('entities')}
          >
            {LL.DOWNLOAD(1)}&nbsp;{LL.CUSTOM_ENTITIES(0)}
          </Button>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => sendExportData('schedule')}
          >
            {LL.DOWNLOAD(1)}&nbsp;{LL.SCHEDULE(0)}
          </Button>
        </Grid>

        <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
          {LL.UPLOAD()}
        </Typography>

        <Box color="warning.main" sx={{ pb: 2 }}>
          <Typography variant="body1">{LL.UPLOAD_TEXT()}</Typography>
        </Box>

        <SingleUpload doRestart={doRestart} />

        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.EMS_ESP_VER()}
        </Typography>

        <Box p={2} mt={2} border="1px solid grey" borderRadius={2}>
          <Typography>
            <b>{LL.VERSION() + ':'}</b>&nbsp;{data.emsesp_version}
            {data.build_flags && (
              <Typography variant="caption">
                &nbsp; &#40;{data.build_flags}&#41;
              </Typography>
            )}
          </Typography>
          <Typography>
            <b>Platform:</b>&nbsp;{getPlatform()}
          </Typography>
          <Typography>
            <b>Release:</b>&nbsp;{isDev ? LL.DEVELOPMENT() : LL.STABLE()}
            {!isDev && (
              <Button
                sx={{ ml: 2 }}
                size="small"
                variant="outlined"
                color="primary"
                onClick={() => showFirmwareDialog(true)}
              >
                {LL.SWITCH_DEV()}
              </Button>
            )}
          </Typography>
          <Typography mt={2} color="secondary">
            <InfoOutlinedIcon color="secondary" sx={{ verticalAlign: 'middle' }} />
            &nbsp;&nbsp;
            {upgradeAvailable ? LL.UPGRADE_AVAILABLE() : LL.LATEST_VERSION()}
            {upgradeAvailable &&
              internet_live &&
              (data.psram ? (
                <Button
                  sx={{ ml: 2, textTransform: 'none' }}
                  size="small"
                  variant="outlined"
                  color="primary"
                  onClick={() => showFirmwareDialog(false)}
                >
                  {isDev
                    ? LL.INSTALL('v' + latestDevVersion)
                    : LL.INSTALL('v' + latestVersion)}
                </Button>
              ) : (
                <>
                  &nbsp;&nbsp;
                  <Link target="_blank" href={getBinURL(isDev)} color="primary">
                    {LL.DOWNLOAD(1)}&nbsp;v
                    {isDev ? latestDevVersion : latestVersion}
                  </Link>
                </>
              ))}
          </Typography>

          {renderUploadDialog()}
        </Box>
      </>
    );
  };

  return (
    <SectionContent>{restarting ? <RestartMonitor /> : content()}</SectionContent>
  );
};

export default DownloadUpload;
