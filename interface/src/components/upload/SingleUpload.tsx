import { useEffect, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import { Box, Button, Typography } from '@mui/material';

import * as SystemApi from 'api/system';

import { useRequest } from 'alova/client';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';

import DragNdrop from './DragNdrop';
import { LinearProgressWithLabel } from './LinearProgressWithLabel';

interface SingleUploadProps {
  doRestart: () => void;
  text: string;
}

const SingleUpload = ({ doRestart, text }: SingleUploadProps) => {
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
    let payload = data;
    if (typeof payload === 'string' && payload.length > 0) {
      try {
        payload = JSON.parse(payload);
      } catch {
        payload = data;
      }
    }
    if (payload && typeof payload === 'object' && 'md5' in payload) {
      setMd5((payload as { md5: string }).md5);
      toast.success(LL.UPLOAD_MD5_RECEIVED());
      setFile(undefined);
    } else {
      if (payload && typeof payload === 'object' && 'md5_ok' in payload) {
        toast.success(LL.UPLOAD_MD5_MATCHED());
      }
      doRestart();
    }
  });

  useEffect(() => {
    const uploadFile = async () => {
      if (file) {
        await sendUpload(file).catch((error: Error) => {
          if (error.message.includes('The user aborted a request')) {
            toast.warning(LL.UPLOAD() + ' ' + LL.ABORTED());
          } else {
            toast.warning('Invalid file extension or incompatible bin file');
          }
        });
      }
    };
    void uploadFile();
  }, [file]);

  return (
    <>
      {isUploading ? (
        <>
          <Box sx={{ width: '100%', pl: 2, pr: 2 }}>
            <LinearProgressWithLabel
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
            sx={{ ml: 2, mt: 2 }}
            startIcon={<CancelIcon />}
            variant="outlined"
            color="secondary"
            onClick={cancelUpload}
          >
            {LL.CANCEL()}
          </Button>
        </>
      ) : (
        <DragNdrop
          text={(md5 ? LL.UPLOAD_MD5_RECEIVED() : text) + '...'}
          onFileSelected={setFile}
        />
      )}

      {md5 && (
        <Box sx={{ mt: 2 }}>
          <Typography variant="body2" color="success">
            {'MD5: ' + md5}
          </Typography>
        </Box>
      )}
    </>
  );
};

export default SingleUpload;
