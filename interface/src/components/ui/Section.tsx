import type { FC, ReactNode } from 'react';

import Icon from './Icon';
import './Section.css';
import './Tag.css';
import { cx } from './cx';
import type { IconName } from './icons';

export interface SectionProps {
  title?: string;
  icon?: IconName;
  // a number in a pill next to the title, e.g. how many items the card lists
  count?: number;
  // shown at the right of the title, e.g. a chip or a button
  actions?: ReactNode;
  // a line of text under the title
  desc?: ReactNode;
  className?: string;
  id?: string;
  children?: ReactNode;
}

// a card on the page background
const Section: FC<SectionProps> = ({
  title,
  icon,
  count,
  actions,
  desc,
  className,
  id,
  children
}) => {
  // without a title, help text sits on the same row as the actions
  const inlineDesc = !title && !!desc;

  return (
    <section id={id} className={cx('section', className)}>
      {(title || actions || inlineDesc) && (
        <div className="sec-head">
          {icon && <Icon name={icon} className="lb" />}
          {title && (
            <h2 className="h6 grow">
              {title}
              {count !== undefined && <span className="count primary">{count}</span>}
            </h2>
          )}
          {inlineDesc && <p className="sec-desc grow">{desc}</p>}
          {actions && <div className="sec-actions">{actions}</div>}
        </div>
      )}
      {!inlineDesc && desc && <p className="sec-desc">{desc}</p>}
      {children}
    </section>
  );
};

export default Section;
