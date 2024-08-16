import { useEffect, useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import { Box, Button, LinearProgress, Typography } from '@mui/material';

import * as SystemApi from 'api/system';

import { useRequest } from 'alova/client';
import { useI18nContext } from 'i18n/i18n-react';

import DragNdrop from './DragNdrop';

const SingleUpload = ({ setRestartNeeded }) => {
  const [md5, setMd5] = useState<string>();
  const [file, setFile] = useState<File>();
  const { LL } = useI18nContext();

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
      setFile(undefined);
    } else {
      setRestartNeeded(true);
    }
  });

  useEffect(async () => {
    if (file) {
      console.log('going to upload file ', file.name);
      await sendUpload(file).catch((error: Error) => {
        if (error.message === 'The user aborted a request') {
          toast.warning(LL.UPLOAD() + ' ' + LL.ABORTED());
        } else if (error.message === 'Network Error') {
          toast.warning('Invalid file extension or incompatible bin file');
        } else {
          toast.error(error.message);
        }
      });
    }
  }, [file]);

  return (
    <>
      <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
        {LL.UPLOAD()}
      </Typography>

      <Box mb={2} color="warning.main">
        <Typography variant="body2">{LL.UPLOAD_TEXT()}</Typography>
      </Box>

      {isUploading ? (
        <>
          <Box width="100%" p={2}>
            <LinearProgress
              variant="determinate"
              value={
                progress.total === 0 || progress.loaded === 0
                  ? 0
                  : progress.loaded <= progress.total
                    ? Math.round((progress.loaded * 100) / progress.total)
                    : Math.round((progress.total * 100) / progress.loaded)
              }
            />
          </Box>

          <Button
            sx={{ ml: 2 }}
            startIcon={<CancelIcon />}
            variant="outlined"
            color="error"
            onClick={cancelUpload}
          >
            {LL.CANCEL()}
          </Button>
        </>
      ) : (
        <DragNdrop onFileSelected={setFile} />
      )}

      {md5 && (
        <Box mt={2}>
          <Typography variant="body2">{'MD5: ' + md5}</Typography>
        </Box>
      )}
    </>
  );
};

export default SingleUpload;
