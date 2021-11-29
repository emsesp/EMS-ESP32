import { FC, useState, useEffect, useCallback, useLayoutEffect } from 'react';

import { Box, styled, Button, Checkbox, MenuItem, Grid, Slider, FormLabel } from '@mui/material';

import * as SystemApi from '../../api/system';
import { addAccessTokenParameter } from '../../api/authentication';

import { SectionContent, ButtonRow, FormLoader, BlockFormControlLabel, ValidatedTextField } from '../../components';

import { LogSettings, LogEntry, LogEntries, LogLevel } from '../../types';
import { updateValue, useRest, extractErrorMessage } from '../../utils';

import DownloadIcon from '@mui/icons-material/GetApp';
import SaveIcon from '@mui/icons-material/Save';

import { EVENT_SOURCE_ROOT } from '../../api/endpoints';
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

  const { loadData, data, setData, saving, saveData } = useRest<LogSettings>({
    read: SystemApi.readLogSettings,
    update: SystemApi.updateLogSettings
  });

  const [errorMessage, setErrorMessage] = useState<string>();
  const [reconnectTimeout, setReconnectTimeout] = useState<NodeJS.Timeout>();
  const [logEntries, setLogEntries] = useState<LogEntries>({ events: [] });
  const [compact, setCompact] = useState<boolean>(false);
  const [lastIndex, setLastIndex] = useState<number>(0);

  const paddedLevelLabel = (level: LogLevel) => {
    const label = levelLabel(level);
    return compact ? ' ' + label[0] : label.padStart(8, '\xa0');
  };

  const paddedNameLabel = (name: string) => {
    const label = '[' + name + ']';
    return compact ? label : label.padEnd(12, '\xa0');
  };

  const paddedIDLabel = (id: number) => {
    const label = id + ':';
    return compact ? label : label.padEnd(7, '\xa0');
  };

  const updateFormValue = updateValue(setData);

  const changeMaxMessages = (event: Event, value: number | number[]) => {
    if (data) {
      setData({
        ...data,
        max_messages: value as number
      });
    }
  };

  const onDownload = () => {
    let result = '';
    for (let i of logEntries.events) {
      result += i.t + ' ' + levelLabel(i.l) + ' ' + i.i + ': [' + i.n + '] ' + i.m + '\n';
    }
    const a = document.createElement('a');
    a.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(result));
    a.setAttribute('download', 'log.txt');
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  };

  const onMessage = useCallback(
    (event: MessageEvent) => {
      const rawData = event.data;
      if (typeof rawData === 'string' || rawData instanceof String) {
        const logentry = JSON.parse(rawData as string) as LogEntry;
        if (logentry.i > lastIndex) {
          setLastIndex(logentry.i);
          setLogEntries((old) => ({ events: [...old.events, logentry] }));
        }
      }
    },
    [lastIndex]
  );

  const fetchLog = useCallback(async () => {
    // console.log('fetching data');
    try {
      setLogEntries((await SystemApi.readLogEntries()).data);
    } catch (error: any) {
      setErrorMessage(extractErrorMessage(error, 'Failed to fetch log'));
    }
  }, []);

  useEffect(() => {
    fetchLog();
  }, [fetchLog]);

  const reloadPage = () => {
    window.location.reload();
  };

  useEffect(() => {
    // console.log('es creating instance');

    const es = new EventSource(addAccessTokenParameter(LOG_EVENTSOURCE_URL));
    es.onmessage = onMessage;
    es.onerror = () => {
      if (reconnectTimeout) {
        // console.log('es reconnecting');
        es.close();
        setReconnectTimeout(setTimeout(reloadPage, 1000));
      }
    };

    return () => {
      // console.log('es closing');
      es.close();
      if (reconnectTimeout) {
        clearTimeout(reconnectTimeout);
      }
    };
  }, [onMessage, reconnectTimeout]);

  const renderSettings = () => {
    if (!data) {
      return <FormLoader loadData={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        <Grid container spacing={3} direction="row" justifyContent="flex-start" alignItems="center">
          <Grid item xs={2}>
            <ValidatedTextField
              name="level"
              label="Log Level"
              disabled={saving}
              value={data.level}
              fullWidth
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              select
            >
              <MenuItem value={3}>ERROR</MenuItem>
              <MenuItem value={4}>WARNING</MenuItem>
              <MenuItem value={5}>NOTICE</MenuItem>
              <MenuItem value={6}>INFO</MenuItem>
              <MenuItem value={7}>DEBUG</MenuItem>
              <MenuItem value={8}>ALL</MenuItem>
            </ValidatedTextField>
          </Grid>
          <Grid item xs={2}>
            <FormLabel>Buffer size</FormLabel>
            <Slider
              value={data.max_messages}
              valueLabelDisplay="auto"
              name="max_messages"
              marks={[
                { value: 25, label: '25' },
                { value: 50, label: '50' },
                { value: 75, label: '75' },
                { value: 100, label: '100' }
              ]}
              step={25}
              min={25}
              max={100}
              onChange={changeMaxMessages}
            />
          </Grid>
          <Grid item xs={2}>
            <BlockFormControlLabel
              control={<Checkbox checked={compact} onChange={() => setCompact(!compact)} value="compact" />}
              label="Compact"
            />
          </Grid>
          <Grid item md>
            <ButtonRow>
              <Button
                startIcon={<SaveIcon />}
                disabled={saving}
                variant="outlined"
                color="primary"
                type="submit"
                onClick={saveData}
              >
                Save
              </Button>
              <Button startIcon={<DownloadIcon />} variant="outlined" color="secondary" onClick={onDownload}>
                Export
              </Button>
            </ButtonRow>
          </Grid>
        </Grid>
      </>
    );
  };

  const renderLog = () => {
    if (data) {
      return (
        <Box
          sx={{
            backgroundColor: 'black',
            overflow: 'scroll',
            position: 'absolute',
            right: 24,
            bottom: 24,
            left: () => leftOffset(),
            top: () => topOffset(),
            p: 1
          }}
        >
          {logEntries &&
            logEntries.events.map((e) => (
              <LogEntryLine key={e.i}>
                <span>{e.t}</span>
                {compact && <span>{paddedLevelLabel(e.l)} </span>}
                {!compact && <span>{paddedLevelLabel(e.l)}&nbsp;</span>}
                <span>{paddedIDLabel(e.i)} </span>
                <span>{paddedNameLabel(e.n)} </span>
                <span>{e.m}</span>
              </LogEntryLine>
            ))}
        </Box>
      );
    }
  };

  return (
    <SectionContent title="System Log" titleGutter id="log-window">
      {renderSettings()}
      {renderLog()}
    </SectionContent>
  );
};

export default SystemLog;
