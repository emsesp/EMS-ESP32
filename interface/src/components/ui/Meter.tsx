import type { FC } from 'react';

import { useI18nContext } from 'i18n/i18n-react';

import './Meter.css';
import ProgressBar from './ProgressBar';

export interface MeterProps {
  label: string;
  used: number;
  total: number;
  unit: string;
  // added after the used / total text, e.g. "120 KB max block"
  note?: string | undefined;
}

const nf = new Intl.NumberFormat();

// a usage bar that turns orange above 85 %
const Meter: FC<MeterProps> = ({ label, used, total, unit, note }) => {
  const { LL } = useI18nContext();
  const pct = total > 0 ? (used / total) * 100 : 0;
  const text = LL.METER_USED({
    used: nf.format(used),
    total: nf.format(total),
    unit
  });

  return (
    <div className="meter">
      <div className="meter-head">
        <span>{label}</span>
        <span className="cap">
          {text}
          {note && ` · ${note}`}
        </span>
      </div>
      <ProgressBar
        value={pct}
        color={pct > 85 ? 'warning' : 'primary'}
        label={label}
      />
    </div>
  );
};

export default Meter;
