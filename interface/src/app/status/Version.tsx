import { useContext, useEffect, useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import CheckIcon from '@mui/icons-material/Done';
import DownloadIcon from '@mui/icons-material/GetApp';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Checkbox,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  FormControlLabel,
  Grid2 as Grid,
  Link,
  Typography
} from '@mui/material';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';
import { getDevVersion, getStableVersion } from 'api/system';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import SystemMonitor from 'app/status/SystemMonitor';
import {
  FormLoader,
  SectionContent,
  SingleUpload,
  useLayoutTitle
} from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

const Version = () => {
  const { LL, locale } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

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
    // older version of EMS-ESP on 4MB boards, can't use OTA because of SSL support in HttpClient
    if (event.data.arduino_version.startsWith('Tasmota')) {
      setDownloadOnly(true);
    }
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
      sendCheckUpgrade(latestDevVersion.name + ',' + latestVersion.name)
        .catch((error: Error) => {
          toast.error('Failed to check for upgrades: ' + error.message);
        })
        .finally(() => {
          setInternetLive(true);
        });
    }
  }, [latestVersion, latestDevVersion]);

  const rtf = new Intl.RelativeTimeFormat(locale, { numeric: 'auto' });
  const DIVISIONS = [
    { amount: 60, name: 'seconds' },
    { amount: 60, name: 'minutes' },
    { amount: 24, name: 'hours' },
    { amount: 7, name: 'days' },
    { amount: 4.34524, name: 'weeks' },
    { amount: 12, name: 'months' },
    { amount: Number.POSITIVE_INFINITY, name: 'years' }
  ];
  function formatTimeAgo(date) {
    let duration = (date.getTime() - new Date().getTime()) / 1000;
    for (let i = 0; i < DIVISIONS.length; i++) {
      const division = DIVISIONS[i];
      if (Math.abs(duration) < division.amount) {
        return rtf.format(
          Math.round(duration),
          division.name as Intl.RelativeTimeFormatUnit
        );
      }
      duration /= division.amount;
    }
  }

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  const doRestart = async () => {
    setRestarting(true);
    await sendAPI({ device: 'system', cmd: 'restart', id: 0 }).catch(
      (error: Error) => {
        toast.error(error.message);
      }
    );
  };

  const getBinURL = () => {
    if (!internetLive) {
      return '';
    }
    const filename =
      'EMS-ESP-' +
      (usingDevVersion ? latestDevVersion.name : latestVersion.name).replaceAll(
        '.',
        '_'
      ) +
      '-' +
      getPlatform() +
      '.bin';
    return usingDevVersion
      ? DEV_URL + filename
      : STABLE_URL + 'v' + latestVersion.name + '/' + filename;
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
          {LL.INSTALL_VERSION(
            usingDevVersion ? latestDevVersion?.name : latestVersion?.name
          )}
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

  const showButtons = (showDev?: boolean) => {
    if (!me.admin) {
      return;
    }

    if (downloadOnly) {
      return (
        <Button
          sx={{ ml: 2 }}
          startIcon={<DownloadIcon />}
          variant="outlined"
          onClick={() => setOpenInstallDialog(false)}
          color="warning"
          size="small"
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
        {upgradeAvailable || (!usingDevVersion && showDev)
          ? LL.UPGRADE()
          : LL.REINSTALL()}
        &hellip;
      </Button>
    );
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    const isDev = data.emsesp_version.includes('dev');

    return (
      <>
        <Box p={2} border="1px solid grey" borderRadius={2}>
          <Typography mb={2} variant="h6" color="primary">
            {LL.THIS_VERSION()}
          </Typography>

          <Grid
            container
            direction="row"
            rowSpacing={1}
            sx={{
              justifyContent: 'flex-start',
              alignItems: 'baseline'
            }}
          >
            <Grid size={{ xs: 4, md: 2 }}>
              <Typography color="secondary">{LL.VERSION()}</Typography>
            </Grid>
            <Grid size={{ xs: 8, md: 10 }}>
              <Typography>
                {data.emsesp_version}
                {data.build_flags && (
                  <Typography variant="caption">
                    &nbsp; &#40;{data.build_flags}&#41;
                  </Typography>
                )}
              </Typography>
            </Grid>

            <Grid size={{ xs: 4, md: 2 }}>
              <Typography color="secondary">{LL.PLATFORM()}</Typography>
            </Grid>
            <Grid size={{ xs: 8, md: 10 }}>
              <Typography>
                {getPlatform()}
                <Typography variant="caption">
                  &nbsp; &#40;{data.psram ? '+PSRAM' : '-PSRAM'}&#41;
                </Typography>
              </Typography>
            </Grid>

            <Grid size={{ xs: 4, md: 2 }}>
              <Typography color="secondary">{LL.RELEASE_TYPE()}</Typography>
            </Grid>
            <Grid size={{ xs: 8, md: 10 }}>
              <FormControlLabel
                disabled={!isDev}
                control={
                  <Checkbox
                    sx={{
                      '&.Mui-checked': {
                        color: 'lightblue'
                      }
                    }}
                  />
                }
                slotProps={{
                  typography: {
                    color: 'grey'
                  }
                }}
                checked={!isDev}
                label={LL.STABLE()}
                sx={{ '& .MuiSvgIcon-root': { fontSize: 16 } }}
              />
              <FormControlLabel
                disabled={isDev}
                control={
                  <Checkbox
                    sx={{
                      '&.Mui-checked': {
                        color: 'lightblue'
                      }
                    }}
                  />
                }
                slotProps={{
                  typography: {
                    color: 'grey'
                  }
                }}
                checked={isDev}
                label={LL.DEVELOPMENT()}
                sx={{ '& .MuiSvgIcon-root': { fontSize: 16 } }}
              />
            </Grid>
          </Grid>

          {internetLive ? (
            <>
              <Typography mt={2} mb={2} variant="h6" color="primary">
                {LL.AVAILABLE_VERSION()}
              </Typography>

              <Grid
                container
                direction="row"
                rowSpacing={1}
                sx={{
                  justifyContent: 'flex-start',
                  alignItems: 'baseline'
                }}
              >
                <Grid size={{ xs: 4, md: 2 }}>
                  <Typography color="secondary">{LL.STABLE()}</Typography>
                </Grid>
                <Grid size={{ xs: 8, md: 10 }}>
                  <Typography>
                    <Link target="_blank" href={STABLE_RELNOTES_URL} color="primary">
                      {latestVersion.name}
                    </Link>
                    {latestVersion.published_at && (
                      <Typography component="span" variant="caption">
                        &nbsp;(
                        {formatTimeAgo(new Date(latestVersion.published_at))})
                      </Typography>
                    )}
                    {!usingDevVersion && showButtons(false)}
                  </Typography>
                </Grid>

                <Grid size={{ xs: 4, md: 2 }}>
                  <Typography color="secondary">{LL.DEVELOPMENT()}</Typography>
                </Grid>
                <Grid size={{ xs: 8, md: 10 }}>
                  <Typography>
                    <Link target="_blank" href={DEV_RELNOTES_URL} color="primary">
                      {latestDevVersion.name}
                    </Link>
                    {latestDevVersion.published_at && (
                      <Typography component="span" variant="caption">
                        &nbsp;(
                        {formatTimeAgo(new Date(latestDevVersion.published_at))})
                      </Typography>
                    )}
                    {showButtons(true)}
                  </Typography>
                </Grid>
              </Grid>

              {upgradeAvailable ? (
                <Typography mt={2} color="warning">
                  <InfoOutlinedIcon
                    color="warning"
                    sx={{ verticalAlign: 'middle', mr: 2 }}
                  />
                  {LL.UPGRADE_AVAILABLE()}
                </Typography>
              ) : (
                <Typography mt={2} color="success">
                  <CheckIcon
                    color="success"
                    sx={{ verticalAlign: 'middle', mr: 2 }}
                  />
                  {LL.LATEST_VERSION()}
                </Typography>
              )}
            </>
          ) : (
            <Typography mt={2} color="warning">
              <WarningIcon color="warning" sx={{ verticalAlign: 'middle', mr: 2 }} />
              {LL.INTERNET_CONNECTION_REQUIRED()}
            </Typography>
          )}
          {me.admin && (
            <>
              {renderInstallDialog()}
              <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
                {LL.UPLOAD()}
              </Typography>
              <SingleUpload text={LL.UPLOAD_DROP_TEXT()} doRestart={doRestart} />
            </>
          )}
        </Box>
      </>
    );
  };

  return (
    <SectionContent>{restarting ? <SystemMonitor /> : content()}</SectionContent>
  );
};

export default Version;
