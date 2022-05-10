import { AxiosPromise } from 'axios';
import { FC } from 'react';

import { FileUploadConfig } from '../../api/endpoints';
import { MessageBox, SingleUpload, useFileUpload } from '../../components';

interface UploadFileProps {
  uploadGeneralFile: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
}

const GeneralFileUpload: FC<UploadFileProps> = ({ uploadGeneralFile }) => {
  const [uploadFile, cancelUpload, uploading, uploadProgress] = useFileUpload({ upload: uploadGeneralFile });

  return (
    <>
      {!uploading && (
        <MessageBox
          message="Upload a new firmware (.bin) file or an exported settings or customizations (.json) file below. EMS-ESP will restart afterwards to apply the new changes."
          level="warning"
          my={2}
        />
      )}
      <SingleUpload onDrop={uploadFile} onCancel={cancelUpload} uploading={uploading} progress={uploadProgress} />
    </>
  );
};

export default GeneralFileUpload;
