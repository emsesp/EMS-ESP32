import { useState } from 'react';
import { toast } from 'react-toastify';

import DownloadIcon from '@mui/icons-material/GetApp';
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

import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import {
  FormLoader,
  SectionContent,
  SingleUpload,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import RestartMonitor from '../status/RestartMonitor';

const UploadDownload = () => {
  const { LL } = useI18nContext();
  const [restarting, setRestarting] = useState<boolean>();
  const [md5, setMd5] = useState<string>();

  const {
    loading: isUploading,
    uploading: progress,
    send: sendUpload,
    abort: cancelUpload
  } = useRequest(SystemApi.uploadFile, {
    immediate: false
  }).onSuccess(({ data }) => {
    if (data) {
      setMd5(data.md5 as string);
      toast.success(LL.UPLOAD() + ' MD5 ' + LL.SUCCESSFUL());
    } else {
      setRestarting(true);
    }
  });

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
    saveFile(event.data, 'entities.json');
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

  // called immediately to get the latest version, on page load
  const { data: latestVersion } = useRequest(getStableVersion);
  const { data: latestDevVersion } = useRequest(getDevVersion);

  const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
  const STABLE_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';

  const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';
  const DEV_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';

  const getBinURL = (v: string) =>
    'EMS-ESP-' +
    v.replaceAll('.', '_') +
    '-' +
    getPlatform().replaceAll('-', '_') +
    '.bin';

  const getPlatform = () => {
    if (data.flash_chip_size === 16384 && data.esp_platform !== 'ESP32-S3') {
      return data.esp_platform + '-16M';
    }
    return data.esp_platform;
  };

  const startUpload = async (files: File[]) => {
    await sendUpload(files[0]).catch((error: Error) => {
      if (error.message === 'The user aborted a request') {
        toast.warning(LL.UPLOAD() + ' ' + LL.ABORTED());
      } else if (error.message === 'Network Error') {
        toast.warning('Invalid file extension or incompatible bin file');
      } else {
        toast.error(error.message);
      }
    });
  };

  const saveFile = (json: unknown, endpoint: string) => {
    const anchor = document.createElement('a');
    anchor.href = URL.createObjectURL(
      new Blob([JSON.stringify(json, null, 2)], {
        type: 'text/plain'
      })
    );
    anchor.download = 'emsesp_' + endpoint;
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

  useLayoutTitle(LL.UPLOAD_DOWNLOAD());

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <>
        <Typography sx={{ pb: 2 }} variant="h6" color="primary">
          {LL.EMS_ESP_VER()}
        </Typography>
        <Box p={2} border="2px solid grey" borderRadius={2}>
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
            </Box>
          )}
        </Box>

        <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
          {LL.UPLOAD()}
        </Typography>
        <Box mb={2} color="warning.main">
          <Typography variant="body2">
            {LL.UPLOAD_TEXT()}
            <br />
            <br />
            {LL.RESTART_TEXT(1)}.
          </Typography>
        </Box>
        {md5 && (
          <Box mb={2}>
            <Typography variant="body2">{'MD5: ' + md5}</Typography>
          </Box>
        )}
        <SingleUpload
          onDrop={startUpload}
          onCancel={cancelUpload}
          isUploading={isUploading}
          progress={progress}
        />
        {!isUploading && (
          <>
            <Typography sx={{ pt: 4, pb: 2 }} variant="h6" color="primary">
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
                All Values
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
          </>
        )}
      </>
    );
  };

  return (
    <SectionContent>{restarting ? <RestartMonitor /> : content()}</SectionContent>
  );
};

export default UploadDownload;
