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
    padding: theme.spacing(2),
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
    fontSize: '14px',
    letterSpacing: 'normal',
    whiteSpace: 'nowrap'
  },
  debug: {
    color: '#0000ff'
  },
  info: {
    color: '#00ff00'
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
      case LogLevel.ERR:
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
      case LogLevel.ERR:
        return 'ERR';
      case LogLevel.NOTICE:
        return 'NOTICE';
      default:
        return '?';
    }
  };

  const paddedLevelLabel = (level: LogLevel) => {
    const label = levelLabel(level);
    return label.padStart(7, '\xa0');
  };

  return (
    <Box className={classes.console}>
      {events.map((e) => (
        <div className={classes.entry}>
          <span>{e.time} </span>
          <span className={styleLevel(e.level)}>
            {paddedLevelLabel(e.level)}{' '}
          </span>
          <span>{e.message}</span>
        </div>
      ))}
    </Box>
  );
};

export default LogEventConsole;
