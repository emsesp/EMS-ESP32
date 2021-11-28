import { AxiosPromise } from "axios";

import { APSettings, APStatus } from "../types";
import { AXIOS } from "./endpoints";

export function readAPStatus(): AxiosPromise<APStatus> {
  return AXIOS.get('/apStatus');
}

export function readAPSettings(): AxiosPromise<APSettings> {
  return AXIOS.get('/apSettings');
}

export function updateAPSettings(apSettings: APSettings): AxiosPromise<APSettings> {
  return AXIOS.post('/apSettings', apSettings);
}
