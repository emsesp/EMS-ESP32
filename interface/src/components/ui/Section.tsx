import type { FC, ReactNode } from 'react';

import Icon from './Icon';
import './Section.css';
import { cx } from './cx';
import type { IconName } from './icons';

export interface SectionProps {
  title?: string;
  icon?: IconName;
  // shown at the right of the title, e.g. a chip or a button
  actions?: ReactNode;
  className?: string;
  id?: string;
  children?: ReactNode;
}

// a card on the page background
const Section: FC<SectionProps> = ({
  title,
  icon,
  actions,
  className,
  id,
  children
}) => (
  <section id={id} className={cx('section', className)}>
    {(title || actions) && (
      <div className="sec-head">
        {icon && <Icon name={icon} className="lb" />}
        {title && <h2 className="h6 grow">{title}</h2>}
        {actions}
      </div>
    )}
    {children}
  </section>
);

export default Section;
