import React, { Component } from 'react';

import { SectionContent } from '../components';
import { UPLOAD_FIRMWARE_ENDPOINT } from '../api';

import UploadFirmwareForm from './UploadFirmwareForm';
import { redirectingAuthorizedUpload } from '../authentication';
import { withSnackbar, WithSnackbarProps } from 'notistack';

interface UploadFirmwareControllerState {
  xhr?: XMLHttpRequest;
  progress?: ProgressEvent;
}

class UploadFirmwareController extends Component<WithSnackbarProps, UploadFirmwareControllerState> {

  state: UploadFirmwareControllerState = {
    xhr: undefined,
    progress: undefined
  };

  componentWillUnmount() {
    this.state.xhr?.abort();
  }

  updateProgress = (progress: ProgressEvent) => {
    this.setState({ progress });
  }

  uploadFile = (file: File) => {
    if (this.state.xhr) {
      return;
    }
    var xhr = new XMLHttpRequest();
    this.setState({ xhr });
    redirectingAuthorizedUpload(xhr, UPLOAD_FIRMWARE_ENDPOINT, file, this.updateProgress).then(() => {
      if (xhr.status !== 200) {
        throw Error("Invalid status code: " + xhr.status);
      }
      this.props.enqueueSnackbar("Activating new firmware", { variant: 'success' });
      this.setState({ xhr: undefined, progress: undefined });
    }).catch((error: Error) => {
      if (error.name === 'AbortError') {
        this.props.enqueueSnackbar("Upload cancelled by user", { variant: 'warning' });
      } else {
        const errorMessage = error.name === 'UploadError' ? "Error during upload" : (error.message || "Unknown error");
        this.props.enqueueSnackbar("Problem uploading: " + errorMessage, { variant: 'error' });
        this.setState({ xhr: undefined, progress: undefined });
      }
    });
  }

  cancelUpload = () => {
    if (this.state.xhr) {
      this.state.xhr.abort();
      this.setState({ xhr: undefined, progress: undefined });
    }
  }

  render() {
    const { xhr, progress } = this.state;
    return (
      <SectionContent title="Upload Firmware">
        <UploadFirmwareForm onFileSelected={this.uploadFile} onCancel={this.cancelUpload} uploading={!!xhr} progress={progress} />
      </SectionContent>
    );
  }

}

export default withSnackbar(UploadFirmwareController);
