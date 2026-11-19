import type { ButtonHTMLAttributes, FC, ReactNode } from 'react';

import Icon from './Icon';
import './ToggleGroup.css';
import { cx } from './cx';
import type { IconName } from './icons';

export interface ToggleButtonProps extends ButtonHTMLAttributes<HTMLButtonElement> {
  on: boolean;
  icon?: IconName;
}

// a button that stays pressed while `on`, used inside a ToggleGroup
export const ToggleButton: FC<ToggleButtonProps> = ({
  on,
  icon,
  className,
  children,
  ...rest
}) => (
  <button
    type="button"
    aria-pressed={on}
    className={cx(on && 'on', className)}
    {...rest}
  >
    {icon && <Icon name={icon} size="xs" />}
    {children}
  </button>
);

export const ToggleGroup: FC<{ label?: string; children: ReactNode }> = ({
  label,
  children
}) => (
  <span className="toggle-group" role="group" aria-label={label}>
    {children}
  </span>
);
