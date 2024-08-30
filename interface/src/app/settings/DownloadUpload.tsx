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

import * as SystemApi from 'api/system';
import {
  API,
  getCustomizations,
  getEntities,
  getSchedule,
  getSettings
} from 'api/app';
import {
  checkUpgrade,
  getDevVersion,
  getStableVersion,
  restart,
  uploadURL
} from 'api/system';

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

const DownloadUpload = () => {
  const { LL } = useI18nContext();

  const [restarting, setRestarting] = useState<boolean>(false);
  const [openDialog, setOpenDialog] = useState<boolean>(false);
  const [useDev, setUseDev] = useState<boolean>(false);
  const [upgradeAvailable, setUpgradeAvailable] = useState<boolean>(false);

  const { send: sendSettings } = useRequest(getSettings(), {
    immediate: false
  }).onSuccess((event) => {
    saveFile(event.data, 'settings.json');
  });

  const { send: sendCustomizations } = useRequest(getCustomizations(), {
    immediate: false
  }).onSuccess((event) => {
    saveFile(event.data, 'customizations.json');
  });

  const { send: sendEntities } = useRequest(getEntities(), {
    immediate: false
  }).onSuccess((event) => {
    saveFile(event.data, 'custom_entities.json');
  });

  const { send: sendSchedule } = useRequest(getSchedule(), {
    immediate: false
  }).onSuccess((event) => {
    saveFile(event.data, 'schedule.json');
  });

  const { send: getAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  }).onSuccess((event) => {
    saveFile(
      event.data,
      String(event.args[0].device) + '_' + String(event.args[0].entity) + '.txt'
    );
  });

  const {
    data: data,
    send: loadData,
    error
  } = useRequest(SystemApi.readHardwareStatus);

  const { send: sendUploadURL } = useRequest(
    (data: { url: string }) => uploadURL(data),
    {
      immediate: false
    }
  );

  const { send: restartCommand } = useRequest(restart(), {
    immediate: false
  });

  const callRestart = async () => {
    setRestarting(true);
    await restartCommand().catch((error: Error) => {
      toast.error(error.message);
    });
  };

  const { send: sendCheckUpgrade } = useRequest(checkUpgrade, {
    immediate: false
  }).onSuccess((event) => {
    setUpgradeAvailable(event.data.upgradeable);
  });

  // called immediately to get the latest version, on page load
  const { data: latestVersion } = useRequest(getStableVersion, {
    // uncomment next 2 lines for testing, uses https://github.com/emsesp/EMS-ESP32/releases/download/v3.6.5/EMS-ESP-3_6_5-ESP32-16MB+.bin
    // immediate: false,
    // initialData: '3.6.5'
  });
  const { data: latestDevVersion } = useRequest(getDevVersion, {
    // uncomment next 2 lines for testing, uses https://github.com/emsesp/EMS-ESP32/releases/download/latest/EMS-ESP-3_7_0-dev_31-ESP32-16MB+.bin
    // immediate: false,
    // initialData: '3.7.0-dev.32'
  }).onSuccess((event) => {
    void sendCheckUpgrade({ version: event.data });
  });

  const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
  const STABLE_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';

  const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';
  const DEV_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';

  const getBinURL = (useDevVersion: boolean) => {
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
    await sendUploadURL({ url: url }).catch((error: Error) => {
      toast.error(error.message);
    });
    setRestarting(true);
  };

  const saveFile = (json: unknown, filename: string) => {
    const anchor = document.createElement('a');
    anchor.href = URL.createObjectURL(
      new Blob([JSON.stringify(json, null, 2)], {
        type: 'text/plain'
      })
    );
    anchor.download = 'emsesp_' + filename;
    anchor.click();
    URL.revokeObjectURL(anchor.href);
    toast.info(LL.DOWNLOAD_SUCCESSFUL());
  };

  const downloadSettings = async () => {
    await sendSettings().catch((error: Error) => {
      toast.error(error.message);
    });
  };

  const downloadCustomizations = async () => {
    await sendCustomizations().catch((error: Error) => {
      toast.error(error.message);
    });
  };

  const downloadEntities = async () => {
    await sendEntities().catch((error: Error) => {
      toast.error(error.message);
    });
  };

  const downloadSchedule = async () => {
    await sendSchedule().catch((error: Error) => {
      toast.error(error.message);
    });
  };

  const callAPI = async (device: string, entity: string) => {
    await getAPI({ device, entity, id: 0 }).catch((error: Error) => {
      toast.error(error.message);
    });
  };

  useLayoutTitle(LL.DOWNLOAD_UPLOAD());

  const renderUploadDialog = () => {
    if (latestDevVersion === undefined || latestVersion === undefined) {
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

  // useDev = true to force using the dev version
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
        <Box color="warning.main">
          <Typography mb={1} variant="body2">
            {LL.HELP_INFORMATION_4()}
          </Typography>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => callAPI('system', 'info')}
          >
            {LL.SUPPORT_INFORMATION(0)}
          </Button>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => callAPI('system', 'allvalues')}
          >
            {LL.ALLVALUES()}
          </Button>
        </Box>

        <Box color="warning.main">
          <Typography mt={2} mb={1} variant="body2">
            {LL.DOWNLOAD_SETTINGS_TEXT()}
          </Typography>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={downloadSettings}
          >
            {LL.SETTINGS_OF('')}
          </Button>
        </Box>

        <Box color="warning.main">
          <Typography mt={2} mb={1} variant="body2">
            {LL.DOWNLOAD_CUSTOMIZATION_TEXT()}
          </Typography>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={downloadCustomizations}
          >
            {LL.CUSTOMIZATIONS()}
          </Button>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={downloadEntities}
          >
            {LL.CUSTOM_ENTITIES(0)}
          </Button>
          <Box color="warning.main">
            <Typography mt={2} mb={1} variant="body2">
              {LL.DOWNLOAD_SCHEDULE_TEXT()}
            </Typography>
          </Box>
          <Button
            sx={{ ml: 2 }}
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={downloadSchedule}
          >
            {LL.SCHEDULE(0)}
          </Button>
        </Box>

        <Box color="warning.main">
          <Typography mt={2} variant="body2">
            {LL.EMS_ESP_VER()}
          </Typography>
        </Box>
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
            {upgradeAvailable && (
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
            )}
          </Typography>

          {renderUploadDialog()}
        </Box>

        <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
          {LL.UPLOAD()}
        </Typography>

        <Box color="warning.main" sx={{ pb: 2 }}>
          <Typography variant="body2">{LL.UPLOAD_TEXT()}</Typography>
        </Box>

        <SingleUpload callRestart={callRestart} />
      </>
    );
  };

  return (
    <SectionContent>
      {restarting ? <RestartMonitor message={LL.WAIT_FIRMWARE()} /> : content()}
    </SectionContent>
  );
};

export default DownloadUpload;
