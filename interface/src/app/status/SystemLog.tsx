import { useEffect, useRef, useState } from 'react';
import { toast } from 'react-toastify';

import DownloadIcon from '@mui/icons-material/GetApp';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Checkbox,
  Grid2 as Grid,
  IconButton,
  MenuItem,
  TextField,
  styled
} from '@mui/material';

import { API } from 'api/app';
import { fetchLogES, readLogSettings, updateLogSettings } from 'api/system';

import { useRequest, useSSE } from 'alova/client';
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

const TextColors = {
  [LogLevel.ERROR]: '#ff0000', // red
  [LogLevel.WARNING]: '#ff0000', // red
  [LogLevel.NOTICE]: '#ffffff', // white
  [LogLevel.INFO]: '#ffcc00', // yellow
  [LogLevel.DEBUG]: '#00ffff', // cyan
  [LogLevel.TRACE]: '#00ffff' // cyan
};

const LogEntryLine = styled('span')(
  ({ details: { level } }: { details: { level: LogLevel } }) => ({
    color: TextColors[level]
  })
);

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
    case LogLevel.DEBUG:
      return 'DEBUG';
    case LogLevel.TRACE:
      return 'TRACE';
    default:
      return '';
  }
};

const SystemLog = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.LOG_OF(LL.SYSTEM(0)));

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
    read: readLogSettings,
    update: updateLogSettings
  });

  const { send } = useRequest(
    (data: string) => API({ device: 'system', cmd: 'read', id: 0, data: data }),
    {
      immediate: false
    }
  );

  const [readValue, setReadValue] = useState('');
  const [readOpen, setReadOpen] = useState(false);
  const [logEntries, setLogEntries] = useState<LogEntry[]>([]);
  const [autoscroll, setAutoscroll] = useState(true);
  const [lastId, setLastId] = useState<number>(-1);

  const ALPHA_NUMERIC_DASH_REGEX = /^[a-fA-F0-9 ]+$/;

  const updateFormValue = updateValueDirty(
    origData,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue
  );

  useSSE(fetchLogES, {
    immediate: true,
    interceptByGlobalResponded: false
  })
    .onMessage((message: { data: string }) => {
      const rawData = message.data;
      const logentry = JSON.parse(rawData) as LogEntry;
      if (lastId < logentry.i) {
        setLogEntries((log) => [...log, logentry]);
        setLastId(logentry.i);
      }
    })
    .onError(() => {
      toast.error('No connection to Log service');
    });

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
    if (logEntries.length && autoscroll) {
      ref.current?.scrollIntoView({
        behavior: 'smooth',
        block: 'end'
      });
    }
  }, [logEntries.length]);

  const sendReadCommand = () => {
    if (readValue === '') {
      setReadOpen(!readOpen);
      return;
    }

    if (readValue.split(' ').filter((word) => word !== '').length > 1) {
      void send(readValue);
      setReadOpen(false);
      setReadValue('');
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        <Grid container spacing={2} alignItems="center">
          <Grid>
            <TextField
              name="level"
              label={LL.LOG_LEVEL()}
              value={data.level}
              sx={{ width: '14ch' }}
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
          {data.psram && (
            <Grid>
              <TextField
                name="max_messages"
                label={LL.BUFFER_SIZE()}
                value={data.max_messages}
                sx={{ width: '15ch' }}
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
          )}
          <Grid>
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
            <BlockFormControlLabel
              control={
                <Checkbox
                  checked={autoscroll}
                  onChange={() => setAutoscroll(!autoscroll)}
                  name="autoscroll"
                />
              }
              label={LL.AUTO_SCROLL()}
            />
          </Grid>
          <Grid>
            <Button
              startIcon={<DownloadIcon />}
              variant="outlined"
              color="secondary"
              onClick={onDownload}
            >
              {LL.EXPORT()}
            </Button>
          </Grid>

          {readOpen ? (
            <Box
              component="form"
              sx={{ display: 'flex', alignItems: 'flex-end' }}
              onSubmit={(e) => {
                e.preventDefault();
                sendReadCommand();
              }}
            >
              <IconButton
                disableRipple
                onClick={() => {
                  setReadOpen(false);
                  setReadValue('');
                }}
              >
                <PlayArrowIcon color="primary" sx={{ my: 2.5 }} />
              </IconButton>
              <TextField
                value={readValue}
                onChange={(e) => {
                  const value = e.target.value;
                  if (value !== '' && !ALPHA_NUMERIC_DASH_REGEX.test(value)) {
                    return;
                  }
                  setReadValue(value);
                }}
                focused={true}
                size="small"
                label="Send Read command" // doesn't need translating - developer only
                helperText="<deviceID> <typeID> [offset] [len]"
              />
            </Box>
          ) : (
            <>
              {data.developer_mode && (
                <IconButton onClick={sendReadCommand}>
                  <PlayArrowIcon color="primary" />
                </IconButton>
              )}
            </>
          )}

          {dirtyFlags && dirtyFlags.length !== 0 && (
            <Grid>
              <Button
                startIcon={<WarningIcon color="warning" />}
                variant="contained"
                color="info"
                onClick={saveSettings}
              >
                {LL.APPLY_CHANGES(dirtyFlags.length)}
              </Button>
            </Grid>
          )}
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
            <div key={e.i} style={{ font: '14px monospace', whiteSpace: 'nowrap' }}>
              <span>{e.t}</span>
              <span>{paddedLevelLabel(e.l)}&nbsp;</span>
              <span>{paddedIDLabel(e.i)} </span>
              <span>{paddedNameLabel(e.n)} </span>
              <LogEntryLine details={{ level: e.l }} key={e.i}>
                {e.m}
              </LogEntryLine>
            </div>
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
