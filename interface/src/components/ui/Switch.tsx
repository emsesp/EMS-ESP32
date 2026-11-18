import { type FC, type ReactNode, useId } from 'react';

import FieldFlags from './FieldFlags';
import './Switch.css';
import { cx } from './cx';

export interface SwitchProps {
  label: ReactNode;
  checked: boolean;
  onChange: (checked: boolean) => void;
  name?: string;
  helperText?: ReactNode;
  // helper text in the warning colour
  warn?: boolean;
  disabled?: boolean | undefined;
  changed?: boolean | undefined;
  restart?: boolean | undefined;
  className?: string;
}

// a full-width row with a switch, its label and an optional help line
const Switch: FC<SwitchProps> = ({
  label,
  checked,
  onChange,
  name,
  helperText,
  warn,
  disabled,
  changed,
  restart,
  className
}) => {
  const id = useId();
  return (
    <div className={cx('field', 'full', 'swrow', disabled && 'disabled', className)}>
      <button
        id={id}
        type="button"
        role="switch"
        name={name}
        aria-checked={checked}
        aria-describedby={helperText ? `${id}-help` : undefined}
        className={cx('switch', checked && 'on')}
        disabled={disabled}
        onClick={() => onChange(!checked)}
      />
      <div className="grow">
        <label htmlFor={id}>
          {label}
          <FieldFlags restart={restart} changed={changed} />
        </label>
        {helperText && (
          <span id={`${id}-help`} className={cx('cap', warn && 'warnc')}>
            {helperText}
          </span>
        )}
      </div>
    </div>
  );
};

export default Switch;
