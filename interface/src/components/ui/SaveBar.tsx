import { type FC, type ReactNode, createContext, useContext } from 'react';
import { createPortal } from 'react-dom';

import Icon from './Icon';
import './SaveBar.css';
import { cx } from './cx';
import type { IconName } from './icons';

// The layout puts an element at the bottom of the page and provides it here; null until it exists.
// Outside the layout (undefined) the bar renders where it is used.
export const SaveBarSlotContext = createContext<HTMLElement | null | undefined>(
  undefined
);

export interface SaveBarProps {
  message: ReactNode;
  icon?: IconName;
  // restart hint instead of unsaved changes
  warning?: boolean;
  // the buttons, e.g. Reset and Apply
  children: ReactNode;
}

const SaveBar: FC<SaveBarProps> = ({
  message,
  icon = 'edit_note',
  warning,
  children
}) => {
  const slot = useContext(SaveBarSlotContext);
  const bar = (
    <div className={cx('savebar', warning && 'warning')}>
      <Icon name={icon} />
      <span className="grow">{message}</span>
      <div className="savebar-actions">{children}</div>
    </div>
  );
  if (slot === undefined) return bar;
  return slot ? createPortal(bar, slot) : null;
};

export default SaveBar;
