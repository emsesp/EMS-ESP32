import { useRequest } from 'alova';
// import axios from 'axios';
import { useCallback, useEffect, useState } from 'react';
import { toast } from 'react-toastify';

// import type { FileUploadConfig } from 'api/endpoints';
// import type { AxiosPromise, CancelTokenSource, AxiosProgressEvent } from 'axios';

import * as SystemApi from 'api/system';
import { useI18nContext } from 'i18n/i18n-react';

interface MediaUploadOptions {
  // TODO fileupload move to alova
  // upload: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
  upload: (file: File) => Promise<void>;
}

const useFileUpload = ({ upload }: MediaUploadOptions) => {
  const { LL } = useI18nContext();

  // const [uploading, setUploading] = useState<boolean>(false);
  const [md5, setMd5] = useState<string>('');
  // const [uploadProgress, setUploadProgress] = useState<AxiosProgressEvent>();
  // const [uploadCancelToken, setUploadCancelToken] = useState<CancelTokenSource>();

  const { uploading, send: sendUpload } = useRequest(SystemApi.uploadFile, {
    immediate: false
  });

  const resetUploadingStates = () => {
    // setUploading(false);
    // setUploadProgress(undefined);
    // setUploadCancelToken(undefined);
    setMd5('');
  };

  // const cancelUpload = useCallback(() => {
  //   uploadCancelToken?.cancel();
  //   resetUploadingStates();
  // }, [uploadCancelToken]);

  // useEffect(
  //   () => () => {
  //     uploadCancelToken?.cancel();
  //   },
  //   [uploadCancelToken]
  // );

  // TODO fileupload move to alova
  // TODO make it single file
  const uploadFile = async (files: File[]) => {
    // TODO remove debug
    console.log('useFileUpload.ts:uploadFile:' + files[0].name, files[0].size);

    await sendUpload(files[0]);

    // const response = await SystemApi.startUploadFile(files[0]);
    // console.log(response.status);

    // const response = await upload(files[0], {
    //   onUploadProgress: setUploadProgress,
    //   cancelToken: cancelToken.token
    // });

    // try {
    //   const cancelToken = axios.CancelToken.source();
    //   setUploadCancelToken(cancelToken);
    //   setUploading(true);
    //   const response = await upload(images[0], {
    //     onUploadProgress: setUploadProgress,
    //     cancelToken: cancelToken.token
    //   });
    //   resetUploadingStates();
    //   if (response.status === 200) {
    //     toast.success(LL.UPLOAD() + ' ' + LL.SUCCESSFUL());
    //   } else if (response.status === 201) {
    //     setMd5(String(response.data));
    //     toast.success(LL.UPLOAD() + ' MD5 ' + LL.SUCCESSFUL());
    //   }
    // } catch (error) {
    //   if (axios.isCancel(error)) {
    //     toast.warning(LL.UPLOAD() + ' ' + LL.ABORTED());
    //   } else {
    //     resetUploadingStates();
    //     toast.error(LL.UPLOAD() + ' ' + LL.FAILED(0));
    //   }
    // }
  };

  return [uploadFile, uploading, md5] as const;
  // return [uploadFile, cancelUpload, uploading, uploadProgress, md5] as const;
};

export default useFileUpload;
