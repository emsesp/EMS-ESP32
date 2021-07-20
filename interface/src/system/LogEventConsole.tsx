import { FC } from 'react';

import { LogEvent, LogLevel } from './types';
import { Theme, makeStyles, Box } from '@material-ui/core';
import { useWindowSize } from '../components';

interface LogEventConsoleProps {
  events: LogEvent[];
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
    fontFamily: 'Courier New, monospace',
    fontSize: '13px',
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
  const { events } = props;

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

  const paddedLevelLabel = (level: LogLevel) => {
    const label = levelLabel(level);
    return label.padStart(8, '\xa0');
  };

  const paddedNameLabel = (name: string) => {
    const label = '[' + name + ']';
    return label.padEnd(12, '\xa0');
  };

  const paddedIDLabel = (id: number) => {
    const label = id + ':';
    return label.padEnd(7, '\xa0');
  };

  return (
    <Box id="log-window" className={classes.console}>
      {events.map((e) => (
        <div className={classes.entry}>
          <span>{e.t}</span>
          <span className={styleLevel(e.l)}>{paddedLevelLabel(e.l)} </span>
          <span>{paddedIDLabel(e.i)} </span>
          <span>{paddedNameLabel(e.n)} </span>
          <span>{e.m}</span>
        </div>
      ))}
    </Box>
  );
};

export default LogEventConsole;
