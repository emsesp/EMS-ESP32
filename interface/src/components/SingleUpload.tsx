import React, { FC, Fragment } from 'react';
import { useDropzone, DropzoneState } from 'react-dropzone';

import { makeStyles, createStyles } from '@material-ui/styles';
import CloudUploadIcon from '@material-ui/icons/CloudUpload';
import CancelIcon from '@material-ui/icons/Cancel';
import { Theme, Box, Typography, LinearProgress, Button } from '@material-ui/core';

interface SingleUploadStyleProps extends DropzoneState {
  uploading: boolean;
}

const progressPercentage = (progress: ProgressEvent) => Math.round((progress.loaded * 100) / progress.total);

const getBorderColor = (theme: Theme, props: SingleUploadStyleProps) => {
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
}

const useStyles = makeStyles((theme: Theme) => createStyles({
  dropzone: {
    padding: theme.spacing(8, 2),
    borderWidth: 2,
    borderRadius: 2,
    borderStyle: 'dashed',
    color: theme.palette.grey[700],
    transition: 'border .24s ease-in-out',
    cursor: (props: SingleUploadStyleProps) => props.uploading ? 'default' : 'pointer',
    width: '100%',
    borderColor: (props: SingleUploadStyleProps) => getBorderColor(theme, props)
  }
}));

export interface SingleUploadProps {
  onDrop: (acceptedFiles: File[]) => void;
  onCancel: () => void;
  accept?: string | string[];
  uploading: boolean;
  progress?: ProgressEvent;
}

const SingleUpload: FC<SingleUploadProps> = ({ onDrop, onCancel, accept, uploading, progress }) => {
  const dropzoneState = useDropzone({ onDrop, accept, disabled: uploading, multiple: false });
  const { getRootProps, getInputProps } = dropzoneState;
  const classes = useStyles({ ...dropzoneState, uploading });


  const renderProgressText = () => {
    if (uploading) {
      if (progress?.lengthComputable) {
        return `Uploading: ${progressPercentage(progress)}%`;
      }
      return "Uploading\u2026";
    }
    return "Drop file or click here";
  }

  const renderProgress = (progress?: ProgressEvent) => (
    <LinearProgress
      variant={!progress || progress.lengthComputable ? "determinate" : "indeterminate"}
      value={!progress ? 0 : progress.lengthComputable ? progressPercentage(progress) : 0}
    />
  );

  return (
    <div {...getRootProps({ className: classes.dropzone })}>
      <input {...getInputProps()} />
      <Box flexDirection="column" display="flex" alignItems="center">
        <CloudUploadIcon fontSize='large' />
        <Typography variant="h6">
          {renderProgressText()}
        </Typography>
        {uploading && (
          <Fragment>
            <Box width="100%" p={2}>
              {renderProgress(progress)}
            </Box>
            <Button startIcon={<CancelIcon />} variant="contained" color="secondary" onClick={onCancel}>
              Cancel
            </Button>
          </Fragment>
        )}
      </Box>
    </div>
  );
}

export default SingleUpload;
