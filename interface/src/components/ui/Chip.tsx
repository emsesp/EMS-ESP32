import type { FC, ReactNode } from 'react';

import './Chip.css';
import type { StatusState } from './Hero';
import StatusDot from './StatusDot';

export interface ChipProps {
  // status dot before the text
  state?: StatusState;
  children: ReactNode;
}

const Chip: FC<ChipProps> = ({ state, children }) => (
  <span className="chip">
    {state && <StatusDot state={state} />}
    {children}
  </span>
);

export default Chip;
