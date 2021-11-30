import { FC } from 'react';

import * as SystemApi from '../../api/system';
import { MessageBox, SectionContent, SingleUpload, useFileUpload } from '../../components';

const UploadFirmwareForm: FC = () => {
  const [uploadFile, cancelUpload, uploading, uploadProgress] = useFileUpload({ upload: SystemApi.uploadFirmware });

  return (
    <SectionContent title="Upload Firmware" titleGutter>
      <MessageBox
        message="Upload a new firmware (.bin) file below to replace the existing firmware."
        level="warning"
        my={2}
      />
      <SingleUpload
        accept="application/octet-stream"
        onDrop={uploadFile}
        onCancel={cancelUpload}
        uploading={uploading}
        progress={uploadProgress}
      />
    </SectionContent>
  );
};

export default UploadFirmwareForm;
