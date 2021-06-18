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
  info: {
    color: '#ffff00'
  },
  notice: {
    color: '#ffff00'
  },
  err: {
    color: '#ff0000'
  },
  unknown: {
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
      case LogLevel.INFO:
        return classes.info;
      case LogLevel.NOTICE:
        return classes.notice;
      case LogLevel.WARNING:
      case LogLevel.ERROR:
        return classes.err;
      default:
        return classes.unknown;
    }
  };

  const levelLabel = (level: LogLevel) => {
    switch (level) {
      case LogLevel.DEBUG:
        return 'DEBUG';
      case LogLevel.INFO:
        return 'INFO';
      case LogLevel.ERROR:
        return 'ERROR';
      case LogLevel.NOTICE:
        return 'NOTICE';
      case LogLevel.WARNING:
        return 'WARNING';
      case LogLevel.TRACE:
        return 'TRACE';
      default:
        return '?';
    }
  };

  const paddedLevelLabel = (level: LogLevel) => {
    const label = levelLabel(level);
    return label.padStart(8, '\xa0');
  };

  const paddedNameLabel = (name: string) => {
    const label = '[' + name + ']';
    return label.padStart(8, '\xa0');
  };

  return (
    <Box id="log-window" className={classes.console}>
      {events.map((e) => (
        <div className={classes.entry}>
          <span>{e.t}</span>
          <span className={styleLevel(e.l)}>{paddedLevelLabel(e.l)} </span>
          <span>{paddedNameLabel(e.n)} </span>
          <span>{e.m}</span>
        </div>
      ))}
    </Box>
  );
};

export default LogEventConsole;
