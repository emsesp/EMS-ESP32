import { AxiosError } from "axios";

export const extractErrorMessage = (error: AxiosError, defaultMessage: string) => (
  (error.response && error.response.data ? error.response.data.message : error.message) || defaultMessage
);
