import { useEffect, useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Divider,
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
    console.log("Latest versions, stable: " + latestVersion + " dev: " + latestDevVersion);
    sendCheckUpgrade(latestDevVersion + "," + latestVersion).catch((error: Error) => {
      console.error("Failed to check upgrade:", error);
    });
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

  useLayoutTitle(LL.EMS_ESP_VER());

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
            changelog
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
        <Box p={2} border="1px solid grey" borderRadius={2}>
          <Grid container spacing={3}>
            <Grid mb={1}>
              <Typography mb={1} fontWeight={'fontWeightBold'}>
                {LL.VERSION()}
              </Typography>
              <Typography mb={1} fontWeight={'fontWeightBold'}>
                Platform
              </Typography>
              <Typography mb={1} fontWeight={'fontWeightBold'}>
                Release
              </Typography>
            </Grid>
            <Grid>
              <Typography mb={1}>
                {data.emsesp_version}
                {data.build_flags && (
                  <Typography variant="caption">
                    &nbsp; &#40;{data.build_flags}&#41;
                  </Typography>
                )}
              </Typography>
              <Typography mb={1}>{getPlatform()}</Typography>
              <Typography>
                {isDev ? LL.DEVELOPMENT() : LL.STABLE()}&nbsp;
                <Link
                  target="_blank"
                  href={isDev ? DEV_RELNOTES_URL : STABLE_RELNOTES_URL}
                  color="primary"
                >
                  (changelog)
                </Link>
              </Typography>
            </Grid>
          </Grid>

          <Divider />

          {!isDev && (
            <Button
              sx={{ mt: 2 }}
              variant="outlined"
              color="primary"
              size="small"
              onClick={() => showFirmwareDialog(true)}
            >
              {LL.SWITCH_DEV()}
            </Button>
          )}

          <Typography mt={2} color="warning">
            <InfoOutlinedIcon color="warning" sx={{ verticalAlign: 'middle' }} />
            &nbsp;&nbsp;
            {upgradeAvailable ? LL.UPGRADE_AVAILABLE() : LL.LATEST_VERSION()}
            {upgradeAvailable &&
              internet_live &&
              (data.psram ? (
                <Button
                  sx={{ ml: 2, textTransform: 'none' }}
                  variant="outlined"
                  color="primary"
                  size="small"
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

export default Version;
