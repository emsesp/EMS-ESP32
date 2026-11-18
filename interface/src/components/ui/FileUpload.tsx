import {
  type ChangeEvent,
  type DragEvent,
  type FC,
  useEffect,
  useRef,
  useState
} from 'react';

import { useI18nContext } from 'i18n/i18n-react';

import Button from './Button';
import './FileUpload.css';
import Icon from './Icon';
import ProgressBar from './ProgressBar';
import { cx } from './cx';
import { toast } from './toast/store';

export interface FileUploadProps {
  // shown in the drop zone
  text: string;
  // allowed extensions, lower case with the dot, e.g. ['.bin', '.md5']
  accept: string[];
  // the user chose a file and pressed Upload
  onUpload: (file: File) => void;
  // 0 to 100 while uploading: shows the progress and a Cancel button instead of the drop zone
  progress?: number | undefined;
  onCancel?: () => void;
}

// drop zone or file picker, then Cancel / Upload for the chosen file
const FileUpload: FC<FileUploadProps> = ({
  text,
  accept,
  onUpload,
  progress,
  onCancel
}) => {
  const { LL } = useI18nContext();
  const [file, setFile] = useState<File>();
  const [over, setOver] = useState(false);
  const input = useRef<HTMLInputElement>(null);
  const uploading = progress !== undefined;

  useEffect(() => {
    if (uploading) setFile(undefined);
  }, [uploading]);

  const choose = (f: File | undefined) => {
    if (!f) return;
    const name = f.name.toLowerCase();
    if (accept.some((ext) => name.endsWith(ext))) {
      setFile(f);
    } else {
      toast.error(LL.FILE_TYPE_INVALID({ types: accept.join(', ') }));
    }
  };

  const onChange = (e: ChangeEvent<HTMLInputElement>) => {
    choose(e.currentTarget.files?.[0]);
    e.currentTarget.value = ''; // lets the same file be chosen again
  };

  const onDrop = (e: DragEvent) => {
    e.preventDefault();
    setOver(false);
    choose(e.dataTransfer?.files[0]);
  };

  if (uploading) {
    return (
      <div className="upl upl-busy">
        <ProgressBar value={progress} label={LL.UPLOAD()} className="grow" />
        <span className="cap num">{Math.round(progress)}%</span>
        {onCancel && (
          <Button color="secondary" icon="cancel" onClick={onCancel}>
            {LL.CANCEL()}
          </Button>
        )}
      </div>
    );
  }

  if (file) {
    return (
      <div className="upl upl-file">
        <Icon name="upload_file" className="lb" />
        <span className="grow mono upl-name">{file.name}</span>
        <div className="upl-actions">
          <Button color="secondary" icon="cancel" onClick={() => setFile(undefined)}>
            {LL.CANCEL()}
          </Button>
          <Button variant="contained" icon="upload" onClick={() => onUpload(file)}>
            {LL.UPLOAD()}
          </Button>
        </div>
      </div>
    );
  }

  return (
    <>
      <button
        type="button"
        className={cx('upl-drop', over && 'on')}
        onClick={() => input.current?.click()}
        onDragOver={(e) => {
          e.preventDefault();
          setOver(true);
        }}
        onDragLeave={() => setOver(false)}
        onDrop={onDrop}
      >
        <Icon name="upload_file" />
        {text}
      </button>
      <input
        ref={input}
        type="file"
        hidden
        accept={accept.join(',')}
        onChange={onChange}
      />
    </>
  );
};

export default FileUpload;
