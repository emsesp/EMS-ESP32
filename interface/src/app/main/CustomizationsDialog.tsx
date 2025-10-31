import { memo, useCallback, useEffect, useMemo, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import CloseIcon from '@mui/icons-material/Close';
import DoneIcon from '@mui/icons-material/Done';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  TextField,
  Typography
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import { useI18nContext } from 'i18n/i18n-react';
import { numberValue, updateValue } from 'utils';

import EntityMaskToggle from './EntityMaskToggle';
import { DeviceEntityMask } from './types';
import type { DeviceEntity } from './types';

interface SettingsCustomizationsDialogProps {
  open: boolean;
  onClose: () => void;
  onSave: (di: DeviceEntity) => void;
  selectedItem: DeviceEntity;
}

interface LabelValueProps {
  label: string;
  value: React.ReactNode;
}

const LabelValue = memo(({ label, value }: LabelValueProps) => (
  <Grid container direction="row">
    <Typography variant="body2" color="warning.main">
      {label}:&nbsp;
    </Typography>
    <Typography variant="body2">{value}</Typography>
  </Grid>
));
LabelValue.displayName = 'LabelValue';

const ICON_SIZE = 16;

const CustomizationsDialog = ({
  open,
  onClose,
  onSave,
  selectedItem
}: SettingsCustomizationsDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<DeviceEntity>(selectedItem);
  const [error, setError] = useState<boolean>(false);

  const updateFormValue = useMemo(
    () =>
      updateValue(
        setEditItem as unknown as React.Dispatch<
          React.SetStateAction<Record<string, unknown>>
        >
      ),
    []
  );

  const isWriteableNumber = useMemo(
    () =>
      typeof editItem.v === 'number' &&
      editItem.w &&
      !(editItem.m & DeviceEntityMask.DV_READONLY),
    [editItem.v, editItem.w, editItem.m]
  );

  useEffect(() => {
    if (open) {
      setError(false);
      setEditItem(selectedItem);
    }
  }, [open, selectedItem]);

  const handleClose = useCallback(
    (_event: React.SyntheticEvent, reason: 'backdropClick' | 'escapeKeyDown') => {
      if (reason !== 'backdropClick') {
        onClose();
      }
    },
    [onClose]
  );

  const save = useCallback(() => {
    if (
      isWriteableNumber &&
      editItem.mi &&
      editItem.ma &&
      editItem.mi > editItem.ma
    ) {
      setError(true);
    } else {
      onSave(editItem);
    }
  }, [isWriteableNumber, editItem, onSave]);

  const updateDeviceEntity = useCallback((updatedItem: DeviceEntity) => {
    setEditItem((prev) => ({ ...prev, m: updatedItem.m }));
  }, []);

  const dialogTitle = useMemo(() => `${LL.EDIT()} ${LL.ENTITY()}`, [LL]);

  const writeableIcon = useMemo(
    () =>
      editItem.w ? (
        <DoneIcon color="success" sx={{ fontSize: ICON_SIZE }} />
      ) : (
        <CloseIcon color="error" sx={{ fontSize: ICON_SIZE }} />
      ),
    [editItem.w]
  );

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>{dialogTitle}</DialogTitle>
      <DialogContent dividers>
        <LabelValue label={LL.ID_OF(LL.ENTITY())} value={editItem.id} />
        <LabelValue
          label={`${LL.DEFAULT(1)} ${LL.ENTITY_NAME(1)}`}
          value={editItem.n}
        />
        <LabelValue label={LL.WRITEABLE()} value={writeableIcon} />

        <Box mt={1} mb={2}>
          <EntityMaskToggle onUpdate={updateDeviceEntity} de={editItem} />
        </Box>

        <Grid container spacing={2}>
          <Grid>
            <TextField
              name="cn"
              label={LL.NEW_NAME_OF(LL.ENTITY())}
              value={editItem.cn}
              sx={{ width: '30ch' }}
              onChange={updateFormValue}
            />
          </Grid>
          {isWriteableNumber && (
            <>
              <Grid>
                <TextField
                  name="mi"
                  label={LL.MIN()}
                  value={numberValue(editItem.mi)}
                  sx={{ width: '11ch' }}
                  type="number"
                  onChange={updateFormValue}
                />
              </Grid>
              <Grid>
                <TextField
                  name="ma"
                  label={LL.MAX()}
                  value={numberValue(editItem.ma)}
                  sx={{ width: '11ch' }}
                  type="number"
                  onChange={updateFormValue}
                />
              </Grid>
            </>
          )}
        </Grid>

        {error && (
          <Typography variant="body2" color="error" mt={2}>
            Error: Check min and max values
          </Typography>
        )}
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
          onClick={save}
          color="primary"
        >
          {LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default CustomizationsDialog;
