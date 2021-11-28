import { FC, useState, useEffect, useCallback } from 'react';

import { Box, styled } from '@mui/material';

import * as SystemApi from '../../api/system';
import { addAccessTokenParameter } from '../../api/authentication';

import { FormLoader, SectionContent } from '../../components';

import { EVENT_SOURCE_ROOT } from '../../api/endpoints';

import { LogSettings, LogEntry, LogEntries, LogLevel } from '../../types';
import { updateValue, useRest, extractErrorMessage } from '../../utils';

export const LOG_EVENTSOURCE_URL = EVENT_SOURCE_ROOT + 'log';

// const useWindowSize() = {
//   const [size, setSize] = useState([0, 0]);
//   useLayoutEffect(() => {
//     function updateSize() {
//       setSize([window.innerWidth, window.innerHeight]);
//     }
//     window.addEventListener('resize', updateSize);
//     updateSize();
//     return () => window.removeEventListener('resize', updateSize);
//   }, []);
//   return size;
// }

const LogEntryLine = styled('div')(({ theme }) => ({
  color: '#bbbbbb',
  fontFamily: 'monospace',
  fontSize: '14px',
  letterSpacing: 'normal',
  whiteSpace: 'nowrap'
}));

// const useStyles = makeStyles((theme: Theme) => ({
//   console: {
//     padding: theme.spacing(1),
//     position: 'absolute',
//     left: (offsets: Offsets) => offsets.leftOffset(),
//     right: 24,
//     top: (offsets: Offsets) => offsets.topOffset(),
//     bottom: 24,
//     backgroundColor: 'black',
//     overflow: 'auto'
//   },
//   entry: {
//     color: '#bbbbbb',
//     fontFamily: 'monospace',
//     fontSize: '14px',
//     letterSpacing: 'normal',
//     whiteSpace: 'nowrap'
//   },
//   debug: {
//     color: '#00FFFF'
//   },
//   trace: {
//     color: '#00FFFF'
//   },
//   info: {
//     color: '#ffff00'
//   },
//   notice: {
//     color: '#ffff00'
//   },
//   error: {
//     color: '#ff0000'
//   },
//   warning: {
//     color: '#ff0000'
//   },
//   default: {
//     color: '#ffffff'
//   }
// }));

interface Offsets {
  topOffset: () => number;
  leftOffset: () => number;
}
const topOffset = () => document.getElementById('log-window')?.getBoundingClientRect().bottom || 0;
const leftOffset = () => document.getElementById('log-window')?.getBoundingClientRect().left || 0;

const SystemLog: FC = () => {
  const { loadData, data, setData, saveData } = useRest<LogSettings>({
    read: SystemApi.readLogSettings,
    update: SystemApi.updateLogSettings
  });

  const [errorMessage, setErrorMessage] = useState<string>();
  const [reconnectTimeout, setReconnectTimeout] = useState<NodeJS.Timeout>();

  const [logEntries, setLogEntries] = useState<LogEntries>({ events: [] });

  const updateFormValue = updateValue(setData);

  const onMessage = useCallback((event: MessageEvent) => {
    const rawData = event.data;
    if (typeof rawData === 'string' || rawData instanceof String) {
      const logentry = JSON.parse(rawData as string) as LogEntry;
      // console.log('got message ' + logentry.m);

      if (logEntries) {
        setLogEntries((old) => ({ events: [...old.events, logentry] }));
        // TODO add index to see last position
        // if (event.i > this.state.last_id) {
        //   this.setState({ last_id: event.i });
        // }
      }
    }
  }, []);

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

  useEffect(() => {
    // console.log('creating es instance');

    const es = new EventSource(addAccessTokenParameter(LOG_EVENTSOURCE_URL));
    es.onmessage = onMessage;
    es.onerror = () => {
      if (reconnectTimeout) {
        es.close();
        // TODO fix reconnect timeout
        // setReconnectTimeout(setTimeout(configureEventSource, 1000));
      }
    };

    return () => {
      // console.log('closing es');
      es.close();
      if (reconnectTimeout) {
        clearTimeout(reconnectTimeout);
      }
    };
  }, [onMessage, reconnectTimeout]);

  const renderLog = () => {
    if (!data) {
      return <FormLoader loadData={loadData} errorMessage={errorMessage} />;
    }

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
      <Box
        id="log-window"
        sx={{
          backgroundColor: 'black',
          overflow: 'auto',
          position: 'absolute',
          right: 24,
          left: 64,
          top: 240,
          bottom: 24,
          ml: 24
          // padding: theme.spacing(1)
          // left: (offsets: Offsets) => offsets.leftOffset(),
          // top: (offsets: Offsets) => offsets.topOffset(),
          // top: (offsets: Offsets) => offsets.topOffset()
        }}
      >
        {logEntries &&
          logEntries.events.map((e) => (
            <LogEntryLine key={e.i}>
              <span>{e.t}</span>
              {data.compact && <span>{paddedLevelLabel(e.l, data.compact)} </span>}
              {!data.compact && (
                // <span className={styleLevel(e.l)}>
                <span>{paddedLevelLabel(e.l, data.compact)}&nbsp;</span>
              )}
              <span>{paddedIDLabel(e.i, data.compact)} </span>
              <span>{paddedNameLabel(e.n, data.compact)} </span>
              <span>{e.m}</span>
            </LogEntryLine>
          ))}
      </Box>
    );
  };

  return (
    <SectionContent title="System Log" titleGutter>
      {renderLog()}
    </SectionContent>
  );
};

export default SystemLog;
