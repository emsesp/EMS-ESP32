import { useEffect, useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import CheckIcon from '@mui/icons-material/Done';
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
import { callAction } from 'api/app';
import { getDevVersion, getStableVersion } from 'api/system';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import RestartMonitor from 'app/status/RestartMonitor';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const Version = () => {
  const { LL } = useI18nContext();

  const [restarting, setRestarting] = useState<boolean>(false);
  const [openDialog, setOpenDialog] = useState<boolean>(false);
  const [useDev, setUseDev] = useState<boolean>(false);
  const [upgradeAvailable, setUpgradeAvailable] = useState<boolean>(false);

  const { send: sendCheckUpgrade } = useRequest(
    (versions: string) => callAction({ action: 'checkUpgrade', param: versions }),
    {
      immediate: false
    }
  ).onSuccess((event) => {
    const data = event.data as { emsesp_version: string; upgradeable: boolean };
    setUpgradeAvailable(data.upgradeable);
  });

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  const { send: sendUploadURL } = useRequest(
    (url: string) => callAction({ action: 'uploadURL', param: url }),
    {
      immediate: false
    }
  );

  // called immediately to get the latest versions on page load
  const { data: latestVersion } = useRequest(getStableVersion);
  const { data: latestDevVersion } = useRequest(getDevVersion);

  useEffect(() => {
    if (latestVersion && latestDevVersion) {
      // console.log("Latest versions, stable: " + latestVersion + " dev: " + latestDevVersion);
      sendCheckUpgrade(latestDevVersion + ',' + latestVersion).catch(
        (error: Error) => {
          toast.error('Failed to check upgrade: ' + error.message);
        }
      );
    }
  }, [latestVersion, latestDevVersion]);

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

  useLayoutTitle('EMS-ESP Firmware');

  // see if we have internet access
  const internet_live =
    latestDevVersion !== undefined && latestVersion !== undefined;

  // check for older boards where auto-upgrade is not supported
  const download_only = data && !data.psram;

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
          {(download_only ? LL.DOWNLOAD(0) : LL.INSTALL('')) + ' ' + ' Firmware'}
        </DialogTitle>
        <DialogContent dividers>
          <Typography mb={2}>
            {LL.INSTALL_VERSION(
              download_only ? LL.DOWNLOAD(1) : LL.INSTALL(''),
              useDev ? latestDevVersion : latestVersion
            )}
          </Typography>
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
            startIcon={<DownloadIcon />}
            variant="outlined"
            onClick={() => setOpenDialog(false)}
            color="primary"
          >
            <Link target="_blank" href={getBinURL(useDev)} color="primary">
              {LL.DOWNLOAD(1)}
            </Link>
          </Button>
          {!download_only && (
            <Button
              startIcon={<WarningIcon color="warning" />}
              variant="outlined"
              onClick={() => installFirmwareURL(getBinURL(useDev))}
              color="primary"
            >
              {LL.INSTALL('')}
            </Button>
          )}
        </DialogActions>
      </Dialog>
    );
  };

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
        <Box p={2} border="1px solid grey" borderRadius={2}>
          <Typography sx={{ pb: 2 }} variant="h6" color="primary">
            Firmware Version
          </Typography>

          <Grid container spacing={11}>
            <Grid mb={1}>
              <Typography mb={1} color="secondary">
                {LL.VERSION()}
              </Typography>
              <Typography mb={1} color="secondary">
                Platform
              </Typography>
              <Typography mb={1} color="secondary">
                Release Type
              </Typography>
            </Grid>
            <Grid mb={1}>
              <Typography mb={1}>
                {data.emsesp_version}
                {data.build_flags && (
                  <Typography variant="caption">
                    &nbsp; &#40;{data.build_flags}&#41;
                  </Typography>
                )}
              </Typography>
              <Typography mb={1}>{getPlatform()}</Typography>
              <Typography mb={1}>
                {isDev ? LL.DEVELOPMENT() : LL.STABLE()}
                {!isDev && internet_live && (
                  <Typography variant="caption">
                    <Button
                      sx={{ ml: 2 }}
                      variant="outlined"
                      color="primary"
                      size="small"
                      onClick={() => showFirmwareDialog(true)}
                    >
                      {LL.SWITCH_DEV()}
                    </Button>
                  </Typography>
                )}
              </Typography>
            </Grid>
          </Grid>

          <Typography sx={{ pb: 2 }} variant="h6" color="primary">
            Latest Available Versions
          </Typography>

          {internet_live ? (
            <>
              <Grid container spacing={4}>
                <Grid mb={1}>
                  <Typography mb={1} color="secondary">
                    Stable Release
                  </Typography>
                  <Typography mb={1} color="secondary">
                    Development Release
                  </Typography>
                </Grid>
                <Grid mb={1}>
                  <Typography mb={1}>
                    {latestVersion}&nbsp;&nbsp;
                    <Link target="_blank" href={STABLE_RELNOTES_URL} color="primary">
                      (changelog)
                    </Link>
                    {!isDev && upgradeAvailable && (
                      <Button
                        sx={{ ml: 2 }}
                        variant="outlined"
                        color="warning"
                        size="small"
                        onClick={() => showFirmwareDialog(false)}
                      >
                        Upgrade&hellip;
                      </Button>
                    )}
                  </Typography>
                  <Typography mb={1}>
                    {latestDevVersion}&nbsp;&nbsp;
                    <Link target="_blank" href={DEV_RELNOTES_URL} color="primary">
                      (changelog)
                    </Link>
                    {isDev && upgradeAvailable && (
                      <Button
                        sx={{ ml: 2 }}
                        variant="outlined"
                        color="warning"
                        size="small"
                        onClick={() => showFirmwareDialog(false)}
                      >
                        Upgrade&hellip;
                      </Button>
                    )}
                  </Typography>
                </Grid>
              </Grid>
              {upgradeAvailable ? (
                <Typography color="warning">
                  <InfoOutlinedIcon
                    color="warning"
                    sx={{ verticalAlign: 'middle', mr: 2 }}
                  />
                  {LL.UPGRADE_AVAILABLE()}
                </Typography>
              ) : (
                <Typography color="success">
                  <CheckIcon
                    color="success"
                    sx={{ verticalAlign: 'middle', mr: 2 }}
                  />
                  {LL.LATEST_VERSION()}
                </Typography>
              )}
            </>
          ) : (
            <Typography mb={1} color="warning">
              not online
            </Typography>
          )}

          {renderUploadDialog()}
        </Box>
      </>
    );
  };

  return (
    <SectionContent>{restarting ? <RestartMonitor /> : content()}</SectionContent>
  );
};

export default Version;
