import { FC, useRef, useState } from 'react';

import * as SystemApi from '../../api/system';
import { SectionContent } from '../../components';
import { FileUploadConfig } from '../../api/endpoints';

import GeneralFileUpload from './GeneralFileUpload';
import RestartMonitor from './RestartMonitor';

import { useI18nContext } from '../../i18n/i18n-react';

const UploadFileForm: FC = () => {
  const [restarting, setRestarting] = useState<boolean>();

  const { LL } = useI18nContext();

  const uploadFile = useRef(async (file: File, config?: FileUploadConfig) => {
    const response = await SystemApi.uploadFile(file, config);
    if (response.status === 200) {
      setRestarting(true);
    }
    return response;
  });

  return (
    <SectionContent title={LL.UPLOAD_DOWNLOAD()} titleGutter>
      {restarting ? <RestartMonitor /> : <GeneralFileUpload uploadGeneralFile={uploadFile.current} />}
    </SectionContent>
  );
};

export default UploadFileForm;
