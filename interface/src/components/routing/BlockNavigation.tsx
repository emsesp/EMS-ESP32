import { memo } from 'react';
import type { Blocker } from 'react-router';

import { Button, Dialog } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

// the unsaved-changes guard; open while the blocker holds a navigation
const BlockNavigation = ({ blocker }: { blocker: Blocker }) => {
  const { LL } = useI18nContext();

  return (
    <Dialog
      open={blocker.state === 'blocked'}
      title={LL.BLOCK_NAVIGATE_1()}
      onClose={() => blocker.reset?.()}
      actions={
        <>
          <Button color="secondary" onClick={() => blocker.reset?.()}>
            {LL.STAY()}
          </Button>
          <Button variant="contained" onClick={() => blocker.proceed?.()}>
            {LL.LEAVE()}
          </Button>
        </>
      }
    >
      <p>{LL.BLOCK_NAVIGATE_2()}</p>
    </Dialog>
  );
};

export default memo(BlockNavigation);
