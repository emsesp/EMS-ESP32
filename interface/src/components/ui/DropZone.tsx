import { type DragEvent, type FC, useRef, useState } from 'react';

import './DropZone.css';
import Icon from './Icon';
import { cx } from './cx';

export interface DropZoneProps {
  text: string;
  // for the file picker, e.g. '.json' or '.bin,.md5'
  accept: string;
  onFile: (file: File | undefined) => void;
}

// dashed area to drop a file on, or click to pick one
const DropZone: FC<DropZoneProps> = ({ text, accept, onFile }) => {
  const [over, setOver] = useState(false);
  const input = useRef<HTMLInputElement>(null);

  const onDrop = (e: DragEvent) => {
    e.preventDefault();
    setOver(false);
    onFile(e.dataTransfer?.files[0]);
  };

  return (
    <>
      <button
        type="button"
        className={cx('drop', over && 'on')}
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
        accept={accept}
        onChange={(e) => {
          onFile(e.currentTarget.files?.[0]);
          e.currentTarget.value = ''; // lets the same file be chosen again
        }}
      />
    </>
  );
};

export default DropZone;
