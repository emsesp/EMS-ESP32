import {
  type KeyboardEvent,
  memo,
  useContext,
  useEffect,
  useLayoutEffect,
  useMemo,
  useRef,
  useState
} from 'react';

import DownloadIcon from '@mui/icons-material/GetApp';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Checkbox,
  Grid,
  IconButton,
  InputAdornment,
  MenuItem,
  TextField,
  styled
} from '@mui/material';

import {
  fetchLogES,
  readLogSettings,
  runConsoleCommand,
  updateLogSettings
} from 'api/system';

import { useRequest, useSSE } from 'alova/client';
import {
  BlockFormControlLabel,
  BlockNavigation,
  FormLoader,
  SectionContent,
  useLayoutTitle
} from 'components';
import { toast } from 'components/toast';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { LogEntry, LogSettings } from 'types';
import { LogLevel } from 'types';
import { updateValueDirty, useRest } from 'utils';

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

// a console command and its output, shown after the log entry that was last when it was sent
interface ConsoleBlock {
  id: number;
  afterLogId: number;
  command: string;
  output: string;
}

const CONSOLE_HEIGHT = 48;
const CONSOLE_HISTORY = 20;

// the shell output has terminal colour codes and \r\n line endings
const ANSI_CODES = new RegExp(String.fromCharCode(27) + '\\[[0-9;]*[A-Za-z]', 'g');
const cleanOutput = (output: string) =>
  output
    .replace(ANSI_CODES, '')
    .replace(/\r/g, '')
    .replace(/^\n+|\n+$/g, '');

const ConsoleBlockItem = ({ block }: { block: ConsoleBlock }) => (
  <div style={{ font: '13px monospace', whiteSpace: 'pre', color: '#7fff7f' }}>
    <span style={{ color: '#00ff00', fontWeight: 'bold' }}># {block.command}</span>
    {block.output && '\n' + block.output}
  </div>
);

