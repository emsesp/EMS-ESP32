import { Component } from 'react';

import {
  restController,
  RestControllerProps,
  RestFormLoader,
  SectionContent
} from '../components';

import { addAccessTokenParameter } from '../authentication';

import { ENDPOINT_ROOT, EVENT_SOURCE_ROOT } from '../api';
export const FETCH_LOG_ENDPOINT = ENDPOINT_ROOT + 'fetchLog';
export const LOG_SETTINGS_ENDPOINT = ENDPOINT_ROOT + 'logSettings';

export const LOG_EVENT_EVENT_SOURCE_URL = EVENT_SOURCE_ROOT + 'log';

import LogEventForm from './LogEventForm';
import LogEventConsole from './LogEventConsole';

import { LogEvent, LogSettings } from './types';

import { Decoder } from '@msgpack/msgpack';
const decoder = new Decoder();

interface LogEventControllerState {
  eventSource?: EventSource;
  events: LogEvent[];
}

type LogEventControllerProps = RestControllerProps<LogSettings>;

class LogEventController extends Component<
  LogEventControllerProps,
  LogEventControllerState
> {
  eventSource?: EventSource;
  reconnectTimeout?: NodeJS.Timeout;

  constructor(props: LogEventControllerProps) {
    super(props);
    this.state = {
      events: []
    };
  }

  componentDidMount() {
    this.props.loadData();
    this.fetchLog();
    this.configureEventSource();
  }

  componentWillUnmount() {
    if (this.eventSource) {
      this.eventSource.close();
    }
    if (this.reconnectTimeout) {
      clearTimeout(this.reconnectTimeout);
    }
  }

  fetchLog = () => {
    fetch(FETCH_LOG_ENDPOINT)
      .then((response) => {
        if (response.status === 200) {
          return response.arrayBuffer();
        } else {
          throw Error('Unexpected status code: ' + response.status);
        }
      })
      .then((arrayBuffer) => {
        const json: any = decoder.decode(arrayBuffer);
        this.setState({ events: json.events });
      })
      .catch((error) => {
        this.setState({ events: [] });
        throw Error('Unexpected error: ' + error);
      });
  };

  configureEventSource = () => {
    this.eventSource = new EventSource(
      addAccessTokenParameter(LOG_EVENT_EVENT_SOURCE_URL)
    );
    this.eventSource.onmessage = this.onMessage;
    this.eventSource.onerror = this.onError;
  };

  onError = () => {
    if (this.eventSource && this.reconnectTimeout) {
      this.eventSource.close();
      this.eventSource = undefined;
      this.reconnectTimeout = setTimeout(this.configureEventSource, 1000);
    }
  };

  onMessage = (event: MessageEvent) => {
    const rawData = event.data;
    if (typeof rawData === 'string' || rawData instanceof String) {
      const event = JSON.parse(rawData as string) as LogEvent;
      this.setState((state) => ({ events: [...state.events, event] }));
    }
  };

  render() {
    return (
      <SectionContent title="System Log" id="log-window">
        <RestFormLoader
          {...this.props}
          render={(formProps) => <LogEventForm {...formProps} />}
        />
        <LogEventConsole events={this.state.events} />
      </SectionContent>
    );
  }
}

export default restController(LOG_SETTINGS_ENDPOINT, LogEventController);
