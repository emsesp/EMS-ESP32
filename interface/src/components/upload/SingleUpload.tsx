import { useEffect, useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import { Box, Button, LinearProgress, Typography } from '@mui/material';

import * as SystemApi from 'api/system';

import { useRequest } from 'alova/client';
import RestartMonitor from 'app/status/RestartMonitor';
import MessageBox from 'components/MessageBox';
import { useI18nContext } from 'i18n/i18n-react';

import DragNdrop from './DragNdrop';

const SingleUpload = () => {
  const [md5, setMd5] = useState<string>();
  const [restarting, setRestarting] = useState<boolean>(false);
  const [restartNeeded, setRestartNeeded] = useState<boolean>(false);

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

  const { send: restartCommand } = useRequest(SystemApi.restart(), {
    immediate: false
  });

  const restart = async () => {
    await restartCommand().catch((error: Error) => {
      toast.error(error.message);
    });
    setRestarting(true);
  };

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

      {isUploading || restartNeeded ? (
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

          {!restartNeeded && (
            <Button
              sx={{ ml: 2 }}
              startIcon={<CancelIcon />}
              variant="outlined"
              color="secondary"
              onClick={cancelUpload}
            >
              {LL.CANCEL()}
            </Button>
          )}
        </>
      ) : (
        <DragNdrop onFileSelected={setFile} width="340px" height="140px" />
      )}

      {md5 && (
        <Box mt={2}>
          <Typography variant="body2">{'MD5: ' + md5}</Typography>
        </Box>
      )}

      {restartNeeded && (
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
      )}

      {restarting && <RestartMonitor />}
    </>
  );
};

export default SingleUpload;
