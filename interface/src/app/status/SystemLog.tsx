import {
  memo,
  useCallback,
  useEffect,
  useLayoutEffect,
  useRef,
  useState
} from 'react';
import { toast } from 'react-toastify';

import DownloadIcon from '@mui/icons-material/GetApp';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Checkbox,
  Grid,
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

const MAX_LOG_ENTRIES = 1000; // Limit log entries to prevent memory issues

const TextColors: Record<LogLevel, string> = {
  [LogLevel.ERROR]: '#ff0000', // red
  [LogLevel.WARNING]: '#ff0000', // red
  [LogLevel.NOTICE]: '#ffffff', // white
  [LogLevel.INFO]: '#ffcc00', // yellow
  [LogLevel.DEBUG]: '#00ffff', // cyan
  [LogLevel.TRACE]: '#00ffff', // cyan
  [LogLevel.ALL]: '#ffffff' // white
};

const LogEntryLine = styled('span')(
  ({ details: { level } }: { details: { level: LogLevel } }) => ({
    color: TextColors[level]
  })
);

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

const paddedLevelLabel = (level: LogLevel, compact: boolean) => {
  const label = levelLabel(level);
  return compact ? ' ' + label[0] : label.padStart(8, '\xa0');
};

const paddedNameLabel = (name: string, compact: boolean) => {
  const label = '[' + name + ']';
  return compact ? label : label.padEnd(12, '\xa0');
};

const paddedIDLabel = (id: number, compact: boolean) => {
  const label = id + ':';
  return compact ? label : label.padEnd(7, '\xa0');
};

// Memoized log entry component to prevent unnecessary re-renders
const LogEntryItem = memo(
  ({ entry, compact }: { entry: LogEntry; compact: boolean }) => {
    return (
      <div style={{ font: '13px monospace', whiteSpace: 'nowrap' }}>
        <span>{entry.t}</span>
        <span>{paddedLevelLabel(entry.l, compact)}&nbsp;</span>
        <span>{paddedIDLabel(entry.i, compact)} </span>
        <span>{paddedNameLabel(entry.n, compact)} </span>
        <LogEntryLine details={{ level: entry.l }}>{entry.m}</LogEntryLine>
      </div>
    );
  },
  (prevProps, nextProps) =>
    prevProps.entry.i === nextProps.entry.i &&
    prevProps.compact === nextProps.compact
);

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
  const [boxPosition, setBoxPosition] = useState({ top: 0, left: 0 });

  const ALPHA_NUMERIC_DASH_REGEX = /^[a-fA-F0-9 ]+$/;

  const updateFormValue = updateValueDirty(
    origData as unknown as Record<string, unknown>,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue as (value: unknown) => void
  );

  // Calculate box position after layout
  useLayoutEffect(() => {
    const logWindow = document.getElementById('log-window');
    if (!logWindow) {
      return;
    }

    const updatePosition = () => {
      const windowElement = document.getElementById('log-window');
      if (!windowElement) {
        return;
      }
      const rect = windowElement.getBoundingClientRect();
      setBoxPosition({ top: rect.bottom, left: rect.left });
    };

    updatePosition();

    // Debounce resize events with requestAnimationFrame
    let rafId: number;
    const handleResize = () => {
      cancelAnimationFrame(rafId);
      rafId = requestAnimationFrame(updatePosition);
    };

    // Update position on window resize
    window.addEventListener('resize', handleResize);
    const resizeObserver = new ResizeObserver(handleResize);
    resizeObserver.observe(logWindow);

    return () => {
      window.removeEventListener('resize', handleResize);
      resizeObserver.disconnect();
      cancelAnimationFrame(rafId);
    };
  }, [data]); // Recalculate when data changes (in case layout shifts)

  // Memoize message handler to avoid recreating on every render
  const handleLogMessage = useCallback((message: { data: string }) => {
    const rawData = message.data;
    const logentry = JSON.parse(rawData) as LogEntry;
    setLogEntries((log) => {
      // Skip if this is a duplicate entry (check last entry id)
      if (log.length > 0) {
        const lastEntry = log[log.length - 1];
        if (lastEntry && logentry.i <= lastEntry.i) {
          return log;
        }
      }
      const newLog = [...log, logentry];
      // Limit log entries to prevent memory issues - only slice when necessary
      if (newLog.length > MAX_LOG_ENTRIES) {
        return newLog.slice(-MAX_LOG_ENTRIES);
      }
      return newLog;
    });
  }, []);

  useSSE(fetchLogES, {
    immediate: true,
    interceptByGlobalResponded: false
  })
    .onMessage(handleLogMessage)
    .onError(() => {
      toast.error('No connection to Log service');
    });

  const onDownload = useCallback(() => {
    const result = logEntries
      .map((i) => `${i.t} ${levelLabel(i.l)} ${i.i}: [${i.n}] ${i.m}`)
      .join('\n');

    const a = document.createElement('a');
    a.setAttribute(
      'href',
      'data:text/plain;charset=utf-8,' + encodeURIComponent(result)
    );
    a.setAttribute('download', 'log.txt');
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  }, [logEntries]);

  const saveSettings = useCallback(async () => {
    await saveData();
  }, [saveData]);

  // handle scrolling - optimized to only scroll when needed
  const ref = useRef<HTMLDivElement>(null);
  const logWindowRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (logEntries.length && autoscroll) {
      const container = logWindowRef.current;
      if (container) {
        requestAnimationFrame(() => {
          container.scrollTop = container.scrollHeight;
        });
      }
    }
  }, [logEntries.length, autoscroll]);

  const sendReadCommand = useCallback(() => {
    if (readValue === '') {
      setReadOpen(!readOpen);
      return;
    }

    if (readValue.split(' ').filter((word) => word !== '').length > 1) {
      void send(readValue);
      setReadOpen(false);
      setReadValue('');
    }
  }, [readValue, readOpen, send]);

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage || ''} />;
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
                aria-label={LL.CANCEL()}
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
                <IconButton onClick={sendReadCommand} aria-label={LL.EXECUTE()}>
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
          ref={logWindowRef}
          sx={{
            backgroundColor: 'black',
            overflowY: 'scroll',
            position: 'absolute',
            right: 18,
            bottom: 18,
            left: boxPosition.left,
            top: boxPosition.top,
            p: 1
          }}
        >
          {logEntries.map((e) => (
            <LogEntryItem key={e.i} entry={e} compact={data.compact} />
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