const SystemLog = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

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

  const { send: sendConsoleCommand, loading: consoleBusy } = useRequest(
    runConsoleCommand,
    { immediate: false }
  );

  const [logEntries, setLogEntries] = useState<LogEntry[]>([]);
  const [consoleBlocks, setConsoleBlocks] = useState<ConsoleBlock[]>([]);
  const [consoleCommand, setConsoleCommand] = useState('');
  const [consoleHistory, setConsoleHistory] = useState<string[]>([]);
  const [historyIndex, setHistoryIndex] = useState(-1);
  const [autoscroll, setAutoscroll] = useState(true);
  const [boxPosition, setBoxPosition] = useState({ top: 0, left: 0 });
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

  const handleLogMessage = (message: { data: string }) => {
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
      return newLog;
    });
  };

  useSSE(fetchLogES, {
    immediate: true,
    interceptByGlobalResponded: false
  })
    .onMessage(handleLogMessage)
    .onError(() => {
      toast.error('No connection to Log service');
    });

  const onDownload = () => {
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
  };

  const saveSettings = async () => {
    await saveData();
  };

  // handle scrolling - optimized to only scroll when needed
  const ref = useRef<HTMLDivElement>(null);
  const logWindowRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if ((logEntries.length || consoleBlocks.length) && autoscroll) {
      const container = logWindowRef.current;
      if (container) {
        requestAnimationFrame(() => {
          container.scrollTop = container.scrollHeight;
        });
      }
    }
  }, [logEntries.length, consoleBlocks.length, autoscroll]);

  const blocksAfterLogId = useMemo(() => {
    const blocks = new Map<number, ConsoleBlock[]>();
    for (const block of consoleBlocks) {
      blocks.set(block.afterLogId, [...(blocks.get(block.afterLogId) ?? []), block]);
    }
    return blocks;
  }, [consoleBlocks]);

  const runCommand = async () => {
    const command = consoleCommand.trim();
    if (command === '' || consoleBusy) {
      return;
    }

    setConsoleHistory((history) =>
      [command, ...history.filter((c) => c !== command)].slice(0, CONSOLE_HISTORY)
    );
    setHistoryIndex(-1);
    setConsoleCommand('');

    let output: string;
    try {
      const result = await sendConsoleCommand(command);
      output = cleanOutput(result.output);
      if (result.truncated) {
        output += '\n... output truncated';
      }
    } catch (error) {
      output = (error as Error).message;
    }

    setConsoleBlocks((blocks) => [
      ...blocks,
      {
        id: blocks.length + 1,
        afterLogId: logEntries.length ? logEntries[logEntries.length - 1]!.i : 0,
        command,
        output
      }
    ]);
  };

  const onConsoleKeyDown = (e: KeyboardEvent) => {
    if (e.key === 'Enter') {
      e.preventDefault();
      void runCommand();
    } else if (e.key === 'ArrowUp' && historyIndex < consoleHistory.length - 1) {
      e.preventDefault();
      setHistoryIndex(historyIndex + 1);
      setConsoleCommand(consoleHistory[historyIndex + 1] ?? '');
    } else if (e.key === 'ArrowDown' && historyIndex >= 0) {
      e.preventDefault();
      setHistoryIndex(historyIndex - 1);
      setConsoleCommand(
        historyIndex > 0 ? (consoleHistory[historyIndex - 1] ?? '') : ''
      );
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage || ''} />;
    }

    const showConsole = me.admin && data.developer_mode;

    return (
      <>
        <Grid container spacing={2} sx={{ alignItems: 'center' }}>
          <Grid>
            <TextField
              name="level"
              label={LL.LOG_LEVEL()}
              value={data.level}
              sx={{ width: '14ch' }}
              variant="outlined"
              onChange={updateFormValue}
              margin="normal"
              disabled={!me.admin}
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
                disabled={!me.admin}
                select
              >
                <MenuItem value={25}>25</MenuItem>
                <MenuItem value={50}>50</MenuItem>
                <MenuItem value={75}>75</MenuItem>
                <MenuItem value={100}>100</MenuItem>
                <MenuItem value={500}>500</MenuItem>
                <MenuItem value={1000}>1000</MenuItem>
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
                  disabled={!me.admin}
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

          {me.admin && dirtyFlags && dirtyFlags.length !== 0 && (
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
            bottom: showConsole ? 18 + CONSOLE_HEIGHT : 18,
            left: boxPosition.left,
            top: boxPosition.top,
            p: 1
          }}
        >
          {blocksAfterLogId.get(0)?.map((b) => (
            <ConsoleBlockItem key={'c' + b.id} block={b} />
          ))}
          {logEntries.map((e) => (
            <div key={e.i}>
              <LogEntryItem entry={e} compact={data.compact} />
              {blocksAfterLogId.get(e.i)?.map((b) => (
                <ConsoleBlockItem key={'c' + b.id} block={b} />
              ))}
            </div>
          ))}

          <div ref={ref} />
        </Box>

        {showConsole && (
          <Box
            sx={{
              position: 'absolute',
              right: 18,
              bottom: 18,
              left: boxPosition.left,
              height: CONSOLE_HEIGHT,
              display: 'flex',
              alignItems: 'flex-end'
            }}
          >
            <TextField
              fullWidth
              size="small"
              placeholder={LL.COMMAND(0)}
              value={consoleCommand}
              onChange={(e) => {
                setConsoleCommand(e.target.value);
                setHistoryIndex(-1);
              }}
              onKeyDown={onConsoleKeyDown}
              disabled={consoleBusy}
              autoComplete="off"
              slotProps={{
                htmlInput: { maxLength: 200, spellCheck: false },
                input: {
                  sx: { fontFamily: 'monospace' },
                  startAdornment: (
                    <InputAdornment position="start">#</InputAdornment>
                  ),
                  endAdornment: (
                    <InputAdornment position="end">
                      <IconButton
                        edge="end"
                        aria-label={LL.RUN()}
                        onClick={() => void runCommand()}
                        disabled={consoleBusy || consoleCommand.trim() === ''}
                      >
                        <PlayArrowIcon color="primary" />
                      </IconButton>
                    </InputAdornment>
                  )
                }
              }}
            />
          </Box>
        )}
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
