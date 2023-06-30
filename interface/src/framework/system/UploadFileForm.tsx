import { useRequest } from 'alova';
import { useRef, useState } from 'react';
import GeneralFileUpload from './GeneralFileUpload';
import RestartMonitor from './RestartMonitor';
import type { FC } from 'react';

import * as SystemApi from 'api/system';
import { SectionContent } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

const UploadFileForm: FC = () => {
  const [restarting, setRestarting] = useState<boolean>();

  const { LL } = useI18nContext();

  const {
    loading,
    data,
    uploading,
    send: sendUpload
  } = useRequest(SystemApi.uploadFile, {
    immediate: false
  });

  const uploadFile = useRef(async (file: File) => {
    // TODO fileupload move to alova
    console.log('UploadFileForm.tsx: uploadFile duplicate!!!'); // TODO do we need this function??? duplicate?
    await sendUpload(file);

    // const response = await SystemApi.uploadFile(file);
    // if (response.status === 200) {
    //   setRestarting(true);
    // }
    // return response;
  });

  return (
    <SectionContent title={LL.UPLOAD_DOWNLOAD()} titleGutter>
      {restarting ? <RestartMonitor /> : <GeneralFileUpload uploadGeneralFile={uploadFile.current} />}
    </SectionContent>
  );
};

export default UploadFileForm;
