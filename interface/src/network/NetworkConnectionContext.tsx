import React from 'react';
import { WiFiNetwork } from './types';

export interface NetworkConnectionContextValue {
  selectedNetwork?: WiFiNetwork;
  selectNetwork: (network: WiFiNetwork) => void;
  deselectNetwork: () => void;
}

const NetworkConnectionContextDefaultValue = {} as NetworkConnectionContextValue;
export const NetworkConnectionContext = React.createContext(
  NetworkConnectionContextDefaultValue
);
