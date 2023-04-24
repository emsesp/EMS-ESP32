import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import {
  Box,
  Button,
  Checkbox,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  InputAdornment,
  MenuItem
} from '@mui/material';
import { useEffect, useState } from 'react';

import { DeviceValueUOM_s } from './types';
import type { EntityItem } from './types';
import type Schema from 'async-validator';
import type { ValidateFieldsError } from 'async-validator';

import { BlockFormControlLabel, ValidatedTextField } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

import { updateValue } from 'utils';
import { validate } from 'validators';

type SettingsEntitiesDialogProps = {
  open: boolean;
  creating: boolean;
  onClose: () => void;
  onSave: (ei: EntityItem) => void;
  selectedEntityItem: EntityItem;
  validator: Schema;
};

const SettingsEntitiesDialog = ({
  open,
  creating,
  onClose,
  onSave,
  selectedEntityItem,
  validator
}: SettingsEntitiesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<EntityItem>(selectedEntityItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValue(setEditItem);

  // on mount
  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedEntityItem);
      // convert to hex strings straight away
      setEditItem({
        ...selectedEntityItem,
        device_id: selectedEntityItem.device_id.toString(16).toUpperCase().slice(-2),
        type_id: selectedEntityItem.type_id.toString(16).toUpperCase().slice(-4)
      });
    }
  }, [open, selectedEntityItem]);

  const close = () => {
    onClose();
  };

  const save = async () => {
    try {
      setFieldErrors(undefined);
      await validate(validator, editItem);
      if (typeof editItem.device_id === 'string') {
        editItem.device_id = parseInt(editItem.device_id, 16);
      }
      if (typeof editItem.type_id === 'string') {
        editItem.type_id = parseInt(editItem.type_id, 16);
      }
      onSave(editItem);
    } catch (errors: any) {
      setFieldErrors(errors);
    }
  };

  const remove = () => {
    editItem.deleted = true;
    onSave(editItem);
  };

  return (
    <Dialog open={open} onClose={close}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(1) : LL.EDIT()}&nbsp;{LL.ENTITY()}
      </DialogTitle>
      <DialogContent dividers>
        <Box display="flex" flexWrap="wrap" mb={1}>
          <Box flexWrap="nowrap" whiteSpace="nowrap" />
        </Box>
        <Grid container spacing={2}>
          <Grid item xs={8}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="name"
              label={LL.NAME(0)}
              value={editItem.name}
              margin="normal"
              fullWidth
              onChange={updateFormValue}
            />
          </Grid>
          <Grid item xs={4} mt={3}>
            <BlockFormControlLabel
              control={<Checkbox checked={editItem.writeable} onChange={updateFormValue} name="writeable" />}
              label={LL.WRITEABLE()}
            />
          </Grid>
          <Grid item xs={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="device_id"
              label={LL.ID_OF(LL.DEVICE())}
              margin="normal"
              type="string"
              fullWidth
              value={editItem.device_id as string}
              onChange={updateFormValue}
              inputProps={{ style: { textTransform: 'uppercase' } }}
              InputProps={{ startAdornment: <InputAdornment position="start">0x</InputAdornment> }}
            />
          </Grid>
          <Grid item xs={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="type_id"
              label={LL.ID_OF(LL.TYPE(1))}
              margin="normal"
              fullWidth
              value={editItem.type_id}
              onChange={updateFormValue}
              inputProps={{ style: { textTransform: 'uppercase' } }}
              InputProps={{ startAdornment: <InputAdornment position="start">0x</InputAdornment> }}
            />
          </Grid>
          <Grid item xs={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="offset"
              label="Offset"
              margin="normal"
              fullWidth
              type="number"
              value={editItem.offset}
              onChange={updateFormValue}
            />
          </Grid>
          <Grid item xs={4}>
            <ValidatedTextField
              name="value_type"
              label="Value Type"
              value={editItem.value_type}
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              fullWidth
              select
            >
              <MenuItem value={0}>BOOL</MenuItem>
              <MenuItem value={1}>INT</MenuItem>
              <MenuItem value={2}>UINT</MenuItem>
              <MenuItem value={3}>SHORT</MenuItem>
              <MenuItem value={4}>USHORT</MenuItem>
              <MenuItem value={5}>ULONG</MenuItem>
              <MenuItem value={6}>TIME</MenuItem>
            </ValidatedTextField>
          </Grid>

          {editItem.value_type !== 0 && (
            <>
              <Grid item xs={4}>
                <ValidatedTextField
                  name="factor"
                  label={LL.FACTOR()}
                  value={editItem.factor}
                  variant="outlined"
                  onChange={updateFormValue}
                  fullWidth
                  margin="normal"
                  type="number"
                  inputProps={{ step: '0.001' }}
                />
              </Grid>
              <Grid item xs={4}>
                <ValidatedTextField
                  name="uom"
                  label={LL.UNIT()}
                  value={editItem.uom}
                  margin="normal"
                  fullWidth
                  onChange={updateFormValue}
                  select
                >
                  {DeviceValueUOM_s.map((val, i) => (
                    <MenuItem key={i} value={i}>
                      {val}
                    </MenuItem>
                  ))}
                </ValidatedTextField>
              </Grid>
            </>
          )}
        </Grid>
      </DialogContent>

      <DialogActions>
        {!creating && (
          <Box flexGrow={1}>
            <Button startIcon={<RemoveIcon />} variant="outlined" color="warning" onClick={remove}>
              {LL.REMOVE()}
            </Button>
          </Box>
        )}
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={close} color="secondary">
          {LL.CANCEL()}
        </Button>
        <Button startIcon={creating ? <AddIcon /> : <DoneIcon />} variant="outlined" onClick={save} color="primary">
          {creating ? LL.ADD(0) : LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default SettingsEntitiesDialog;
