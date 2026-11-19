import { type ChangeEvent, useEffect, useRef, useState } from 'react';

import * as NTPApi from 'api/ntp';

import { useRequest } from 'alova/client';
import { ntpInfo } from 'app/status/status';
import {
  Button,
  Dialog,
  FormSection,
  Icon,
  Section,
  Select,
  Switch,
  TextField,
  toast
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { NTPSettingsType, NTPStatusType, Time } from 'types';
import { formatDateTime, formatLocalDateTime, useInterval } from 'utils';
import { createNTPSettingsValidator } from 'validators/ntp';

import FormPage, { useFormPage } from './FormPage';
import './NTPSettings.css';
import {
  TIME_ZONES,
  TIME_ZONE_OPTIONS,
  migrateNtpTimeZones,
  selectedTimeZone
} from './TZ';

const parseLocal = (status: NTPStatusType) => ({
  base: Date.parse(status.local_time.substring(0, 19)),
  at: Date.now()
});

// ticks every second between polls, so it lives apart from the form
const NTPClock = ({ canSetTime }: { canSetTime: boolean }) => {
  const { LL } = useI18nContext();
  const [clock, setClock] = useState<{ base: number; at: number }>();
  const [now, setNow] = useState(() => Date.now());
  const [setting, setSetting] = useState(false);
  const [localTime, setLocalTime] = useState('');
  const [processing, setProcessing] = useState(false);

  const { data: status, send: loadStatus } = useRequest(
    NTPApi.readNTPStatus
  ).onSuccess((event) => setClock(parseLocal(event.data)));
  const { send: updateTime } = useRequest(
    (local_time: Time) => NTPApi.updateTime(local_time),
    { immediate: false }
  );

  useInterval(() => setNow(Date.now()), 1000);
  useInterval(() => void loadStatus(), 30000);

  const openSetTime = () => {
    setLocalTime(formatLocalDateTime(new Date()));
    setSetting(true);
  };

  const configureTime = async () => {
    setProcessing(true);
    try {
      await updateTime({ local_time: formatLocalDateTime(new Date(localTime)) });
      toast.success(LL.TIME_SET());
      setSetting(false);
      await loadStatus();
    } catch {
      toast.error(LL.PROBLEM_UPDATING());
    } finally {
      setProcessing(false);
    }
  };

  const time = clock
    ? formatDateTime(
        formatLocalDateTime(new Date(clock.base + Math.max(now - clock.at, 0)))
      )
    : '…';

  return (
    <Section className="ntp-clock">
      <Icon name="schedule" />
      <div className="grow">
        <div className="ntp-time">{time}</div>
        <div className="cap">
          {LL.NTP_TIME_ON_DEVICE()}
          {status && ` · NTP ${ntpInfo(status.status, LL).text}`}
        </div>
      </div>
      {canSetTime && (
        <Button icon="edit_calendar" onClick={openSetTime}>
          {LL.SET_TIME(0)}
        </Button>
      )}
      <Dialog
        open={setting}
        title={LL.SET_TIME(1)}
        onClose={() => setSetting(false)}
        onSubmit={() => void configureTime()}
        actions={
          <>
            <Button
              icon="cancel"
              color="secondary"
              onClick={() => setSetting(false)}
            >
              {LL.CANCEL()}
            </Button>
            <Button type="submit" icon="check" disabled={processing || !localTime}>
              {LL.UPDATE()}
            </Button>
          </>
        }
      >
        <p>{LL.SET_TIME_TEXT()}</p>
        <div className="form">
          <TextField
            label={LL.LOCAL_TIME(0)}
            name="local_time"
            type="datetime-local"
            step={1}
            fullWidth
            value={localTime}
            disabled={processing}
            onChange={(e) => setLocalTime(e.currentTarget.value)}
          />
        </div>
      </Dialog>
    </Section>
  );
};

type TzKeys = ['tz_label', 'tz_format'] | ['tz_label_t', 'tz_format_t'];

const NTPSettings = () => {
  const { LL } = useI18nContext();
  const form = useFormPage<NTPSettingsType>({
    read: NTPApi.readNTPSettings,
    update: NTPApi.updateNTPSettings,
    validator: createNTPSettingsValidator
  });
  const { data: settings, dirtyFlags, setOrigData, updateDataValue } = form;
  const migratingTz = useRef(false);

  // If firmware has not yet rewritten tz_format from the label (or the user
  // landed here before reboot), persist the current TIME_ZONES values quietly.
  useEffect(() => {
    if (!settings || migratingTz.current || dirtyFlags.length > 0) {
      return;
    }
    const migrated = migrateNtpTimeZones(settings);
    if (!migrated) {
      return;
    }
    migratingTz.current = true;
    void (async () => {
      try {
        updateDataValue(migrated);
        await NTPApi.updateNTPSettings(migrated);
        setOrigData(migrated);
      } catch {
        // Boot-time firmware migration still applies on the next restart.
      } finally {
        migratingTz.current = false;
      }
    })();
  }, [settings, dirtyFlags.length, setOrigData, updateDataValue]);

  // the POSIX format follows the label, so it doesn't count as a second change
  const timeZone = ([label, format]: TzKeys) => {
    const value = settings
      ? (selectedTimeZone(settings[label], settings[format]) ?? '')
      : '';
    return {
      ...form.select(label),
      value,
      options: value
        ? TIME_ZONE_OPTIONS
        : [{ value: '', label: `${LL.TIME_ZONE()}...` }, ...TIME_ZONE_OPTIONS],
      onChange: (e: ChangeEvent<HTMLSelectElement>) => {
        const tz = e.currentTarget.value;
        form.set({ [label]: tz, [format]: TIME_ZONES[tz] ?? '' });
        form.setDirtyFlags((flags) => flags.filter((k) => k !== format));
      }
    };
  };

  return (
    <FormPage form={form}>
      {(data) => (
        <>
          <NTPClock canSetTime={!data.enabled && dirtyFlags.length === 0} />
          <FormSection icon="schedule" title={LL.NTP_TIME_SERVER()}>
            <Switch label={LL.ENABLE_NTP()} {...form.check('enabled')} />
            <TextField
              label={LL.NTP_SERVER()}
              mono
              disabled={!data.enabled}
              {...form.text('server')}
            />
            <Select
              label={LL.TIME_ZONE()}
              {...timeZone(['tz_label', 'tz_format'])}
            />
          </FormSection>
          {data.enabled && (
            <FormSection icon="thermostat" title={LL.NTP_THERMOSTAT_CLOCK()}>
              <Select
                label={LL.NTP_THERMOSTAT_SYNC()}
                helperText={LL.NTP_THERMOSTAT_SYNC_HELP()}
                options={[
                  { value: '0', label: LL.OFF() },
                  { value: '1', label: LL.NTP_THERMOSTAT_SYNC_1() },
                  { value: '2', label: LL.NTP_THERMOSTAT_SYNC_2() }
                ]}
                {...form.select('thermostat_sync')}
              />
              {data.thermostat_sync === 2 && (
                <Select
                  label={LL.NTP_THERMOSTAT_TZ()}
                  {...timeZone(['tz_label_t', 'tz_format_t'])}
                />
              )}
            </FormSection>
          )}
        </>
      )}
    </FormPage>
  );
};

export default NTPSettings;
