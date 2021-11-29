import React from 'react';
import { WiFiNetwork } from '../../types';

export interface WiFiConnectionContextValue {
  selectedNetwork?: WiFiNetwork;
  selectNetwork: (network: WiFiNetwork) => void;
  deselectNetwork: () => void;
}

const WiFiConnectionContextDefaultValue = {} as WiFiConnectionContextValue;
export const WiFiConnectionContext = React.createContext(WiFiConnectionContextDefaultValue);
