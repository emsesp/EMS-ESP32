import { memo, useCallback } from 'react';
import type { Blocker } from 'react-router';

import {
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import { useI18nContext } from 'i18n/i18n-react';

const BlockNavigation = ({ blocker }: { blocker: Blocker }) => {
  const { LL } = useI18nContext();

  const handleReset = useCallback(() => {
    blocker.reset?.();
  }, [blocker]);

  const handleProceed = useCallback(() => {
    blocker.proceed?.();
  }, [blocker]);

  return (
    <Dialog sx={dialogStyle} open={blocker.state === 'blocked'}>
      <DialogTitle>{LL.BLOCK_NAVIGATE_1()}</DialogTitle>
      <DialogContent dividers>{LL.BLOCK_NAVIGATE_2()}</DialogContent>
      <DialogActions>
        <Button variant="outlined" onClick={handleReset} color="secondary">
          {LL.STAY()}
        </Button>
        <Button variant="contained" onClick={handleProceed} color="primary">
          {LL.LEAVE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default memo(BlockNavigation);
