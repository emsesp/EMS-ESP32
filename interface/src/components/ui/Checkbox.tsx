import type { FC, InputHTMLAttributes, ReactNode } from 'react';

import './Switch.css';
import { cx } from './cx';

export interface CheckboxProps extends Omit<
  InputHTMLAttributes<HTMLInputElement>,
  'type' | 'onChange'
> {
  label?: ReactNode;
  checked: boolean;
  onChange: (checked: boolean) => void;
}

// a native checkbox with an optional label; without one, pass aria-label
const Checkbox: FC<CheckboxProps> = ({ label, onChange, className, ...rest }) => (
  <label className={cx('checkbox', className)}>
    <input
      type="checkbox"
      onChange={(e) => onChange(e.currentTarget.checked)}
      {...rest}
    />
    {label}
  </label>
);

export default Checkbox;
