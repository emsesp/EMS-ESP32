import type { FC } from 'react';

import type { StatusState } from './Hero';
import './StatusDot.css';
import { cx } from './cx';

const StatusDot: FC<{ state: StatusState }> = ({ state }) => (
  <span className={cx('dot', state)} />
);

export default StatusDot;
