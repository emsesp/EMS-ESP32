import { memo, useCallback, useEffect, useState } from 'react';
import type { FC } from 'react';
import { useLocation } from 'react-router';

import { routeTrail } from 'app/routes';
import { AppBarSlotContext, SaveBarSlotContext, cx } from 'components/ui';
import type { RequiredChildrenProps } from 'utils';

import './Layout.css';
import LayoutAppBar from './LayoutAppBar';
import LayoutDrawer from './LayoutDrawer';
import LayoutTabBar from './LayoutTabBar';
import SetupDialog from './SetupDialog';
import TelnetBanner from './TelnetBanner';
import TokensResetBanner from './TokensResetBanner';

const LayoutComponent: FC<RequiredChildrenProps> = ({ children }) => {
  const [drawerOpen, setDrawerOpen] = useState(false);
  const [saveBarSlot, setSaveBarSlot] = useState<HTMLElement | null>(null);
  const [appBarSlot, setAppBarSlot] = useState<HTMLElement | null>(null);
  const { pathname } = useLocation();
  const trail = routeTrail(pathname);

  const toggleDrawer = useCallback(() => setDrawerOpen((open) => !open), []);
  const closeDrawer = useCallback(() => setDrawerOpen(false), []);

  useEffect(closeDrawer, [pathname, closeDrawer]);

  useEffect(() => {
    if (!drawerOpen) return;
    const onKey = (e: KeyboardEvent) => e.key === 'Escape' && closeDrawer();
    document.addEventListener('keydown', onKey);
    return () => document.removeEventListener('keydown', onKey);
  }, [drawerOpen, closeDrawer]);

  return (
    <div className={cx('shell', drawerOpen && 'drawer-open')}>
      <LayoutDrawer active={trail[0]?.path} />
      <div className="scrim" onClick={closeDrawer} />
      <LayoutAppBar
        trail={trail}
        onToggleDrawer={toggleDrawer}
        actionsRef={setAppBarSlot}
      />
      <main className="main">
        <SetupDialog />
        <TokensResetBanner />
        <TelnetBanner />
        <AppBarSlotContext.Provider value={appBarSlot}>
          <SaveBarSlotContext.Provider value={saveBarSlot}>
            {children}
          </SaveBarSlotContext.Provider>
        </AppBarSlotContext.Provider>
        <div className="savebar-slot" ref={setSaveBarSlot} />
      </main>
      <LayoutTabBar active={trail[0]?.path} onMore={toggleDrawer} />
    </div>
  );
};

const Layout = memo(LayoutComponent);

export default Layout;
