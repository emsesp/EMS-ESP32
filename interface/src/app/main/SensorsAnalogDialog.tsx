import { useEffect, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid2 as Grid,
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

import { AnalogType, AnalogTypeNames, DeviceValueUOM_s } from './types';
import type { AnalogSensor } from './types';

interface DashboardSensorsAnalogDialogProps {
  open: boolean;
  onClose: () => void;
  onSave: (as: AnalogSensor) => void;
  creating: boolean;
  selectedItem: AnalogSensor;
  validator: Schema;
}

const SensorsAnalogDialog = ({
  open,
  onClose,
  onSave,
  creating,
  selectedItem,
  validator
}: DashboardSensorsAnalogDialogProps) => {
  const { LL } = useI18nContext();
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const [editItem, setEditItem] = useState<AnalogSensor>(selectedItem);
  const updateFormValue = updateValue(setEditItem);

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
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
      onSave(editItem);
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
    }
  };

  const remove = () => {
    editItem.d = true;
    onSave(editItem);
  };

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(0) : LL.EDIT()}&nbsp;
        {LL.ANALOG_SENSOR(0)}
      </DialogTitle>
      <DialogContent dividers>
        <Grid container spacing={2}>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="g"
              label="GPIO"
              sx={{ width: '11ch' }}
              value={numberValue(editItem.g)}
              type="number"
              variant="outlined"
              onChange={updateFormValue}
            />
          </Grid>
          {creating && (
            <Grid>
              <Box color="warning.main" mt={2}>
                <Typography variant="body2">{LL.WARN_GPIO()}</Typography>
              </Box>
            </Grid>
          )}
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="n"
              label={LL.NAME(0)}
              value={editItem.n}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
            />
          </Grid>
          <Grid>
            <TextField
              name="t"
              label={LL.TYPE(0)}
              value={editItem.t}
              fullWidth
              select
              onChange={updateFormValue}
            >
              {AnalogTypeNames.map((val, i) => (
                <MenuItem key={val} value={i}>
                  {val}
                </MenuItem>
              ))}
            </TextField>
          </Grid>
          {editItem.t >= AnalogType.COUNTER && editItem.t <= AnalogType.RATE && (
            <Grid>
              <TextField
                name="u"
                label={LL.UNIT()}
                value={editItem.u}
                sx={{ width: '15ch' }}
                select
                onChange={updateFormValue}
              >
                {DeviceValueUOM_s.map((val, i) => (
                  <MenuItem key={val} value={i}>
                    {val}
                  </MenuItem>
                ))}
              </TextField>
            </Grid>
          )}
          {editItem.t === AnalogType.ADC && (
            <Grid>
              <TextField
                name="o"
                label={LL.OFFSET()}
                value={numberValue(editItem.o)}
                type="number"
                sx={{ width: '11ch' }}
                variant="outlined"
                onChange={updateFormValue}
                slotProps={{
                  input: {
                    startAdornment: (
                      <InputAdornment position="start">mV</InputAdornment>
                    )
                  },
                  htmlInput: { min: '0', max: '3300', step: '1' }
                }}
              />
            </Grid>
          )}
          {editItem.t === AnalogType.COUNTER && (
            <Grid>
              <TextField
                name="o"
                label={LL.STARTVALUE()}
                value={numberValue(editItem.o)}
                type="number"
                sx={{ width: '11ch' }}
                variant="outlined"
                onChange={updateFormValue}
                slotProps={{
                  htmlInput: { step: '0.001' }
                }}
              />
            </Grid>
          )}
          {editItem.t >= AnalogType.COUNTER && editItem.t <= AnalogType.RATE && (
            <Grid>
              <TextField
                name="f"
                label={LL.FACTOR()}
                value={numberValue(editItem.f)}
                sx={{ width: '11ch' }}
                type="number"
                variant="outlined"
                onChange={updateFormValue}
                slotProps={{
                  htmlInput: { step: '0.001' }
                }}
              />
            </Grid>
          )}
          {editItem.t === AnalogType.DIGITAL_OUT &&
            (editItem.g === 25 || editItem.g === 26) && (
              <Grid>
                <TextField
                  name="o"
                  label={LL.VALUE(0)}
                  value={numberValue(editItem.o)}
                  sx={{ width: '11ch' }}
                  type="number"
                  variant="outlined"
                  onChange={updateFormValue}
                  slotProps={{
                    htmlInput: { min: '0', max: '255', step: '1' }
                  }}
                />
              </Grid>
            )}
          {editItem.t === AnalogType.DIGITAL_OUT &&
            editItem.g !== 25 &&
            editItem.g !== 26 && (
              <>
                <Grid>
                  <TextField
                    name="o"
                    label={LL.VALUE(0)}
                    value={numberValue(editItem.o)}
                    select
                    variant="outlined"
                    onChange={updateFormValue}
                  >
                    <MenuItem value={0}>{LL.OFF()}</MenuItem>
                    <MenuItem value={1}>{LL.ON()}</MenuItem>
                  </TextField>
                </Grid>
                <Grid>
                  <TextField
                    name="f"
                    label={LL.POLARITY()}
                    value={editItem.f}
                    sx={{ width: '15ch' }}
                    select
                    onChange={updateFormValue}
                  >
                    <MenuItem value={1}>{LL.ACTIVEHIGH()}</MenuItem>
                    <MenuItem value={-1}>{LL.ACTIVELOW()}</MenuItem>
                  </TextField>
                </Grid>
                <Grid>
                  <TextField
                    name="u"
                    label={LL.STARTVALUE()}
                    sx={{ width: '15ch' }}
                    value={editItem.u}
                    select
                    onChange={updateFormValue}
                  >
                    <MenuItem value={0}>{LL.UNCHANGED()}</MenuItem>
                    <MenuItem value={1}>
                      {LL.ALWAYS()}&nbsp;{LL.OFF()}
                    </MenuItem>
                    <MenuItem value={2}>
                      {LL.ALWAYS()}&nbsp;{LL.ON()}
                    </MenuItem>
                  </TextField>
                </Grid>
              </>
            )}
          {(editItem.t === AnalogType.PWM_0 ||
            editItem.t === AnalogType.PWM_1 ||
            editItem.t === AnalogType.PWM_2) && (
            <>
              <Grid>
                <TextField
                  name="f"
                  label={LL.FREQ()}
                  value={numberValue(editItem.f)}
                  type="number"
                  variant="outlined"
                  sx={{ width: '11ch' }}
                  onChange={updateFormValue}
                  slotProps={{
                    input: {
                      startAdornment: (
                        <InputAdornment position="start">Hz</InputAdornment>
                      )
                    },
                    htmlInput: { min: '1', max: '5000', step: '1' }
                  }}
                />
              </Grid>
              <Grid>
                <TextField
                  name="o"
                  label={LL.DUTY_CYCLE()}
                  value={numberValue(editItem.o)}
                  type="number"
                  sx={{ width: '11ch' }}
                  variant="outlined"
                  onChange={updateFormValue}
                  slotProps={{
                    input: {
                      startAdornment: (
                        <InputAdornment position="start">%</InputAdornment>
                      )
                    },
                    htmlInput: { min: '0', max: '100', step: '0.1' }
                  }}
                />
              </Grid>
            </>
          )}
        </Grid>
      </DialogContent>
      <DialogActions>
        {!creating && (
          <Box flexGrow={1} sx={{ '& button': { mt: 0 } }}>
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
          {creating ? LL.ADD(0) : LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default SensorsAnalogDialog;
