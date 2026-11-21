import { useNavigate, useSearchParams } from 'react-router';

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

// detail level (remembered) and search term, shared by Home and the System menu
export const useSystemView = (): SystemView => {
  const [level, setLevel] = usePersistState<1 | 2>(1, 'system-level');
  const [params, setParams] = useSearchParams();
  const q = params.get('q') ?? '';
  const setQ = (value: string) =>
    setParams(value ? { q: value } : {}, { replace: true });
  return { level, setLevel, q, setQ };
};

interface SystemBarProps {
  page: 'home' | 'system';
  view: SystemView;
}

// switch between Home and System menu, search and detail level; the same on both pages
export const SystemBar = ({ page, view }: SystemBarProps) => {
  const { LL } = useI18nContext();
  const navigate = useNavigate();
  return (
    <div className="sys-bar">
      <ToggleGroup label={LL.SYS_HOME()}>
        <ToggleButton on={page === 'home'} onClick={() => void navigate('/home')}>
          {LL.SYS_HOME()}
        </ToggleButton>
        <ToggleButton
          on={page === 'system'}
          onClick={() => void navigate('/system')}
        >
          {LL.SYS_MENU()}
        </ToggleButton>
      </ToggleGroup>
      <SearchField
        label={LL.SEARCH()}
        value={view.q}
        onChange={(e) => {
          // a search on Home opens the System menu with the hits
          if (page === 'home') {
            void navigate(`/system?q=${encodeURIComponent(e.target.value)}`);
          } else {
            view.setQ(e.target.value);
          }
        }}
      />
      <span className="grow" />
      <ToggleGroup label={LL.SYS_LEVEL_NORMAL()}>
        <ToggleButton on={view.level === 1} onClick={() => view.setLevel(1)}>
          {LL.SYS_LEVEL_NORMAL()}
        </ToggleButton>
        <ToggleButton on={view.level === 2} onClick={() => view.setLevel(2)}>
          {LL.ALLVALUES()}
        </ToggleButton>
      </ToggleGroup>
    </div>
  );
};
