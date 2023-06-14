import DownloadIcon from '@mui/icons-material/GetApp';
import { Typography, Button, Box } from '@mui/material';
import { useRequest } from 'alova';
import { toast } from 'react-toastify';
import type { FileUploadConfig } from 'api/endpoints';
import type { AxiosPromise } from 'axios';
import type { FC } from 'react';

import { SingleUpload, useFileUpload } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import * as EMSESP from 'project/api';

interface UploadFileProps {
  uploadGeneralFile: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
}

const GeneralFileUpload: FC<UploadFileProps> = ({ uploadGeneralFile }) => {
  const [uploadFile, cancelUpload, uploading, uploadProgress, md5] = useFileUpload({ upload: uploadGeneralFile });

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

  const { LL } = useI18nContext();

  const saveFile = (json: any, endpoint: string) => {
    const a = document.createElement('a');
    const filename = 'emsesp_' + endpoint + '.json';
    a.href = URL.createObjectURL(
      new Blob([JSON.stringify(json, null, 2)], {
        type: 'text/plain'
      })
    );
    a.setAttribute('download', filename);
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
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

  return (
    <>
      {!uploading && (
        <>
          <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
            {LL.UPLOAD()}
          </Typography>
          <Box mb={2} color="warning.main">
            <Typography variant="body2">{LL.UPLOAD_TEXT()} </Typography>
          </Box>
        </>
      )}
      {md5 !== '' && (
        <Box mb={2}>
          <Typography variant="body2">{'MD5: ' + md5}</Typography>
        </Box>
      )}
      <SingleUpload onDrop={uploadFile} onCancel={cancelUpload} uploading={uploading} progress={uploadProgress} />

      {!uploading && (
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
};

export default GeneralFileUpload;
