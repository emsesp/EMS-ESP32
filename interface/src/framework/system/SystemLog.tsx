import { useEffect, useRef, useState } from 'react';
import type { FC } from 'react';
import { toast } from 'react-toastify';

import DownloadIcon from '@mui/icons-material/GetApp';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Checkbox,
  Grid,
  MenuItem,
  TextField,
  styled
} from '@mui/material';

import * as SystemApi from 'api/system';
import { fetchLogES } from 'api/system';

import { useSSE } from '@alova/scene-react';
import { useRequest } from 'alova';
import {
  BlockFormControlLabel,
  BlockNavigation,
  FormLoader,
  SectionContent,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { LogEntry, LogSettings } from 'types';
import { LogLevel } from 'types';
import { updateValueDirty, useRest } from 'utils';

const LogEntryLine = styled('div')(() => ({
  color: '#bbbbbb',
  fontFamily: 'monospace',
  fontSize: '14px',
  letterSpacing: 'normal',
  whiteSpace: 'nowrap'
}));

const topOffset = () =>
  document.getElementById('log-window')?.getBoundingClientRect().bottom || 0;
const leftOffset = () =>
  document.getElementById('log-window')?.getBoundingClientRect().left || 0;

const levelLabel = (level: LogLevel) => {
  switch (level) {
    case LogLevel.ERROR:
      return 'ERROR';
    case LogLevel.WARNING:
      return 'WARNING';
    case LogLevel.NOTICE:
      return 'NOTICE';
    case LogLevel.INFO:
      return 'INFO';
    case LogLevel.TRACE:
      return 'TRACE';
    default:
      return '';
  }
};

const SystemLog: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.LOG_OF(''));

  const {
    loadData,
    data,
    updateDataValue,
    origData,
    dirtyFlags,
    setDirtyFlags,
    blocker,
    saveData,
    errorMessage
  } = useRest<LogSettings>({
    read: SystemApi.readLogSettings,
    update: SystemApi.updateLogSettings
  });

  const [logEntries, setLogEntries] = useState<LogEntry[]>([]);
  const [lastIndex, setLastIndex] = useState<number>(0);

  const updateFormValue = updateValueDirty(
    origData,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue
  );

  // eslint-disable-next-line @typescript-eslint/unbound-method
  const { onMessage, onError } = useSSE(fetchLogES, {
    immediate: true,
    // withCredentials: true,
    interceptByGlobalResponded: false
  });

  onMessage((message: { id: number; data: string }) => {
    const rawData = message.data;
    const logentry = JSON.parse(rawData) as LogEntry;
    if (logentry.i > lastIndex) {
      setLastIndex(logentry.i);
      setLogEntries((log) => [...log, logentry]);
    }
  });

  onError(() => {
    toast.error('No connection to Log server');
  });

  // called on page load to reset pointer and fetch all log entries
  useRequest(SystemApi.fetchLog());

  const paddedLevelLabel = (level: LogLevel) => {
    const label = levelLabel(level);
    return data?.compact ? ' ' + label[0] : label.padStart(8, '\xa0');
  };

  const paddedNameLabel = (name: string) => {
    const label = '[' + name + ']';
    return data?.compact ? label : label.padEnd(12, '\xa0');
  };

  const paddedIDLabel = (id: number) => {
    const label = id + ':';
    return data?.compact ? label : label.padEnd(7, '\xa0');
  };

  const onDownload = () => {
    let result = '';
    for (const i of logEntries) {
      result +=
        i.t + ' ' + levelLabel(i.l) + ' ' + i.i + ': [' + i.n + '] ' + i.m + '\n';
    }
    const a = document.createElement('a');
    a.setAttribute(
      'href',
      'data:text/plain;charset=utf-8,' + encodeURIComponent(result)
    );
    a.setAttribute('download', 'log.txt');
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  };

  const saveSettings = async () => {
    await saveData();
  };

  // handle scrolling
  const ref = useRef<HTMLDivElement>(null);
  useEffect(() => {
    if (logEntries.length) {
      ref.current?.scrollIntoView({
        behavior: 'smooth',
        block: 'end'
      });
    }
  }, [logEntries.length]);

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        <Grid
          container
          spacing={3}
          direction="row"
          justifyContent="flex-start"
          alignItems="center"
        >
          <Grid item xs={2}>
            <TextField
              name="level"
              label={LL.LOG_LEVEL()}
              value={data.level}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={-1}>OFF</MenuItem>
              <MenuItem value={3}>ERROR</MenuItem>
              <MenuItem value={4}>WARNING</MenuItem>
              <MenuItem value={5}>NOTICE</MenuItem>
              <MenuItem value={6}>INFO</MenuItem>
              <MenuItem value={9}>ALL</MenuItem>
            </TextField>
          </Grid>
          <Grid item xs={2}>
            <TextField
              name="max_messages"
              label={LL.BUFFER_SIZE()}
              value={data.max_messages}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={25}>25</MenuItem>
              <MenuItem value={50}>50</MenuItem>
              <MenuItem value={75}>75</MenuItem>
              <MenuItem value={100}>100</MenuItem>
            </TextField>
          </Grid>
          <Grid item>
            <BlockFormControlLabel
              control={
                <Checkbox
                  checked={data.compact}
                  onChange={updateFormValue}
                  name="compact"
                />
              }
              label={LL.COMPACT()}
            />
          </Grid>
          <Box
            sx={{
              '& button, & a, & .MuiCard-root': {
                mt: 3,
                mx: 0.6
              }
            }}
          >
            <Button
              startIcon={<DownloadIcon />}
              variant="outlined"
              color="secondary"
              onClick={onDownload}
            >
              {LL.EXPORT()}
            </Button>
            {dirtyFlags && dirtyFlags.length !== 0 && (
              <Button
                startIcon={<WarningIcon color="warning" />}
                variant="contained"
                color="info"
                onClick={saveSettings}
              >
                {LL.APPLY_CHANGES(dirtyFlags.length)}
              </Button>
            )}
          </Box>
        </Grid>
        <Box
          sx={{
            backgroundColor: 'black',
            overflowY: 'scroll',
            position: 'absolute',
            right: 18,
            bottom: 18,
            left: () => leftOffset(),
            top: () => topOffset(),
            p: 1
          }}
        >
          {logEntries.map((e) => (
            <LogEntryLine key={e.i}>
              <span>{e.t}</span>
              {data.compact && <span>{paddedLevelLabel(e.l)} </span>}
              {!data.compact && <span>{paddedLevelLabel(e.l)}&nbsp;</span>}
              <span>{paddedIDLabel(e.i)} </span>
              <span>{paddedNameLabel(e.n)} </span>
              <span>{e.m}</span>
            </LogEntryLine>
          ))}
          <div ref={ref} />
        </Box>
      </>
    );
  };

  return (
    <SectionContent id="log-window">
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {content()}
    </SectionContent>
  );
};

export default SystemLog;
