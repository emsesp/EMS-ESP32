import { Component } from 'react';

import {
  restController,
  RestControllerProps,
  SectionContent,
  BlockFormControlLabel
} from '../components';

import {
  ValidatorForm,
  SelectValidator
} from 'react-material-ui-form-validator';

import { Grid, Slider, FormLabel, Checkbox, MenuItem } from '@material-ui/core';

import {
  addAccessTokenParameter,
  redirectingAuthorizedFetch
} from '../authentication';

import { ENDPOINT_ROOT, EVENT_SOURCE_ROOT } from '../api';
export const FETCH_LOG_ENDPOINT = ENDPOINT_ROOT + 'fetchLog';
export const LOG_SETTINGS_ENDPOINT = ENDPOINT_ROOT + 'logSettings';
export const LOG_EVENT_EVENT_SOURCE_URL = EVENT_SOURCE_ROOT + 'log';

import LogEventConsole from './LogEventConsole';

import { LogEvent, LogSettings } from './types';

import { Decoder } from '@msgpack/msgpack';
const decoder = new Decoder();

interface LogEventControllerState {
  eventSource?: EventSource;
  events: LogEvent[];
  compact: boolean;
  level: number;
  max_messages: number;
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
      events: [],
      compact: false,
      level: 6,
      max_messages: 25
    };
  }

  componentDidMount() {
    this.fetchValues();
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

  changeCompact = (
    event: React.ChangeEvent<HTMLInputElement>,
    checked: boolean
  ) => {
    this.setState({
      compact: checked
    });
  };

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

  fetchValues = () => {
    redirectingAuthorizedFetch(LOG_SETTINGS_ENDPOINT)
      .then((response) => {
        if (response.status === 200) {
          return response.json();
        }
        throw Error('Unexpected status code: ' + response.status);
      })
      .then((json) => {
        this.setState({ level: json.level, max_messages: json.max_messages });
      })
      .catch((error) => {
        const errorMessage = error.message || 'Unknown error';
        this.props.enqueueSnackbar('Problem fetching: ' + errorMessage, {
          variant: 'error'
        });
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

  changeMaxMessages = (
    event: React.ChangeEvent<{}>,
    value: number | number[]
  ) => {
    this.setState({
      max_messages: value as number
    });
    this.send_data(this.state.level, value as number);
  };

  changeLevel = (event: React.ChangeEvent<HTMLSelectElement>) => {
    this.setState({
      level: parseInt(event.target.value)
    });
    this.send_data(parseInt(event.target.value), this.state.max_messages);
  };

  send_data = (level: number, max_messages: number) => {
    redirectingAuthorizedFetch(LOG_SETTINGS_ENDPOINT, {
      method: 'POST',
      body: JSON.stringify({
        level: level,
        max_messages: max_messages
      }),
      headers: {
        'Content-Type': 'application/json'
      }
    })
      .then((response) => {
        if (response.status !== 200) {
          throw Error('Unexpected response code: ' + response.status);
        }
      })
      .catch((error) => {
        this.props.enqueueSnackbar(
          error.message || 'Problem applying log settings',
          { variant: 'warning' }
        );
      });
  };

  render() {
    const { saveData } = this.props;
    return (
      <SectionContent title="System Log" id="log-window">
        <ValidatorForm onSubmit={saveData}>
          <Grid
            container
            spacing={3}
            direction="row"
            justify="flex-start"
            alignItems="center"
          >
            <Grid item xs={2}>
              <SelectValidator
                name="level"
                label="Log Level"
                value={this.state.level}
                fullWidth
                variant="outlined"
                onChange={this.changeLevel}
                margin="normal"
              >
                <MenuItem value={3}>ERROR</MenuItem>
                <MenuItem value={4}>WARNING</MenuItem>
                <MenuItem value={5}>NOTICE</MenuItem>
                <MenuItem value={6}>INFO</MenuItem>
                <MenuItem value={7}>DEBUG</MenuItem>
                <MenuItem value={8}>ALL</MenuItem>
              </SelectValidator>
            </Grid>
            <Grid item xs={2}>
              <FormLabel>Buffer size</FormLabel>
              <Slider
                value={this.state.max_messages}
                valueLabelDisplay="auto"
                name="max_messages"
                marks={[
                  { value: 25, label: '25' },
                  { value: 50, label: '50' },
                  { value: 75, label: '75' }
                ]}
                step={25}
                min={25}
                max={75}
                onChange={this.changeMaxMessages}
              />
            </Grid>
            <Grid item xs={4}>
              <BlockFormControlLabel
                control={
                  <Checkbox
                    checked={this.state.compact}
                    onChange={this.changeCompact}
                    value="compact"
                  />
                }
                label="Compact Layout"
              />
            </Grid>
          </Grid>
        </ValidatorForm>

        <LogEventConsole
          level={this.state.level}
          compact={this.state.compact}
          events={this.state.events}
        />
      </SectionContent>
    );
  }
}

export default restController(LOG_SETTINGS_ENDPOINT, LogEventController);
