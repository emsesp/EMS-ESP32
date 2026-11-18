import type { ButtonHTMLAttributes, FC } from 'react';

import './Button.css';
import Icon from './Icon';
import { cx } from './cx';
import type { IconName } from './icons';

export interface IconButtonProps extends ButtonHTMLAttributes<HTMLButtonElement> {
  icon: IconName;
  // accessible name, also shown as tooltip
  label: string;
  // grey instead of the primary colour
  plain?: boolean;
}

const IconButton: FC<IconButtonProps> = ({
  icon,
  label,
  plain,
  className,
  type = 'button',
  ...rest
}) => (
  <button
    type={type}
    className={cx('icon-btn', plain && 'plain', className)}
    aria-label={label}
    title={label}
    {...rest}
  >
    <Icon name={icon} />
  </button>
);

export default IconButton;
