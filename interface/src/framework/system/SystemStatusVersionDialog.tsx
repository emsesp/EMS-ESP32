import { Box, Button, Dialog, DialogActions, DialogContent, DialogTitle, Link, Typography } from '@mui/material';
import { useRequest } from 'alova';
import { useCallback, useEffect } from 'react';
import { dialogStyle } from 'CustomTheme';
import * as SystemApi from 'api/system';

import MessageBox from 'components/MessageBox';
import { useI18nContext } from 'i18n/i18n-react';

type SystemStatusVersionDialogProps = {
  open: boolean;
  onClose: () => void;
  version: string;
  platform: string;
};

const SystemStatusVersionDialog = ({ open, onClose, version, platform }: SystemStatusVersionDialogProps) => {
  const { LL } = useI18nContext();
  const { send: getLatestVersion, data: latestVersion } = useRequest(SystemApi.getStableVersion, {
    immediate: false,
    force: true
  });
  const { send: getLatestDevVersion, data: latestDevVersion } = useRequest(SystemApi.getDevVersion, {
    immediate: false,
    force: true
  });

  const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
  const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';

  const STABLE_RELNOTES_URL = 'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';
  const DEV_RELNOTES_URL = 'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';

  const uploadURL = window.location.origin + '/system/upload';

  const connected = latestVersion && latestDevVersion;

  const getVersions = useCallback(async () => {
    await getLatestVersion();
    await getLatestDevVersion();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  useEffect(() => {
    if (open) {
      void getVersions();
    }
  }, [getVersions, open]);

  const getBinURL = (v: string) => 'EMS-ESP-' + v.replaceAll('.', '_') + '-' + platform.replaceAll('-', '_') + '.bin';

  return (
    <Dialog sx={dialogStyle} open={open} onClose={onClose}>
      <DialogTitle>{LL.VERSION_CHECK(1)}</DialogTitle>
      <DialogContent dividers>
        <MessageBox my={0} level="info" message={LL.VERSION_ON() + ' ' + version + ' (' + platform + ')'} />
        {latestVersion && (
          <Box mt={2} mb={2}>
            {LL.THE_LATEST()}&nbsp;<b>{LL.OFFICIAL()}</b>&nbsp;{LL.RELEASE_IS()}&nbsp;<b>{latestVersion}</b>
            &nbsp;(
            <Link target="_blank" href={STABLE_RELNOTES_URL} color="primary">
              {LL.RELEASE_NOTES()}
            </Link>
            )&nbsp;(
            <Link
              target="_blank"
              href={STABLE_URL + 'v' + latestVersion + '/' + getBinURL(latestVersion)}
              color="primary"
            >
              {LL.DOWNLOAD(1)}
            </Link>
            )
          </Box>
        )}
        {latestDevVersion && (
          <Box mt={2} mb={2}>
            {LL.THE_LATEST()}&nbsp;<b>{LL.DEVELOPMENT()}</b>&nbsp;{LL.RELEASE_IS()}&nbsp;
            <b>{latestDevVersion}</b>
            &nbsp;(
            <Link target="_blank" href={DEV_RELNOTES_URL} color="primary">
              {LL.RELEASE_NOTES()}
            </Link>
            )&nbsp;(
            <Link target="_blank" href={DEV_URL + getBinURL(latestDevVersion)} color="primary">
              {LL.DOWNLOAD(1)}
            </Link>
            )
          </Box>
        )}
        {connected && (
          <Box color="warning.main" mt={2}>
            <Typography variant="body2">
              {LL.USE()}&nbsp;
              <Link href={uploadURL} color="primary">
                {LL.UPLOAD()}
              </Link>
              &nbsp;{LL.SYSTEM_APPLY_FIRMWARE()}
            </Typography>
          </Box>
        )}
        {!connected && <MessageBox my={2} level="warning" message="No internet connection" />}
      </DialogContent>
      <DialogActions>
        <Button variant="outlined" onClick={onClose} color="secondary">
          {LL.CLOSE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default SystemStatusVersionDialog;
