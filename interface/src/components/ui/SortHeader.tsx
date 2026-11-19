import { type FC, type ReactNode, type ThHTMLAttributes, useState } from 'react';

import Icon from './Icon';
import './SortHeader.css';
import { cx } from './cx';

export type SortDir = 'asc' | 'desc';

export interface SortHeaderProps extends ThHTMLAttributes<HTMLTableCellElement> {
  dir?: SortDir | undefined;
  onSort: () => void;
  children: ReactNode;
}

// a <th> whose label is a button; clicking cycles ascending, descending, unsorted
const SortHeader: FC<SortHeaderProps> = ({
  dir,
  onSort,
  className,
  children,
  ...rest
}) => (
  <th
    className={cx('sort', className)}
    aria-sort={
      dir === 'asc' ? 'ascending' : dir === 'desc' ? 'descending' : undefined
    }
    {...rest}
  >
    <button type="button" onClick={onSort}>
      {children}
      <Icon
        name={dir ? 'arrow_upward' : 'unfold_more'}
        size="xs"
        className={cx(dir === 'desc' && 'down')}
      />
    </button>
  </th>
);

// sorts rows by the clicked column; compare functions are ascending
export const useTableSort = <T, K extends string>(
  rows: readonly T[],
  compare: Record<K, (a: T, b: T) => number>
) => {
  const [sort, setSort] = useState<{ key: K; dir: SortDir }>();

  let sorted = rows;
  if (sort) {
    const out = [...rows].sort(compare[sort.key]);
    sorted = sort.dir === 'desc' ? out.reverse() : out;
  }

  const header = (key: K) => ({
    dir: sort?.key === key ? sort.dir : undefined,
    onSort: () =>
      setSort((s) =>
        s?.key !== key
          ? { key, dir: 'asc' }
          : s.dir === 'asc'
            ? { key, dir: 'desc' }
            : undefined
      )
  });

  return { rows: sorted, header };
};

// ascending compare functions for useTableSort
export const byText =
  <T,>(get: (row: T) => string | undefined) =>
  (a: T, b: T) =>
    (get(a) ?? '').localeCompare(get(b) ?? '', undefined, { numeric: true });

const asText = (v: unknown): string =>
  typeof v === 'string'
    ? v
    : typeof v === 'number' || typeof v === 'boolean'
      ? String(v)
      : v === undefined || v === null
        ? ''
        : JSON.stringify(v);

// numbers numerically when both are numbers, anything else as text
export const byValue =
  <T,>(get: (row: T) => unknown) =>
  (a: T, b: T) => {
    const x = get(a);
    const y = get(b);
    if (typeof x === 'number' && typeof y === 'number') return x - y;
    return (
      (typeof x === 'number' ? -1 : 0) - (typeof y === 'number' ? -1 : 0) ||
      asText(x).localeCompare(asText(y), undefined, { numeric: true })
    );
  };

// missing values sort first
export const byNumber =
  <T,>(get: (row: T) => number | boolean | undefined) =>
  (a: T, b: T) => {
    const x = get(a);
    const y = get(b);
    if (x === undefined || y === undefined)
      return x === y ? 0 : x === undefined ? -1 : 1;
    return Number(x) - Number(y);
  };

export default SortHeader;
