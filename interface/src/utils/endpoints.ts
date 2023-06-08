// TODO can be removed!
export const extractErrorMessage = (error: any, defaultMessage: string) => {
  if (error.request) {
    return defaultMessage + ' (' + error.request.status + ': ' + error.request.statusText + ')';
  } else if (error instanceof Error) {
    return defaultMessage + ' (' + error.message + ')';
  }
  return defaultMessage;
};
