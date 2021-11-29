import { FC } from 'react';
import { Box } from '@mui/material';

import { SectionContent, SingleUpload, useFileUpload } from '../../components';
import * as SystemApi from '../../api/system';

const UploadFirmwareForm: FC = () => {
  const [uploadFile, cancelUpload, uploading, uploadProgress] = useFileUpload({ upload: SystemApi.uploadFirmware });

  return (
    <SectionContent title="Upload Firmware" titleGutter>
      <Box py={2}>Upload a new firmware (.bin) file below to replace the existing firmware.</Box>
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
