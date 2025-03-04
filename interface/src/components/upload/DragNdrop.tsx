// Code inspired by Prince Azubuike from https://medium.com/@dprincecoder/creating-a-drag-and-drop-file-upload-component-in-react-a-step-by-step-guide-4d93b6cc21e0
import { type ChangeEvent, useRef, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import CloudUploadIcon from '@mui/icons-material/CloudUpload';
import UploadIcon from '@mui/icons-material/Upload';
import { Box, Button } from '@mui/material';

import { useI18nContext } from 'i18n/i18n-react';

import './dragNdrop.css';

const DragNdrop = ({ text, onFileSelected }) => {
  const [file, setFile] = useState<File>();
  const [dragged, setDragged] = useState(false);
  const inputRef = useRef<HTMLInputElement | null>(null);
  const { LL } = useI18nContext();

  const checkFileExtension = (file: File) => {
    const validExtensions = ['.json', '.bin', '.md5'];
    const fileName = file.name;
    const fileExtension = fileName.substring(fileName.lastIndexOf('.'));
    if (validExtensions.includes(fileExtension)) {
      setFile(file);
    } else {
      alert('Invalid file type');
    }
  };

  const handleFileChange = (e: ChangeEvent<HTMLInputElement>) => {
    if (!e.target.files) {
      return;
    }
    checkFileExtension(e.target.files[0]);
    e.target.value = ''; // this is to allow the same file to be selected again
  };

  const handleDrop = (event) => {
    event.preventDefault();
    const droppedFiles = event.dataTransfer.files;
    if (droppedFiles.length > 0) {
      checkFileExtension(droppedFiles[0] as File);
    }
  };

  const handleRemoveFile = (event) => {
    event.stopPropagation();
    setFile(undefined);
    setDragged(false);
  };

  const handleUploadClick = (event) => {
    event.stopPropagation();
    onFileSelected(file);
  };

  const handleBrowseClick = () => {
    inputRef.current?.click();
  };

  const handleDragOver = (event) => {
    event.preventDefault(); // prevent file from being opened
    setDragged(true);
  };

  return (
    <div
      className={`document-uploader ${file || dragged ? 'active' : ''}`}
      onDrop={handleDrop}
      onDragOver={handleDragOver}
      onDragLeave={() => setDragged(false)}
      onClick={handleBrowseClick}
    >
      <div className="upload-info">
        <CloudUploadIcon sx={{ marginRight: 4 }} color="primary" fontSize="large" />
        <p>{text}</p>
      </div>

      <input
        type="file"
        hidden
        onChange={handleFileChange}
        ref={inputRef}
        accept=".json,.txt,.csv,.bin,.md5"
        multiple={false}
        style={{ display: 'none' }}
      />

      {file && (
        <>
          <div className="file-info">
            <p>{file.name}</p>
          </div>
          <Box>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              color="secondary"
              onClick={(e) => handleRemoveFile(e)}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              sx={{ ml: 2 }}
              startIcon={<UploadIcon />}
              variant="outlined"
              color="primary"
              onClick={handleUploadClick}
            >
              {LL.UPLOAD()}
            </Button>
          </Box>
        </>
      )}
    </div>
  );
};

export default DragNdrop;
