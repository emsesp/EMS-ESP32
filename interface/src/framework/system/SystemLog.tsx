import DownloadIcon from '@mui/icons-material/GetApp';
import WarningIcon from '@mui/icons-material/Warning';
import { Box, styled, Button, Checkbox, MenuItem, Grid } from '@mui/material';
import { useState, useEffect, useCallback, useLayoutEffect } from 'react';
import type { FC } from 'react';

import type { LogSettings, LogEntry, LogEntries } from 'types';
import { addAccessTokenParameter } from 'api/authentication';
import { EVENT_SOURCE_ROOT } from 'api/endpoints';
import * as SystemApi from 'api/system';

import { SectionContent, FormLoader, BlockFormControlLabel, ValidatedTextField, ButtonRow } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import { LogLevel } from 'types';
import { useRest, updateValueDirty, extractErrorMessage } from 'utils';

export const LOG_EVENTSOURCE_URL = EVENT_SOURCE_ROOT + 'log';

const useWindowSize = () => {
  const [size, setSize] = useState([0, 0]);
  useLayoutEffect(() => {
    function updateSize() {
      setSize([window.innerWidth, window.innerHeight]);
    }
    window.addEventListener('resize', updateSize);
    updateSize();
    return () => window.removeEventListener('resize', updateSize);
  }, []);
  return size;
};

const LogEntryLine = styled('div')(({ theme }) => ({
  color: '#bbbbbb',
  fontFamily: 'monospace',
  fontSize: '14px',
  letterSpacing: 'normal',
  whiteSpace: 'nowrap'
}));

const topOffset = () => document.getElementById('log-window')?.getBoundingClientRect().bottom || 0;
const leftOffset = () => document.getElementById('log-window')?.getBoundingClientRect().left || 0;

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
    case LogLevel.DEBUG:
      return 'DEBUG';
    case LogLevel.TRACE:
      return 'TRACE';
    default:
      return '';
  }
};

const SystemLog: FC = () => {
  useWindowSize();

  const { LL } = useI18nContext();

  const { loadData, data, setData, saveData, origData, dirtyFlags, setDirtyFlags } = useRest<LogSettings>({
    read: SystemApi.readLogSettings,
    update: SystemApi.updateLogSettings
  });

  const [errorMessage, setErrorMessage] = useState<string>();
  const [logEntries, setLogEntries] = useState<LogEntries>({ events: [] });
  const [lastIndex, setLastIndex] = useState<number>(0);

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

  const updateFormValue = updateValueDirty(origData, dirtyFlags, setDirtyFlags, setData);

  const onDownload = () => {
    let result = '';
    for (const i of logEntries.events) {
      result += i.t + ' ' + levelLabel(i.l) + ' ' + i.i + ': [' + i.n + '] ' + i.m + '\n';
    }
    const a = document.createElement('a');
    a.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(result));
    a.setAttribute('download', 'log.txt');
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  };

  const onMessage = (event: MessageEvent) => {
    const rawData = event.data;
    if (typeof rawData === 'string' || rawData instanceof String) {
      const logentry = JSON.parse(rawData as string) as LogEntry;
      if (logentry.i > lastIndex) {
        setLastIndex(logentry.i);
        setLogEntries((old) => ({ events: [...old.events, logentry] }));
      }
    }
  };

  const fetchLog = useCallback(async () => {
    try {
      await SystemApi.readLogEntries();
    } catch (error) {
      setErrorMessage(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  }, [LL]);

  useEffect(() => {
    void fetchLog();
  }, [fetchLog]);

  useEffect(() => {
    const es = new EventSource(addAccessTokenParameter(LOG_EVENTSOURCE_URL));
    es.onmessage = onMessage;
    es.onerror = () => {
      es.close();
      window.location.reload();
    };

    return () => {
      es.close();
    };
    // eslint-disable-next-line
  }, []);

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        <Grid container spacing={3} direction="row" justifyContent="flex-start" alignItems="center">
          <Grid item xs={2}>
            <ValidatedTextField
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
              <MenuItem value={7}>DEBUG</MenuItem>
              <MenuItem value={9}>ALL</MenuItem>
            </ValidatedTextField>
          </Grid>
          <Grid item xs={2}>
            <ValidatedTextField
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
            </ValidatedTextField>
          </Grid>
          <Grid item>
            <BlockFormControlLabel
              control={<Checkbox checked={data.compact} onChange={updateFormValue} name="compact" />}
              label={LL.COMPACT()}
            />
          </Grid>
          <ButtonRow>
            <Button startIcon={<DownloadIcon />} variant="outlined" color="secondary" onClick={onDownload}>
              {LL.EXPORT()}
            </Button>
            {dirtyFlags && dirtyFlags.length !== 0 && (
              <Button startIcon={<WarningIcon color="warning" />} variant="contained" color="info" onClick={saveData}>
                {LL.UPDATE()}
              </Button>
            )}
          </ButtonRow>
        </Grid>
        <Box
          sx={{
            backgroundColor: 'black',
            overflow: 'scroll',
            position: 'absolute',
            right: 18,
            bottom: 18,
            left: () => leftOffset(),
            top: () => topOffset(),
            p: 1
          }}
        >
          {logEntries &&
            logEntries.events.map((e) => (
              <LogEntryLine key={e.i}>
                <span>{e.t}</span>
                {data.compact && <span>{paddedLevelLabel(e.l)} </span>}
                {!data.compact && <span>{paddedLevelLabel(e.l)}&nbsp;</span>}
                <span>{paddedIDLabel(e.i)} </span>
                <span>{paddedNameLabel(e.n)} </span>
                <span>{e.m}</span>
              </LogEntryLine>
            ))}
        </Box>
      </>
    );
  };

  return (
    <SectionContent title={LL.LOG_OF(LL.SYSTEM(2))} titleGutter id="log-window">
      {content()}
    </SectionContent>
  );
};

export default SystemLog;
