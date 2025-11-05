import { useCallback, useEffect, useMemo, useState } from 'react';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
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
  TextField,
  ToggleButton,
  ToggleButtonGroup,
  Typography
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import type Schema from 'async-validator';
import type { ValidateFieldsError } from 'async-validator';
import { BlockFormControlLabel, ValidatedTextField } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { updateValue } from 'utils';
import { validate } from 'validators';

import { ScheduleFlag } from './types';
import type { ScheduleItem } from './types';

// Constants
const FLAG_MASK_127 = 127;
const SCHEDULE_TYPE_THRESHOLD = 128;
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

interface SchedulerDialogProps {
  open: boolean;
  creating: boolean;
  onClose: () => void;
  onSave: (ei: ScheduleItem) => void;
  selectedItem: ScheduleItem;
  validator: Schema;
  dow: string[];
}

const SchedulerDialog = ({
  open,
  creating,
  onClose,
  onSave,
  selectedItem,
  validator,
  dow
}: SchedulerDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<ScheduleItem>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const [scheduleType, setScheduleType] = useState<ScheduleFlag>();

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
      setEditItem(selectedItem);
      // Set the flags based on type when page is loaded:
      // 0-127 is day schedule
      // 128 is timer
      // 129 is on change
      // 130 is on condition
      // 132 is immediate
      setScheduleType(
        selectedItem.flags < SCHEDULE_TYPE_THRESHOLD
          ? ScheduleFlag.SCHEDULE_DAY
          : selectedItem.flags
      );
    }
  }, [open, selectedItem]);

  // Helper function to handle save operations
  const handleSave = useCallback(
    async (itemToSave: ScheduleItem) => {
      try {
        setFieldErrors(undefined);
        await validate(validator, itemToSave);
        onSave(itemToSave);
      } catch (error) {
        setFieldErrors(error as ValidateFieldsError);
      }
    },
    [validator, onSave]
  );

  const save = useCallback(async () => {
    await handleSave(editItem);
  }, [editItem, handleSave]);

  const saveandactivate = useCallback(async () => {
    await handleSave({ ...editItem, active: true });
  }, [editItem, handleSave]);

  const remove = useCallback(() => {
    onSave({ ...editItem, deleted: true });
  }, [editItem, onSave]);

  // Optimize DOW flag conversion
  const getFlagDOWnumber = useCallback((flags: string[]) => {
    return flags.reduce((acc, flag) => acc | Number(flag), 0) & FLAG_MASK_127;
  }, []);

  const getFlagDOWstring = useCallback((f: number) => {
    return FLAG_VALUES.filter((flag) => (f & flag) === flag).map((flag) =>
      String(flag)
    );
  }, []);

  // Day of week display component
  const DayOfWeekButton = useCallback(
    (flag: number) => {
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
    },
    [editItem.flags, dow]
  );

  const handleClose = useCallback(
    (_event: React.SyntheticEvent, reason: 'backdropClick' | 'escapeKeyDown') => {
      if (reason !== 'backdropClick') {
        onClose();
      }
    },
    [onClose]
  );

  const handleScheduleTypeChange = useCallback(
    (_event: React.SyntheticEvent<HTMLElement>, flag: ScheduleFlag | null) => {
      if (flag !== null) {
        setFieldErrors(undefined); // clear any validation errors
        setScheduleType(flag);
        // wipe the time field when changing the schedule type
        // set the flags based on type
        const newFlags = flag === ScheduleFlag.SCHEDULE_DAY ? 0 : flag;
        setEditItem((prev) => ({ ...prev, time: '', flags: newFlags }));
      }
    },
    []
  );

  const handleDOWChange = useCallback(
    (_event: React.SyntheticEvent<HTMLElement>, flags: string[]) => {
      const newFlags = getFlagDOWnumber(flags);
      setEditItem((prev) => ({ ...prev, flags: newFlags }));
    },
    [getFlagDOWnumber]
  );

  // Memoize derived values
  const isDaySchedule = useMemo(
    () => scheduleType === ScheduleFlag.SCHEDULE_DAY,
    [scheduleType]
  );
  const isTimerSchedule = useMemo(
    () => scheduleType === ScheduleFlag.SCHEDULE_TIMER,
    [scheduleType]
  );
  const isImmediateSchedule = useMemo(
    () => scheduleType === ScheduleFlag.SCHEDULE_IMMEDIATE,
    [scheduleType]
  );
  const needsTimeField = useMemo(
    () => isDaySchedule || isTimerSchedule,
    [isDaySchedule, isTimerSchedule]
  );

  const dowFlags = useMemo(
    () => getFlagDOWstring(editItem.flags),
    [editItem.flags, getFlagDOWstring]
  );

  const timeFieldValue = useMemo(() => {
    if (needsTimeField) {
      return editItem.time === '' ? DEFAULT_TIME : editItem.time;
    }
    return editItem.time === DEFAULT_TIME ? '' : editItem.time;
  }, [editItem.time, needsTimeField]);

  const timeFieldLabel = useMemo(() => {
    if (scheduleType === ScheduleFlag.SCHEDULE_TIMER) return LL.TIMER(1);
    if (scheduleType === ScheduleFlag.SCHEDULE_CONDITION) return LL.CONDITION();
    if (scheduleType === ScheduleFlag.SCHEDULE_ONCHANGE) return LL.ONCHANGE();
    if (scheduleType === ScheduleFlag.SCHEDULE_IMMEDIATE) return LL.IMMEDIATE();
    return LL.TIME(1);
  }, [scheduleType, LL]);

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
          <ToggleButton value={ScheduleFlag.SCHEDULE_IMMEDIATE}>
            <Typography
              sx={{ fontSize: TYPOGRAPHY_FONT_SIZE }}
              color={isImmediateSchedule ? 'primary' : 'grey'}
            >
              {LL.IMMEDIATE()}
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

        {!isImmediateSchedule && (
          <>
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
                    <Box color="warning.main" ml={2} mt={4}>
                      <Typography variant="body2">
                        {LL.SCHEDULER_HELP_2()}
                      </Typography>
                    </Box>
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
          </>
        )}
        <ValidatedTextField
          fieldErrors={fieldErrors || {}}
          name="cmd"
          label={LL.COMMAND(0)}
          multiline
          fullWidth
          value={editItem.cmd}
          margin="normal"
          onChange={updateFormValue}
        />
        <TextField
          name="value"
          label={LL.VALUE(0)}
          multiline
          margin="normal"
          fullWidth
          value={editItem.value}
          onChange={updateFormValue}
        />
        <ValidatedTextField
          fieldErrors={fieldErrors || {}}
          name="name"
          label={LL.NAME(0) + ' (' + LL.OPTIONAL() + ')'}
          value={editItem.name}
          fullWidth
          margin="normal"
          onChange={updateFormValue}
        />
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
        {isImmediateSchedule && editItem.cmd !== '' && (
          <Button
            startIcon={<PlayArrowIcon />}
            variant="outlined"
            onClick={saveandactivate}
            color="success"
          >
            {LL.EXECUTE()}
          </Button>
        )}
      </DialogActions>
    </Dialog>
  );
};

export default SchedulerDialog;
