import { useCallback, useEffect, useState } from 'react';
import axios, { AxiosPromise, CancelTokenSource } from 'axios';
import { useSnackbar } from "notistack";
import { extractErrorMessage } from '../../utils';
import { FileUploadConfig } from '../../api/endpoints';

interface MediaUploadOptions {
  upload: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
}

const useUploadFile = ({ upload }: MediaUploadOptions) => {
  const { enqueueSnackbar } = useSnackbar();
  const [uploading, setUploading] = useState<boolean>(false);
  const [uploadProgress, setUploadProgress] = useState<ProgressEvent>();
  const [uploadCancelToken, setUploadCancelToken] = useState<CancelTokenSource>();

  const resetUploadingStates = () => {
    setUploading(false);
    setUploadProgress(undefined);
    setUploadCancelToken(undefined);
  };

  const cancelUpload = useCallback(() => {
    uploadCancelToken?.cancel();
    resetUploadingStates();
  }, [uploadCancelToken]);

  useEffect(() => {
    return () => {
      uploadCancelToken?.cancel();
    };
  }, [uploadCancelToken]);

  const uploadFile = async (images: File[]) => {
    try {
      const cancelToken = axios.CancelToken.source();
      setUploadCancelToken(cancelToken);
      setUploading(true);
      await upload(images[0], {
        onUploadProgress: setUploadProgress,
        cancelToken: cancelToken.token
      });
      resetUploadingStates();
      enqueueSnackbar('Upload successful', { variant: 'success' });
    } catch (error: any) {
      if (axios.isCancel(error)) {
        enqueueSnackbar('Upload aborted', { variant: 'warning' });
      } else {
        resetUploadingStates();
        enqueueSnackbar(extractErrorMessage(error, 'Upload failed'), { variant: 'error' });
      }
    }
  };

  return [uploadFile, cancelUpload, uploading, uploadProgress] as const;
};

export default useUploadFile;
