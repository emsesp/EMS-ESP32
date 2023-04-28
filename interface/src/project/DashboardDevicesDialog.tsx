import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';

import {
  Button,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  InputAdornment,
  MenuItem,
  TextField,
  FormHelperText,
  Grid
} from '@mui/material';
import { useState, useEffect } from 'react';

import { DeviceValueUOM, DeviceValueUOM_s } from './types';
import type { DeviceValue } from './types';

import { useI18nContext } from 'i18n/i18n-react';
import { updateValue } from 'utils';

type DashboardDevicesDialogProps = {
  open: boolean;
  onClose: () => void;
  onSave: (as: DeviceValue) => void;
  selectedItem: DeviceValue;
};

const DashboarDevicesDialog = ({ open, onClose, onSave, selectedItem }: DashboardDevicesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<DeviceValue>(selectedItem);
  const updateFormValue = updateValue(setEditItem);

  useEffect(() => {
    if (open) {
      setEditItem(selectedItem);
    }
  }, [open, selectedItem]);

  const close = () => {
    onClose();
  };

  const save = () => {
    onSave(editItem);
  };

  const isCmdOnly = (dv: DeviceValue) => dv.v === '' && dv.c;

  const setUom = (uom: number) => {
    switch (uom) {
      case DeviceValueUOM.HOURS:
        return LL.HOURS();
      case DeviceValueUOM.MINUTES:
        return LL.MINUTES();
      case DeviceValueUOM.SECONDS:
        return LL.SECONDS();
      default:
        return DeviceValueUOM_s[uom];
    }
  };

  return (
    <Dialog open={open} onClose={close}>
      <DialogTitle>
        <DialogTitle>{isCmdOnly(editItem) ? LL.RUN_COMMAND() : LL.CHANGE_VALUE()}</DialogTitle>
      </DialogTitle>
      <DialogContent dividers>
        <Grid container spacing={1}>
          <Grid item>
            {editItem.l && (
              <TextField
                name="v"
                label={editItem.id.slice(2)}
                value={editItem.v}
                autoFocus
                sx={{ width: '30ch' }}
                select
                onChange={updateFormValue}
              >
                {editItem.l.map((val) => (
                  <MenuItem value={val} key={val}>
                    {val}
                  </MenuItem>
                ))}
              </TextField>
            )}
            {!editItem.l && (
              <TextField
                name="v"
                label={editItem.id.slice(2)}
                value={typeof editItem.v === 'number' ? Math.round(editItem.v * 10) / 10 : editItem.v}
                autoFocus
                multiline={editItem.u ? false : true}
                sx={{ width: '30ch' }}
                type={editItem.u ? 'number' : 'text'}
                onChange={updateFormValue}
                inputProps={editItem.u ? { min: editItem.m, max: editItem.x, step: editItem.s } : {}}
                InputProps={{
                  startAdornment: <InputAdornment position="start">{setUom(editItem.u)}</InputAdornment>
                }}
              />
            )}
          </Grid>
          {editItem.h && <FormHelperText>{editItem.h}</FormHelperText>}
        </Grid>
      </DialogContent>

      <DialogActions>
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={close} color="secondary">
          {LL.CANCEL()}
        </Button>
        <Button startIcon={<WarningIcon color="warning" />} variant="contained" onClick={save} color="info">
          {LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default DashboarDevicesDialog;
