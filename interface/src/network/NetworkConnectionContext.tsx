import React from 'react';
import { WiFiNetwork } from './types';

export interface NetworkConnectionContext {
  selectedNetwork?: WiFiNetwork;
  selectNetwork: (network: WiFiNetwork) => void;
  deselectNetwork: () => void;
}

const NetworkConnectionContextDefaultValue = {} as NetworkConnectionContext
export const NetworkConnectionContext = React.createContext(
  NetworkConnectionContextDefaultValue
);
