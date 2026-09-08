import { memo, useContext, useEffect, useState } from 'react';
import { Link } from 'react-router';

import CancelIcon from '@mui/icons-material/Cancel';
import CloseIcon from '@mui/icons-material/Close';
import CheckIcon from '@mui/icons-material/Done';
import DownloadIcon from '@mui/icons-material/GetApp';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  IconButton,
  Table,
  TableBody,
  TableCell,
  TableRow,
  Typography
} from '@mui/material';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import SystemMonitor from 'app/status/SystemMonitor';
import {
  FormLoader,
  SectionContent,
  SingleUpload,
  useLayoutTitle
} from 'components';
import { toast } from 'components/toast';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { TranslationFunctions } from 'i18n/i18n-types';
import type { VersionInfo } from 'types';
import { prettyDateTime } from 'utils/time';

// Constants moved outside component to avoid recreation
const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
const STABLE_RELNOTES_URL =
  'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';
const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';
const DEV_RELNOTES_URL =
  'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';

// Types for better type safety
interface PartitionData {
  partition: string;
  version: string;
  install_date?: string;
  size: number;
}

interface VersionData {
  emsesp_version: string;
  arduino_version: string;
  esp_platform: string;
  flash_chip_size: number;
  psram: boolean;
  build_flags?: string;
  partition: string;
  partitions: PartitionData[];
  developer_mode: boolean;
  disable_reset: boolean;
}

// Memoized components for better performance
const VersionInfoDialog = memo(
  ({
    showVersionInfo,
    latestVersion,
    latestDevVersion,
    partitionVersion,
    partition,
    currentPartition,
    size,
    locale,
    LL,
    onClose
  }: {
    showVersionInfo: number;
    latestVersion: VersionInfo | undefined;
    latestDevVersion: VersionInfo | undefined;
    partitionVersion: VersionInfo | undefined;
    partition: string;
    currentPartition: string;
    size: number;
    locale: string;
    LL: TranslationFunctions;
    onClose: () => void;
  }) => {
    if (showVersionInfo === 0) return null;

    const isStable = showVersionInfo === 1;
    const isDev = showVersionInfo === 2;
    const isPartition = showVersionInfo === 3;

    const version = isStable
      ? latestVersion
      : isDev
        ? latestDevVersion
        : partitionVersion;
    const relNotesUrl = isStable
      ? STABLE_RELNOTES_URL
      : isDev
        ? DEV_RELNOTES_URL
        : '';

    return (
      <Dialog sx={dialogStyle} open={showVersionInfo !== 0} onClose={onClose}>
        <DialogTitle>{LL.FIRMWARE_VERSION_INFO()}</DialogTitle>
        <DialogContent dividers>
          <Table size="small" sx={{ borderCollapse: 'collapse', minWidth: 0 }}>
            <TableBody>
              <TableRow sx={{ height: 24, borderBottom: 'none' }}>
                <TableCell
                  component="th"
                  scope="row"
                  sx={{
                    color: 'lightblue',
                    borderBottom: 'none',
                    pr: 1,
                    py: 0.5,
                    fontSize: 13
                  }}
                >
                  {LL.VERSION()}
                </TableCell>
                <TableCell sx={{ borderBottom: 'none', py: 0.5, fontSize: 13 }}>
                  {isPartition
                    ? typeof version === 'string'
                      ? version
                      : version?.version
                    : version?.version}
                </TableCell>
              </TableRow>
              <TableRow sx={{ height: 24, borderBottom: 'none' }}>
                <TableCell
                  component="th"
                  scope="row"
                  sx={{
                    color: 'lightblue',
                    borderBottom: 'none',
                    pr: 1,
                    py: 0.5,
                    fontSize: 13,
                    width: 140
                  }}
                >
                  {isPartition ? LL.TYPE(0) : LL.RELEASE_TYPE()}
                </TableCell>
                <TableCell sx={{ borderBottom: 'none', py: 0.5, fontSize: 13 }}>
                  {partition === currentPartition && LL.ACTIVE() + ' '}
                  {isStable
                    ? LL.STABLE()
                    : isDev
                      ? LL.DEVELOPMENT()
                      : 'Partition ' + LL.VERSION()}
                </TableCell>
              </TableRow>
              {isPartition && (
                <TableRow sx={{ height: 24, borderBottom: 'none' }}>
                  <TableCell
                    component="th"
                    scope="row"
                    sx={{
                      color: 'lightblue',
                      borderBottom: 'none',
                      pr: 1,
                      py: 0.5,
                      fontSize: 13
                    }}
                  >
                    Partition
                  </TableCell>
                  <TableCell sx={{ borderBottom: 'none', py: 0.5, fontSize: 13 }}>
                    {partition}
                  </TableCell>
                </TableRow>
              )}
              {isPartition && (
                <TableRow sx={{ height: 24, borderBottom: 'none' }}>
                  <TableCell
                    component="th"
                    scope="row"
                    sx={{
                      color: 'lightblue',
                      borderBottom: 'none',
                      pr: 1,
                      py: 0.5,
                      fontSize: 13
                    }}
                  >
                    Size
                  </TableCell>
                  <TableCell sx={{ borderBottom: 'none', py: 0.5, fontSize: 13 }}>
                    {size} KB
                  </TableCell>
                </TableRow>
              )}
              {version && version.date && (
                <TableRow sx={{ height: 24, borderBottom: 'none' }}>
                  <TableCell
                    component="th"
                    scope="row"
                    sx={{
                      color: 'lightblue',
                      borderBottom: 'none',
                      pr: 1,
                      py: 0.5,
                      fontSize: 13
                    }}
                  >
                    {isPartition ? 'Install Date' : 'Build Date'}
                  </TableCell>
                  <TableCell sx={{ borderBottom: 'none', py: 0.5, fontSize: 13 }}>
                    {prettyDateTime(locale, new Date(version.date))}
                  </TableCell>
                </TableRow>
              )}
            </TableBody>
          </Table>
        </DialogContent>
        <DialogActions>
          {!isPartition && (
            <Button
              variant="outlined"
              component="a"
              href={relNotesUrl}
              target="_blank"
              color="primary"
            >
              Changelog
            </Button>
          )}
          <Button variant="outlined" onClick={onClose} color="secondary">
            {LL.CLOSE()}
          </Button>
        </DialogActions>
      </Dialog>
    );
  }
);

