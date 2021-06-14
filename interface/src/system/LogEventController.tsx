import { Component } from 'react';
import { FormActions, FormButton } from '../components';

import {
  createStyles,
  WithStyles,
  withStyles,
  Typography,
  Theme,
  Paper
} from '@material-ui/core';

import { LogEvent } from './types';
import { EVENT_SOURCE_ROOT } from '../api/Env';
import LogEventConsole from './LogEventConsole';
import { addAccessTokenParameter } from '../authentication';

import SaveIcon from '@material-ui/icons/Save';

const LOG_EVENT_EVENT_SOURCE_URL = EVENT_SOURCE_ROOT + 'log';

interface LogEventControllerState {
  eventSource?: EventSource;
  events: LogEvent[];
}

const styles = (theme: Theme) =>
  createStyles({
    content: {
      padding: theme.spacing(2),
      margin: theme.spacing(3)
    }
  });

type LogEventControllerProps = WithStyles<typeof styles>;

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
    const { classes } = this.props;
    return (
      <Paper id="log-window" className={classes.content}>
        <Typography variant="h6">System Log</Typography>
        <FormActions>
          <FormButton
            startIcon={<SaveIcon />}
            variant="contained"
            color="secondary"
            // onClick={this.requestNetworkScan}
          >
            Save
          </FormButton>
        </FormActions>
        <LogEventConsole events={this.state.events} />
      </Paper>
    );
  }
}

export default withStyles(styles)(LogEventController);
