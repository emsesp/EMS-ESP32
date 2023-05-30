import CancelIcon from '@mui/icons-material/Cancel';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import WarningIcon from '@mui/icons-material/Warning';

import {
  Button,
  Typography,
  Box,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  InputAdornment,
  Grid,
  MenuItem,
  TextField
} from '@mui/material';
import { useState, useEffect } from 'react';

import { AnalogType, AnalogTypeNames, DeviceValueUOM_s } from './types';
import type { AnalogSensor } from './types';
import type Schema from 'async-validator';
import type { ValidateFieldsError } from 'async-validator';
import { ValidatedTextField } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import { numberValue, updateValue } from 'utils';

import { validate } from 'validators';

type DashboardSensorsAnalogDialogProps = {
  open: boolean;
  onClose: () => void;
  onSave: (as: AnalogSensor) => void;
  creating: boolean;
  selectedItem: AnalogSensor;
  validator: Schema;
};

const DashboardSensorsAnalogDialog = ({
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

  const remove = () => {
    editItem.d = true;
    onSave(editItem);
  };

  return (
    <Dialog open={open} onClose={close}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(0) : LL.EDIT()}&nbsp;{LL.ANALOG_SENSOR(0)}
      </DialogTitle>
      <DialogContent dividers>
        <Grid container spacing={2}>
          <Grid item xs={4}>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="g"
              label="GPIO"
              disabled={!creating}
              value={numberValue(editItem.g)}
              type="number"
              variant="outlined"
              onChange={updateFormValue}
            />
          </Grid>
          {creating && (
            <Grid item>
              <Box color="warning.main" mt={2}>
                <Typography variant="body2">{LL.WARN_GPIO()}</Typography>
              </Box>
            </Grid>
          )}
          <Grid item xs={12}>
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
          <Grid item xs={8}>
            <TextField name="t" label={LL.TYPE(0)} value={editItem.t} fullWidth select onChange={updateFormValue}>
              {AnalogTypeNames.map((val, i) => (
                <MenuItem key={i} value={i}>
                  {val}
                </MenuItem>
              ))}
            </TextField>
          </Grid>
          {editItem.t >= AnalogType.COUNTER && editItem.t <= AnalogType.RATE && (
            <>
              <Grid item xs={4}>
                <TextField name="u" label={LL.UNIT()} value={editItem.u} fullWidth select onChange={updateFormValue}>
                  {DeviceValueUOM_s.map((val, i) => (
                    <MenuItem key={i} value={i}>
                      {val}
                    </MenuItem>
                  ))}
                </TextField>
              </Grid>
              {editItem.t === AnalogType.ADC && (
                <Grid item xs={4}>
                  <TextField
                    name="o"
                    label={LL.OFFSET()}
                    value={numberValue(editItem.o)}
                    fullWidth
                    type="number"
                    variant="outlined"
                    onChange={updateFormValue}
                    inputProps={{ min: '0', max: '3300', step: '1' }}
                    InputProps={{
                      startAdornment: <InputAdornment position="start">mV</InputAdornment>
                    }}
                  />
                </Grid>
              )}
              {editItem.t === AnalogType.COUNTER && (
                <Grid item xs={4}>
                  <TextField
                    name="o"
                    label={LL.STARTVALUE()}
                    value={numberValue(editItem.o)}
                    fullWidth
                    type="number"
                    variant="outlined"
                    onChange={updateFormValue}
                    inputProps={{ step: '0.001' }}
                  />
                </Grid>
              )}
              <Grid item xs={4}>
                <TextField
                  name="f"
                  label={LL.FACTOR()}
                  value={numberValue(editItem.f)}
                  fullWidth
                  type="number"
                  variant="outlined"
                  onChange={updateFormValue}
                  inputProps={{ step: '0.001' }}
                />
              </Grid>
            </>
          )}
          {editItem.t === AnalogType.DIGITAL_OUT && (editItem.g === 25 || editItem.g === 26) && (
            <Grid item xs={4}>
              <TextField
                name="o"
                label={LL.VALUE(0)}
                value={numberValue(editItem.o)}
                fullWidth
                type="number"
                variant="outlined"
                onChange={updateFormValue}
                inputProps={{ min: '0', max: '255', step: '1' }}
              />
            </Grid>
          )}
          {editItem.t === AnalogType.DIGITAL_OUT && editItem.g !== 25 && editItem.g !== 26 && (
            <Grid item xs={4}>
              <TextField
                name="o"
                label={LL.VALUE(0)}
                value={numberValue(editItem.o)}
                fullWidth
                type="number"
                variant="outlined"
                onChange={updateFormValue}
                inputProps={{ min: '0', max: '1', step: '1' }}
              />
            </Grid>
          )}
          {editItem.t >= AnalogType.PWM_0 && (
            <>
              <Grid item xs={4}>
                <TextField
                  name="f"
                  label={LL.FREQ()}
                  value={numberValue(editItem.f)}
                  fullWidth
                  type="number"
                  variant="outlined"
                  onChange={updateFormValue}
                  inputProps={{ min: '1', max: '5000', step: '1' }}
                  InputProps={{
                    startAdornment: <InputAdornment position="start">Hz</InputAdornment>
                  }}
                />
              </Grid>
              <Grid item xs={4}>
                <TextField
                  name="o"
                  label={LL.DUTY_CYCLE()}
                  value={numberValue(editItem.o)}
                  fullWidth
                  type="number"
                  variant="outlined"
                  onChange={updateFormValue}
                  inputProps={{ min: '0', max: '100', step: '0.1' }}
                  InputProps={{
                    startAdornment: <InputAdornment position="start">%</InputAdornment>
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
            <Button startIcon={<RemoveIcon />} variant="outlined" color="error" onClick={remove}>
              {LL.REMOVE()}
            </Button>
          </Box>
        )}
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={close} color="secondary">
          {LL.CANCEL()}
        </Button>
        <Button startIcon={<WarningIcon color="warning" />} variant="contained" onClick={save} color="info">
          {creating ? LL.ADD(0) : LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default DashboardSensorsAnalogDialog;
