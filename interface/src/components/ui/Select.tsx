import { type FC, type ReactNode, type SelectHTMLAttributes, useId } from 'react';

import FieldFlags from './FieldFlags';
import './Field.css';
import { cx } from './cx';

export interface SelectOption {
  value: string;
  label: string;
  disabled?: boolean;
}

export interface SelectProps extends Omit<
  SelectHTMLAttributes<HTMLSelectElement>,
  'size'
> {
  label: string;
  options: SelectOption[];
  // label is for screen readers only
  hideLabel?: boolean;
  fullWidth?: boolean;
  error?: string | undefined;
  helperText?: ReactNode;
  changed?: boolean | undefined;
  restart?: boolean | undefined;
}

const Select: FC<SelectProps> = ({
  label,
  options,
  hideLabel,
  fullWidth,
  error,
  helperText,
  changed,
  restart,
  className,
  id,
  ...rest
}) => {
  const autoId = useId();
  const selectId = id ?? autoId;
  const help = error ?? helperText;

  return (
    <div
      className={cx(
        'field',
        fullWidth && 'full',
        error && 'invalid',
        rest.disabled && 'disabled',
        className
      )}
    >
      <label htmlFor={selectId} className={cx(hideLabel && 'sr-only')}>
        {label}
        <FieldFlags restart={restart} changed={changed} />
      </label>
      <select
        id={selectId}
        className="input"
        aria-describedby={help ? `${selectId}-help` : undefined}
        {...rest}
      >
        {options.map((o) => (
          <option key={o.value} value={o.value} disabled={o.disabled}>
            {o.label}
          </option>
        ))}
      </select>
      {help && (
        <span id={`${selectId}-help`} className={cx('cap', error && 'err')}>
          {help}
        </span>
      )}
    </div>
  );
};

export default Select;
