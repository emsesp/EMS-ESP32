import { type FC, useState } from 'react';
import { toast } from 'react-toastify';

import DownloadIcon from '@mui/icons-material/GetApp';
import { Box, Button, Divider, Link, Typography } from '@mui/material';

import * as SystemApi from 'api/system';

import * as EMSESP from 'project/api';
import { useRequest } from 'alova';
import {
  FormLoader,
  SectionContent,
  SingleUpload,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { APIcall } from 'project/types';

import RestartMonitor from './RestartMonitor';

const UploadDownload: FC = () => {
  const { LL } = useI18nContext();
  const [restarting, setRestarting] = useState<boolean>();
  const [md5, setMd5] = useState<string>();

  const { send: getSettings, onSuccess: onSuccessGetSettings } = useRequest(
    EMSESP.getSettings(),
    {
      immediate: false
    }
  );
  const { send: getCustomizations, onSuccess: onSuccessGetCustomizations } =
    useRequest(EMSESP.getCustomizations(), {
      immediate: false
    });
  const { send: getEntities, onSuccess: onSuccessGetEntities } = useRequest(
    EMSESP.getEntities(),
    {
      immediate: false
    }
  );
  const { send: getSchedule, onSuccess: onSuccessGetSchedule } = useRequest(
    EMSESP.getSchedule(),
    {
      immediate: false
    }
  );
  const { send: getAPI, onSuccess: onGetAPI } = useRequest(
    (data: APIcall) => EMSESP.API(data),
    {
      immediate: false
    }
  );

  const {
    data: data,
    send: loadData,
    error
  } = useRequest(SystemApi.readESPSystemStatus, { force: true });

  const { data: latestVersion } = useRequest(SystemApi.getStableVersion, {
    immediate: true,
    force: true
  });

  const { data: latestDevVersion } = useRequest(SystemApi.getDevVersion, {
    immediate: true,
    force: true
  });

  const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
  const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';

  const STABLE_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';
  const DEV_RELNOTES_URL =
    'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';

  const getBinURL = (v: string) =>
    'EMS-ESP-' +
    v.replaceAll('.', '_') +
    '-' +
    data.esp_platform.replaceAll('-', '_') +
    '.bin';

  const {
    loading: isUploading,
    uploading: progress,
    send: sendUpload,
    onSuccess: onSuccessUpload,
    abort: cancelUpload
  } = useRequest(SystemApi.uploadFile, {
    immediate: false,
    force: true
  });

  onSuccessUpload(({ data }) => {
    if (data) {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-argument
      setMd5(data.md5);
      toast.success(LL.UPLOAD() + ' MD5 ' + LL.SUCCESSFUL());
    } else {
      setRestarting(true);
    }
  });

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

  onSuccessGetSettings((event) => {
    saveFile(event.data, 'settings.json');
  });
  onSuccessGetCustomizations((event) => {
    saveFile(event.data, 'customizations.json');
  });
  onSuccessGetEntities((event) => {
    saveFile(event.data, 'entities.json');
  });
  onSuccessGetSchedule((event) => {
    saveFile(event.data, 'schedule.json');
  });
  onGetAPI((event) => {
    saveFile(
      event.data,

      event.sendArgs[0].device + '_' + event.sendArgs[0].entity + '.txt'
    );
  });

  const downloadSettings = async () => {
    await getSettings().catch((error: Error) => {
      toast.error(error.message);
    });
  };

  const downloadCustomizations = async () => {
    await getCustomizations().catch((error: Error) => {
      toast.error(error.message);
    });
  };

  const downloadEntities = async () => {
    await getEntities().catch((error: Error) => {
      toast.error(error.message);
    });
  };

  const downloadSchedule = async () => {
    await getSchedule().catch((error: Error) => {
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
          <b>{data.emsesp_version}</b>&nbsp;({data.esp_platform})
          {data.model.length > 0 && <p>Gateway Hardware:&nbsp;{data.model}</p>}
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
                  STABLE_URL +
                  'v' +
                  latestVersion +
                  '/' +
                  getBinURL(latestVersion as string)
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
                href={DEV_URL + getBinURL(latestDevVersion as string)}
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
