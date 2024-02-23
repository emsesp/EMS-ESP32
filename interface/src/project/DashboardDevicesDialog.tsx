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
  Typography,
  CircularProgress
} from '@mui/material';
import { useState, useEffect } from 'react';

import { DeviceValueUOM, DeviceValueUOM_s } from './types';
import type { DeviceValue } from './types';
import type Schema from 'async-validator';

import type { ValidateFieldsError } from 'async-validator';
import { dialogStyle } from 'CustomTheme';
import { ValidatedTextField } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { updateValue, numberValue } from 'utils';

import { validate } from 'validators';

type DashboardDevicesDialogProps = {
  open: boolean;
  onClose: () => void;
  onSave: (as: DeviceValue) => void;
  selectedItem: DeviceValue;
  writeable: boolean;
  validator: Schema;
  progress: boolean;
};

const DashboardDevicesDialog = ({
  open,
  onClose,
  onSave,
  selectedItem,
  writeable,
  validator,
  progress
}: DashboardDevicesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<DeviceValue>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValue(setEditItem);

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
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

  const showHelperText = (dv: DeviceValue) =>
    dv.h ? (
      dv.h
    ) : dv.l ? (
      dv.l.join(' | ')
    ) : dv.m !== undefined && dv.x !== undefined ? (
      <>
        {dv.m}&nbsp;&rarr;&nbsp;{dv.x}
      </>
    ) : undefined;

  return (
    <Dialog sx={dialogStyle} open={open} onClose={close}>
      <DialogTitle>
        {selectedItem.v === '' && selectedItem.c ? LL.RUN_COMMAND() : writeable ? LL.CHANGE_VALUE() : LL.VALUE(1)}
      </DialogTitle>
      <DialogContent dividers>
        <Box color="warning.main" p={0} pl={0} pr={0} mt={0} mb={2}>
          <Typography variant="body2">{editItem.id.slice(2)}</Typography>
        </Box>
        <Grid>
          <Grid item>
            {editItem.l ? (
              <TextField
                name="v"
                label={LL.VALUE(1)}
                value={editItem.v}
                disabled={!writeable}
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
            ) : editItem.s || editItem.u !== DeviceValueUOM.NONE ? (
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="v"
                label={LL.VALUE(1)}
                value={numberValue(Math.round(editItem.v * 10) / 10)}
                autoFocus
                disabled={!writeable}
                type="number"
                sx={{ width: '30ch' }}
                onChange={updateFormValue}
                inputProps={editItem.s ? { min: editItem.m, max: editItem.x, step: editItem.s } : {}}
                InputProps={{
                  startAdornment: <InputAdornment position="start">{setUom(editItem.u)}</InputAdornment>
                }}
              />
            ) : (
              <ValidatedTextField
                fieldErrors={fieldErrors}
                name="v"
                label={LL.VALUE(1)}
                value={editItem.v}
                disabled={!writeable}
                autoFocus
                sx={{ width: '30ch' }}
                multiline={editItem.u ? false : true}
                onChange={updateFormValue}
              />
            )}
          </Grid>
          {writeable && (
            <Grid item>
              <FormHelperText>{showHelperText(editItem)}</FormHelperText>
            </Grid>
          )}
        </Grid>
      </DialogContent>

      <DialogActions>
        {writeable ? (
          <Box
            sx={{
              '& button, & a, & .MuiCard-root': {
                mx: 0.6
              },
              position: 'relative'
            }}
          >
            <Button startIcon={<CancelIcon />} variant="outlined" onClick={close} color="secondary">
              {LL.CANCEL()}
            </Button>
            <Button startIcon={<WarningIcon color="warning" />} variant="contained" onClick={save} color="info">
              {selectedItem.v === '' && selectedItem.c ? LL.EXECUTE() : LL.UPDATE()}
            </Button>
            {progress && (
              <CircularProgress
                size={24}
                sx={{
                  color: '#4caf50',
                  position: 'absolute',
                  right: '20%',
                  marginTop: '6px'
                }}
              />
            )}
          </Box>
        ) : (
          <Button variant="outlined" onClick={close} color="secondary">
            {LL.CLOSE()}
          </Button>
        )}
      </DialogActions>
    </Dialog>
  );
};

export default DashboardDevicesDialog;
