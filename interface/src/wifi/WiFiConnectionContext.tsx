import React from 'react';
import { WiFiNetwork } from './types';

export interface WiFiConnectionContext {
  selectedNetwork?: WiFiNetwork;
  selectNetwork: (network: WiFiNetwork) => void;
  deselectNetwork: () => void;
}

const WiFiConnectionContextDefaultValue = {} as WiFiConnectionContext
export const WiFiConnectionContext = React.createContext(
  WiFiConnectionContextDefaultValue
);
