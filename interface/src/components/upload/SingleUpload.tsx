import { FC, Fragment } from 'react';
import { useDropzone, DropzoneState } from 'react-dropzone';

import { AxiosProgressEvent } from 'axios';

import { Box, Button, LinearProgress, Theme, Typography, useTheme } from '@mui/material';

import CloudUploadIcon from '@mui/icons-material/CloudUpload';
import CancelIcon from '@mui/icons-material/Cancel';

import { useI18nContext } from 'i18n/i18n-react';

const getBorderColor = (theme: Theme, props: DropzoneState) => {
  if (props.isDragAccept) {
    return theme.palette.success.main;
  }
  if (props.isDragReject) {
    return theme.palette.error.main;
  }
  if (props.isDragActive) {
    return theme.palette.info.main;
  }
  return theme.palette.grey[700];
};

export interface SingleUploadProps {
  onDrop: (acceptedFiles: File[]) => void;
  onCancel: () => void;
  uploading: boolean;
  progress?: AxiosProgressEvent;
}

const SingleUpload: FC<SingleUploadProps> = ({ onDrop, onCancel, uploading, progress }) => {
  const dropzoneState = useDropzone({
    onDrop,
    accept: {
      'application/octet-stream': ['.bin'],
      'application/json': ['.json'],
      'text/plain': ['.md5']
    },
    disabled: uploading,
    multiple: false
  });
  const { getRootProps, getInputProps } = dropzoneState;
  const theme = useTheme();

  const { LL } = useI18nContext();

  const progressText = () => {
    if (uploading) {
      if (progress?.total) {
        return LL.UPLOADING() + ': ' + Math.round((progress.loaded * 100) / progress.total) + '%';
      }
      return LL.UPLOADING() + `\u2026`;
    }
    return LL.UPLOAD_DROP_TEXT();
  };

  return (
    <Box
      {...getRootProps({
        sx: {
          py: 8,
          px: 2,
          borderWidth: 2,
          borderRadius: 2,
          borderStyle: 'dashed',
          color: theme.palette.grey[400],
          transition: 'border .24s ease-in-out',
          width: '100%',
          cursor: uploading ? 'default' : 'pointer',
          borderColor: getBorderColor(theme, dropzoneState)
        }
      })}
    >
      <input {...getInputProps()} />
      <Box flexDirection="column" display="flex" alignItems="center">
        <CloudUploadIcon fontSize="large" />
        <Typography variant="h6">{progressText()}</Typography>
        {uploading && (
          <Fragment>
            <Box width="100%" p={2}>
              <LinearProgress
                variant={!progress || progress.total ? 'determinate' : 'indeterminate'}
                value={!progress ? 0 : progress.total ? Math.round((progress.loaded * 100) / progress.total) : 0}
              />
            </Box>
            <Button startIcon={<CancelIcon />} variant="outlined" color="secondary" onClick={onCancel}>
              {LL.CANCEL()}
            </Button>
          </Fragment>
        )}
      </Box>
    </Box>
  );
};

export default SingleUpload;
