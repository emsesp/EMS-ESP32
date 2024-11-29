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
  const [openInstallDialog, setOpenInstallDialog] = useState<boolean>(false);
  const [usingDevVersion, setUsingDevVersion] = useState<boolean>(false);
  const [upgradeAvailable, setUpgradeAvailable] = useState<boolean>(false);
  const [internetLive, setInternetLive] = useState<boolean>(false);
  const [downloadOnly, setDownloadOnly] = useState<boolean>(false);

  const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
  const STABLE_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';

  const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';
  const DEV_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';

  const { send: sendCheckUpgrade } = useRequest(
    (versions: string) => callAction({ action: 'checkUpgrade', param: versions }),
    {
      immediate: false
    }
  ).onSuccess((event) => {
    const data = event.data as { emsesp_version: string; upgradeable: boolean };
    setUpgradeAvailable(data.upgradeable);
  });

  const {
    data: data,
    send: loadData,
    error
  } = useRequest(SystemApi.readSystemStatus).onSuccess((event) => {
    // older version of EMS-ESP didn't have the psram set, so we can't do an OTA upgrade
    setDownloadOnly(event.data.psram === undefined);
    setUsingDevVersion(event.data.emsesp_version.includes('dev'));
  });

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
      sendCheckUpgrade(latestDevVersion + ',' + latestVersion)
        .catch((error: Error) => {
          toast.error('Failed to check for upgrades: ' + error.message);
        })
        .finally(() => {
          setInternetLive(true);
        });
    }
  }, [latestVersion, latestDevVersion]);

  const getBinURL = () => {
    if (!latestVersion || !latestDevVersion) {
      return '';
    }
    const filename =
      'EMS-ESP-' +
      (usingDevVersion ? latestDevVersion : latestVersion).replaceAll('.', '_') +
      '-' +
      getPlatform() +
      '.bin';
    return usingDevVersion
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

  const renderInstallDialog = () => (
    <Dialog
      sx={dialogStyle}
      open={openInstallDialog}
      onClose={() => closeInstallDialog()}
    >
      <DialogTitle>
        {LL.INSTALL() +
          ' ' +
          (usingDevVersion ? LL.DEVELOPMENT() : LL.STABLE()) +
          ' Firmware'}
      </DialogTitle>
      <DialogContent dividers>
        <Typography mb={2}>
          {LL.INSTALL_VERSION(usingDevVersion ? latestDevVersion : latestVersion)}
        </Typography>
      </DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={() => closeInstallDialog()}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          onClick={() => closeInstallDialog()}
          color="primary"
        >
          <Link underline="none" target="_blank" href={getBinURL()} color="primary">
            {LL.DOWNLOAD(1)}
          </Link>
        </Button>
        <Button
          startIcon={<WarningIcon color="warning" />}
          variant="outlined"
          onClick={() => installFirmwareURL(getBinURL())}
          color="primary"
        >
          {LL.INSTALL()}
        </Button>
      </DialogActions>
    </Dialog>
  );

  const showFirmwareDialog = (useDevVersion?: boolean) => {
    setUsingDevVersion(useDevVersion || usingDevVersion);
    setOpenInstallDialog(true);
  };

  const closeInstallDialog = () => {
    setOpenInstallDialog(false);
    setUsingDevVersion(data.emsesp_version.includes('dev'));
  };

  const switchToDev = () => {
    setUsingDevVersion(true);
    setUpgradeAvailable(true);
  };

  const showButtons = () => {
    if (!upgradeAvailable) {
      return;
    }

    if (downloadOnly) {
      return (
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          onClick={() => setOpenInstallDialog(false)}
          color="warning"
          size="small"
          sx={{ ml: 2 }}
        >
          <Link underline="none" target="_blank" href={getBinURL()} color="warning">
            {LL.DOWNLOAD(1)}
          </Link>
        </Button>
      );
    }

    return (
      <Button
        sx={{ ml: 2 }}
        variant="outlined"
        color="warning"
        size="small"
        onClick={() => showFirmwareDialog()}
      >
        {LL.UPGRADE()}&hellip;
      </Button>
    );
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <>
        <Box p={2} border="1px solid grey" borderRadius={2}>
          <Typography sx={{ pb: 2 }} variant="h6" color="primary">
            Firmware Version
          </Typography>

          <Grid container spacing={4}>
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
                {data.emsesp_version.includes('dev')
                  ? LL.DEVELOPMENT()
                  : LL.STABLE()}
              </Typography>
            </Grid>
          </Grid>

          <Typography sx={{ pb: 2 }} variant="h6" color="primary">
            {LL.AVAILABLE_VERSION()}
          </Typography>

          {internetLive ? (
            <>
              <Grid container spacing={4}>
                <Grid mb={1}>
                  <Typography mb={1} color="secondary">
                    {LL.STABLE()}
                  </Typography>
                  <Typography mb={1} color="secondary">
                    {LL.DEVELOPMENT()}
                  </Typography>
                </Grid>
                <Grid mb={1}>
                  <Typography mb={1}>
                    {latestVersion}&nbsp;&nbsp;
                    <Link target="_blank" href={STABLE_RELNOTES_URL} color="primary">
                      (changelog)
                    </Link>
                    {!usingDevVersion && showButtons()}
                  </Typography>
                  <Typography mb={1}>
                    {latestDevVersion}&nbsp;&nbsp;
                    <Link target="_blank" href={DEV_RELNOTES_URL} color="primary">
                      (changelog)
                    </Link>
                    {usingDevVersion && showButtons()}
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

              {!data.emsesp_version.includes('dev') && !usingDevVersion && (
                <Typography variant="caption">
                  <Button
                    sx={{ mt: 2 }}
                    variant="outlined"
                    color="primary"
                    size="small"
                    onClick={() => switchToDev()}
                  >
                    {LL.SWITCH_DEV()}
                  </Button>
                </Typography>
              )}
            </>
          ) : (
            <Typography mb={1} color="warning">
              <WarningIcon color="warning" sx={{ verticalAlign: 'middle', mr: 2 }} />
              device cannot access internet
            </Typography>
          )}

          {renderInstallDialog()}
        </Box>
      </>
    );
  };

  return (
    <SectionContent>{restarting ? <RestartMonitor /> : content()}</SectionContent>
  );
};

export default Version;
