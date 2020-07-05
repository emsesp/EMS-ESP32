import React from 'react';
import Sockette from 'sockette';
import throttle from 'lodash/throttle';
import { withSnackbar, WithSnackbarProps } from 'notistack';

import { addAccessTokenParameter } from '../authentication';
import { extractEventValue } from '.';

export interface WebSocketControllerProps<D> extends WithSnackbarProps {
  handleValueChange: (name: keyof D) => (event: React.ChangeEvent<HTMLInputElement>) => void;

  setData: (data: D, callback?: () => void) => void;
  saveData: () => void;
  saveDataAndClear(): () => void;

  connected: boolean;
  data?: D;
}

interface WebSocketControllerState<D> {
  ws: Sockette;
  connected: boolean;
  clientId?: string;
  data?: D;
}

enum WebSocketMessageType {
  ID = "id",
  PAYLOAD = "payload"
}

interface WebSocketIdMessage {
  type: typeof WebSocketMessageType.ID;
  id: string;
}

interface WebSocketPayloadMessage<D> {
  type: typeof WebSocketMessageType.PAYLOAD;
  origin_id: string;
  payload: D;
}

export type WebSocketMessage<D> = WebSocketIdMessage | WebSocketPayloadMessage<D>;

export function webSocketController<D, P extends WebSocketControllerProps<D>>(wsUrl: string, wsThrottle: number, WebSocketController: React.ComponentType<P & WebSocketControllerProps<D>>) {
  return withSnackbar(
    class extends React.Component<Omit<P, keyof WebSocketControllerProps<D>> & WithSnackbarProps, WebSocketControllerState<D>> {
      constructor(props: Omit<P, keyof WebSocketControllerProps<D>> & WithSnackbarProps) {
        super(props);
        this.state = {
          ws: new Sockette(addAccessTokenParameter(wsUrl), {
            onmessage: this.onMessage,
            onopen: this.onOpen,
            onclose: this.onClose,
          }),
          connected: false
        }
      }

      componentWillUnmount() {
        this.state.ws.close();
      }

      onMessage = (event: MessageEvent) => {
        const rawData = event.data;
        if (typeof rawData === 'string' || rawData instanceof String) {
          this.handleMessage(JSON.parse(rawData as string) as WebSocketMessage<D>);
        }
      }

      handleMessage = (message: WebSocketMessage<D>) => {
        switch (message.type) {
          case WebSocketMessageType.ID:
            this.setState({ clientId: message.id });
            break;
          case WebSocketMessageType.PAYLOAD:
            const { clientId, data } = this.state;
            if (clientId && (!data || clientId !== message.origin_id)) {
              this.setState(
                { data: message.payload }
              );
            }
            break;
        }
      }

      onOpen = () => {
        this.setState({ connected: true });
      }

      onClose = () => {
        this.setState({ connected: false, clientId: undefined, data: undefined });
      }

      setData = (data: D, callback?: () => void) => {
        this.setState({ data }, callback);
      }

      saveData = throttle(() => {
        const { ws, connected, data } = this.state;
        if (connected) {
          ws.json(data);
        }
      }, wsThrottle);

      saveDataAndClear = throttle(() => {
        const { ws, connected, data } = this.state;
        if (connected) {
          this.setState({
            data: undefined
          }, () => ws.json(data));
        }
      }, wsThrottle);

      handleValueChange = (name: keyof D) => (event: React.ChangeEvent<HTMLInputElement>) => {
        const data = { ...this.state.data!, [name]: extractEventValue(event) };
        this.setState({ data });
      }

      render() {
        return <WebSocketController
          {...this.props as P}
          handleValueChange={this.handleValueChange}
          setData={this.setData}
          saveData={this.saveData}
          saveDataAndClear={this.saveDataAndClear}
          connected={this.state.connected}
          data={this.state.data}
        />;
      }

    });
}
