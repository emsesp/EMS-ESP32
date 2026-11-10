// drag/drop code inspired by Prince Azubuike from https://medium.com/@dprincecoder/creating-a-drag-and-drop-file-upload-component-in-react-a-step-by-step-guide-4d93b6cc21e0
import {
  type ChangeEvent,
  type DragEvent,
  type MouseEvent,
  useRef,
  useState
} from 'react';
import { Link } from 'react-router';

import CancelIcon from '@mui/icons-material/Cancel';
import CloudUploadIcon from '@mui/icons-material/CloudUpload';
import UploadIcon from '@mui/icons-material/Upload';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Typography,
  styled
} from '@mui/material';

import { callAction } from 'api/app';

import { dialogStyle } from '@/CustomTheme';
import { useRequest } from 'alova/client';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';

const DocumentUploader = styled(Box)<{ active?: boolean }>(({ theme, active }) => ({
  border: `2px dashed ${active ? '#6dc24b' : '#4282fe'}`,
  backgroundColor: '#2e3339',
  padding: theme.spacing(1.25),
  display: 'flex',
  flexDirection: 'column',
  alignItems: 'center',
  justifyContent: 'center',
  position: 'relative',
  borderRadius: theme.spacing(1),
  cursor: 'pointer',
  minHeight: '120px',
  transition: 'border-color 0.2s ease-in-out'
}));

const UploadInfo = styled(Box)({
  display: 'flex',
  alignItems: 'center'
});

const FileInfo = styled(Box)({
  display: 'flex',
  flexDirection: 'column',
  width: '100%',
  justifyContent: 'space-between',
  alignItems: 'center'
});

const FileName = styled(Typography)(({ theme }) => ({
  fontSize: '14px',
  color: '#6dc24b',
  margin: theme.spacing(1, 0)
}));

interface DragNdropProps {
  text: string;
  extensions: string[];
  onFileSelected: (file: File) => void;
}

const DragNdrop = ({ text, extensions, onFileSelected }: DragNdropProps) => {
  const [file, setFile] = useState<File>();
  const [dragged, setDragged] = useState(false);
  const inputRef = useRef<HTMLInputElement | null>(null);
  const { LL } = useI18nContext();
  const [showUpgradeDialog, setShowUpgradeDialog] = useState(false);
  const [upgradeImportantMessageType, setUpgradeImportantMessageType] =
    useState<number>(0);

  const { send: checkUpgradeImportantMessages } = useRequest(
    (version: string) =>
      callAction({ action: 'upgradeImportantMessages', param: version }),
    {
      immediate: false
    }
  )
    .onSuccess((event) => {
      const upgradeImportantMessageType_n = (
        event.data as { upgradeImportantMessageType: number }
      ).upgradeImportantMessageType;
      setUpgradeImportantMessageType(upgradeImportantMessageType_n);
      if (upgradeImportantMessageType_n === 0) {
        if (file) {
          onFileSelected(file);
        }
      }
    })
    .onError((error) => {
      toast.error(String(error.error?.message || 'An error occurred'));
    });

  const checkFileExtension = (file: File) => {
    const fileName = file.name.toLowerCase();
    const fileExtension = fileName.substring(fileName.lastIndexOf('.'));
    if (extensions.includes(fileExtension)) {
      setFile(file);
    } else {
      alert('Invalid file type');
    }
  };

  const handleFileChange = (e: ChangeEvent<HTMLInputElement>) => {
    if (!e.target.files || e.target.files.length === 0) {
      return;
    }
    const selectedFile = e.target.files[0];
    if (selectedFile) {
      checkFileExtension(selectedFile);
    }
    e.target.value = ''; // this is to allow the same file to be selected again
  };

  const handleDrop = (event: DragEvent<HTMLDivElement>) => {
    event.preventDefault();
    const droppedFiles = event.dataTransfer.files;
    if (droppedFiles.length > 0) {
      checkFileExtension(droppedFiles[0] as File);
    }
  };

  const handleRemoveFile = (event: MouseEvent<HTMLButtonElement>) => {
    event.stopPropagation();
    setFile(undefined);
    setDragged(false);
  };

  const handleUploadClick = (event: MouseEvent<HTMLButtonElement>) => {
    event.stopPropagation();
    void checkUpgradeImportantMessages(file?.name || '');
    setShowUpgradeDialog(true);
  };

  const handleBrowseClick = () => {
    inputRef.current?.click();
  };

  const handleDragOver = (event: DragEvent<HTMLDivElement>) => {
    event.preventDefault(); // prevent file from being opened
    setDragged(true);
  };

  return (
    <DocumentUploader
      active={!!(file || dragged)}
      onDrop={handleDrop}
      onDragOver={handleDragOver}
      onDragLeave={() => setDragged(false)}
      onClick={handleBrowseClick}
    >
      <UploadInfo>
        <CloudUploadIcon sx={{ marginRight: 4 }} color="primary" fontSize="large" />
        <Typography>{text}</Typography>
      </UploadInfo>

      <input
        type="file"
        hidden
        onChange={handleFileChange}
        ref={inputRef}
        accept={extensions.join(',')}
        multiple={false}
        style={{ display: 'none' }}
      />

      {file && (
        <>
          <FileInfo>
            <FileName>{file.name}</FileName>
          </FileInfo>
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
          {showUpgradeDialog && upgradeImportantMessageType > 0 && (
            <Dialog
              sx={dialogStyle}
              open={showUpgradeDialog}
              onClose={() => setShowUpgradeDialog(false)}
            >
              <DialogTitle>
                <WarningIcon
                  color="warning"
                  sx={{ fontSize: 18, verticalAlign: 'middle' }}
                />
                &nbsp;&nbsp;
                {LL.UPGRADE_IMPORTANT_MESSAGES()}
              </DialogTitle>
              <DialogContent dividers>
                {upgradeImportantMessageType === 2 &&
                  LL.UPGRADE_IMPORTANT_MESSAGES_2()}
                {upgradeImportantMessageType === 3 &&
                  LL.UPGRADE_IMPORTANT_MESSAGES_3()}
                <Typography sx={{ mt: 2 }}>
                  <Link
                    to="https://docs.emsesp.org/FAQ#upgrading-the-firmware"
                    target="_blank"
                    rel="noreferrer"
                    style={{ color: 'lightblue' }}
                  >
                    {LL.ONLINE_HELP()}
                  </Link>
                </Typography>
              </DialogContent>
              <DialogActions>
                <Button
                  startIcon={<CancelIcon />}
                  variant="outlined"
                  onClick={() => setShowUpgradeDialog(false)}
                  color="secondary"
                >
                  {LL.CANCEL()}
                </Button>
                <Button
                  startIcon={<UploadIcon />}
                  variant="outlined"
                  onClick={() => onFileSelected(file)}
                  color="primary"
                >
                  {LL.UPLOAD()}
                </Button>
              </DialogActions>
            </Dialog>
          )}
        </>
      )}
    </DocumentUploader>
  );
};

export default DragNdrop;
