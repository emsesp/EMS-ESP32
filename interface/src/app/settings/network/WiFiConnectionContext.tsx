import { createContext } from 'react';

import type { FormState } from 'app/settings/FormPage';
import type { NetworkSettingsType, SystemStatus, WiFiNetwork } from 'types';

export interface WiFiConnectionContextValue {
  form: FormState<NetworkSettingsType>;
  status?: SystemStatus | undefined;
  // fills the SSID from a scanned network and goes back to the settings tab
  selectNetwork: (network: WiFiNetwork) => void;
}

export const WiFiConnectionContext = createContext({} as WiFiConnectionContextValue);
