import { useState } from 'react';
import { toast } from 'react-toastify';

import DownloadIcon from '@mui/icons-material/GetApp';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import WarningIcon from '@mui/icons-material/Warning';
import { Box, Button, Divider, Link, Typography } from '@mui/material';

import * as SystemApi from 'api/system';
import {
  API,
  getCustomizations,
  getEntities,
  getSchedule,
  getSettings
} from 'api/app';
import { getDevVersion, getStableVersion } from 'api/system';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import RestartMonitor from 'app/status/RestartMonitor';
import {
  FormLoader,
  MessageBox,
  SectionContent,
  SingleUpload,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const DownloadUpload = () => {
  const { LL } = useI18nContext();

  const [restarting, setRestarting] = useState<boolean>(false);
  const [restartNeeded, setRestartNeeded] = useState<boolean>(false);

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
    (data: { url: string }) => SystemApi.uploadURL(data),
    {
      immediate: false
    }
  );

  const { send: restartCommand } = useRequest(SystemApi.restart(), {
    immediate: false
  });

  const restart = async () => {
    await restartCommand()
      .then(() => {
        setRestarting(true);
      })
      .catch((error: Error) => {
        toast.error(error.message);
      });
  };

  // called immediately to get the latest version, on page load
  const { data: latestVersion } = useRequest(getStableVersion, {
    // immediate: true
    // uncomment for testing
    // https://github.com/emsesp/EMS-ESP32/releases/download/v3.6.5/EMS-ESP-3_6_5-ESP32-16MB+.bin
    immediate: false,
    initialData: '3.6.5'
  });
  const { data: latestDevVersion } = useRequest(getDevVersion, {
    // immediate: true
    // uncomment for testing
    // https://github.com/emsesp/EMS-ESP32/releases/download/latest/EMS-ESP-3_7_0-dev_31-ESP32-16MB+.bin
    immediate: false,
    initialData: '3.7.0-dev.31'
  });

  const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
  const STABLE_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';

  const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';
  const DEV_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';

  const getBinURL = (v: string) =>
    'EMS-ESP-' + v.replaceAll('.', '_') + '-' + getPlatform() + '.bin';

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

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

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
          {LL.VERSION_ON() + ' '}
          <b>{data.emsesp_version}</b>&nbsp;({getPlatform()})
          <Divider />
          {latestVersion && (
            <Box mt={2}>
              {LL.THE_LATEST()}&nbsp;{LL.OFFICIAL()}&nbsp;{LL.RELEASE_IS()}
              &nbsp;<b>{latestVersion}</b>
              &nbsp;(
              <Link target="_blank" href={STABLE_RELNOTES_URL} color="primary">
                {LL.RELEASE_NOTES()}
              </Link>
              )&nbsp;(
              <Link
                target="_blank"
                href={
                  STABLE_URL + 'v' + latestVersion + '/' + getBinURL(latestVersion)
                }
                color="primary"
              >
                {LL.DOWNLOAD(1)}
              </Link>
              )
              <Button
                sx={{ ml: 2 }}
                size="small"
                startIcon={<WarningIcon color="warning" />}
                variant="outlined"
                color="primary"
                onClick={() =>
                  installFirmwareURL(
                    STABLE_URL + 'v' + latestVersion + '/' + getBinURL(latestVersion)
                  )
                }
              >
                Install
              </Button>
            </Box>
          )}
          {latestDevVersion && (
            <Box mt={2}>
              {LL.THE_LATEST()}&nbsp;{LL.DEVELOPMENT()}&nbsp;{LL.RELEASE_IS()}
              &nbsp;
              <b>{latestDevVersion}</b>
              &nbsp;(
              <Link target="_blank" href={DEV_RELNOTES_URL} color="primary">
                {LL.RELEASE_NOTES()}
              </Link>
              )&nbsp;(
              <Link
                target="_blank"
                href={DEV_URL + getBinURL(latestDevVersion)}
                color="primary"
              >
                {LL.DOWNLOAD(1)}
              </Link>
              )
              <Button
                sx={{ ml: 2 }}
                size="small"
                startIcon={<WarningIcon color="warning" />}
                variant="outlined"
                color="primary"
                onClick={() =>
                  installFirmwareURL(DEV_URL + getBinURL(latestDevVersion))
                }
              >
                Install
              </Button>
            </Box>
          )}
        </Box>

        <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
          {LL.UPLOAD()}
        </Typography>

        <Box mb={2} color="warning.main">
          <Typography variant="body2">{LL.UPLOAD_TEXT()}</Typography>
        </Box>

        {restartNeeded ? (
          <MessageBox mt={2} level="warning" message={LL.RESTART_TEXT(0)}>
            <Button
              startIcon={<PowerSettingsNewIcon />}
              variant="contained"
              color="error"
              onClick={restart}
            >
              {LL.RESTART()}
            </Button>
          </MessageBox>
        ) : (
          <SingleUpload setRestartNeeded={setRestartNeeded} />
        )}
      </>
    );
  };

  return (
    <SectionContent>
      {restarting ? (
        <RestartMonitor message="Please wait while the firmware is being uploaded and installed. This can take a few minutes. EMS-ESP will automatically restart when completed." />
      ) : (
        content()
      )}
    </SectionContent>
  );
};

export default DownloadUpload;
