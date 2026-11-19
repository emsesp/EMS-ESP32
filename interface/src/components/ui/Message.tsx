import type { FC, ReactNode } from 'react';

import Icon from './Icon';
import './Message.css';
import { cx } from './cx';
import type { IconName } from './icons';

export type MessageLevel = 'info' | 'warning' | 'error' | 'success';

const ICONS: Record<MessageLevel, IconName> = {
  info: 'info',
  warning: 'warning',
  error: 'error',
  success: 'check_circle'
};

export interface MessageProps {
  level: MessageLevel;
  // buttons, shown after the text (below it on phones)
  actions?: ReactNode;
  className?: string;
  children: ReactNode;
}

const Message: FC<MessageProps> = ({ level, actions, className, children }) => (
  <div
    className={cx('msg', level, className)}
    role={level === 'error' || level === 'warning' ? 'alert' : 'status'}
  >
    <Icon name={ICONS[level]} />
    <div className="grow">{children}</div>
    {actions && <div className="msg-actions">{actions}</div>}
  </div>
);

export default Message;
