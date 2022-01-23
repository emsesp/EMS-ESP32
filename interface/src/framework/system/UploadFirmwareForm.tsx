import { FC, useRef, useState } from 'react';

import * as SystemApi from '../../api/system';
import { SectionContent } from '../../components';
import { FileUploadConfig } from '../../api/endpoints';

import FirmwareFileUpload from './FirmwareFileUpload';
import FirmwareRestartMonitor from './FirmwareRestartMonitor';

const UploadFirmwareForm: FC = () => {
  const [restarting, setRestarting] = useState<boolean>();

  const uploadFirmware = useRef(async (file: File, config?: FileUploadConfig) => {
    const response = await SystemApi.uploadFirmware(file, config);
    setRestarting(true);
    return response;
  });

  return (
    <SectionContent title="Upload Firmware" titleGutter>
      {restarting ? <FirmwareRestartMonitor /> : <FirmwareFileUpload uploadFirmware={uploadFirmware.current} />}
    </SectionContent>
  );
};

export default UploadFirmwareForm;
