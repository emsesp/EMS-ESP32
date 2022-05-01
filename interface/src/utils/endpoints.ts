import { AxiosError } from 'axios';

export const extractErrorMessage = (error: unknown, defaultMessage: string) => {
  if (error instanceof AxiosError) {
    return error.response && error.response.data && error?.response?.data?.message;
  } else if (error instanceof Error) {
    return error.message;
  }
  return defaultMessage;
};
