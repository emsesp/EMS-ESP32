import { type FC, type ReactNode, type TextareaHTMLAttributes, useId } from 'react';

import './Field.css';
import FieldFlags from './FieldFlags';
import { cx } from './cx';

export interface TextAreaProps extends TextareaHTMLAttributes<HTMLTextAreaElement> {
  label: string;
  name: string;
  error?: string | undefined;
  helperText?: ReactNode;
  changed?: boolean | undefined;
  restart?: boolean | undefined;
}

// always full width, monospaced (certificates, payloads)
const TextArea: FC<TextAreaProps> = ({
  label,
  error,
  helperText,
  changed,
  restart,
  className,
  id,
  rows = 4,
  ...rest
}) => {
  const autoId = useId();
  const areaId = id ?? autoId;
  const help = error ?? helperText;
  return (
    <div
      className={cx(
        'field',
        'full',
        error && 'invalid',
        rest.disabled && 'disabled',
        className
      )}
    >
      <label htmlFor={areaId}>
        {label}
        <FieldFlags restart={restart} changed={changed} />
      </label>
      <textarea
        id={areaId}
        className="input mono"
        rows={rows}
        spellCheck={false}
        aria-invalid={error ? true : undefined}
        aria-describedby={help ? `${areaId}-help` : undefined}
        {...rest}
      />
      {help && (
        <span id={`${areaId}-help`} className={cx('cap', error && 'err')}>
          {help}
        </span>
      )}
    </div>
  );
};

export default TextArea;
