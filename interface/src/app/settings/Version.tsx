import { memo, useContext, useEffect, useRef, useState } from 'react';
import { Link } from 'react-router';

import CancelIcon from '@mui/icons-material/Cancel';
import CloseIcon from '@mui/icons-material/Close';
import DescriptionIcon from '@mui/icons-material/Description';
import DownloadIcon from '@mui/icons-material/GetApp';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import RestartAltIcon from '@mui/icons-material/RestartAlt';
import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import SwapHorizIcon from '@mui/icons-material/SwapHoriz';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Box,
  Button,
  Chip,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  IconButton,
  Table,
  TableBody,
  TableCell,
  TableRow,
  Typography
} from '@mui/material';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';
import { REQUEST_FAILED } from 'api/endpoints';

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
import { isNewerVersion, onlineRecoveryRelease } from 'utils/versions';

// Constants moved outside component to avoid recreation
const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
const STABLE_RELNOTES_URL =
  'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';
const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';
const DEV_RELNOTES_URL =
  'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';
const MAX_VERSION_POLLS = 5; // approx 15 seconds of waiting

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
  recovery_version?: string; // only on boards with a factory partition, empty if not installed
  recovery_official?: boolean; // only when a recovery app is installed
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
              startIcon={<DescriptionIcon />}
              variant="outlined"
              component="a"
              href={relNotesUrl}
              target="_blank"
              color="primary"
            >
              Changelog
            </Button>
          )}
          <Button
            startIcon={<CloseIcon />}
            variant="outlined"
            onClick={onClose}
            color="secondary"
          >
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
          {upgradeImportantMessageType === 3 && LL.UPGRADE_IMPORTANT_MESSAGES_3()}
          <Typography sx={{ mt: 2 }}>
            <Link to="/settings/downloadUpload" style={{ color: 'lightblue' }}>
              {LL.DOWNLOAD_SYSTEM_BACKUP()}
            </Link>
          </Typography>
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
    const isRecovery = partition === 'boot';

    return (
      <Dialog sx={dialogStyle} open={openInstallPartitionDialog} onClose={onClose}>
        <DialogTitle>{isRecovery ? LL.RECOVERY_APP() : LL.INSTALL()}</DialogTitle>
        <DialogContent dividers>
          <Typography sx={{ mb: 2 }}>
            {isRecovery
              ? 'This will boot in recovery mode'
              : LL.INSTALL_VERSION(LL.INSTALL(), version)}
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
            startIcon={
              isRecovery ? <RestartAltIcon /> : <WarningIcon color="warning" />
            }
            variant="outlined"
            onClick={() => onInstall(partition)}
            color={isRecovery ? 'success' : 'primary'}
          >
            {isRecovery ? LL.RESTART() : LL.INSTALL()}
          </Button>
        </DialogActions>
      </Dialog>
    );
  }
);

// Helper function moved outside component
const getFlashSize = (data: VersionData): string => {
  if (data.flash_chip_size >= 32768) return '32MB';
  if (data.flash_chip_size >= 16384) return '16MB';
  return '4MB';
};

const getPlatform = (data: VersionData): string => {
  return `${data.esp_platform}-${getFlashSize(data)}${data.psram ? '+' : ''}`;
};

