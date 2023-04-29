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
  Grid,
  Box,
  Typography
} from '@mui/material';
import { useState, useEffect } from 'react';

import { formatValueNoUOM } from './deviceValue';
import { DeviceValueUOM, DeviceValueUOM_s } from './types';
import type { DeviceValue } from './types';
import type Schema from 'async-validator';

import type { ValidateFieldsError } from 'async-validator';
import { ValidatedTextField } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { updateValue } from 'utils';

import { validate } from 'validators';

type DashboardDevicesDialogProps = {
  open: boolean;
  onClose: () => void;
  onSave: (as: DeviceValue) => void;
  selectedItem: DeviceValue;
  validator: Schema;
};

const DashboarDevicesDialog = ({ open, onClose, onSave, selectedItem, validator }: DashboardDevicesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<DeviceValue>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValue(setEditItem);

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
      // format value and convert to string
      setEditItem({
        ...selectedItem,
        v: formatValueNoUOM(selectedItem.v, selectedItem.u)
      });
    }
  }, [open, selectedItem]);

  const close = () => {
    onClose();
  };

  const save = async () => {
    try {
      setFieldErrors(undefined);
      await validate(validator, editItem);
      onSave(editItem);
    } catch (errors: any) {
      setFieldErrors(errors);
    }
  };

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
      <DialogTitle>{selectedItem.v === '' && selectedItem.c ? LL.RUN_COMMAND() : LL.CHANGE_VALUE()}</DialogTitle>
      <DialogContent dividers>
        <Box color="warning.main" p={0} pl={0} pr={0} mt={0} mb={2}>
          <Typography variant="body2">{editItem.id.slice(2)}</Typography>
        </Box>
        <Grid container spacing={1}>
          <Grid item>
            {editItem.l && (
              <TextField
                name="v"
                label={LL.VALUE(1)}
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
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="v"
                label={LL.VALUE(1)}
                value={editItem.v}
                autoFocus
                sx={{ width: '30ch' }}
                multiline={editItem.u ? false : true}
                onChange={updateFormValue}
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
