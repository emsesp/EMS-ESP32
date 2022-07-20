import { FC, useContext } from 'react';
import { AxiosPromise } from 'axios';

import { Typography, Button, Box } from '@mui/material';

import { FileUploadConfig } from '../../api/endpoints';
import { SingleUpload, useFileUpload } from '../../components';

import { AuthenticatedContext } from '../../contexts/authentication';

import DownloadIcon from '@mui/icons-material/GetApp';

import { useSnackbar } from 'notistack';

import { extractErrorMessage } from '../../utils';

import * as EMSESP from '../../project/api';

interface UploadFileProps {
  uploadGeneralFile: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
}

const GeneralFileUpload: FC<UploadFileProps> = ({ uploadGeneralFile }) => {
  const [uploadFile, cancelUpload, uploading, uploadProgress] = useFileUpload({ upload: uploadGeneralFile });

  const { me } = useContext(AuthenticatedContext);
  const { enqueueSnackbar } = useSnackbar();

  const saveFile = (json: any, endpoint: string) => {
    const a = document.createElement('a');
    const filename = 'emsesp_' + endpoint + '_json.txt';
    a.href = URL.createObjectURL(
      new Blob([JSON.stringify(json, null, 2)], {
        type: 'text/plain'
      })
    );
    a.setAttribute('download', filename);
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    enqueueSnackbar('File downloaded', { variant: 'info' });
  };

  const downloadSettings = async () => {
    try {
      const response = await EMSESP.getSettings();
      if (response.status !== 200) {
        enqueueSnackbar('Unable to get settings', { variant: 'error' });
      } else {
        saveFile(response.data, 'settings');
      }
    } catch (error: unknown) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem with downloading'), { variant: 'error' });
    }
  };

  const downloadCustomizations = async () => {
    try {
      const response = await EMSESP.getCustomizations();
      if (response.status !== 200) {
        enqueueSnackbar('Unable to get customizations', { variant: 'error' });
      } else {
        saveFile(response.data, 'customizations');
      }
    } catch (error: unknown) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem with downloading'), { variant: 'error' });
    }
  };

  return (
    <>
      <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
        Upload
      </Typography>
      {!uploading && (
        <Box mb={2} color="warning.main">
          <Typography variant="body2">
            Upload a new firmware (.bin) file, settings or customizations (*_json.txt) file below.
          </Typography>
        </Box>
      )}
      <SingleUpload onDrop={uploadFile} onCancel={cancelUpload} uploading={uploading} progress={uploadProgress} />

      <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
        Download
      </Typography>
      {me.admin && (
        <>
          <Box color="warning.main">
            <Typography mb={1} variant="body2">
              Download the application settings. Be careful when sharing your settings as this file contains passwords
              and other sensitive system information.
            </Typography>
          </Box>
          <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={() => downloadSettings()}>
            settings
          </Button>

          <Box color="warning.main">
            <Typography mt={2} mb={1} variant="body2">
              Download the entity customizations.
            </Typography>
          </Box>
          <Button
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => downloadCustomizations()}
          >
            customizations
          </Button>
        </>
      )}
    </>
  );
};

export default GeneralFileUpload;
