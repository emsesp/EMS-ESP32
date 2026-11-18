import type { ButtonHTMLAttributes, FC } from 'react';

import './Button.css';
import Icon from './Icon';
import { cx } from './cx';
import type { IconName } from './icons';

export interface ButtonProps extends Omit<
  ButtonHTMLAttributes<HTMLButtonElement>,
  'color'
> {
  variant?: 'outlined' | 'contained' | 'text';
  color?: 'primary' | 'secondary' | 'warning' | 'error';
  size?: 'sm' | undefined;
  icon?: IconName | undefined;
}

const Button: FC<ButtonProps> = ({
  variant = 'outlined',
  color = 'primary',
  size,
  icon,
  className,
  type = 'button',
  children,
  ...rest
}) => (
  <button
    type={type}
    className={cx('btn', variant, color, size, className)}
    {...rest}
  >
    {icon && <Icon name={icon} size={size ? 'xs' : 'sm'} />}
    {children}
  </button>
);

export default Button;
