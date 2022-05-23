import { FC, useRef, useState } from 'react';

import * as SystemApi from '../../api/system';
import { SectionContent } from '../../components';
import { FileUploadConfig } from '../../api/endpoints';

import GeneralFileUpload from './GeneralFileUpload';
import RestartMonitor from './RestartMonitor';

const UploadFileForm: FC = () => {
  const [restarting, setRestarting] = useState<boolean>();

  const uploadFile = useRef(async (file: File, config?: FileUploadConfig) => {
    const response = await SystemApi.uploadFile(file, config);
    setRestarting(true);
    return response;
  });

  return (
    <SectionContent title="Upload File" titleGutter>
      {restarting ? <RestartMonitor /> : <GeneralFileUpload uploadGeneralFile={uploadFile.current} />}
    </SectionContent>
  );
};

export default UploadFileForm;
