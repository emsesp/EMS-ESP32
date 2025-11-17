import { useCallback, useEffect, useMemo, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  InputAdornment,
  MenuItem,
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
  analogGPIOList: number[];
  validator: Schema;
}

const SensorsAnalogDialog = ({
  open,
  onClose,
  onSave,
  creating,
  selectedItem,
  analogGPIOList,
  validator
}: DashboardSensorsAnalogDialogProps) => {
  const { LL } = useI18nContext();
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const [editItem, setEditItem] = useState<AnalogSensor>(selectedItem);

  const updateFormValue = useMemo(
    () =>
      updateValue((updater) =>
        setEditItem(
          (prev) =>
            updater(
              prev as unknown as Record<string, unknown>
            ) as unknown as AnalogSensor
        )
      ),
    [setEditItem]
  );

  // Memoize helper functions to check sensor type conditions
  const isCounterOrRate = useMemo(
    () => editItem.t >= AnalogType.COUNTER && editItem.t <= AnalogType.RATE,
    [editItem.t]
  );
  const isFreqType = useMemo(
    () => editItem.t >= AnalogType.FREQ_0 && editItem.t <= AnalogType.FREQ_2,
    [editItem.t]
  );
  const isPWM = useMemo(
    () =>
      editItem.t === AnalogType.PWM_0 ||
      editItem.t === AnalogType.PWM_1 ||
      editItem.t === AnalogType.PWM_2,
    [editItem.t]
  );
  const isDigitalOutGPIO = useMemo(
    () =>
      editItem.t === AnalogType.DIGITAL_OUT &&
      (editItem.g === 25 || editItem.g === 26),
    [editItem.t, editItem.g]
  );
  const isDigitalOutNonGPIO = useMemo(
    () =>
      editItem.t === AnalogType.DIGITAL_OUT &&
      editItem.g !== 25 &&
      editItem.g !== 26,
    [editItem.t, editItem.g]
  );

  // Memoize menu items to avoid recreation on each render
  const analogTypeMenuItems = useMemo(
    () =>
      AnalogTypeNames.map((val, i) => (
        <MenuItem key={val} value={i}>
          {val}
        </MenuItem>
      )),
    []
  );

  const uomMenuItems = useMemo(
    () =>
      DeviceValueUOM_s.map((val, i) => (
        <MenuItem key={val} value={i}>
          {val}
        </MenuItem>
      )),
    []
  );

  // Reset form when dialog opens or selectedItem changes
  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
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
      onSave(editItem);
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
    }
  }, [validator, editItem, onSave]);

  const remove = useCallback(() => {
    onSave({ ...editItem, d: true });
  }, [editItem, onSave]);

  const dialogTitle = useMemo(
    () =>
      `${creating ? LL.ADD(1) + ' ' + LL.NEW(0) : LL.EDIT()} ${LL.ANALOG_SENSOR(0)}`,
    [creating, LL]
  );

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>{dialogTitle}</DialogTitle>
      <DialogContent dividers>
        <Grid container spacing={2}>
          <ValidatedTextField
            name="g"
            label="GPIO"
            value={editItem.g}
            sx={{ width: '9ch' }}
            disabled={editItem.s}
            select
            onChange={updateFormValue}
          >
            {analogGPIOList?.map((gpio: number) => (
              <MenuItem key={gpio} value={gpio}>
                {gpio}
              </MenuItem>
            ))}
          </ValidatedTextField>
          <Grid>
            <ValidatedTextField
              name="n"
              label={LL.NAME(0)}
              value={editItem.n}
              fullWidth
              onChange={updateFormValue}
            />
          </Grid>
          <Grid>
            <ValidatedTextField
              name="t"
              label={LL.TYPE(0)}
              value={editItem.t}
              fullWidth
              select
              onChange={updateFormValue}
              disabled={editItem.s}
            >
              {analogTypeMenuItems}
            </ValidatedTextField>
          </Grid>
          {(isCounterOrRate || isFreqType) && (
            <Grid>
              <ValidatedTextField
                name="u"
                label={LL.UNIT()}
                value={editItem.u}
                sx={{ width: '15ch' }}
                select
                onChange={updateFormValue}
                disabled={editItem.s}
              >
                {uomMenuItems}
              </ValidatedTextField>
            </Grid>
          )}
          {editItem.t === AnalogType.ADC && (
            <Grid>
              <ValidatedTextField
                name="o"
                label={LL.OFFSET()}
                value={numberValue(editItem.o)}
                type="number"
                sx={{ width: '11ch' }}
                onChange={updateFormValue}
                disabled={editItem.s}
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
          {editItem.t === AnalogType.NTC && (
            <Grid>
              <ValidatedTextField
                name="o"
                label={LL.OFFSET()}
                value={numberValue(editItem.o)}
                sx={{ width: '11ch' }}
                type="number"
                onChange={updateFormValue}
                disabled={editItem.s}
                slotProps={{
                  input: {
                    startAdornment: (
                      <InputAdornment position="start">°C</InputAdornment>
                    )
                  },
                  htmlInput: { min: '-20', max: '20', step: '0.1' }
                }}
              />
            </Grid>
          )}
          {editItem.t === AnalogType.COUNTER && (
            <Grid>
              <ValidatedTextField
                name="o"
                label={LL.STARTVALUE()}
                value={numberValue(editItem.o)}
                type="number"
                sx={{ width: '11ch' }}
                onChange={updateFormValue}
                disabled={editItem.s}
                slotProps={{
                  htmlInput: { step: '0.001' }
                }}
              />
            </Grid>
          )}
          {editItem.t === AnalogType.RGB && (
            <Grid>
              <ValidatedTextField
                name="o"
                label={'RGB ' + LL.VALUE(0)}
                value={numberValue(editItem.o)}
                type="number"
                sx={{ width: '11ch' }}
                onChange={updateFormValue}
                disabled={editItem.s}
              />
            </Grid>
          )}
          {isCounterOrRate && (
            <Grid>
              <ValidatedTextField
                name="f"
                label={LL.FACTOR()}
                value={numberValue(editItem.f)}
                sx={{ width: '14ch' }}
                type="number"
                onChange={updateFormValue}
                disabled={editItem.s}
                slotProps={{
                  htmlInput: { step: '0.001' }
                }}
              />
            </Grid>
          )}
          {isDigitalOutGPIO && (
            <Grid>
              <ValidatedTextField
                name="o"
                label={LL.VALUE(0)}
                value={numberValue(editItem.o)}
                sx={{ width: '11ch' }}
                type="number"
                onChange={updateFormValue}
                disabled={editItem.s}
                slotProps={{
                  htmlInput: { min: '0', max: '255', step: '1' }
                }}
              />
            </Grid>
          )}
          {isDigitalOutNonGPIO && (
            <>
              <Grid>
                <ValidatedTextField
                  name="o"
                  label={LL.VALUE(0)}
                  value={numberValue(editItem.o)}
                  select
                  onChange={updateFormValue}
                  disabled={editItem.s}
                >
                  <MenuItem value={0}>{LL.OFF()}</MenuItem>
                  <MenuItem value={1}>{LL.ON()}</MenuItem>
                </ValidatedTextField>
              </Grid>
              <Grid>
                <ValidatedTextField
                  name="f"
                  label={LL.POLARITY()}
                  value={editItem.f}
                  sx={{ width: '15ch' }}
                  select
                  onChange={updateFormValue}
                  disabled={editItem.s}
                >
                  <MenuItem value={1}>{LL.ACTIVEHIGH()}</MenuItem>
                  <MenuItem value={-1}>{LL.ACTIVELOW()}</MenuItem>
                </ValidatedTextField>
              </Grid>
              <Grid>
                <ValidatedTextField
                  name="u"
                  label={LL.STARTVALUE()}
                  sx={{ width: '15ch' }}
                  value={editItem.u}
                  select
                  onChange={updateFormValue}
                  disabled={editItem.s}
                >
                  <MenuItem value={0}>{LL.UNCHANGED()}</MenuItem>
                  <MenuItem value={1}>
                    {LL.ALWAYS()}&nbsp;{LL.OFF()}
                  </MenuItem>
                  <MenuItem value={2}>
                    {LL.ALWAYS()}&nbsp;{LL.ON()}
                  </MenuItem>
                </ValidatedTextField>
              </Grid>
            </>
          )}
          {isPWM && (
            <>
              <Grid>
                <ValidatedTextField
                  name="f"
                  label={LL.FREQ()}
                  value={numberValue(editItem.f)}
                  type="number"
                  sx={{ width: '11ch' }}
                  onChange={updateFormValue}
                  disabled={editItem.s}
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
                <ValidatedTextField
                  name="o"
                  label={LL.DUTY_CYCLE()}
                  value={numberValue(editItem.o)}
                  type="number"
                  sx={{ width: '11ch' }}
                  onChange={updateFormValue}
                  disabled={editItem.s}
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
          {editItem.t === AnalogType.PULSE && (
            <>
              <Grid>
                <ValidatedTextField
                  name="o"
                  label={LL.POLARITY()}
                  value={editItem.o}
                  sx={{ width: '11ch' }}
                  select
                  onChange={updateFormValue}
                  disabled={editItem.s}
                >
                  <MenuItem value={0}>{LL.ACTIVEHIGH()}</MenuItem>
                  <MenuItem value={1}>{LL.ACTIVELOW()}</MenuItem>
                </ValidatedTextField>
              </Grid>
              <Grid>
                <ValidatedTextField
                  name="f"
                  label="Pulse"
                  value={numberValue(editItem.f)}
                  type="number"
                  sx={{ width: '15ch' }}
                  onChange={updateFormValue}
                  disabled={editItem.s}
                  slotProps={{
                    input: {
                      startAdornment: (
                        <InputAdornment position="start">s</InputAdornment>
                      )
                    },
                    htmlInput: { min: '0', max: '10000', step: '0.1' }
                  }}
                />
              </Grid>
            </>
          )}
        </Grid>
        {fieldErrors && Object.keys(fieldErrors).length > 0 && (
          <Box mt={1}>
            {Object.values(fieldErrors).map((errArr, idx) =>
              Array.isArray(errArr)
                ? errArr.map((err, j) => (
                    <Typography
                      key={`${idx}-${j}`}
                      color="error"
                      variant="caption"
                      display="block"
                    >
                      {err.message}
                    </Typography>
                  ))
                : null
            )}
          </Box>
        )}
        {editItem.s && (
          <Grid>
            <Typography mt={1} color="warning.main" variant="body2">
              <WarningIcon
                fontSize="small"
                sx={{ mr: 1, verticalAlign: 'middle' }}
                color="warning"
              />
              {LL.SYSTEM(0)} {LL.SENSOR(0)}
            </Typography>
          </Grid>
        )}
      </DialogContent>
      <DialogActions>
        {!creating && (
          <Box flexGrow={1} sx={{ '& button': { mt: 0 } }}>
            <Button
              startIcon={<RemoveIcon />}
              disabled={editItem.s}
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
          startIcon={<DoneIcon />}
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
