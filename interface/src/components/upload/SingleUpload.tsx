import { ChangeEvent, useRef, useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import UploadIcon from '@mui/icons-material/Upload';
import { Box, Button, LinearProgress, Typography } from '@mui/material';

import * as SystemApi from 'api/system';

import { useRequest } from 'alova/client';
import RestartMonitor from 'app/status/RestartMonitor';
import { useI18nContext } from 'i18n/i18n-react';

const SingleUpload = () => {
  const [md5, setMd5] = useState<string>();
  const [restarting, setRestarting] = useState<boolean>();

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
      setRestarting(true);
    }
  });

  const [file, setFile] = useState<File>();
  const inputRef = useRef<HTMLInputElement | null>(null);

  const { LL } = useI18nContext();

  console.log(progress); // TODO remove

  const handleUploadClick = () => {
    inputRef.current?.click();
  };

  const handleFileChange = async (e: ChangeEvent<HTMLInputElement>) => {
    if (!e.target.files) {
      return;
    }

    setFile(e.target.files[0]);

    await sendUpload(e.target.files[0]).catch((error: Error) => {
      if (error.message === 'The user aborted a request') {
        toast.warning(LL.UPLOAD() + ' ' + LL.ABORTED());
      } else if (error.message === 'Network Error') {
        toast.warning('Invalid file extension or incompatible bin file');
      } else {
        toast.error(error.message);
      }
    });
  };

  return (
    <>
      <input
        type="file"
        ref={inputRef}
        style={{ display: 'none' }}
        onChange={handleFileChange}
      />

      <Button
        sx={{ ml: 2 }}
        startIcon={<UploadIcon />}
        variant="outlined"
        color="secondary"
        onClick={handleUploadClick}
      >
        {file ? LL.UPLOADING() + ` ${file.name}` : LL.UPLOAD()}
      </Button>

      {isUploading && (
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
            startIcon={<CancelIcon />}
            variant="outlined"
            color="secondary"
            onClick={cancelUpload}
          >
            {LL.CANCEL()}
          </Button>
        </>
      )}

      {md5 && (
        <Box mt={2}>
          <Typography variant="body2">{'MD5: ' + md5}</Typography>
        </Box>
      )}

      {restarting && <RestartMonitor />}
    </>
  );
};

export default SingleUpload;
