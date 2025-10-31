import { useCallback, useEffect, useMemo, useState } from 'react';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';
import DoneIcon from '@mui/icons-material/Done';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';
import EditOutlinedIcon from '@mui/icons-material/EditOutlined';
import InsertCommentOutlinedIcon from '@mui/icons-material/InsertCommentOutlined';
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

import { DeviceValueType, DeviceValueTypeNames, DeviceValueUOM_s } from './types';
import type { EntityItem } from './types';

// Constant value type options for the dropdown
const VALUE_TYPE_OPTIONS = [
  DeviceValueType.BOOL,
  DeviceValueType.INT8,
  DeviceValueType.UINT8,
  DeviceValueType.INT16,
  DeviceValueType.UINT16,
  DeviceValueType.UINT24,
  DeviceValueType.TIME,
  DeviceValueType.UINT32,
  DeviceValueType.STRING
] as const;

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
  const updateFormValue = useMemo(
    () =>
      updateValue(
        setEditItem as unknown as React.Dispatch<
          React.SetStateAction<Record<string, unknown>>
        >
      ),
    []
  );

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      // Convert to hex strings - combined into single setEditItem call
      const deviceIdHex =
        typeof selectedItem.device_id === 'number'
          ? selectedItem.device_id.toString(16).toUpperCase()
          : selectedItem.device_id;
      const typeIdHex =
        typeof selectedItem.type_id === 'number'
          ? selectedItem.type_id.toString(16).toUpperCase()
          : selectedItem.type_id;
      const factorValue =
        selectedItem.value_type === DeviceValueType.BOOL &&
        typeof selectedItem.factor === 'number'
          ? selectedItem.factor.toString(16).toUpperCase()
          : selectedItem.factor;

      setEditItem({
        ...selectedItem,
        device_id: deviceIdHex,
        type_id: typeIdHex,
        factor: factorValue
      });
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

  const save = useCallback(async () => {
    try {
      setFieldErrors(undefined);
      await validate(validator, editItem);

      // Create a copy to avoid mutating the state directly
      const processedItem: EntityItem = { ...editItem };

      if (typeof processedItem.device_id === 'string') {
        processedItem.device_id = Number.parseInt(processedItem.device_id, 16);
      }
      if (typeof processedItem.type_id === 'string') {
        processedItem.type_id = Number.parseInt(processedItem.type_id, 16);
      }
      if (
        processedItem.value_type === DeviceValueType.BOOL &&
        typeof processedItem.factor === 'string'
      ) {
        processedItem.factor = Number.parseInt(processedItem.factor, 16);
      }
      onSave(processedItem);
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
    }
  }, [validator, editItem, onSave]);

  const remove = useCallback(() => {
    const itemWithDeleted = { ...editItem, deleted: true };
    onSave(itemWithDeleted);
  }, [editItem, onSave]);

  const dup = useCallback(() => {
    onDup(editItem);
  }, [editItem, onDup]);

  // Memoize UOM menu items to avoid recreating on every render
  const uomMenuItems = useMemo(
    () =>
      DeviceValueUOM_s.map((val, i) => (
        <MenuItem key={val} value={i}>
          {val}
        </MenuItem>
      )),
    []
  );

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(1) : LL.EDIT()}&nbsp;{LL.ENTITY()}
      </DialogTitle>
      <DialogContent dividers>
        <Grid container spacing={2} rowSpacing={0}>
          <Grid size={12}>
            <ValidatedTextField
              fieldErrors={fieldErrors || {}}
              name="name"
              label={LL.NAME(0)}
              value={editItem.name}
              margin="normal"
              fullWidth
              onChange={updateFormValue}
            />
          </Grid>
          <Grid mt={3}>
            <BlockFormControlLabel
              control={
                <Checkbox
                  icon={<InsertCommentOutlinedIcon htmlColor="white" />}
                  checkedIcon={<CommentsDisabledOutlinedIcon color="primary" />}
                  checked={editItem.hide}
                  onChange={updateFormValue}
                  name="hide"
                />
              }
              label="API/MQTT"
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
                  {uomMenuItems}
                </TextField>
              </Grid>
            </>
          )}
          {editItem.ram === 0 && (
            <>
              <Grid mt={3}>
                <BlockFormControlLabel
                  control={
                    <Checkbox
                      icon={<EditOffOutlinedIcon color="primary" />}
                      checkedIcon={<EditOutlinedIcon htmlColor="white" />}
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
                  fieldErrors={fieldErrors || {}}
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
                  fieldErrors={fieldErrors || {}}
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
                  fieldErrors={fieldErrors || {}}
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
                  {VALUE_TYPE_OPTIONS.map((valueType) => (
                    <MenuItem key={valueType} value={valueType}>
                      {DeviceValueTypeNames[valueType]}
                    </MenuItem>
                  ))}
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
                        {uomMenuItems}
                      </TextField>
                    </Grid>
                  </>
                )}
              {editItem.value_type === DeviceValueType.STRING &&
                editItem.device_id !== '0' && (
                  <Grid>
                    <ValidatedTextField
                      fieldErrors={fieldErrors || {}}
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
                    fieldErrors={fieldErrors || {}}
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
