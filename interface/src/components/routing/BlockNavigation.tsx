import { memo } from 'react';
import type { Blocker } from 'react-router';

import { ConfirmDialog } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

interface BlockNavigationProps {
  blocker: Blocker;
  // an upload or restore is running, rather than unsaved changes
  busy?: boolean;
}

// asks before leaving the page; open while the blocker holds a navigation
const BlockNavigation = ({ blocker, busy }: BlockNavigationProps) => {
  const { LL } = useI18nContext();

  return (
    <ConfirmDialog
      open={blocker.state === 'blocked'}
      title={busy ? LL.BLOCK_BUSY_1() : LL.BLOCK_NAVIGATE_1()}
      label={LL.LEAVE()}
      icon="logout"
      cancelLabel={LL.STAY()}
      cancelIcon="undo"
      onClose={() => blocker.reset?.()}
      onConfirm={() => blocker.proceed?.()}
    >
      {busy ? LL.BLOCK_BUSY_2() : LL.BLOCK_NAVIGATE_2()}
    </ConfirmDialog>
  );
};

export default memo(BlockNavigation);
