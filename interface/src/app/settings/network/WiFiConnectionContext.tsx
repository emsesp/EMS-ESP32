import { createContext } from 'react';

import type { WiFiNetwork } from 'types';

export interface WiFiConnectionContextValue {
  selectedNetwork?: WiFiNetwork;
  selectNetwork: (network: WiFiNetwork) => void;
  deselectNetwork: () => void;
}

const WiFiConnectionContextDefaultValue = {} as WiFiConnectionContextValue;
export const WiFiConnectionContext = createContext(
  WiFiConnectionContextDefaultValue
);
