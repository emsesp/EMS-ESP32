import { useEffect, useState } from 'react';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import CircleIcon from '@mui/icons-material/Circle';
import DoneIcon from '@mui/icons-material/Done';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutlined';
import {
  Box,
  Button,
  Checkbox,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  MenuItem,
  TextField,
  ToggleButton,
  ToggleButtonGroup,
  Typography
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import { BlockFormControlLabel, ValidatedTextField } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { updateValue } from 'utils';
import { ValidationError, validate } from 'validators';
import type Schema from 'validators/schema';
import type { ValidateFieldsError } from 'validators/schema';

import { ScheduleFlag } from './types';
import type { ScheduleItem } from './types';

// Constants
const FLAG_MASK_127 = 127;
const SCHEDULE_TYPE_THRESHOLD = 127;
const FLAG_ALL_DAYS = 127;
const DEFAULT_TIME = '00:00';
const TYPOGRAPHY_FONT_SIZE = 10;

// Day of week flag configuration (static, defined outside component)
const DAY_FLAGS = [
  { value: '2', flag: ScheduleFlag.SCHEDULE_MON },
  { value: '4', flag: ScheduleFlag.SCHEDULE_TUE },
  { value: '8', flag: ScheduleFlag.SCHEDULE_WED },
  { value: '16', flag: ScheduleFlag.SCHEDULE_THU },
  { value: '32', flag: ScheduleFlag.SCHEDULE_FRI },
  { value: '64', flag: ScheduleFlag.SCHEDULE_SAT },
  { value: '1', flag: ScheduleFlag.SCHEDULE_SUN }
] as const;

// Day of week flag values array (static)
const FLAG_VALUES = [
  ScheduleFlag.SCHEDULE_SUN,
  ScheduleFlag.SCHEDULE_MON,
  ScheduleFlag.SCHEDULE_TUE,
  ScheduleFlag.SCHEDULE_WED,
  ScheduleFlag.SCHEDULE_THU,
  ScheduleFlag.SCHEDULE_FRI,
  ScheduleFlag.SCHEDULE_SAT
] as const;

const getFlagDOWnumber = (flags: string[]) =>
  flags.reduce((acc, flag) => acc | Number(flag), 0) & FLAG_MASK_127;

const getFlagDOWstring = (f: number) =>
  FLAG_VALUES.filter((flag) => (f & flag) === flag).map((flag) => String(flag));

interface SchedulerDialogProps {
  open: boolean;
  creating: boolean;
  onClose: () => void;
  onSave: (ei: ScheduleItem) => void;
  selectedItem: ScheduleItem;
  validator: Schema;
  dow: string[];
  commandNames: string[];
}

const SchedulerDialog = ({
  open,
  creating,
  onClose,
  onSave,
  selectedItem,
  validator,
  dow,
  commandNames
}: SchedulerDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<ScheduleItem>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const [scheduleType, setScheduleType] = useState<ScheduleFlag>();

  const updateFormValue = updateValue(
    setEditItem as unknown as React.Dispatch<
      React.SetStateAction<Record<string, unknown>>
    >
  );

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
      setScheduleType(
        selectedItem.flags <= SCHEDULE_TYPE_THRESHOLD
          ? ScheduleFlag.SCHEDULE_DAY
          : selectedItem.flags
      );
    }
  }, [open, selectedItem]);

  const handleSave = async (itemToSave: ScheduleItem) => {
    try {
      setFieldErrors(undefined);
      await validate(validator, itemToSave);
      onSave(itemToSave);
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
    }
  };

  const save = async () => {
    await handleSave(editItem);
  };

  const remove = () => {
    onSave({ ...editItem, deleted: true });
  };

  const DayOfWeekButton = (flag: number) => {
    const dayIndex = Math.log2(flag);
    const isSelected = (editItem.flags & flag) === flag;
    return (
      <Typography
        sx={{ fontSize: TYPOGRAPHY_FONT_SIZE }}
        color={isSelected ? 'primary' : 'grey'}
      >
        {dow[dayIndex]}
      </Typography>
    );
  };

  const handleClose = (
    _event: React.SyntheticEvent,
    reason: 'backdropClick' | 'escapeKeyDown'
  ) => {
    if (reason !== 'backdropClick') {
      onClose();
    }
  };

  const handleScheduleTypeChange = (
    _event: React.SyntheticEvent<HTMLElement>,
    flag: ScheduleFlag | null
  ) => {
    if (flag !== null) {
      setFieldErrors(undefined); // clear any validation errors
      setScheduleType(flag);
      // wipe the time field when changing the schedule type
      // set the flags based on type
      const newFlags = flag === ScheduleFlag.SCHEDULE_DAY ? FLAG_ALL_DAYS : flag;
      setEditItem((prev) => ({ ...prev, time: '', flags: newFlags }));
    }
  };

  const handleDOWChange = (
    _event: React.SyntheticEvent<HTMLElement>,
    flags: string[]
  ) => {
    const newFlags =
      getFlagDOWnumber(flags) === 0 ? FLAG_ALL_DAYS : getFlagDOWnumber(flags);
    setEditItem((prev) => ({ ...prev, flags: newFlags }));
  };

  const isDaySchedule = scheduleType === ScheduleFlag.SCHEDULE_DAY;
  const isTimerSchedule = scheduleType === ScheduleFlag.SCHEDULE_TIMER;
  const needsTimeField = isDaySchedule || isTimerSchedule;

  const dowFlags = getFlagDOWstring(editItem.flags);

  const timeFieldValue = needsTimeField
    ? editItem.time === ''
      ? DEFAULT_TIME
      : editItem.time
    : editItem.time === DEFAULT_TIME
      ? ''
      : editItem.time;

  const timeFieldLabel = (() => {
    if (scheduleType === ScheduleFlag.SCHEDULE_TIMER) return LL.TIMER(1);
    if (scheduleType === ScheduleFlag.SCHEDULE_CONDITION) return LL.CONDITION();
    if (scheduleType === ScheduleFlag.SCHEDULE_ONCHANGE) return LL.ONCHANGE();
    return LL.TIME(1);
  })();

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>
        {creating ? `${LL.ADD(1)} ${LL.NEW(0)}` : LL.EDIT()}&nbsp;
        {LL.SCHEDULE(1)}
      </DialogTitle>
      <DialogContent dividers>
        <ToggleButtonGroup
          size="small"
          color="secondary"
          value={scheduleType}
          exclusive
          disabled={!creating}
          onChange={handleScheduleTypeChange}
        >
          <ToggleButton value={ScheduleFlag.SCHEDULE_DAY}>
            <Typography
              sx={{ fontSize: TYPOGRAPHY_FONT_SIZE }}
              color={isDaySchedule ? 'primary' : 'grey'}
            >
              {LL.SCHEDULE(0)}
            </Typography>
          </ToggleButton>
          <ToggleButton value={ScheduleFlag.SCHEDULE_TIMER}>
            <Typography
              sx={{ fontSize: TYPOGRAPHY_FONT_SIZE }}
              color={isTimerSchedule ? 'primary' : 'grey'}
            >
              {LL.TIMER(0)}
            </Typography>
          </ToggleButton>
          <ToggleButton value={ScheduleFlag.SCHEDULE_ONCHANGE}>
            <Typography
              sx={{ fontSize: TYPOGRAPHY_FONT_SIZE }}
              color={
                scheduleType === ScheduleFlag.SCHEDULE_ONCHANGE ? 'primary' : 'grey'
              }
            >
              {LL.ONCHANGE()}
            </Typography>
          </ToggleButton>
          <ToggleButton value={ScheduleFlag.SCHEDULE_CONDITION}>
            <Typography
              sx={{ fontSize: TYPOGRAPHY_FONT_SIZE }}
              color={
                scheduleType === ScheduleFlag.SCHEDULE_CONDITION ? 'primary' : 'grey'
              }
            >
              {LL.CONDITION()}
            </Typography>
          </ToggleButton>
        </ToggleButtonGroup>

        {isDaySchedule && (
          <ToggleButtonGroup
            size="small"
            color="secondary"
            value={dowFlags}
            onChange={handleDOWChange}
          >
            {DAY_FLAGS.map(({ value, flag }) => (
              <ToggleButton key={value} value={value}>
                {DayOfWeekButton(flag)}
              </ToggleButton>
            ))}
          </ToggleButtonGroup>
        )}

        <Grid container>
          <BlockFormControlLabel
            control={
              <Checkbox
                checked={editItem.active}
                onChange={updateFormValue}
                name="active"
              />
            }
            label={LL.ACTIVE()}
          />
          <CircleIcon
            color={editItem.active ? 'success' : 'error'}
            sx={{
              fontSize: 16,
              mt: '12px'
            }}
          />
        </Grid>

        <Grid container>
          {needsTimeField ? (
            <>
              <TextField
                name="time"
                type="time"
                label={timeFieldLabel}
                value={timeFieldValue}
                margin="normal"
                onChange={updateFormValue}
              />
              {isTimerSchedule && (
                <Typography sx={{ ml: 2, mt: 4 }} color="warning" variant="body2">
                  00:00 = {LL.SCHEDULER_HELP_2()}
                </Typography>
              )}
            </>
          ) : (
            <TextField
              name="time"
              label={timeFieldLabel}
              multiline
              fullWidth
              value={timeFieldValue}
              margin="normal"
              onChange={updateFormValue}
            />
          )}
        </Grid>
        <TextField
          name="cmd_name"
          label={LL.COMMAND(0)}
          value={editItem.cmd_name}
          fullWidth
          select
          margin="normal"
          onChange={updateFormValue}
        >
          {commandNames.map((name) => (
            <MenuItem key={name} value={name}>
              {name}
            </MenuItem>
          ))}
        </TextField>
        <ValidatedTextField
          fieldErrors={fieldErrors || {}}
          name="name"
          label={LL.NAME(0)}
          value={editItem.name}
          fullWidth
          margin="normal"
          onChange={updateFormValue}
        />
      </DialogContent>

      <DialogActions>
        {!creating && (
          <Box sx={{ flexGrow: 1 }}>
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

export default SchedulerDialog;
