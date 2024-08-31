import { useState } from 'react';

import {
  Box,
  CircularProgress,
  Dialog,
  DialogContent,
  Typography
} from '@mui/material';

import { readHardwareStatus } from 'api/system';

import { dialogStyle } from 'CustomTheme';
import { useAutoRequest } from 'alova/client';
import MessageBox from 'components/MessageBox';
import { useI18nContext } from 'i18n/i18n-react';

const RestartMonitor = () => {
  const [errorMessage, setErrorMessage] = useState<string>();

  const { LL } = useI18nContext();

  let count = 0;

  const { data } = useAutoRequest(readHardwareStatus, {
    pollingTime: 1000,
    force: true,
    initialData: { status: 'Getting ready...' },
    async middleware(_, next) {
      if (count++ >= 1) {
        // skip first request (1 seconds) to allow AsyncWS to send its response
        await next();
      }
    }
  })
    .onSuccess((event) => {
      if (event.data.status === 'ready' || event.data.status === undefined) {
        document.location.href = '/';
      }
    })
    .onError((error, _method) => {
      setErrorMessage(error.message);
    });

  return (
    <Dialog fullWidth={true} sx={dialogStyle} open={true}>
      <DialogContent dividers>
        <Box m={2} py={2} display="flex" alignItems="center" flexDirection="column">
          <Typography
            color="secondary"
            variant="h6"
            fontWeight={400}
            textAlign="center"
          >
            {/* TODO: translate these */}
            {data?.status === 'uploading'
              ? LL.WAIT_FIRMWARE()
              : data?.status === 'restarting'
                ? LL.APPLICATION_RESTARTING()
                : data?.status === 'ready'
                  ? 'Reloading'
                  : 'Preparing'}
          </Typography>
          <Typography mt={2} variant="h6" fontWeight={400} textAlign="center">
            {LL.PLEASE_WAIT()}&hellip;
          </Typography>

          {errorMessage ? (
            <MessageBox my={2} level="error" message={errorMessage} />
          ) : (
            <Box py={2}>
              <CircularProgress size={48} />
            </Box>
          )}
        </Box>
      </DialogContent>
    </Dialog>
  );
};

export default RestartMonitor;
