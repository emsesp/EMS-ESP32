import { useContext, useEffect, useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import CloseIcon from '@mui/icons-material/Close';
import CheckIcon from '@mui/icons-material/Done';
import DownloadIcon from '@mui/icons-material/GetApp';
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
  Grid,
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
  const [fetchDevVersion, setFetchDevVersion] = useState<boolean>(false);
  const [devUpgradeAvailable, setDevUpgradeAvailable] = useState<boolean>(false);
  const [stableUpgradeAvailable, setStableUpgradeAvailable] =
    useState<boolean>(false);
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
    const data = event.data as {
      emsesp_version: string;
      dev_upgradeable: boolean;
      stable_upgradeable: boolean;
    };
    setDevUpgradeAvailable(data.dev_upgradeable);
    setStableUpgradeAvailable(data.stable_upgradeable);
  });

  const {
    data: data,
    send: loadData,
    error
  } = useRequest(SystemApi.readSystemStatus).onSuccess((event) => {
    // older version of EMS-ESP using ESP32 (not S3) and no PSRAM, can't use OTA because of SSL support in HttpClient
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
  const DIVISIONS: Array<{ amount: number; name: string }> = [
    { amount: 60, name: 'seconds' },
    { amount: 60, name: 'minutes' },
    { amount: 24, name: 'hours' },
    { amount: 7, name: 'days' },
    { amount: 4.34524, name: 'weeks' },
    { amount: 12, name: 'months' },
    { amount: Number.POSITIVE_INFINITY, name: 'years' }
  ];
  function formatTimeAgo(date: Date) {
    let duration = (date.getTime() - new Date().getTime()) / 1000;
    for (let i = 0; i < DIVISIONS.length; i++) {
      const division = DIVISIONS[i];
      if (division && Math.abs(duration) < division.amount) {
        return rtf.format(
          Math.round(duration),
          division.name as Intl.RelativeTimeFormatUnit
        );
      }
      if (division) {
        duration /= division.amount;
      }
    }
    return rtf.format(0, 'seconds');
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

  const getBinURL = (showingDev: boolean) => {
    if (!internetLive) {
      return '';
    }

    const filename =
      'EMS-ESP-' +
      (showingDev ? latestDevVersion.name : latestVersion.name).replaceAll(
        '.',
        '_'
      ) +
      '-' +
      getPlatform() +
      '.bin';
    return showingDev
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

  const renderInstallDialog = () => {
    const binURL = getBinURL(fetchDevVersion);

    return (
      <Dialog
        sx={dialogStyle}
        open={openInstallDialog}
        onClose={() => closeInstallDialog()}
      >
        <DialogTitle>
          {LL.UPDATE() +
            ' ' +
            (fetchDevVersion ? LL.DEVELOPMENT() : LL.STABLE()) +
            ' Firmware'}
        </DialogTitle>
        <DialogContent dividers>
          <Typography mb={2}>
            {LL.INSTALL_VERSION(
              downloadOnly ? LL.DOWNLOAD(1) : LL.INSTALL(),
              fetchDevVersion ? latestDevVersion?.name : latestVersion?.name
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
            <Link underline="none" target="_blank" href={binURL} color="primary">
              {LL.DOWNLOAD(0)}
            </Link>
          </Button>
          {!downloadOnly && (
            <Button
              startIcon={<WarningIcon color="warning" />}
              variant="outlined"
              onClick={() => installFirmwareURL(binURL)}
              color="primary"
            >
              {LL.INSTALL()}
            </Button>
          )}
        </DialogActions>
      </Dialog>
    );
  };

  const showFirmwareDialog = (useDevVersion: boolean) => {
    setFetchDevVersion(useDevVersion);
    setOpenInstallDialog(true);
  };

  const closeInstallDialog = () => {
    setOpenInstallDialog(false);
  };

  const showButtons = (showingDev: boolean) => {
    const choice = showingDev
      ? !usingDevVersion
        ? LL.SWITCH_RELEASE_TYPE(LL.DEVELOPMENT())
        : devUpgradeAvailable
          ? LL.UPDATE_AVAILABLE()
          : undefined
      : usingDevVersion
        ? LL.SWITCH_RELEASE_TYPE(LL.STABLE())
        : stableUpgradeAvailable
          ? LL.UPDATE_AVAILABLE()
          : undefined;

    if (!choice) {
      return (
        <>
          <CheckIcon
            color="success"
            sx={{ verticalAlign: 'middle', ml: 0.5, mr: 0.5 }}
          />
          <span style={{ color: '#66bb6a', fontSize: '0.8em' }}>
            {LL.LATEST_VERSION(usingDevVersion ? LL.DEVELOPMENT() : LL.STABLE())}
          </span>
        </>
      );
    }

    if (!me.admin) {
      return;
    }

    return (
      <Button
        sx={{ ml: 2 }}
        variant="outlined"
        color={choice === LL.UPDATE_AVAILABLE() ? 'success' : 'warning'}
        size="small"
        onClick={() => showFirmwareDialog(showingDev)}
      >
        {choice}
      </Button>
    );
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />;
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
                  &nbsp; &#40;
                  {data.psram ? (
                    <CheckIcon
                      color="success"
                      sx={{
                        fontSize: '1.5em',
                        verticalAlign: 'middle'
                      }}
                    />
                  ) : (
                    <CloseIcon
                      color="error"
                      sx={{
                        fontSize: '1.5em',
                        verticalAlign: 'middle'
                      }}
                    />
                  )}
                  PSRAM&#41;
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
                    {showButtons(false)}
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
