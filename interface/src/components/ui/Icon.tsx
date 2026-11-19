import type { FC } from 'react';

import './Icon.css';
import { cx } from './cx';
import { type IconName, icons } from './icons';

export interface IconProps {
  name: IconName;
  size?: 'sm' | 'xs' | undefined;
  className?: string | undefined;
  // accessible name; without it the icon is decorative and hidden from screen readers
  label?: string | undefined;
}

const pathOf = (svg: string) => / d="([^"]+)"/.exec(svg)?.[1] ?? '';

const Icon: FC<IconProps> = ({ name, size, className, label }) => (
  <svg
    className={cx('icon', size, className)}
    viewBox="0 -960 960 960"
    {...(label ? { role: 'img', 'aria-label': label } : { 'aria-hidden': true })}
  >
    <path d={pathOf(icons[name])} />
  </svg>
);

export default Icon;
