import { useCallback, useEffect, useMemo, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import {
  Box,
  Button,
  Checkbox,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  TextField
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import { BlockFormControlLabel } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { updateValue } from 'utils';

import type { ModuleItem } from './types';

interface ModulesDialogProps {
  open: boolean;
  onClose: () => void;
  onSave: (mi: ModuleItem) => void;
  selectedItem: ModuleItem;
}

const ModulesDialog = ({
  open,
  onClose,
  onSave,
  selectedItem
}: ModulesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<ModuleItem>(selectedItem);

  const updateFormValue = useMemo(
    () =>
      updateValue(
        setEditItem as unknown as React.Dispatch<
          React.SetStateAction<Record<string, unknown>>
        >
      ),
    []
  );

  // Sync form state when dialog opens or selected item changes
  useEffect(() => {
    if (open) {
      setEditItem(selectedItem);
    }
  }, [open, selectedItem]);

  const handleSave = useCallback(() => {
    onSave(editItem);
  }, [editItem, onSave]);

  const dialogTitle = useMemo(
    () => `${LL.EDIT()} ${editItem.key}`,
    [LL, editItem.key]
  );

  return (
    <Dialog sx={dialogStyle} fullWidth maxWidth="xs" open={open} onClose={onClose}>
      <DialogTitle>{dialogTitle}</DialogTitle>
      <DialogContent dividers>
        <Grid container>
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={editItem.enabled}
                onChange={updateFormValue}
                name="enabled"
              />
            }
            label="Enabled"
          />
        </Grid>
        <Box mt={2} mb={1}>
          <TextField
            name="license"
            label="License Key"
            multiline
            rows={6}
            fullWidth
            value={editItem.license}
            onChange={updateFormValue}
          />
        </Box>
      </DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={onClose}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<DoneIcon />}
          variant="outlined"
          onClick={handleSave}
          color="primary"
        >
          {LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default ModulesDialog;
