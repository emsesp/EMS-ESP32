import { FC } from 'react';
import { AxiosPromise } from 'axios';

import { Typography, Button, Box } from '@mui/material';

import { FileUploadConfig } from 'api/endpoints';

import { SingleUpload, useFileUpload } from 'components';

import DownloadIcon from '@mui/icons-material/GetApp';

import { toast } from 'react-toastify';

import { extractErrorMessage } from 'utils';

import * as EMSESP from 'project/api';

import { useI18nContext } from 'i18n/i18n-react';

interface UploadFileProps {
  uploadGeneralFile: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
}

const GeneralFileUpload: FC<UploadFileProps> = ({ uploadGeneralFile }) => {
  const [uploadFile, cancelUpload, uploading, uploadProgress, md5] = useFileUpload({ upload: uploadGeneralFile });

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

  const downloadSettings = async () => {
    try {
      const response = await EMSESP.getSettings();
      if (response.status !== 200) {
        toast.error(LL.PROBLEM_LOADING());
        saveFile(response.data, 'settings');
      }
    } catch (error) {
      toast.error(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  };

  const downloadCustomizations = async () => {
    try {
      const response = await EMSESP.getCustomizations();
      if (response.status !== 200) {
        toast.error(LL.PROBLEM_LOADING());
      } else {
        saveFile(response.data, 'customizations');
      }
    } catch (error) {
      toast.error(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  };

  const downloadSchedule = async () => {
    try {
      const response = await EMSESP.readSchedule();
      if (response.status !== 200) {
        toast.error(LL.PROBLEM_LOADING());
        saveFile(response.data, 'schedule');
      }
    } catch (error) {
      toast.error(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
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
          <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={() => downloadSettings()}>
            {LL.SETTINGS_OF('')}
          </Button>
          <Box color="warning.main">
            <Typography mt={2} mb={1} variant="body2">
              {LL.DOWNLOAD_CUSTOMIZATION_TEXT()}{' '}
            </Typography>
          </Box>
          <Button
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => downloadCustomizations()}
          >
            {LL.CUSTOMIZATIONS()}
          </Button>
          <Box color="warning.main">
            <Typography mt={2} mb={1} variant="body2">
              {LL.DOWNLOAD_SCHEDULE_TEXT()}{' '}
            </Typography>
          </Box>
          <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={() => downloadSchedule()}>
            {LL.SCHEDULE(0)}
          </Button>
        </>
      )}
    </>
  );
};

export default GeneralFileUpload;
