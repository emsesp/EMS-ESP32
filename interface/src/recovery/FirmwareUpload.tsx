import { type ChangeEvent, type DragEvent, useRef, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import CloudUploadIcon from '@mui/icons-material/CloudUpload';
import UploadIcon from '@mui/icons-material/Upload';
import { Box, Button, Typography, styled } from '@mui/material';

import { toast } from 'components/toast';
import { LinearProgressWithLabel } from 'components/upload/LinearProgressWithLabel';

import { HttpError, type UploadHandle, uploadFile } from './api';

const DocumentUploader = styled(Box)<{ active?: boolean }>(({ theme, active }) => ({
  border: `2px dashed ${active ? '#6dc24b' : '#4282fe'}`,
  backgroundColor: '#2e3339',
  padding: theme.spacing(1.25),
  display: 'flex',
  flexDirection: 'column',
  alignItems: 'center',
  justifyContent: 'center',
  borderRadius: theme.spacing(1),
  cursor: 'pointer',
  minHeight: '120px',
  transition: 'border-color 0.2s ease-in-out'
}));

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
}

const FirmwareUpload = ({
  target,
  chip,
  onUploaded,
  onUnauthorized
}: FirmwareUploadProps) => {
  const [file, setFile] = useState<File>();
  const [dragged, setDragged] = useState(false);
  const [md5, setMd5] = useState<string>();
  const [progress, setProgress] = useState<number>();
  const uploadRef = useRef<UploadHandle | null>(null);
  const inputRef = useRef<HTMLInputElement | null>(null);

  const selectFile = (selected?: File) => {
    if (!selected) return;
    if (/\.(bin|md5)$/i.test(selected.name)) {
      setFile(selected);
    } else {
      toast.warning('Please select an EMS-ESP firmware .bin or .md5 file');
    }
  };

  const onFileChange = (e: ChangeEvent<HTMLInputElement>) => {
    selectFile(e.target.files?.[0]);
    e.target.value = ''; // allow the same file to be selected again
  };

  const onDrop = (e: DragEvent<HTMLDivElement>) => {
    e.preventDefault();
    setDragged(false);
    selectFile(e.dataTransfer.files[0]);
  };

  const startUpload = async () => {
    if (!file) return;
    setProgress(0);
    uploadRef.current = uploadFile(file, target, setProgress);
    try {
      const result = await uploadRef.current.promise;
      if (result.md5) {
        setMd5(result.md5);
        toast.success('MD5 received, now upload the firmware .bin');
      } else {
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
      } else {
        toast.error(uploadErrorMessage(status, chip));
        if (status === 401 || status === 403) onUnauthorized();
      }
    } finally {
      uploadRef.current = null;
      setProgress(undefined);
      setFile(undefined);
    }
  };

  if (progress !== undefined) {
    return (
      <>
        <Typography sx={{ mb: 1 }}>
          Uploading {file?.name} to {target}&hellip;
        </Typography>
        <LinearProgressWithLabel value={progress} />
        <Button
          sx={{ mt: 2 }}
          startIcon={<CancelIcon />}
          variant="outlined"
          color="secondary"
          onClick={() => uploadRef.current?.abort()}
        >
          Cancel
        </Button>
      </>
    );
  }

  return (
    <>
      <DocumentUploader
        active={!!(file || dragged)}
        onDrop={onDrop}
        onDragOver={(e) => {
          e.preventDefault();
          setDragged(true);
        }}
        onDragLeave={() => setDragged(false)}
        onClick={() => inputRef.current?.click()}
      >
        <Box sx={{ display: 'flex', alignItems: 'center' }}>
          <CloudUploadIcon sx={{ mr: 4 }} color="primary" fontSize="large" />
          <Typography>
            {md5
              ? 'MD5 received. Now drop the firmware .bin file here, or click to browse'
              : 'Drop an EMS-ESP firmware .bin file here, or click to browse'}
          </Typography>
        </Box>
        <input
          type="file"
          hidden
          ref={inputRef}
          accept=".bin,.md5"
          onChange={onFileChange}
          style={{ display: 'none' }}
        />
        {file && (
          <>
            <Typography sx={{ fontSize: 14, color: '#6dc24b', my: 1 }}>
              {file.name}
            </Typography>
            <Box>
              <Button
                startIcon={<CancelIcon />}
                variant="outlined"
                color="secondary"
                onClick={(e) => {
                  e.stopPropagation();
                  setFile(undefined);
                }}
              >
                Cancel
              </Button>
              <Button
                sx={{ ml: 2 }}
                startIcon={<UploadIcon />}
                variant="outlined"
                color="primary"
                onClick={(e) => {
                  e.stopPropagation();
                  void startUpload();
                }}
              >
                Upload
              </Button>
            </Box>
          </>
        )}
      </DocumentUploader>
      {md5 && (
        <Typography sx={{ mt: 2 }} variant="body2" color="success">
          {'MD5: ' + md5}
        </Typography>
      )}
    </>
  );
};

export default FirmwareUpload;
