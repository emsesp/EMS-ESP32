import {
  type KeyboardEvent,
  memo,
  useContext,
  useEffect,
  useMemo,
  useRef,
  useState
} from 'react';

import {
  fetchLogES,
  readLogSettings,
  runConsoleCommand,
  updateLogSettings
} from 'api/system';

import { useRequest, useSSE } from 'alova/client';
import {
  Button,
  IconButton,
  Listbox,
  Loader,
  SearchField,
  Section,
  ToggleButton,
  ToggleGroup,
  cx,
  toast
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { LogEntry, LogSettings } from 'types';
import { LogLevel } from 'types';

import './SystemLog.css';

const LEVEL_NAMES: Partial<Record<LogLevel, string>> = {
  [LogLevel.ERROR]: 'ERROR',
  [LogLevel.WARNING]: 'WARNING',
  [LogLevel.NOTICE]: 'NOTICE',
  [LogLevel.INFO]: 'INFO',
  [LogLevel.DEBUG]: 'DEBUG',
  [LogLevel.TRACE]: 'TRACE'
};
const levelLabel = (level: LogLevel) => LEVEL_NAMES[level] ?? '';

const LEVEL_OPTIONS = [
  { value: '-1', label: 'OFF' },
  { value: '3', label: 'ERROR' },
  { value: '4', label: 'WARNING' },
  { value: '5', label: 'NOTICE' },
  { value: '6', label: 'INFO' },
  { value: '7', label: 'DEBUG' },
  { value: '8', label: 'TRACE' },
  { value: '9', label: 'ALL' }
];
const BUFFER_OPTIONS = [25, 50, 75, 100, 500, 1000].map((n) => ({
  value: String(n),
  label: String(n)
}));

// lines kept in the browser; the device buffer is much smaller
const MAX_LINES = 2000;
const CONSOLE_HISTORY = 20;

// a console command and its output, shown after the log entry that was last when it was sent
interface ConsoleBlock {
  id: number;
  afterLogId: number;
  command: string;
  output: string;
}

// the shell output has terminal colour codes and \r\n line endings
const ANSI_CODES = new RegExp(String.fromCharCode(27) + '\\[[0-9;]*[A-Za-z]', 'g');
const cleanOutput = (output: string) =>
  output
    .replace(ANSI_CODES, '')
    .replace(/\r/g, '')
    .replace(/^\n+|\n+$/g, '');

const LogLine = memo(({ entry, compact }: { entry: LogEntry; compact: boolean }) => {
  const label = levelLabel(entry.l);
  return (
    <div className={cx('ln', label.toLowerCase())}>
      <span className="t">{entry.t}</span>
      <span className="l">{compact ? label[0] : label}</span>
      {!compact && <span className="m">[{entry.n}]</span>}
      <span className="s">{entry.m}</span>
    </div>
  );
});

const ConsoleLines = ({ blocks }: { blocks?: ConsoleBlock[] | undefined }) =>
  blocks?.map((b) => (
    <div key={'c' + b.id} className="ln cmd">
      <span className="s">
        <b># {b.command}</b>
        {b.output && '\n' + b.output}
      </span>
    </div>
  ));

const SystemLog = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  const {
    data: settings,
    send: loadSettings,
    update: updateSettings,
    error
  } = useRequest(readLogSettings);
  const { send: saveSettings } = useRequest(
    (s: LogSettings) => updateLogSettings(s),
    { immediate: false }
  );
  const { send: sendConsoleCommand, loading: consoleBusy } = useRequest(
    runConsoleCommand,
    { immediate: false }
  );

  const [entries, setEntries] = useState<LogEntry[]>([]);
  const [blocks, setBlocks] = useState<ConsoleBlock[]>([]);
  const [query, setQuery] = useState('');
  const [autoScroll, setAutoScroll] = useState(true);
  // id of the last line shown; later lines are held back while paused
  const [pausedAt, setPausedAt] = useState<number>();
  const [command, setCommand] = useState('');
  const [history, setHistory] = useState<string[]>([]);
  const [historyIndex, setHistoryIndex] = useState(-1);
  const consoleRef = useRef<HTMLDivElement>(null);

  useSSE(fetchLogES, { immediate: true, interceptByGlobalResponded: false })
    .onMessage((message: { data: string }) => {
      const entry = JSON.parse(message.data) as LogEntry;
      setEntries((log) => {
        const last = log[log.length - 1];
        if (last && entry.i <= last.i) {
          return log;
        }
        const next = [...log, entry];
        return next.length > MAX_LINES ? next.slice(-MAX_LINES) : next;
      });
    })
    .onError(() => {
      toast.error(LL.LOG_NO_CONNECTION());
    });

  const shown = useMemo(() => {
    const q = query.trim().toLowerCase();
    return entries.filter(
      (e) =>
        (pausedAt === undefined || e.i <= pausedAt) &&
        (!q || `${e.t} ${levelLabel(e.l)} ${e.n} ${e.m}`.toLowerCase().includes(q))
    );
  }, [entries, query, pausedAt]);

  const held =
    pausedAt === undefined ? 0 : entries.filter((e) => e.i > pausedAt).length;

  const blocksAfter = useMemo(() => {
    const map = new Map<number, ConsoleBlock[]>();
    for (const b of blocks) {
      map.set(b.afterLogId, [...(map.get(b.afterLogId) ?? []), b]);
    }
    return map;
  }, [blocks]);

  useEffect(() => {
    const el = consoleRef.current;
    if (autoScroll && el) {
      requestAnimationFrame(() => {
        el.scrollTop = el.scrollHeight;
      });
    }
  }, [shown.length, blocks.length, autoScroll]);

  if (!settings) {
    return <Loader error={error?.message} onRetry={() => void loadSettings()} />;
  }

  const compact = settings.compact;
  const showConsole = me.admin && settings.developer_mode;

  // admins change the device setting straight away, guests only change the view
  const changeSetting = async (change: Partial<LogSettings>) => {
    const prev = settings;
    const next = { ...settings, ...change };
    updateSettings({ data: next });
    if (!me.admin) return;
    try {
      await saveSettings(next);
    } catch (e) {
      updateSettings({ data: prev });
      toast.error((e as Error).message);
    }
  };

  const togglePause = () =>
    setPausedAt(
      pausedAt === undefined ? (entries[entries.length - 1]?.i ?? 0) : undefined
    );

  const onDownload = () => {
    const text = entries
      .map((e) => `${e.t} ${levelLabel(e.l)} ${e.i}: [${e.n}] ${e.m}`)
      .join('\n');
    const a = document.createElement('a');
    a.href = 'data:text/plain;charset=utf-8,' + encodeURIComponent(text);
    a.download = 'log.txt';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  };

  const runCommand = async () => {
    const cmd = command.trim();
    if (cmd === '' || consoleBusy) {
      return;
    }
    setHistory((h) =>
      [cmd, ...h.filter((c) => c !== cmd)].slice(0, CONSOLE_HISTORY)
    );
    setHistoryIndex(-1);
    setCommand('');

    let output: string;
    try {
      const result = await sendConsoleCommand(cmd);
      if (typeof result?.output !== 'string') {
        throw new Error('Invalid response from device');
      }
      output = cleanOutput(result.output);
      if (result.truncated) {
        output += '\n... output truncated';
      }
    } catch (e) {
      output = (e as Error).message;
    }

    setBlocks((b) => [
      ...b,
      {
        id: b.length + 1,
        afterLogId: pausedAt ?? entries[entries.length - 1]?.i ?? 0,
        command: cmd,
        output
      }
    ]);
  };

  const onCommandKeyDown = (e: KeyboardEvent) => {
    if (e.key === 'Enter') {
      e.preventDefault();
      void runCommand();
    } else if (e.key === 'ArrowUp' && historyIndex < history.length - 1) {
      e.preventDefault();
      setHistoryIndex(historyIndex + 1);
      setCommand(history[historyIndex + 1] ?? '');
    } else if (e.key === 'ArrowDown' && historyIndex >= 0) {
      e.preventDefault();
      setHistoryIndex(historyIndex - 1);
      setCommand(historyIndex > 0 ? (history[historyIndex - 1] ?? '') : '');
    }
  };

  return (
    <Section className="log-page">
      <div className="log-toolbar">
        <Listbox
          label={LL.LOG_LEVEL()}
          options={LEVEL_OPTIONS}
          value={String(settings.level)}
          disabled={!me.admin}
          onChange={(v) => void changeSetting({ level: Number(v) })}
        />
        {settings.psram && (
          <Listbox
            label={LL.BUFFER_SIZE()}
            options={BUFFER_OPTIONS}
            value={String(settings.max_messages)}
            disabled={!me.admin}
            onChange={(v) => void changeSetting({ max_messages: Number(v) })}
          />
        )}
        <SearchField
          label={LL.FILTER_LINES()}
          value={query}
          onInput={(e) => setQuery(e.currentTarget.value)}
        />
        <span className="grow" />
        <ToggleGroup>
          <ToggleButton
            on={compact}
            icon="density_small"
            onClick={() => void changeSetting({ compact: !compact })}
          >
            {LL.COMPACT()}
          </ToggleButton>
          <ToggleButton
            on={autoScroll}
            icon="vertical_align_bottom"
            onClick={() => setAutoScroll(!autoScroll)}
          >
            {LL.AUTO_SCROLL()}
          </ToggleButton>
          <ToggleButton
            on={pausedAt !== undefined}
            icon={pausedAt !== undefined ? 'play_arrow' : 'pause'}
            onClick={togglePause}
          >
            {pausedAt !== undefined ? LL.RESUME() : LL.PAUSE()}
          </ToggleButton>
        </ToggleGroup>
        <Button icon="download" size="sm" onClick={onDownload}>
          {LL.DOWNLOAD()}
        </Button>
      </div>

      <div
        ref={consoleRef}
        className={cx('console', compact && 'compact')}
        role="log"
        aria-label={LL.SYSTEM_LOG()}
      >
        <ConsoleLines blocks={blocksAfter.get(0)} />
        {shown.map((e) => (
          <div key={e.i}>
            <LogLine entry={e} compact={compact} />
            <ConsoleLines blocks={blocksAfter.get(e.i)} />
          </div>
        ))}
        {held > 0 && <div className="ln muted">{LL.LOG_PAUSED({ num: held })}</div>}
      </div>

      {showConsole && (
        <div className="cmdline">
          <span className="mono lb">#</span>
          <input
            className="input mono grow"
            aria-label={LL.COMMAND(0)}
            placeholder={LL.COMMAND(0)}
            value={command}
            maxLength={200}
            spellCheck={false}
            autoComplete="off"
            disabled={consoleBusy}
            onInput={(e) => {
              setCommand(e.currentTarget.value);
              setHistoryIndex(-1);
            }}
            onKeyDown={onCommandKeyDown}
          />
          <IconButton
            icon="play_arrow"
            label={LL.RUN()}
            disabled={consoleBusy || command.trim() === ''}
            onClick={() => void runCommand()}
          />
        </div>
      )}

      <p className="cap log-foot">
        {LL.SHOWING()} {shown.length}
        {query && ` / ${entries.length}`}
      </p>
    </Section>
  );
};

export default SystemLog;
