import { useCallback, useEffect, useMemo, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
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

import type { TemperatureSensor } from './types';

interface SensorsTemperatureDialogProps {
  open: boolean;
  onClose: () => void;
  onSave: (ts: TemperatureSensor) => void;
  selectedItem: TemperatureSensor;
  validator: Schema;
}

// Constants
const OFFSET_MIN = -5;
const OFFSET_MAX = 5;
const OFFSET_STEP = 0.1;
const TEMP_UNIT = '°C';

const SensorsTemperatureDialog = ({
  open,
  onClose,
  onSave,
  selectedItem,
  validator
}: SensorsTemperatureDialogProps) => {
  const { LL } = useI18nContext();
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const [editItem, setEditItem] = useState<TemperatureSensor>(selectedItem);

  const updateFormValue = useMemo(
    () =>
      updateValue(
        setEditItem as unknown as (
          updater: (
            prevState: Readonly<Record<string, unknown>>
          ) => Record<string, unknown>
        ) => void
      ),
    [setEditItem]
  );

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
    }
  }, [open, selectedItem]);

  const handleClose = useCallback(
    (_event: React.SyntheticEvent, reason?: string) => {
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

  const dialogTitle = useMemo(() => `${LL.EDIT()} ${LL.TEMP_SENSOR()}`, [LL]);

  const offsetValue = useMemo(() => numberValue(editItem.o), [editItem.o]);

  const slotProps = useMemo(
    () => ({
      input: {
        startAdornment: <InputAdornment position="start">{TEMP_UNIT}</InputAdornment>
      },
      htmlInput: {
        min: OFFSET_MIN,
        max: OFFSET_MAX,
        step: OFFSET_STEP
      }
    }),
    []
  );

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>{dialogTitle}</DialogTitle>
      <DialogContent dividers>
        <Box color="warning.main" mb={2}>
          <Typography variant="body2">
            {LL.ID_OF(LL.SENSOR(0))}: {editItem.id}
          </Typography>
        </Box>
        <Grid container spacing={2}>
          <Grid>
            <ValidatedTextField
              fieldErrors={fieldErrors ?? {}}
              name="n"
              label={LL.NAME(0)}
              value={editItem.n}
              sx={{ width: '30ch' }}
              onChange={updateFormValue}
            />
          </Grid>
          <Grid>
            <TextField
              name="o"
              label={LL.OFFSET()}
              value={offsetValue}
              sx={{ width: '11ch' }}
              type="number"
              variant="outlined"
              onChange={updateFormValue}
              slotProps={slotProps}
            />
          </Grid>
        </Grid>
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
          {LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default SensorsTemperatureDialog;
