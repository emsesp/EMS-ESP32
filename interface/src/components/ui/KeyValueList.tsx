import type { FC, ReactNode } from 'react';

import './KeyValueList.css';

export interface KeyValueRow {
  label: string;
  value: ReactNode;
  // dimmed note after the value, e.g. "DHCP"
  extra?: ReactNode;
}

export interface KeyValueListProps {
  // false, null and undefined entries are skipped, so rows can be conditional
  rows: (KeyValueRow | false | null | undefined)[];
}

const KeyValueList: FC<KeyValueListProps> = ({ rows }) => (
  <dl className="kv">
    {rows.filter(Boolean).map((row) => {
      const { label, value, extra } = row as KeyValueRow;
      return (
        <div className="kv-row" key={label}>
          <dt className="k">{label}</dt>
          <dd className="v">{value}</dd>
          {extra && <dd className="x">{extra}</dd>}
        </div>
      );
    })}
  </dl>
);

export default KeyValueList;
