import axios from 'axios';
import { useCallback, useEffect, useState } from 'react';
import { toast } from 'react-toastify';

import type { FileUploadConfig } from 'api/endpoints';
import type { AxiosPromise, CancelTokenSource, AxiosProgressEvent } from 'axios';

import { useI18nContext } from 'i18n/i18n-react';
import { extractErrorMessage } from 'utils';

interface MediaUploadOptions {
  upload: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
}

const useFileUpload = ({ upload }: MediaUploadOptions) => {
  const { LL } = useI18nContext();

  const [uploading, setUploading] = useState<boolean>(false);
  const [md5, setMd5] = useState<string>('');
  const [uploadProgress, setUploadProgress] = useState<AxiosProgressEvent>();
  const [uploadCancelToken, setUploadCancelToken] = useState<CancelTokenSource>();

  const resetUploadingStates = () => {
    setUploading(false);
    setUploadProgress(undefined);
    setUploadCancelToken(undefined);
    setMd5('');
  };

  const cancelUpload = useCallback(() => {
    uploadCancelToken?.cancel();
    resetUploadingStates();
  }, [uploadCancelToken]);

  useEffect(
    () => () => {
      uploadCancelToken?.cancel();
    },
    [uploadCancelToken]
  );

  const uploadFile = async (images: File[]) => {
    try {
      const cancelToken = axios.CancelToken.source();
      setUploadCancelToken(cancelToken);
      setUploading(true);
      const response = await upload(images[0], {
        onUploadProgress: setUploadProgress,
        cancelToken: cancelToken.token
      });
      resetUploadingStates();
      if (response.status === 200) {
        toast.success(LL.UPLOAD() + ' ' + LL.SUCCESSFUL());
      } else if (response.status === 201) {
        setMd5(String(response.data));
        toast.success(LL.UPLOAD() + ' MD5 ' + LL.SUCCESSFUL());
      }
    } catch (error) {
      if (axios.isCancel(error)) {
        toast.warning(LL.UPLOAD() + ' ' + LL.ABORTED());
      } else {
        resetUploadingStates();
        toast.error(extractErrorMessage(error, LL.UPLOAD() + ' ' + LL.FAILED()));
      }
    }
  };

  return [uploadFile, cancelUpload, uploading, uploadProgress, md5] as const;
};

export default useFileUpload;
