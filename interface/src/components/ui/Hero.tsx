import type { FC, ReactNode } from 'react';

import './Hero.css';
import Icon from './Icon';
import './Section.css';
import { cx } from './cx';
import type { IconName } from './icons';

export type StatusState = 'ok' | 'warn' | 'err' | 'off';

export interface HeroProps {
  state: StatusState;
  icon: IconName;
  title: string;
  // the line under the title
  text?: ReactNode;
  // buttons at the right, below the text on phones
  actions?: ReactNode;
}

// the summary card at the top of a status page
const Hero: FC<HeroProps> = ({ state, icon, title, text, actions }) => (
  <section className={cx('section', 'hero', state)}>
    <div className="hero-icon">
      <Icon name={icon} />
    </div>
    <div className="grow">
      <h2 className="hero-title">{title}</h2>
      {text && <div className="cap">{text}</div>}
    </div>
    {actions && <div className="hero-actions">{actions}</div>}
  </section>
);

export default Hero;
