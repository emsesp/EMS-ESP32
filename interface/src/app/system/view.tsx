import { useEffect, useState } from 'react';
import { useSearchParams } from 'react-router';

import {
  AppBarActions,
  SearchField,
  ToggleButton,
  ToggleGroup
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { usePersistState } from 'utils';

import './System.css';

export interface SystemView {
  level: 1 | 2; // 1 = Normal, 2 = All values
  setLevel: (level: 1 | 2) => void;
  q: string; // search term, kept in the URL (?q=) so a search can be shared
  setQ: (q: string) => void;
}

// detail level (remembered) and search term, of the Dashboard
export const useSystemView = (): SystemView => {
  const [level, setLevel] = usePersistState<1 | 2>(1, 'system-level');
  const [params, setParams] = useSearchParams();
  const q = params.get('q') ?? '';
  const setQ = (value: string) =>
    setParams(value ? { q: value } : {}, { replace: true });
  return { level, setLevel, q, setQ };
};

interface SystemBarProps {
  view: SystemView;
  // the level only matters where values are hidden by it; not on the Favorites page
  showLevel?: boolean;
}

// on a phone the title bar has no room for the controls
const useNarrow = () => {
  const query = '(max-width: 599px)';
  const [narrow, setNarrow] = useState(() => window.matchMedia(query).matches);
  useEffect(() => {
    const m = window.matchMedia(query);
    const on = () => setNarrow(m.matches);
    m.addEventListener('change', on);
    return () => m.removeEventListener('change', on);
  }, []);
  return narrow;
};

// search, and the detail level of the values: in the title bar, on a phone in a row of its own.
// The level is only shown where it does something.
export const SystemBar = ({ view, showLevel = true }: SystemBarProps) => {
  const { LL } = useI18nContext();
  const narrow = useNarrow();
  const controls = (
    <>
      <SearchField
        label={LL.SEARCH()}
        value={view.q}
        onChange={(e) => view.setQ(e.target.value)}
      />
      {narrow && <span className="grow" />}
      {showLevel && (
        <ToggleGroup label={LL.SYS_LEVEL_NORMAL()}>
          <ToggleButton on={view.level === 1} onClick={() => view.setLevel(1)}>
            {LL.SYS_LEVEL_NORMAL()}
          </ToggleButton>
          <ToggleButton on={view.level === 2} onClick={() => view.setLevel(2)}>
            {LL.ALLVALUES()}
          </ToggleButton>
        </ToggleGroup>
      )}
    </>
  );
  return narrow ? (
    <div className="sys-bar">{controls}</div>
  ) : (
    <AppBarActions>{controls}</AppBarActions>
  );
};
