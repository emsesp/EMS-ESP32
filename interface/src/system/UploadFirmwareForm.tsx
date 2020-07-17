import React, { Fragment } from 'react';
import { SingleUpload } from '../components';
import { Box } from '@material-ui/core';

interface UploadFirmwareFormProps {
  uploading: boolean;
  progress?: ProgressEvent;
  onFileSelected: (file: File) => void;
  onCancel: () => void;
}

class UploadFirmwareForm extends React.Component<UploadFirmwareFormProps> {

  handleDrop = (files: File[]) => {
    const file = files[0];
    if (file) {
      this.props.onFileSelected(files[0]);
    }
  };

  render() {
    const { uploading, progress, onCancel } = this.props;
    return (
      <Fragment>
        <Box py={2}>
          Upload a new firmware (.bin) file below to replace the existing firmware.
        </Box>
        <SingleUpload accept="application/octet-stream" onDrop={this.handleDrop} uploading={uploading} progress={progress} onCancel={onCancel} />
      </Fragment>
    );
  }

}

export default UploadFirmwareForm;
