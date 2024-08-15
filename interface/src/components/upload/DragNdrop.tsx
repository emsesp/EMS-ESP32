import { type ChangeEvent, useRef, useState } from 'react';
import { AiOutlineCloudUpload } from 'react-icons/ai';
import { MdClear } from 'react-icons/md';

import UploadIcon from '@mui/icons-material/Upload';
import { Button } from '@mui/material';

import { useI18nContext } from 'i18n/i18n-react';

import './drag-drop.css';

const DragNdrop = ({ onFileSelected, width, height }) => {
  const [file, setFile] = useState<File>();
  const inputRef = useRef<HTMLInputElement | null>(null);
  const { LL } = useI18nContext();

  const handleFileChange = (e: ChangeEvent<HTMLInputElement>) => {
    if (!e.target.files) {
      return;
    }
    setFile(e.target.files[0]);
  };

  const handleDrop = (event) => {
    event.preventDefault();
    const droppedFiles = event.dataTransfer.files;
    if (droppedFiles.length > 0) {
      setFile(droppedFiles[0]);
    }
  };

  const handleRemoveFile = (event) => {
    event.stopPropagation();
    setFile(undefined);
  };

  const handleUploadClick = (event) => {
    event.stopPropagation();
    onFileSelected(file);
  };

  const handleBrowseClick = () => {
    inputRef.current?.click();
  };

  return (
    <section className="drag-drop" style={{ width: width, height: height }}>
      <div
        className={`document-uploader ${file ? 'upload-box active' : 'upload-box'}`}
        onDrop={handleDrop}
        onDragOver={(event) => event.preventDefault()}
        onClick={handleBrowseClick}
      >
        <>
          <div className="upload-info">
            <AiOutlineCloudUpload />
            <div>
              <p>Drag and drop a file here or click to select one</p>
            </div>
          </div>

          <input
            type="file"
            hidden
            id="browse"
            onChange={handleFileChange}
            ref={inputRef}
            accept=".json,.txt,.csv,.bin"
            multiple={false}
          />
        </>

        {file && (
          <>
            <div className="file-list">
              <div className="file-item">
                <div className="file-info">
                  <p>{file.name}</p>
                </div>
                <div className="file-actions">
                  <MdClear
                    style={{ width: 18, verticalAlign: 'middle' }}
                    onClick={(e) => handleRemoveFile(e)}
                  />
                </div>
              </div>
            </div>

            <Button
              startIcon={<UploadIcon />}
              variant="outlined"
              color="secondary"
              onClick={handleUploadClick}
            >
              {LL.UPLOAD()}
            </Button>
          </>
        )}
      </div>
    </section>
  );
};

export default DragNdrop;
