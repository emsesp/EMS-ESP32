import DownloadIcon from '@mui/icons-material/GetApp';
import { Typography, Button, Box } from '@mui/material';
import { useRequest } from 'alova';
import { useState, type FC } from 'react';
import { toast } from 'react-toastify';
import RestartMonitor from './RestartMonitor';

import * as SystemApi from 'api/system';
import { SectionContent, SingleUpload } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import * as EMSESP from 'project/api';

const UploadFileForm: FC = () => {
  const { LL } = useI18nContext();
  const [restarting, setRestarting] = useState<boolean>(false);
  const [md5, setMd5] = useState<string>();

  const { send: getSettings, onSuccess: onSuccessGetSettings } = useRequest(EMSESP.getSettings(), {
    immediate: false
  });
  const { send: getCustomizations, onSuccess: onSuccessgetCustomizations } = useRequest(EMSESP.getCustomizations(), {
    immediate: false
  });
  const { send: getEntities, onSuccess: onSuccessGetEntities } = useRequest(EMSESP.getEntities(), {
    immediate: false
  });
  const { send: getSchedule, onSuccess: onSuccessGetSchedule } = useRequest(EMSESP.getSchedule(), {
    immediate: false
  });

  const {
    loading: isUploading,
    uploading: progress,
    send: sendUpload,
    onSuccess: onSuccessUpload,
    abort
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

  const cancelUpload = () => {
    console.log('aborting upload...'); // TODO remove debug
    abort();
    toast.warning(LL.UPLOAD() + ' ' + LL.ABORTED());
  };

  const startUpload = async (files: File[]) => {
    await sendUpload(files[0]);
  };

  const saveFile = (json: any, endpoint: string) => {
    const anchor = document.createElement('a');
    anchor.href = URL.createObjectURL(
      new Blob([JSON.stringify(json, null, 2)], {
        type: 'text/plain'
      })
    );
    anchor.download = 'emsesp_' + endpoint + '.json';
    anchor.click();
    URL.revokeObjectURL(anchor.href);
    toast.info(LL.DOWNLOAD_SUCCESSFUL());
  };

  onSuccessGetSettings((event) => {
    saveFile(event.data, 'settings');
  });
  onSuccessgetCustomizations((event) => {
    saveFile(event.data, 'customizations');
  });
  onSuccessGetEntities((event) => {
    saveFile(event.data, 'entities');
  });
  onSuccessGetSchedule((event) => {
    saveFile(event.data, 'schedule');
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
    await getSchedule().catch((error) => {
      toast.error(error.message);
    });
  };

  const content = () => (
    <>
      <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
        {LL.UPLOAD()}
      </Typography>
      <Box mb={2} color="warning.main">
        <Typography variant="body2">{LL.UPLOAD_TEXT()} </Typography>
      </Box>
      {md5 && (
        <Box mb={2}>
          <Typography variant="body2">{'MD5: ' + md5}</Typography>
        </Box>
      )}
      <SingleUpload onDrop={startUpload} onCancel={cancelUpload} isUploading={isUploading} progress={progress} />
      {!isUploading && (
        <>
          <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
            {LL.DOWNLOAD(0)}
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
              {LL.DOWNLOAD_CUSTOMIZATION_TEXT()}{' '}
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
              {LL.DOWNLOAD_SCHEDULE_TEXT()}{' '}
            </Typography>
          </Box>
          <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={downloadSchedule}>
            {LL.SCHEDULE(0)}
          </Button>
        </>
      )}
    </>
  );
  return (
    <SectionContent title={LL.UPLOAD_DOWNLOAD()} titleGutter>
      {restarting ? <RestartMonitor /> : content()}
    </SectionContent>
  );
};

export default UploadFileForm;
