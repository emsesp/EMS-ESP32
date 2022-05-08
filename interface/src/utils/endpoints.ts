import { AxiosError } from 'axios';

export const extractErrorMessage = (error: unknown, defaultMessage: string) => {
  if (error instanceof AxiosError) {
    return defaultMessage + ' (' + error.request.statusText + ')';
  } else if (error instanceof Error) {
    return defaultMessage + ' (' + error.message + ')';
  }
  return defaultMessage;
};
