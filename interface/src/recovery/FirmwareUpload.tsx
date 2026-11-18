import { useRef, useState } from 'react';

import { Button, FileUpload, Message, toast } from 'components/ui';
import { CUSTOM_BUILD_URL } from 'utils/versions';

import { HttpError, type UploadHandle, uploadFile } from './api';

const uploadErrorMessage = (status: number, chip: string) => {
  switch (status) {
    case 400:
      return 'Invalid target partition';
    case 401:
    case 403:
      return 'Not authorized, please sign in again';
    case 409:
      return 'This is the recovery firmware, it can only be installed from EMS-ESP';
    case 503:
      return `This firmware is not built for the ${chip}`;
    case 507:
      return 'The firmware does not fit in the partition';
    case 500:
      return 'Upload failed, flash write error or MD5 mismatch';
    default:
      return 'Upload failed. Only EMS-ESP firmware .bin and .md5 files are accepted';
  }
};

interface FirmwareUploadProps {
  target: string;
  chip: string;
  onUploaded: () => void;
  onUnauthorized: () => void;
  // reports while an upload runs, so the target can't be changed meanwhile
  onBusy: (busy: boolean) => void;
}

const FirmwareUpload = ({
  target,
  chip,
  onUploaded,
  onUnauthorized,
  onBusy
}: FirmwareUploadProps) => {
  const [uploading, setUploading] = useState<File>();
  const [md5, setMd5] = useState<string>();
  // the file the device stopped because it isn't an official build
  const [customFile, setCustomFile] = useState<File>();
  const [progress, setProgress] = useState<number>();
  const uploadRef = useRef<UploadHandle | null>(null);

  const startUpload = async (file: File, custom = false) => {
    setUploading(file);
    setProgress(0);
    setCustomFile(undefined);
    onBusy(true);
    uploadRef.current = uploadFile(file, target, setProgress, custom);
    try {
      const result = await uploadRef.current.promise;
      if (result.md5) {
        setMd5(result.md5);
        toast.success('MD5 received, now upload the firmware .bin');
      } else {
        if (result.official) {
          toast.success('Official build');
        }
        if (result.md5_ok) {
          toast.success('Firmware MD5 matches');
        }
        toast.success(`Firmware written to ${target}`);
        onUploaded();
      }
    } catch (error) {
      const status = error instanceof HttpError ? error.status : 0;
      if (status === -1) {
        toast.warning('Upload aborted');
      } else if (status === 412) {
        setCustomFile(file);
      } else {
        toast.error(uploadErrorMessage(status, chip));
        if (status === 401 || status === 403) onUnauthorized();
      }
    } finally {
      uploadRef.current = null;
      setProgress(undefined);
      setUploading(undefined);
      onBusy(false);
    }
  };

  return (
    <>
      {uploading && (
        <p className="rc-p">
          Uploading <span className="mono">{uploading.name}</span> to {target}
          &hellip;
        </p>
      )}
      <FileUpload
        text={
          md5
            ? 'MD5 received. Now drop the firmware .bin file here, or click to browse'
            : 'Drop an EMS-ESP firmware .bin file here, or click to browse'
        }
        accept={['.bin', '.md5']}
        onUpload={(file) => void startUpload(file)}
        progress={progress}
        onCancel={() => uploadRef.current?.abort()}
      />
      {customFile && (
        <Message
          level="warning"
          className="rc-msg"
          actions={
            <Button
              size="sm"
              color="warning"
              icon="warning"
              onClick={() => void startUpload(customFile, true)}
            >
              Install anyway
            </Button>
          }
        >
          This is not an official EMS-ESP build. Only install firmware you trust.{' '}
          <a href={CUSTOM_BUILD_URL} target="_blank" rel="noreferrer">
            About official and custom builds
          </a>
        </Message>
      )}
      {md5 && <p className="cap mono rc-md5">MD5: {md5}</p>}
    </>
  );
};

export default FirmwareUpload;
