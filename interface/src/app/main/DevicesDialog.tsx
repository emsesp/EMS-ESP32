import { useCallback, useEffect, useMemo, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  CircularProgress,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  FormHelperText,
  Grid,
  InputAdornment,
  MenuItem,
  TextField,
  Typography
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import type Schema from 'async-validator';
import type { ValidateFieldsError } from 'async-validator';
import { ValidatedTextField } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { numberValue, updateValue } from 'utils';
import { validate } from 'validators';

import { DeviceValueUOM, DeviceValueUOM_s } from './types';
import type { DeviceValue } from './types';

interface DevicesDialogProps {
  open: boolean;
  onClose: () => void;
  onSave: (as: DeviceValue) => void;
  selectedItem: DeviceValue;
  writeable: boolean;
  validator: Schema;
  progress: boolean;
}

const DevicesDialog = ({
  open,
  onClose,
  onSave,
  selectedItem,
  writeable,
  validator,
  progress
}: DevicesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<DeviceValue>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = useMemo(() => updateValue(setEditItem), [setEditItem]);

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
    }
  }, [open, selectedItem]);

  const save = useCallback(async () => {
    try {
      setFieldErrors(undefined);
      await validate(validator, editItem);
      onSave(editItem);
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
    }
  }, [validator, editItem, onSave]);

  const setUom = useCallback(
    (uom?: DeviceValueUOM) => {
      if (uom === undefined) {
        return;
      }
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
    },
    [LL]
  );

  const showHelperText = useCallback((dv: DeviceValue) => {
    if (dv.h) return dv.h;
    if (dv.l) return dv.l.join(' | ');
    if (dv.m !== undefined && dv.x !== undefined) {
      return (
        <>
          {dv.m}&nbsp;&rarr;&nbsp;{dv.x}
        </>
      );
    }
    return undefined;
  }, []);

  const isCommand = useMemo(
    () => selectedItem.v === '' && selectedItem.c,
    [selectedItem.v, selectedItem.c]
  );

  const dialogTitle = useMemo(() => {
    if (isCommand) return LL.RUN_COMMAND();
    return writeable ? LL.CHANGE_VALUE() : LL.VALUE(0);
  }, [isCommand, writeable, LL]);

  const buttonLabel = useMemo(() => {
    return isCommand ? LL.EXECUTE() : LL.UPDATE();
  }, [isCommand, LL]);

  const helperText = useMemo(
    () => showHelperText(editItem),
    [editItem, showHelperText]
  );

  const valueLabel = LL.VALUE(0);

  return (
    <Dialog sx={dialogStyle} open={open} onClose={onClose}>
      <DialogTitle>{dialogTitle}</DialogTitle>
      <DialogContent dividers>
        <Box color="warning.main" mb={2}>
          <Typography variant="body2">{editItem.id.slice(2)}</Typography>
        </Box>
        <Grid container>
          <Grid size={12}>
            {editItem.l ? (
              <TextField
                name="v"
                value={editItem.v}
                aria-label={valueLabel}
                disabled={!writeable}
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
                fieldErrors={fieldErrors || {}}
                name="v"
                label={valueLabel}
                value={numberValue(Math.round((editItem.v as number) * 10) / 10)}
                autoFocus
                disabled={!writeable}
                type="number"
                sx={{ width: '30ch' }}
                onChange={updateFormValue}
                slotProps={{
                  htmlInput: editItem.s
                    ? { min: editItem.m, max: editItem.x, step: editItem.s }
                    : {},
                  input: {
                    startAdornment: (
                      <InputAdornment position="start">
                        {setUom(editItem.u)}
                      </InputAdornment>
                    )
                  }
                }}
              />
            ) : (
              <ValidatedTextField
                fieldErrors={fieldErrors || {}}
                name="v"
                label={valueLabel}
                value={editItem.v}
                disabled={!writeable}
                sx={{ width: '30ch' }}
                multiline={!editItem.u}
                onChange={updateFormValue}
              />
            )}
          </Grid>
          {writeable && helperText && (
            <Grid>
              <FormHelperText>{helperText}</FormHelperText>
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
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={onClose}
              color="secondary"
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<WarningIcon color="warning" />}
              variant="outlined"
              onClick={save}
              color="primary"
            >
              {buttonLabel}
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
          <Button variant="outlined" onClick={onClose} color="secondary">
            {LL.CLOSE()}
          </Button>
        )}
      </DialogActions>
    </Dialog>
  );
};

export default DevicesDialog;
