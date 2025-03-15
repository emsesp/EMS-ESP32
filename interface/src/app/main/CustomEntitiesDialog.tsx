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
  Grid2 as Grid,
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

import { DeviceValueType, DeviceValueTypeNames, DeviceValueUOM_s } from './types';
import type { EntityItem } from './types';

interface CustomEntitiesDialogProps {
  open: boolean;
  creating: boolean;
  onClose: () => void;
  onSave: (ei: EntityItem) => void;
  onDup: (ei: EntityItem) => void;
  selectedItem: EntityItem;
  validator: Schema;
}

const CustomEntitiesDialog = ({
  open,
  creating,
  onClose,
  onSave,
  onDup,
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
        type_id: selectedItem.type_id.toString(16).toUpperCase(),
        factor:
          selectedItem.value_type === DeviceValueType.BOOL
            ? selectedItem.factor.toString(16).toUpperCase()
            : selectedItem.factor
      });
    }
  }, [open, selectedItem]);

  const handleClose = (_event, reason: 'backdropClick' | 'escapeKeyDown') => {
    if (reason !== 'backdropClick') {
      onClose();
    }
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
      if (
        editItem.value_type === DeviceValueType.BOOL &&
        typeof editItem.factor === 'string'
      ) {
        editItem.factor = parseInt(editItem.factor, 16);
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

  const dup = () => {
    onDup(editItem);
  };

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(1) : LL.EDIT()}&nbsp;{LL.ENTITY()}
      </DialogTitle>
      <DialogContent dividers>
        <Box display="flex" flexWrap="wrap" mb={1}>
          <Box flexWrap="nowrap" whiteSpace="nowrap" />
        </Box>
        <Grid container spacing={2} rowSpacing={0}>
          <Grid size={12}>
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
          <Grid>
            <TextField
              name="ram"
              label={LL.VALUE(0) + ' ' + LL.TYPE(1)}
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
            <>
              <Grid>
                <TextField
                  name="value"
                  label={LL.DEFAULT(0) + ' ' + LL.VALUE(0)}
                  type="string"
                  value={editItem.value as string}
                  variant="outlined"
                  onChange={updateFormValue}
                  fullWidth
                  margin="normal"
                />
              </Grid>
              <Grid>
                <TextField
                  name="uom"
                  label={LL.UNIT()}
                  value={editItem.uom}
                  margin="normal"
                  onChange={updateFormValue}
                  select
                >
                  {DeviceValueUOM_s.map((val, i) => (
                    <MenuItem key={val} value={i}>
                      {val}
                    </MenuItem>
                  ))}
                </TextField>
              </Grid>
            </>
          )}
          {editItem.ram === 0 && (
            <>
              <Grid mt={3} size={9}>
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
              <Grid>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="device_id"
                  label={LL.ID_OF(LL.DEVICE())}
                  margin="normal"
                  sx={{ width: '11ch' }}
                  type="string"
                  value={editItem.device_id as string}
                  onChange={updateFormValue}
                  slotProps={{
                    input: {
                      startAdornment: (
                        <InputAdornment position="start">0x</InputAdornment>
                      )
                    },
                    htmlInput: { style: { textTransform: 'uppercase' } }
                  }}
                />
              </Grid>
              <Grid>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="type_id"
                  label={LL.ID_OF(LL.TYPE(1))}
                  margin="normal"
                  sx={{ width: '11ch' }}
                  type="string"
                  value={editItem.type_id as string}
                  onChange={updateFormValue}
                  slotProps={{
                    input: {
                      startAdornment: (
                        <InputAdornment position="start">0x</InputAdornment>
                      )
                    },
                    htmlInput: { style: { textTransform: 'uppercase' } }
                  }}
                />
              </Grid>
              <Grid>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="offset"
                  label={LL.OFFSET()}
                  margin="normal"
                  sx={{ width: '11ch' }}
                  type="number"
                  value={numberValue(editItem.offset)}
                  onChange={updateFormValue}
                />
              </Grid>
              <Grid>
                <TextField
                  name="value_type"
                  label={LL.VALUE(0) + ' ' + LL.TYPE(1)}
                  value={editItem.value_type}
                  variant="outlined"
                  sx={{ width: '11ch' }}
                  onChange={updateFormValue}
                  margin="normal"
                  select
                >
                  <MenuItem value={DeviceValueType.BOOL}>
                    {DeviceValueTypeNames[DeviceValueType.BOOL]}
                  </MenuItem>
                  <MenuItem value={DeviceValueType.INT8}>
                    {DeviceValueTypeNames[DeviceValueType.INT8]}
                  </MenuItem>
                  <MenuItem value={DeviceValueType.UINT8}>
                    {DeviceValueTypeNames[DeviceValueType.UINT8]}
                  </MenuItem>
                  <MenuItem value={DeviceValueType.INT16}>
                    {DeviceValueTypeNames[DeviceValueType.INT16]}
                  </MenuItem>
                  <MenuItem value={DeviceValueType.UINT16}>
                    {DeviceValueTypeNames[DeviceValueType.UINT16]}
                  </MenuItem>
                  <MenuItem value={DeviceValueType.UINT24}>
                    {DeviceValueTypeNames[DeviceValueType.UINT24]}
                  </MenuItem>
                  <MenuItem value={DeviceValueType.TIME}>
                    {DeviceValueTypeNames[DeviceValueType.TIME]}
                  </MenuItem>
                  <MenuItem value={DeviceValueType.UINT32}>
                    {DeviceValueTypeNames[DeviceValueType.UINT32]}
                  </MenuItem>
                  <MenuItem value={DeviceValueType.STRING}>
                    {DeviceValueTypeNames[DeviceValueType.STRING]}
                  </MenuItem>
                </TextField>
              </Grid>

              {editItem.value_type !== DeviceValueType.BOOL &&
                editItem.value_type !== DeviceValueType.STRING && (
                  <>
                    <Grid>
                      <TextField
                        name="factor"
                        label={LL.FACTOR()}
                        value={numberValue(editItem.factor as number)}
                        variant="outlined"
                        onChange={updateFormValue}
                        sx={{ width: '11ch' }}
                        margin="normal"
                        type="number"
                        slotProps={{
                          htmlInput: { step: '0.001' }
                        }}
                      />
                    </Grid>
                    <Grid>
                      <TextField
                        name="uom"
                        label={LL.UNIT()}
                        value={editItem.uom}
                        margin="normal"
                        sx={{ width: '11ch' }}
                        onChange={updateFormValue}
                        select
                      >
                        {DeviceValueUOM_s.map((val, i) => (
                          <MenuItem key={val} value={i}>
                            {val}
                          </MenuItem>
                        ))}
                      </TextField>
                    </Grid>
                  </>
                )}
              {editItem.value_type === DeviceValueType.STRING &&
                editItem.device_id !== '0' && (
                  <Grid>
                    <ValidatedTextField
                      fieldErrors={fieldErrors}
                      name="factor"
                      label={LL.BYTES()}
                      value={numberValue(editItem.factor as number)}
                      sx={{ width: '11ch' }}
                      variant="outlined"
                      onChange={updateFormValue}
                      margin="normal"
                      type="number"
                      slotProps={{
                        htmlInput: { step: '1', min: '1', max: '255' }
                      }}
                    />
                  </Grid>
                )}
              {editItem.value_type === DeviceValueType.BOOL && (
                <Grid>
                  <ValidatedTextField
                    fieldErrors={fieldErrors}
                    name="factor"
                    label={LL.BITMASK()}
                    value={editItem.factor as string}
                    sx={{ width: '11ch' }}
                    variant="outlined"
                    onChange={updateFormValue}
                    margin="normal"
                    type="string"
                    slotProps={{
                      input: {
                        startAdornment: (
                          <InputAdornment position="start">0x</InputAdornment>
                        )
                      },
                      htmlInput: { style: { textTransform: 'uppercase' } }
                    }}
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
            <Button
              sx={{ ml: 1 }}
              startIcon={<AddIcon />}
              variant="outlined"
              color="primary"
              onClick={dup}
            >
              {LL.DUPLICATE()}
            </Button>
          </Box>
        )}
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={onClose}
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
