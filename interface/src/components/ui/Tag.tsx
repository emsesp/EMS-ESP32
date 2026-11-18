import type { FC, ReactNode } from 'react';

import Icon from './Icon';
import './Tag.css';
import { cx } from './cx';
import type { IconName } from './icons';

export type TagKind = 'plain' | 'new' | 'mod' | 'sys' | 'type' | 'ok' | 'warn' | 'err';

export interface TagProps {
  kind?: TagKind;
  icon?: IconName;
  className?: string;
  children: ReactNode;
}

// a small label in a table cell or after a title
const Tag: FC<TagProps> = ({ kind = 'plain', icon, className, children }) => (
  <span className={cx('tag', kind, className)}>
    {icon && <Icon name={icon} size="xs" />}
    {children}
  </span>
);

export default Tag;