const InstallDialog = memo(
  ({
    openInstallDialog,
    fetchDevVersion,
    latestVersion,
    latestDevVersion,
    upgradeImportantMessageType,
    platform,
    LL,
    onClose,
    onInstall
  }: {
    openInstallDialog: boolean;
    fetchDevVersion: boolean;
    latestVersion: VersionInfo | undefined;
    latestDevVersion: VersionInfo | undefined;
    upgradeImportantMessageType: number;
    platform: string;
    LL: TranslationFunctions;
    onClose: () => void;
    onInstall: (url: string) => void;
  }) => {
    const binURL = (() => {
      if (!latestVersion || !latestDevVersion) return '';
      const version = fetchDevVersion ? latestDevVersion : latestVersion;
      const filename = `EMS-ESP-${version.version.replaceAll('.', '_')}-${platform}.bin`;
      return fetchDevVersion
        ? `${DEV_URL}${filename}`
        : `${STABLE_URL}v${version.version}/${filename}`;
    })();

    return (
      <Dialog sx={dialogStyle} open={openInstallDialog} onClose={onClose}>
        <DialogTitle>
          {`${LL.INSTALL()} ${fetchDevVersion ? LL.DEVELOPMENT() : LL.STABLE()} Firmware`}
        </DialogTitle>
        <DialogContent dividers>
          <Typography sx={{ mb: 2 }}>
            {LL.INSTALL_VERSION(
              LL.INSTALL(),
              fetchDevVersion ? latestDevVersion?.version : latestVersion?.version
            )}
          </Typography>
          {upgradeImportantMessageType === 2 && LL.UPGRADE_IMPORTANT_MESSAGES_2()}
          {upgradeImportantMessageType === 1 && (
            <>
              {LL.UPGRADE_IMPORTANT_MESSAGES_1()}
              <Typography sx={{ mt: 2 }}>
                <Link to="/settings/downloadUpload" style={{ color: 'lightblue' }}>
                  {LL.DOWNLOAD_SYSTEM_BACKUP()}
                </Link>
              </Typography>
            </>
          )}
          <Typography sx={{ mt: 2 }}>
            <Link
              to="https://docs.emsesp.org/FAQ#upgrading-the-firmware"
              target="_blank"
              rel="noreferrer"
              style={{ color: 'lightblue' }}
            >
              {LL.ONLINE_HELP()}
            </Link>
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button
            startIcon={<CancelIcon />}
            variant="outlined"
            onClick={onClose}
            color="secondary"
          >
            {LL.CANCEL()}
          </Button>
          <Button
            startIcon={<DownloadIcon />}
            variant="outlined"
            onClick={onClose}
            color="primary"
          >
            <Link
              to={binURL}
              target="_blank"
              rel="noreferrer"
              style={{ color: 'lightblue', textDecoration: 'none' }}
            >
              {LL.DOWNLOAD(0)}
            </Link>
          </Button>
          <Button
            startIcon={<WarningIcon color="warning" />}
            variant="outlined"
            onClick={() => onInstall(binURL)}
            color="primary"
          >
            {LL.INSTALL()}
          </Button>
        </DialogActions>
      </Dialog>
    );
  }
);

