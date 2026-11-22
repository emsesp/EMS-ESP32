import { useSearchParams } from 'react-router';

import { SearchField, ToggleButton, ToggleGroup } from 'components/ui';
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

// search, and the detail level of the values
export const SystemBar = ({ view, showLevel = true }: SystemBarProps) => {
  const { LL } = useI18nContext();
  return (
    <div className="sys-bar">
      <SearchField
        label={LL.SEARCH()}
        value={view.q}
        onChange={(e) => view.setQ(e.target.value)}
      />
      <span className="grow" />
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
    </div>
  );
};
