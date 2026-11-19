import type { FC, ReactNode } from 'react';
import { Link } from 'react-router';

import type { ButtonProps } from './Button';
import './Button.css';
import Icon from './Icon';
import { cx } from './cx';
import type { IconName } from './icons';

export interface ButtonLinkProps {
  // a route; or href for an external page, opened in a new tab
  to?: string;
  href?: string;
  color?: ButtonProps['color'];
  size?: ButtonProps['size'];
  icon?: IconName | undefined;
  children: ReactNode;
}

// a link that looks like a Button
const ButtonLink: FC<ButtonLinkProps> = ({
  to,
  href,
  color = 'primary',
  size,
  icon,
  children
}) => {
  const className = cx('btn', color, size);
  const body = (
    <>
      {icon && <Icon name={icon} size={size ? 'xs' : 'sm'} />}
      {children}
    </>
  );
  return href ? (
    <a href={href} target="_blank" rel="noreferrer" className={className}>
      {body}
    </a>
  ) : (
    <Link to={to ?? '/'} className={className}>
      {body}
    </Link>
  );
};

export default ButtonLink;