const InstallPartitionDialog = memo(
  ({
    openInstallPartitionDialog,
    version,
    partition,
    LL,
    onClose,
    onInstall
  }: {
    openInstallPartitionDialog: boolean;
    version: string;
    partition: string;
    LL: TranslationFunctions;
    onClose: () => void;
    onInstall: (partition: string) => void;
  }) => {
    return (
      <Dialog sx={dialogStyle} open={openInstallPartitionDialog} onClose={onClose}>
        <DialogTitle>
          {LL.INSTALL()} {LL.STORED_VERSIONS()}
        </DialogTitle>
        <DialogContent dividers>
          <Typography sx={{ mb: 2 }}>
            {LL.INSTALL_VERSION(LL.INSTALL(), version)}
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button
            startIcon={<CancelIcon />}
            variant="outlined"
            onClick={onClose}
            color="secondary"
          >
            {LL.CANCEL()}
          </Button>

          <Button
            startIcon={<WarningIcon color="warning" />}
            variant="outlined"
            onClick={() => onInstall(partition)}
            color="primary"
          >
            {LL.INSTALL()}
          </Button>
        </DialogActions>
      </Dialog>
    );
  }
);

// Helper function moved outside component
const getPlatform = (data: VersionData): string => {
  return `${data.esp_platform}-${data.flash_chip_size >= 16384 ? '16MB' : '4MB'}${data.psram ? '+' : ''}`;
};

