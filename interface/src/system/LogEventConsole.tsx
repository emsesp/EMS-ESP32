import { FC } from 'react';

import { LogEvent, LogLevel } from './types';
import { Theme, makeStyles, Box } from '@material-ui/core';
import { useWindowSize } from '../components';

interface LogEventConsoleProps {
  events: LogEvent[];
  compact: boolean;
  level: number;
}

interface Offsets {
  topOffset: () => number;
  leftOffset: () => number;
}

const topOffset = () =>
  document.getElementById('log-window')?.getBoundingClientRect().bottom || 0;

const leftOffset = () =>
  document.getElementById('log-window')?.getBoundingClientRect().left || 0;

const useStyles = makeStyles((theme: Theme) => ({
  console: {
    padding: theme.spacing(1),
    position: 'absolute',
    left: (offsets: Offsets) => offsets.leftOffset(),
    right: 24,
    top: (offsets: Offsets) => offsets.topOffset(),
    bottom: 24,
    backgroundColor: 'black',
    overflow: 'auto'
  },
  entry: {
    color: '#bbbbbb',
    fontFamily: 'monospace',
    fontSize: '14px',
    letterSpacing: 'normal',
    whiteSpace: 'nowrap'
  },
  debug: {
    color: '#00FFFF'
  },
  trace: {
    color: '#00FFFF'
  },
  info: {
    color: '#ffff00'
  },
  notice: {
    color: '#ffff00'
  },
  error: {
    color: '#ff0000'
  },
  warning: {
    color: '#ff0000'
  },
  default: {
    color: '#ffffff'
  }
}));

const LogEventConsole: FC<LogEventConsoleProps> = (props) => {
  useWindowSize();
  const classes = useStyles({ topOffset, leftOffset });
  const { events, compact } = props;

  const styleLevel = (level: LogLevel) => {
    switch (level) {
      case LogLevel.DEBUG:
        return classes.debug;
      case LogLevel.TRACE:
        return classes.trace;
      case LogLevel.INFO:
        return classes.info;
      case LogLevel.NOTICE:
        return classes.notice;
      case LogLevel.WARNING:
        return classes.warning;
      case LogLevel.ERROR:
        return classes.error;
      default:
        return classes.default;
    }
  };

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

  return (
    <Box id="log-window" className={classes.console}>
      {events.map((e) => (
        <div className={classes.entry} key={e.i}>
          <span>{e.t}</span>
          {compact && <span>{paddedLevelLabel(e.l, compact)} </span>}
          {!compact && (
            <span className={styleLevel(e.l)}>
              {paddedLevelLabel(e.l, compact)}{' '}
            </span>
          )}
          <span>{paddedIDLabel(e.i, compact)} </span>
          <span>{paddedNameLabel(e.n, compact)} </span>
          <span>{e.m}</span>
        </div>
      ))}
    </Box>
  );
};

export default LogEventConsole;
