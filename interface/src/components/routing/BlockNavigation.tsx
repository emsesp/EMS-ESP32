import { FC } from 'react';
import type { Blocker } from '@remix-run/router';
import { Button, Dialog, DialogActions, DialogContent, DialogTitle } from '@mui/material';

import { useI18nContext } from '../../i18n/i18n-react';

interface BlockNavigationProps {
  blocker: Blocker;
}

const BlockNavigation: FC<BlockNavigationProps> = ({ blocker }) => {
  const { LL } = useI18nContext();

  return (
    <Dialog open={blocker.state === 'blocked'}>
      <DialogTitle>{LL.BLOCK_NAVIGATE_1()}</DialogTitle>
      <DialogContent dividers>{LL.BLOCK_NAVIGATE_2()}</DialogContent>
      <DialogActions>
        <Button variant="outlined" onClick={() => blocker.reset?.()} color="secondary">
          {LL.STAY()}
        </Button>
        <Button variant="contained" onClick={() => blocker.proceed?.()} color="primary" autoFocus>
          {LL.LEAVE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default BlockNavigation;
