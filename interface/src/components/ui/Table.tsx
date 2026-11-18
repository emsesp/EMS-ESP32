import type { FC, KeyboardEvent, TableHTMLAttributes } from 'react';

import './Table.css';
import { cx } from './cx';

export interface TableProps extends TableHTMLAttributes<HTMLTableElement> {
  // below 600 px every row becomes a card; cells show their data-label in front
  stack?: boolean;
}

// a plain table in the app style; cells with className "r" are right aligned
const Table: FC<TableProps> = ({ className, stack, ...rest }) => (
  <table className={cx('tbl', stack && 'stack', className)} {...rest} />
);

// props for a row that opens something, by mouse or keyboard
export const rowAction = (onOpen: () => void, label?: string) => ({
  className: 'click',
  tabIndex: 0,
  'aria-label': label,
  onClick: onOpen,
  onKeyDown: (e: KeyboardEvent) => {
    if (e.target === e.currentTarget && (e.key === 'Enter' || e.key === ' ')) {
      e.preventDefault();
      onOpen();
    }
  }
});

export default Table;
