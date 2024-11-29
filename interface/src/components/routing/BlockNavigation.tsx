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

  return (
    <Dialog sx={dialogStyle} open={blocker.state === 'blocked'}>
      <DialogTitle>{LL.BLOCK_NAVIGATE_1()}</DialogTitle>
      <DialogContent dividers>{LL.BLOCK_NAVIGATE_2()}</DialogContent>
      <DialogActions>
        <Button
          variant="outlined"
          onClick={() => blocker.reset?.()}
          color="secondary"
        >
          {LL.STAY()}
        </Button>
        <Button
          variant="contained"
          onClick={() => blocker.proceed?.()}
          color="primary"
        >
          {LL.LEAVE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default BlockNavigation;