const Version = () => {
  const { LL, locale } = useI18nContext();
  const { me, versions, refreshVersions } = useContext(AuthenticatedContext);

  const [restarting, setRestarting] = useState<boolean>(false);
  const [confirmFactoryReset, setConfirmFactoryReset] = useState<boolean>(false);
  const [confirmRestart, setConfirmRestart] = useState<boolean>(false);
  const [openInstallDialog, setOpenInstallDialog] = useState<boolean>(false);

  const [partitionVersion, setPartitionVersion] = useState<VersionInfo | undefined>(
    undefined
  );
  const [partition, setPartition] = useState<string>('');
  const [openInstallPartitionDialog, setOpenInstallPartitionDialog] =
    useState<boolean>(false);

  const [fetchDevVersion, setFetchDevVersion] = useState<boolean>(false);
  const [showVersionInfo, setShowVersionInfo] = useState<number>(0); // 1 = stable, 2 = dev, 3 = partition
  const [firmwareSize, setFirmwareSize] = useState<number>(0);

  const latestVersion: VersionInfo | undefined = versions?.stable
    ? { version: versions.stable.version, date: versions.stable.date }
    : undefined;
  const latestDevVersion: VersionInfo | undefined = versions?.dev
    ? { version: versions.dev.version, date: versions.dev.date }
    : undefined;
  const usingDevVersion = versions?.current?.type === 'dev';
  const stableUpgradeAvailable = versions?.stable?.upgradeable ?? false;
  const devUpgradeAvailable = versions?.dev?.upgradeable ?? false;
  const internetLive = Boolean(versions?.stable || versions?.dev);

  useEffect(() => {
    if (internetLive) {
      return;
    }
    const interval = setInterval(() => {
      void refreshVersions();
    }, 3000);
    return () => clearInterval(interval);
  }, [internetLive, refreshVersions]);

  const { send: sendSetPartition } = useRequest(
    (partition: string) => callAction({ action: 'setPartition', param: partition }),
    { immediate: false }
  ).onError((error) => {
    toast.error(String(error.error?.message || 'An error occurred'));
  });

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  const { send: sendUploadURL } = useRequest(
    (url: string) => callAction({ action: 'uploadURL', param: url }),
    { immediate: false }
  );

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  const [upgradeImportantMessageType, setUpgradeImportantMessageType] =
    useState<number>(0);

  const { send: checkUpgradeImportantMessages } = useRequest(
    (version: string) =>
      callAction({ action: 'upgradeImportantMessages', param: version }),
    {
      immediate: false
    }
  )
    .onSuccess((event) => {
      const upgradeImportantMessageType_n = (
        event.data as { upgradeImportantMessageType: number }
      ).upgradeImportantMessageType;
      setUpgradeImportantMessageType(upgradeImportantMessageType_n);
    })
    .onError((error) => {
      toast.error(String(error.error?.message || 'An error occurred'));
    });

  const platform = data ? getPlatform(data) : '';

  const otherPartitions =
    data?.partitions.filter((p) => p.partition !== data.partition) ?? [];

  const setPartitionVersionInfo = (partition: string) => {
    setShowVersionInfo(3);
    const partitionData = data?.partitions.find((p) => p.partition === partition);
    if (partitionData) {
      setPartitionVersion({
        version: partitionData.version,
        date: partitionData.install_date ?? ''
      });
      setPartition(partitionData.partition);
      setFirmwareSize(partitionData.size);
    }
  };

  const doRestart = async () => {
    setConfirmRestart(false);
    await sendAPI({ device: 'system', cmd: 'restart', id: 0 }).catch(
      (error: Error) => {
        toast.error(error.message);
      }
    );
    setRestarting(true);
  };

  const doFormat = async () => {
    await sendAPI({ device: 'system', cmd: 'format', id: 0 }).then(() => {
      setRestarting(true);
      setConfirmFactoryReset(false);
    });
  };

  const handleFactoryResetClose = () => setConfirmFactoryReset(false);
  const handleFactoryResetClick = () => setConfirmFactoryReset(true);
  const handleRestartClose = () => setConfirmRestart(false);
  const handleRestartClick = () => setConfirmRestart(true);

  const installFirmwareURL = async (url: string) => {
    await sendUploadURL(url).catch((error: Error) => {
      toast.error(error.message);
    });
    await doRestart();
  };

  const installPartitionFirmware = async (partition: string) => {
    await sendSetPartition(partition).catch((error: Error) => {
      toast.error(error.message);
    });
    setRestarting(true);
  };

  const showPartitionDialog = (
    version: string,
    partition: string,
    install_date: string
  ) => {
    setOpenInstallPartitionDialog(true);
    setPartitionVersion({ version: version, date: install_date });
    setPartition(partition);
  };

  const showFirmwareDialog = (useDevVersion: boolean) => {
    setFetchDevVersion(useDevVersion);
    const targetVersion = useDevVersion
      ? latestDevVersion?.version
      : latestVersion?.version;
    if (targetVersion) {
      void checkUpgradeImportantMessages(targetVersion);
    }
    setOpenInstallDialog(true);
  };

  const closeInstallDialog = () => setOpenInstallDialog(false);
  const closeInstallPartitionDialog = () => setOpenInstallPartitionDialog(false);

  const handleVersionInfoClose = () => {
    setShowVersionInfo(0);
    setPartitionVersion(undefined);
    setPartition('');
  };

  useLayoutTitle('EMS-ESP Firmware');

  const showButtons = (showingDev: boolean) => {
    const choice = showingDev
      ? !usingDevVersion
        ? LL.SWITCH_RELEASE_TYPE(LL.DEVELOPMENT())
        : devUpgradeAvailable
          ? LL.UPDATE_AVAILABLE()
          : undefined
      : usingDevVersion
        ? LL.SWITCH_RELEASE_TYPE(LL.STABLE())
        : stableUpgradeAvailable
          ? LL.UPDATE_AVAILABLE()
          : undefined;

    if (!choice) {
      return (
        <>
          <CheckIcon
            color="success"
            sx={{ verticalAlign: 'middle', ml: 0.5, mr: 0.5 }}
          />
          <span style={{ color: '#66bb6a', fontSize: '0.8em' }}>
            {LL.LATEST_VERSION(usingDevVersion ? LL.DEVELOPMENT() : LL.STABLE())}
          </span>
          <Button
            sx={{ ml: 1 }}
            variant="outlined"
            size="small"
            onClick={() => showFirmwareDialog(showingDev)}
          >
            {LL.REINSTALL()}
          </Button>
        </>
      );
    }

    if (!me.admin) return null;

    const isUpdateAvailable = choice === LL.UPDATE_AVAILABLE();

    return (
      <Button
        sx={{ ml: 1 }}
        variant="outlined"
        color={isUpdateAvailable ? 'success' : 'warning'}
        size="small"
        onClick={() => showFirmwareDialog(showingDev)}
      >
        {choice}
        {isUpdateAvailable && (
          <Box
            component="span"
            aria-label="update available"
            sx={{
              display: 'inline-block',
              width: 8,
              height: 8,
              ml: 1,
              verticalAlign: 'middle',
              borderRadius: '50%',
              backgroundColor: '#ffeb3b',
              boxShadow: '0 0 6px rgba(255, 235, 59, 0.8)'
            }}
          />
        )}
      </Button>
    );
  };

  if (restarting) {
    return <SystemMonitor />;
  }

  if (!data) {
    return (
      <SectionContent>
        <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />
      </SectionContent>
    );
  }

  return (
    <SectionContent>
      <Box sx={{ p: 2, border: '1px solid #565656', borderRadius: 2 }}>
        <Typography sx={{ mb: 1 }} variant="h6" color="primary">
          {LL.THIS_VERSION()}
        </Typography>

        <Grid
          container
          direction="row"
          sx={{
            justifyContent: 'flex-start',
            alignItems: 'baseline'
          }}
        >
          <Grid size={{ xs: 4, md: 2 }}>
            <Typography color="secondary">{LL.VERSION()}</Typography>
          </Grid>
          <Grid size={{ xs: 8, md: 10 }}>
            <Typography>
              {data.emsesp_version}
              {data.build_flags && (
                <Typography variant="caption">
                  &nbsp; &#40;{data.build_flags}&#41;
                </Typography>
              )}
              <IconButton
                onClick={() => setPartitionVersionInfo(data.partition)}
                aria-label={LL.FIRMWARE_VERSION_INFO()}
              >
                <InfoOutlinedIcon color="primary" sx={{ fontSize: 18 }} />
              </IconButton>
            </Typography>
          </Grid>

          <Grid size={{ xs: 4, md: 2 }}>
            <Typography color="secondary">{LL.PLATFORM()}</Typography>
          </Grid>
          <Grid size={{ xs: 8, md: 10 }}>
            <Typography>
              {platform}
              <Typography variant="caption">
                &nbsp; &#40;
                {data.psram ? (
                  <CheckIcon
                    color="success"
                    sx={{
                      fontSize: '1.5em',
                      verticalAlign: 'middle'
                    }}
                  />
                ) : (
                  <CloseIcon
                    color="error"
                    sx={{
                      fontSize: '1.5em',
                      verticalAlign: 'middle'
                    }}
                  />
                )}
                PSRAM&#41;
              </Typography>
            </Typography>
          </Grid>
        </Grid>

        {internetLive ? (
          <>
            <Typography sx={{ mt: 4, mb: 1 }} variant="h6" color="primary">
              {LL.AVAILABLE_VERSION()}
            </Typography>

            <Grid
              container
              direction="row"
              rowSpacing={1}
              sx={{
                justifyContent: 'flex-start',
                alignItems: 'baseline'
              }}
            >
              {otherPartitions.length > 0 && data.developer_mode && (
                <>
                  <Grid size={{ xs: 4, md: 2 }}>
                    <Typography color="secondary">{LL.STORED_VERSIONS()}</Typography>
                  </Grid>
                  <Grid size={{ xs: 8, md: 10 }}>
                    {otherPartitions.map((partition) => (
                      <Typography key={partition.partition} sx={{ mb: 1 }}>
                        {partition.version}
                        <IconButton
                          onClick={() =>
                            setPartitionVersionInfo(partition.partition)
                          }
                          aria-label={LL.FIRMWARE_VERSION_INFO()}
                        >
                          <InfoOutlinedIcon color="primary" sx={{ fontSize: 18 }} />
                        </IconButton>
                        <Button
                          sx={{ ml: 0 }}
                          variant="outlined"
                          size="small"
                          onClick={() =>
                            showPartitionDialog(
                              partition.version,
                              partition.partition,
                              partition.install_date ?? ''
                            )
                          }
                        >
                          {LL.INSTALL()}
                        </Button>
                      </Typography>
                    ))}
                  </Grid>
                </>
              )}
              <Grid size={{ xs: 4, md: 2 }}>
                <Typography color="secondary">{LL.STABLE()}</Typography>
              </Grid>
              <Grid size={{ xs: 8, md: 10 }}>
                <Typography>
                  {latestVersion?.version}
                  <IconButton
                    onClick={() => setShowVersionInfo(1)}
                    aria-label={LL.FIRMWARE_VERSION_INFO()}
                  >
                    <InfoOutlinedIcon color="primary" sx={{ fontSize: 18 }} />
                  </IconButton>
                  {showButtons(false)}
                </Typography>
              </Grid>

              <Grid size={{ xs: 4, md: 2 }}>
                <Typography color="secondary">{LL.DEVELOPMENT()}</Typography>
              </Grid>
              <Grid size={{ xs: 8, md: 10 }}>
                <Typography>
                  {latestDevVersion?.version}
                  <IconButton
                    onClick={() => setShowVersionInfo(2)}
                    aria-label={LL.FIRMWARE_VERSION_INFO()}
                  >
                    <InfoOutlinedIcon color="primary" sx={{ fontSize: 18 }} />
                  </IconButton>
                  {showButtons(true)}
                </Typography>
              </Grid>
            </Grid>
          </>
        ) : (
          <Typography sx={{ mt: 2 }} color="warning">
            {LL.INTERNET_CONNECTION_CHECK()}&hellip;
          </Typography>
        )}
        {me.admin && (
          <>
            <VersionInfoDialog
              showVersionInfo={showVersionInfo}
              latestVersion={latestVersion}
              latestDevVersion={latestDevVersion}
              partitionVersion={partitionVersion}
              locale={locale}
              partition={partition}
              currentPartition={data?.partition ?? ''}
              size={firmwareSize}
              LL={LL}
              onClose={handleVersionInfoClose}
            />
            <InstallDialog
              openInstallDialog={openInstallDialog}
              fetchDevVersion={fetchDevVersion}
              latestVersion={latestVersion}
              latestDevVersion={latestDevVersion}
              upgradeImportantMessageType={upgradeImportantMessageType}
              platform={platform}
              LL={LL}
              onClose={closeInstallDialog}
              onInstall={installFirmwareURL}
            />
            <InstallPartitionDialog
              openInstallPartitionDialog={openInstallPartitionDialog}
              version={partitionVersion?.version || ''}
              partition={partition}
              LL={LL}
              onClose={closeInstallPartitionDialog}
              onInstall={installPartitionFirmware}
            />
            <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
              {LL.UPLOAD()}
            </Typography>
            <SingleUpload text={LL.UPLOAD_FIRMWARE_TEXT()} doRestart={doRestart} />
          </>
        )}
      </Box>

      {me.admin && (
        <>
          <Dialog
            sx={dialogStyle}
            open={confirmFactoryReset}
            onClose={handleFactoryResetClose}
          >
            <DialogTitle>{LL.FACTORY_RESET()}</DialogTitle>
            <DialogContent dividers>{LL.SYSTEM_FACTORY_TEXT_DIALOG()}</DialogContent>
            <DialogActions>
              <Button
                startIcon={<CancelIcon />}
                variant="outlined"
                onClick={handleFactoryResetClose}
                color="secondary"
              >
                {LL.CANCEL()}
              </Button>
              <Button
                startIcon={<SettingsBackupRestoreIcon />}
                variant="outlined"
                onClick={doFormat}
                color="error"
              >
                {LL.FACTORY_RESET()}
              </Button>
            </DialogActions>
          </Dialog>

          <Dialog
            sx={dialogStyle}
            open={confirmRestart}
            onClose={handleRestartClose}
          >
            <DialogTitle>{LL.RESTART()}</DialogTitle>
            <DialogContent dividers>{LL.RESTART_CONFIRM()}</DialogContent>
            <DialogActions>
              <Button
                startIcon={<CancelIcon />}
                variant="outlined"
                onClick={handleRestartClose}
                color="secondary"
              >
                {LL.CANCEL()}
              </Button>
              <Button
                startIcon={<PowerSettingsNewIcon />}
                variant="outlined"
                onClick={doRestart}
                color="success"
              >
                {LL.RESTART()}
              </Button>
            </DialogActions>
          </Dialog>

          <Box
            sx={{
              mt: 2,
              display: 'flex',
              justifyContent: 'space-between',
              alignItems: 'center',
              flexWrap: 'nowrap',
              whiteSpace: 'nowrap',
              gap: 1
            }}
          >
            {!data.disable_reset && (
              <Button
                startIcon={<SettingsBackupRestoreIcon />}
                variant="outlined"
                onClick={handleFactoryResetClick}
                color="error"
              >
                {LL.FACTORY_RESET()}
              </Button>
            )}
            <Button
              startIcon={<PowerSettingsNewIcon />}
              variant="outlined"
              onClick={handleRestartClick}
              color="success"
            >
              {LL.RESTART()}
            </Button>
          </Box>
        </>
      )}
    </SectionContent>
  );
};

export default memo(Version);
