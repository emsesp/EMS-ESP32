import DownloadIcon from '@mui/icons-material/GetApp';
import { Typography, Button, Box } from '@mui/material';
import { useRequest } from 'alova';
import { useState, type FC } from 'react';
import { toast } from 'react-toastify';
import RestartMonitor from './RestartMonitor';

import * as SystemApi from 'api/system';
import { SectionContent, SingleUpload, useLayoutTitle } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import * as EMSESP from 'project/api';

const UploadDownload: FC = () => {
  const { LL } = useI18nContext();
  const [restarting, setRestarting] = useState<boolean>();
  const [md5, setMd5] = useState<string>();

  const { send: getSettings, onSuccess: onSuccessGetSettings } = useRequest(EMSESP.getSettings(), {
    immediate: false
  });
  const { send: getCustomizations, onSuccess: onSuccessGetCustomizations } = useRequest(EMSESP.getCustomizations(), {
    immediate: false
  });
  const { send: getEntities, onSuccess: onSuccessGetEntities } = useRequest(EMSESP.getEntities(), {
    immediate: false
  });
  const { send: getSchedule, onSuccess: onSuccessGetSchedule } = useRequest(EMSESP.getSchedule(), {
    immediate: false
  });
  const { send: getAPI, onSuccess: onGetAPI } = useRequest((data) => EMSESP.API(data), {
    immediate: false
  });

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

  onSuccessUpload(({ data }: any) => {
    if (data) {
      setMd5(data.md5);
      toast.success(LL.UPLOAD() + ' MD5 ' + LL.SUCCESSFUL());
    } else {
      setRestarting(true);
    }
  });

  const startUpload = async (files: File[]) => {
    await sendUpload(files[0]).catch((err) => {
      if (err.message === 'The user aborted a request') {
        toast.warning(LL.UPLOAD() + ' ' + LL.ABORTED());
      } else if (err.message === 'Network Error') {
        toast.warning('Invalid file extension or incompatible bin file');
      } else {
        toast.error(err.message);
      }
    });
  };

  const saveFile = (json: any, endpoint: string) => {
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
    saveFile(event.data, event.sendArgs[0].device + '_' + event.sendArgs[0].entity + '.txt');
  });

  const downloadSettings = async () => {
    await getSettings().catch((error) => {
      toast.error(error.message);
    });
  };

  const downloadCustomizations = async () => {
    await getCustomizations().catch((error) => {
      toast.error(error.message);
    });
  };

  const downloadEntities = async () => {
    await getEntities().catch((error) => {
      toast.error(error.message);
    });
  };

  const downloadSchedule = async () => {
    await getSchedule()
      .catch((error) => {
        toast.error(error.message);
      })
      .finally(() => {
        toast.info(LL.DOWNLOAD_SUCCESSFUL());
      });
  };

  const callAPI = async (device: string, entity: string) => {
    await getAPI({ device, entity, id: 0 }).catch((error) => {
      toast.error(error.message);
    });
  };

  useLayoutTitle(LL.UPLOAD_DOWNLOAD());

  const content = () => (
    <>
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
      <SingleUpload onDrop={startUpload} onCancel={cancelUpload} isUploading={isUploading} progress={progress} />
      {!isUploading && (
        <>
          <Typography sx={{ pt: 4, pb: 2 }} variant="h6" color="primary">
            {LL.DOWNLOAD(0)}&nbsp;{LL.SUPPORT_INFORMATION(1)}
          </Typography>
          <Button
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

          <Typography sx={{ pt: 4, pb: 2 }} variant="h6" color="primary">
            {LL.DOWNLOAD(0)}&nbsp;{LL.SETTINGS(1)}
          </Typography>
          <Box color="warning.main">
            <Typography mb={1} variant="body2">
              {LL.DOWNLOAD_SETTINGS_TEXT()}
            </Typography>
          </Box>
          <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={downloadSettings}>
            {LL.SETTINGS_OF('')}
          </Button>
          <Box color="warning.main">
            <Typography mt={2} mb={1} variant="body2">
              {LL.DOWNLOAD_CUSTOMIZATION_TEXT()}
            </Typography>
          </Box>
          <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={downloadCustomizations}>
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
          <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={downloadSchedule}>
            {LL.SCHEDULE(0)}
          </Button>
        </>
      )}
    </>
  );
  return <SectionContent>{restarting ? <RestartMonitor /> : content()}</SectionContent>;
};

export default UploadDownload;
