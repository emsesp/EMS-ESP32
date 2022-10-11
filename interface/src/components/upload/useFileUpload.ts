import { useCallback, useEffect, useState } from 'react';
import axios, { AxiosPromise, CancelTokenSource, AxiosProgressEvent } from 'axios';
import { useSnackbar } from 'notistack';

import { extractErrorMessage } from '../../utils';
import { FileUploadConfig } from '../../api/endpoints';

import { useI18nContext } from '../../i18n/i18n-react';

interface MediaUploadOptions {
  upload: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
}

const useFileUpload = ({ upload }: MediaUploadOptions) => {
  const { LL } = useI18nContext();

  const { enqueueSnackbar } = useSnackbar();
  const [uploading, setUploading] = useState<boolean>(false);
  const [uploadProgress, setUploadProgress] = useState<AxiosProgressEvent>();
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
      enqueueSnackbar(LL.UPLOAD() + ' ' + LL.SUCCESSFUL(), { variant: 'success' });
    } catch (error) {
      if (axios.isCancel(error)) {
        enqueueSnackbar(LL.UPLOAD() + ' ' + LL.ABORTED(), { variant: 'warning' });
      } else {
        resetUploadingStates();
        enqueueSnackbar(extractErrorMessage(error, LL.UPLOAD() + ' ' + LL.FAILED()), { variant: 'error' });
      }
    }
  };

  return [uploadFile, cancelUpload, uploading, uploadProgress] as const;
};

export default useFileUpload;
