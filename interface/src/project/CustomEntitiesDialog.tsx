import { useEffect, useState } from 'react';

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
  MenuItem,
  TextField
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import type Schema from 'async-validator';
import type { ValidateFieldsError } from 'async-validator';
import { BlockFormControlLabel, ValidatedTextField } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { numberValue, updateValue } from 'utils';
import { validate } from 'validators';

import { DeviceValueType, DeviceValueUOM_s } from './types';
import type { EntityItem } from './types';

interface CustomEntitiesDialogProps {
  open: boolean;
  creating: boolean;
  onClose: () => void;
  onSave: (ei: EntityItem) => void;
  selectedItem: EntityItem;
  validator: Schema;
}

const CustomEntitiesDialog = ({
  open,
  creating,
  onClose,
  onSave,
  selectedItem,
  validator
}: CustomEntitiesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<EntityItem>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const updateFormValue = updateValue(setEditItem);

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
      // convert to hex strings straight away
      setEditItem({
        ...selectedItem,
        device_id: selectedItem.device_id.toString(16).toUpperCase(),
        type_id: selectedItem.type_id.toString(16).toUpperCase()
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
      if (typeof editItem.device_id === 'string') {
        editItem.device_id = parseInt(editItem.device_id, 16);
      }
      if (typeof editItem.type_id === 'string') {
        editItem.type_id = parseInt(editItem.type_id, 16);
      }
      onSave(editItem);
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
    }
  };

  const remove = () => {
    editItem.deleted = true;
    onSave(editItem);
  };

  return (
    <Dialog sx={dialogStyle} open={open} onClose={close}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(1) : LL.EDIT()}&nbsp;{LL.ENTITY()}
      </DialogTitle>
      <DialogContent dividers>
        <Box display="flex" flexWrap="wrap" mb={1}>
          <Box flexWrap="nowrap" whiteSpace="nowrap" />
        </Box>
        <Grid container spacing={2}>
          <Grid item xs={4}>
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
          <Grid item xs={4}>
            <TextField
              name="ram"
              label={LL.VALUE(1) + ' ' + LL.TYPE(1)}
              value={editItem.ram}
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              fullWidth
              select
            >
              <MenuItem value={0}>EMS-{LL.VALUE(1)}</MenuItem>
              <MenuItem value={1}>RAM-{LL.VALUE(1)}</MenuItem>
            </TextField>
          </Grid>
          {editItem.ram === 1 && (
            <Grid item xs={4}>
              <TextField
                name="value"
                label={LL.DEFAULT(0) + ' ' + LL.VALUE(1)}
                value={editItem.value}
                variant="outlined"
                onChange={updateFormValue}
                fullWidth
                margin="normal"
              />
            </Grid>
          )}
          {editItem.ram === 0 && (
            <>
              <Grid item xs={4} mt={3}>
                <BlockFormControlLabel
                  control={
                    <Checkbox
                      checked={editItem.writeable}
                      onChange={updateFormValue}
                      name="writeable"
                    />
                  }
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
                  InputProps={{
                    startAdornment: (
                      <InputAdornment position="start">0x</InputAdornment>
                    )
                  }}
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
                  InputProps={{
                    startAdornment: (
                      <InputAdornment position="start">0x</InputAdornment>
                    )
                  }}
                />
              </Grid>
              <Grid item xs={4}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="offset"
                  label={LL.OFFSET()}
                  margin="normal"
                  fullWidth
                  type="number"
                  value={editItem.offset}
                  onChange={updateFormValue}
                />
              </Grid>
              <Grid item xs={4}>
                <TextField
                  name="value_type"
                  label={LL.VALUE(1) + ' ' + LL.TYPE(1)}
                  value={editItem.value_type}
                  variant="outlined"
                  onChange={updateFormValue}
                  margin="normal"
                  fullWidth
                  select
                >
                  <MenuItem value={DeviceValueType.BOOL}>BOOL</MenuItem>
                  <MenuItem value={DeviceValueType.INT}>INT</MenuItem>
                  <MenuItem value={DeviceValueType.UINT}>UINT</MenuItem>
                  <MenuItem value={DeviceValueType.SHORT}>SHORT</MenuItem>
                  <MenuItem value={DeviceValueType.USHORT}>USHORT</MenuItem>
                  <MenuItem value={DeviceValueType.ULONG}>ULONG</MenuItem>
                  <MenuItem value={DeviceValueType.TIME}>TIME</MenuItem>
                  <MenuItem value={DeviceValueType.STRING}>RAW</MenuItem>
                </TextField>
              </Grid>

              {editItem.value_type !== DeviceValueType.BOOL &&
                editItem.value_type !== DeviceValueType.STRING && (
                  <>
                    <Grid item xs={4}>
                      <TextField
                        name="factor"
                        label={LL.FACTOR()}
                        value={numberValue(editItem.factor)}
                        variant="outlined"
                        onChange={updateFormValue}
                        fullWidth
                        margin="normal"
                        type="number"
                        inputProps={{ step: '0.001' }}
                      />
                    </Grid>
                    <Grid item xs={4}>
                      <TextField
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
                      </TextField>
                    </Grid>
                  </>
                )}
              {editItem.value_type === DeviceValueType.STRING &&
                editItem.device_id !== '0' && (
                  <Grid item xs={4}>
                    <TextField
                      name="factor"
                      label="Bytes"
                      value={editItem.factor}
                      variant="outlined"
                      onChange={updateFormValue}
                      fullWidth
                      margin="normal"
                      type="number"
                      inputProps={{ min: '1', max: '27', step: '1' }}
                    />
                  </Grid>
                )}
            </>
          )}
        </Grid>
      </DialogContent>

      <DialogActions>
        {!creating && (
          <Box flexGrow={1}>
            <Button
              startIcon={<RemoveIcon />}
              variant="outlined"
              color="warning"
              onClick={remove}
            >
              {LL.REMOVE()}
            </Button>
          </Box>
        )}
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={close}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={creating ? <AddIcon /> : <DoneIcon />}
          variant="outlined"
          onClick={save}
          color="primary"
        >
          {creating ? LL.ADD(0) : LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default CustomEntitiesDialog;
