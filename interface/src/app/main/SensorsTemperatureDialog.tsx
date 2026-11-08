import { useEffect, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import WarningIcon from '@mui/icons-material/Warning';
import {
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
import { ValidatedTextField } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { numberValue, updateValue } from 'utils';
import { ValidationError, validate } from 'validators';
import type Schema from 'validators/schema';
import type { ValidateFieldsError } from 'validators/schema';

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

  const updateFormValue = updateValue(
    setEditItem as unknown as (
      updater: (
        prevState: Readonly<Record<string, unknown>>
      ) => Record<string, unknown>
    ) => void
  );

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
    }
  }, [open, selectedItem]);

  const handleClose = (_event: React.SyntheticEvent, reason?: string) => {
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
      setFieldErrors((error as ValidationError).fieldErrors);
    }
  };

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>{`${LL.EDIT()} ${LL.TEMP_SENSOR()}`}</DialogTitle>
      <DialogContent dividers>
        <Typography sx={{ mb: 2 }} color="warning" variant="body2">
          {LL.ID_OF(LL.SENSOR(0))}: {editItem.id}
        </Typography>
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
              value={numberValue(editItem.o)}
              sx={{ width: '11ch' }}
              type="number"
              variant="outlined"
              onChange={updateFormValue}
              slotProps={{
                input: {
                  startAdornment: (
                    <InputAdornment position="start">{TEMP_UNIT}</InputAdornment>
                  )
                },
                htmlInput: {
                  min: OFFSET_MIN,
                  max: OFFSET_MAX,
                  step: OFFSET_STEP
                }
              }}
            />
          </Grid>
        </Grid>
        {editItem.s && (
          <Grid>
            <Typography sx={{ mt: 1 }} color="warning" variant="body2">
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
