import type { FC, InputHTMLAttributes } from 'react';

import './Field.css';
import Icon from './Icon';
import './SearchField.css';
import { cx } from './cx';

export interface SearchFieldProps extends Omit<
  InputHTMLAttributes<HTMLInputElement>,
  'size' | 'type'
> {
  // accessible name, also used as the placeholder when none is given
  label: string;
}

const SearchField: FC<SearchFieldProps> = ({
  label,
  className,
  placeholder,
  ...rest
}) => (
  <span className={cx('search', className)}>
    <Icon name="search" size="sm" />
    <input
      type="search"
      className="input"
      aria-label={label}
      placeholder={placeholder ?? label}
      autoComplete="off"
      {...rest}
    />
  </span>
);

export default SearchField;