const Version = () => {
  const { LL, locale } = useI18nContext();
  const { me, versions, refreshVersions } = useContext(AuthenticatedContext);

  const [restarting, setRestarting] = useState<boolean>(false);
  const [installingRecovery, setInstallingRecovery] = useState<boolean>(false);
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
  const upgradeAvailable = versions?.current?.upgradeable ?? false;
  const internetLive = Boolean(versions?.stable || versions?.dev);
  const versionPolls = useRef(0);

  useEffect(() => {
    if (internetLive || versionPolls.current >= MAX_VERSION_POLLS) {
      return;
    }
    const interval = setInterval(() => {
      versionPolls.current += 1;
      if (versionPolls.current >= MAX_VERSION_POLLS) {
        clearInterval(interval);
      }
      void refreshVersions();
    }, 3000);
    return () => clearInterval(interval);
  }, [internetLive, refreshVersions]);

  // a 400 means the device refused to boot from the partition, as the image in it doesn't pass verification
  const { send: sendSetPartition } = useRequest(
    (partition: string) => callAction({ action: 'setPartition', param: partition }),
    { immediate: false }
  ).onError((error) => {
    const message = String(error.error?.message || 'An error occurred');
    if (message !== REQUEST_FAILED) {
      toast.error(message);
    } else if (error.args[0] === 'boot') {
      toast.error(LL.RECOVERY_START_FAILED());
    } else {
      toast.error(LL.PARTITION_START_FAILED());
    }
  });

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  // custom: the user agreed to install a build that isn't an official EMS-ESP build
  const { send: sendUploadURL } = useRequest(
    (url: string, custom = false) =>
      callAction({ action: 'uploadURL', param: url, custom }),
    { immediate: false }
  );
  const [installURL, setInstallURL] = useState<string>();
  const installCustom = () =>
    installURL ? sendUploadURL(installURL, true) : Promise.resolve();

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
    await sendAPI({ device: 'system', cmd: 'format', id: 0 })
      .then(() => {
        setRestarting(true);
        setConfirmFactoryReset(false);
      })
      .catch((error: Error) => {
        toast.error(error.message);
      });
  };

  const handleFactoryResetClose = () => setConfirmFactoryReset(false);
  const handleFactoryResetClick = () => setConfirmFactoryReset(true);
  const handleRestartClose = () => setConfirmRestart(false);
  const handleRestartClick = () => setConfirmRestart(true);

  const installFirmwareURL = async (url: string) => {
    setInstallURL(url);
    const started = await sendUploadURL(url)
      .then(() => true)
      .catch((error: Error) => {
        toast.error(error.message);
        return false;
      });
    if (started) {
      await doRestart();
    }
  };

  const hasRecoverySlot =
    data?.recovery_version !== undefined && getFlashSize(data) !== '4MB';

  const onlineRecovery = (() => {
    const release = onlineRecoveryRelease(versions);
    if (!release || !platform) return undefined;
    const version = release.info.recovery;
    const filename = `EMS-ESP-Recovery-${version.replaceAll('.', '_')}-${platform}.bin`;
    return {
      version,
      url: release.dev
        ? `${DEV_URL}${filename}`
        : `${STABLE_URL}v${release.info.version}/${filename}`
    };
  })();

  // downloads the recovery app from GitHub into the factory partition, EMS-ESP doesn't restart
  const installRecovery = async () => {
    const release = onlineRecovery;
    if (!release) {
      toast.error(LL.RECOVERY_NOT_FOUND());
      return;
    }

    setInstallURL(release.url);
    await sendUploadURL(release.url)
      .then(() => setInstallingRecovery(true))
      .catch((error: Error) => {
        toast.error(error.message);
      });
  };

  const installPartitionFirmware = async (partition: string) => {
    const switched = await sendSetPartition(partition)
      .then(() => true)
      .catch(() => false); // onError shows the message
    if (switched) {
      setRestarting(true);
    }
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

  // the third column, nothing when offline as we don't know if there are updates
  // install is for something that isn't installed yet, so there's nothing to update
  const updateCell = (
    updateAvailable: boolean,
    onUpdate: () => void,
    install = false
  ) => {
    if (!internetLive || (install && !updateAvailable)) return null;

    if (!updateAvailable) {
      return (
        <Typography variant="body2" color="text.secondary">
          {LL.NO_UPDATES_AVAILABLE()}
        </Typography>
      );
    }

    if (install) {
      return me.admin ? (
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="success"
          size="small"
          onClick={onUpdate}
        >
          {LL.INSTALL()}
        </Button>
      ) : null;
    }

    if (!me.admin) {
      return (
        <Typography variant="body2" color="success">
          {LL.UPDATE_AVAILABLE_BUTTON()}
        </Typography>
      );
    }

    return (
      <Button variant="outlined" color="success" size="small" onClick={onUpdate}>
        {LL.UPDATE_AVAILABLE_BUTTON()}
        <Box
          component="span"
          aria-hidden
          sx={{
            display: 'inline-block',
            width: 8,
            height: 8,
            ml: 1.5,
            borderRadius: '50%',
            backgroundColor: '#ffeb3b',
            boxShadow: '0 0 6px rgba(255, 235, 59, 0.8)'
          }}
        />
      </Button>
    );
  };

  if (restarting) {
    return <SystemMonitor onInstallCustom={installCustom} />;
  }

  if (installingRecovery) {
    return (
      <SystemMonitor
        onInstallCustom={installCustom}
        onDone={(success) => {
          setInstallingRecovery(false);
          if (success) {
            toast.success(LL.RECOVERY_INSTALLED());
          }
          void loadData();
          void refreshVersions();
        }}
      />
    );
  }

  if (!data) {
    return (
      <SectionContent>
        <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />
      </SectionContent>
    );
  }

  const isDevBuild = versions?.current
    ? usingDevVersion
    : data.emsesp_version.includes('dev');
  const isDebugBuild = data.build_flags?.includes('DEBUG') ?? false;
  const recoveryUpdateAvailable =
    !!onlineRecovery &&
    (!data.recovery_version ||
      isNewerVersion(onlineRecovery.version, data.recovery_version));
  const switchVersion = usingDevVersion ? latestVersion : latestDevVersion;

  return (
    <SectionContent>
      <Box sx={{ p: 2, border: '1px solid #565656', borderRadius: 2 }}>
        <Typography sx={{ mb: 1 }} variant="h6" color="primary">
          {LL.INSTALLED_VERSIONS()}
        </Typography>

        <Table
          size="small"
          sx={{
            width: 'auto',
            '& .MuiTableCell-root': {
              borderBottom: 'none',
              py: 0.75,
              pl: 0,
              pr: 3,
              verticalAlign: 'middle'
            }
          }}
        >
          <TableBody>
            <TableRow>
              <TableCell sx={{ whiteSpace: 'nowrap' }}>
                <Typography color="secondary">{LL.EMSESP_APP()}</Typography>
              </TableCell>
              <TableCell>
                <Box
                  sx={{
                    display: 'flex',
                    alignItems: 'center',
                    flexWrap: 'wrap',
                    gap: 1
                  }}
                >
                  <Typography component="span">{data.emsesp_version}</Typography>
                  <IconButton
                    size="small"
                    onClick={() => setPartitionVersionInfo(data.partition)}
                    aria-label={LL.FIRMWARE_VERSION_INFO()}
                    sx={{ p: 0.5, ml: -0.5 }}
                  >
                    <InfoOutlinedIcon color="primary" sx={{ fontSize: 18 }} />
                  </IconButton>
                  {data.official_build !== undefined && (
                    <Chip
                      size="small"
                      variant="outlined"
                      color={data.official_build ? 'success' : 'default'}
                      label={
                        data.official_build ? LL.OFFICIAL_BUILD() : LL.CUSTOM_BUILD()
                      }
                    />
                  )}
                  {isDebugBuild && (
                    <Chip
                      size="small"
                      variant="outlined"
                      color="error"
                      label="DEBUG"
                    />
                  )}
                  <Chip
                    size="small"
                    variant="outlined"
                    color={isDevBuild ? 'warning' : 'primary'}
                    label={isDevBuild ? 'dev' : 'stable'}
                  />
                </Box>
              </TableCell>
              <TableCell>
                {updateCell(upgradeAvailable, () =>
                  showFirmwareDialog(usingDevVersion)
                )}
              </TableCell>
            </TableRow>

            {hasRecoverySlot && (
              <TableRow>
                <TableCell sx={{ whiteSpace: 'nowrap' }}>
                  <Typography color="secondary">{LL.RECOVERY_APP()}</Typography>
                </TableCell>
                <TableCell>
                  <Box
                    sx={{
                      display: 'flex',
                      alignItems: 'center',
                      flexWrap: 'wrap',
                      gap: 1
                    }}
                  >
                    <Typography component="span">
                      {data.recovery_version || LL.RECOVERY_NOT_INSTALLED()}
                    </Typography>
                    {data.recovery_version &&
                      data.recovery_official !== undefined && (
                        <Chip
                          size="small"
                          variant="outlined"
                          color={data.recovery_official ? 'success' : 'default'}
                          label={
                            data.recovery_official
                              ? LL.OFFICIAL_BUILD()
                              : LL.CUSTOM_BUILD()
                          }
                        />
                      )}
                  </Box>
                </TableCell>
                <TableCell>
                  {updateCell(
                    recoveryUpdateAvailable,
                    () => void installRecovery(),
                    !data.recovery_version
                  )}
                </TableCell>
              </TableRow>
            )}
          </TableBody>
        </Table>

        {me.admin && internetLive && switchVersion && (
          <Box sx={{ mt: 2 }}>
            <Button
              startIcon={<SwapHorizIcon />}
              variant="outlined"
              color="warning"
              onClick={() => showFirmwareDialog(!usingDevVersion)}
            >
              {usingDevVersion
                ? LL.SWITCH_TO_STABLE(switchVersion.version)
                : LL.SWITCH_TO_DEV(switchVersion.version)}
            </Button>
          </Box>
        )}

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

        {me.admin && (
          <>
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
            <SingleUpload
              text={LL.UPLOAD_FIRMWARE_TEXT()}
              extensions={['.bin', '.md5']}
              doRestart={doRestart}
              onRecoveryInstalled={() => {
                void loadData();
                void refreshVersions();
              }}
            />
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
                startIcon={<RestartAltIcon />}
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
              flexWrap: 'wrap',
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
            <Box sx={{ display: 'flex', gap: 1, ml: 'auto' }}>
              {data.recovery_version && (
                <Button
                  startIcon={<RestartAltIcon />}
                  variant="outlined"
                  onClick={() => {
                    const version = data.recovery_version;
                    if (version) {
                      showPartitionDialog(version, 'boot', '');
                    }
                  }}
                >
                  {LL.RESTART_INTO_RECOVERY()}
                </Button>
              )}
              <Button
                startIcon={<RestartAltIcon />}
                variant="outlined"
                onClick={handleRestartClick}
                color="success"
              >
                {LL.RESTART()}
              </Button>
            </Box>
          </Box>
        </>
      )}
    </SectionContent>
  );
};

export default memo(Version);
