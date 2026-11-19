import type { FC, ReactNode } from 'react';

import './Field.css';
import FieldFlags from './FieldFlags';
import type { SelectOption } from './Select';
import { ToggleButton, ToggleGroup } from './ToggleGroup';
import { cx } from './cx';

export interface SegmentedProps {
  label: string;
  // label is for screen readers only
  hideLabel?: boolean;
  options: SelectOption[];
  value: string;
  onChange: (value: string) => void;
  helperText?: ReactNode;
  changed?: boolean | undefined;
  restart?: boolean | undefined;
  fullWidth?: boolean;
}

// a short list of choices shown as one row of toggle buttons
const Segmented: FC<SegmentedProps> = ({
  label,
  hideLabel,
  options,
  value,
  onChange,
  helperText,
  changed,
  restart,
  fullWidth
}) => (
  <div className={cx('field', 'segmented', fullWidth && 'full')}>
    <span className={cx('seg-label', hideLabel && 'sr-only')}>
      {label}
      <FieldFlags restart={restart} changed={changed} />
    </span>
    <ToggleGroup label={label}>
      {options.map((o) => (
        <ToggleButton
          key={o.value}
          on={o.value === value}
          onClick={() => onChange(o.value)}
        >
          {o.label}
        </ToggleButton>
      ))}
    </ToggleGroup>
    {helperText && <span className="cap">{helperText}</span>}
  </div>
);

export default Segmented;
