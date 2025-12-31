import {
  memo,
  useCallback,
  useContext,
  useEffect,
  useMemo,
  useRef,
  useState
} from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import CloseIcon from '@mui/icons-material/Close';
import CheckIcon from '@mui/icons-material/Done';
import DownloadIcon from '@mui/icons-material/GetApp';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
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
  Link,
  Table,
  TableBody,
  TableCell,
  TableRow,
  Typography
} from '@mui/material';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';
import { getDevVersion, getStableVersion } from 'api/system';

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
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { TranslationFunctions } from 'i18n/i18n-types';
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
}

interface UpgradeCheckData {
  emsesp_version: string;
  dev_upgradeable: boolean;
  stable_upgradeable: boolean;
}

interface VersionInfo {
  name: string;
  published_at?: string;
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
    latestVersion?: VersionInfo;
    latestDevVersion?: VersionInfo;
    partitionVersion?: VersionInfo | undefined;
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
                      : version?.name
                    : version?.name}
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
              {version?.published_at && (
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
                    {prettyDateTime(locale, new Date(version.published_at))}
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
    downloadOnly,
    platform,
    LL,
    onClose,
    onInstall
  }: {
    openInstallDialog: boolean;
    fetchDevVersion: boolean;
    latestVersion?: VersionInfo;
    latestDevVersion?: VersionInfo;
    downloadOnly: boolean;
    platform: string;
    LL: TranslationFunctions;
    onClose: () => void;
    onInstall: (url: string) => void;
  }) => {
    const binURL = useMemo(() => {
      if (!latestVersion || !latestDevVersion) return '';

      const version = fetchDevVersion ? latestDevVersion : latestVersion;
      const filename = `EMS-ESP-${version.name.replaceAll('.', '_')}-${platform}.bin`;

      return fetchDevVersion
        ? `${DEV_URL}${filename}`
        : `${STABLE_URL}v${version.name}/${filename}`;
    }, [fetchDevVersion, latestVersion, latestDevVersion, platform]);

    return (
      <Dialog sx={dialogStyle} open={openInstallDialog} onClose={onClose}>
        <DialogTitle>
          {`${LL.INSTALL()} ${fetchDevVersion ? LL.DEVELOPMENT() : LL.STABLE()} Firmware`}
        </DialogTitle>
        <DialogContent dividers>
          <Typography mb={2}>
            {LL.INSTALL_VERSION(
              downloadOnly ? LL.DOWNLOAD(1) : LL.INSTALL(),
              fetchDevVersion ? latestDevVersion?.name : latestVersion?.name
            )}
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
            <Link underline="none" target="_blank" href={binURL} color="primary">
              {LL.DOWNLOAD(0)}
            </Link>
          </Button>
          {!downloadOnly && (
            <Button
              startIcon={<WarningIcon color="warning" />}
              variant="outlined"
              onClick={() => onInstall(binURL)}
              color="primary"
            >
              {LL.INSTALL()}
            </Button>
          )}
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
          <Typography mb={2}>{LL.INSTALL_VERSION(LL.INSTALL(), version)}</Typography>
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
  const { me } = useContext(AuthenticatedContext);

  // State management
  const [restarting, setRestarting] = useState<boolean>(false);
  const [openInstallDialog, setOpenInstallDialog] = useState<boolean>(false);

  const [partitionVersion, setPartitionVersion] = useState<VersionInfo | undefined>(
    undefined
  );
  const [partition, setPartition] = useState<string>('');
  const [openInstallPartitionDialog, setOpenInstallPartitionDialog] =
    useState<boolean>(false);

  const [usingDevVersion, setUsingDevVersion] = useState<boolean>(false);
  const [fetchDevVersion, setFetchDevVersion] = useState<boolean>(false);
  const [devUpgradeAvailable, setDevUpgradeAvailable] = useState<boolean>(false);
  const [stableUpgradeAvailable, setStableUpgradeAvailable] =
    useState<boolean>(false);
  const [internetLive, setInternetLive] = useState<boolean>(false);
  const [downloadOnly, setDownloadOnly] = useState<boolean>(false);
  const [showVersionInfo, setShowVersionInfo] = useState<number>(0); // 1 = stable, 2 = dev, 3 = partition
  const [firmwareSize, setFirmwareSize] = useState<number>(0);

  const { send: sendCheckUpgrade } = useRequest(
    (versions: string) => callAction({ action: 'checkUpgrade', param: versions }),
    { immediate: false }
  ).onSuccess((event) => {
    const data = event.data as UpgradeCheckData;
    setDevUpgradeAvailable(data.dev_upgradeable);
    setStableUpgradeAvailable(data.stable_upgradeable);
  });

  const { send: sendSetPartition } = useRequest(
    (partition: string) => callAction({ action: 'setPartition', param: partition }),
    { immediate: false }
  ).onError((error) => {
    toast.error(String(error.error?.message || 'An error occurred'));
  });

  const {
    data,
    send: loadData,
    error
  } = useRequest(SystemApi.readSystemStatus).onSuccess((event) => {
    const systemData = event.data as VersionData;
    if (systemData.arduino_version.startsWith('Tasmota')) {
      setDownloadOnly(true);
    }
    setUsingDevVersion(systemData.emsesp_version.includes('dev'));
  });

  const { send: sendUploadURL } = useRequest(
    (url: string) => callAction({ action: 'uploadURL', param: url }),
    { immediate: false }
  );

  const { data: latestVersion } = useRequest(getStableVersion);
  const { data: latestDevVersion } = useRequest(getDevVersion);

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  // Memoized values
  const platform = useMemo(() => (data ? getPlatform(data) : ''), [data]);

  // Memoize filtered partitions to avoid recomputing on every render
  const otherPartitions = useMemo(
    () => data?.partitions.filter((p) => p.partition !== data.partition) ?? [],
    [data]
  );

  const setPartitionVersionInfo = useCallback(
    (partition: string) => {
      setShowVersionInfo(3);

      // search for the partition in the data.partitions array
      const partitionData = data?.partitions.find((p) => p.partition === partition);
      if (partitionData) {
        setPartitionVersion({
          name: partitionData.version,
          published_at: partitionData.install_date ?? ''
        });
        setPartition(partitionData.partition);
        setFirmwareSize(partitionData.size);
      }
    },
    [data]
  );

  const doRestart = useCallback(async () => {
    await sendAPI({ device: 'system', cmd: 'restart', id: 0 }).catch(
      (error: Error) => {
        toast.error(error.message);
      }
    );
    setRestarting(true);
  }, [sendAPI]);

  const installFirmwareURL = useCallback(
    async (url: string) => {
      await sendUploadURL(url).catch((error: Error) => {
        toast.error(error.message);
      });
      await doRestart();
    },
    [sendUploadURL, doRestart]
  );

  const installPartitionFirmware = useCallback(
    async (partition: string) => {
      await sendSetPartition(partition).catch((error: Error) => {
        toast.error(error.message);
      });
      setRestarting(true);
    },
    [sendSetPartition]
  );

  const showPartitionDialog = useCallback(
    (version: string, partition: string, install_date: string) => {
      setOpenInstallPartitionDialog(true);
      setPartitionVersion({ name: version, published_at: install_date });
      setPartition(partition);
    },
    []
  );

  const showFirmwareDialog = useCallback((useDevVersion: boolean) => {
    setFetchDevVersion(useDevVersion);
    setOpenInstallDialog(true);
  }, []);

  const closeInstallDialog = useCallback(() => {
    setOpenInstallDialog(false);
  }, []);

  const closeInstallPartitionDialog = useCallback(() => {
    setOpenInstallPartitionDialog(false);
  }, []);

  const handleVersionInfoClose = useCallback(() => {
    setShowVersionInfo(0);
    setPartitionVersion(undefined);
    setPartition('');
  }, []);

  // check upgrades - only once when both versions are available
  const upgradeCheckedRef = useRef(false);
  useEffect(() => {
    if (latestVersion && latestDevVersion && !upgradeCheckedRef.current) {
      upgradeCheckedRef.current = true;
      const versions = `${latestDevVersion.name},${latestVersion.name}`;
      sendCheckUpgrade(versions)
        .catch((error: Error) => {
          toast.error(`Failed to check for upgrades: ${error.message}`);
        })
        .finally(() => {
          setInternetLive(true);
        });
    }
  }, [latestVersion, latestDevVersion, sendCheckUpgrade]);

  useLayoutTitle('EMS-ESP Firmware');

  // Memoized button rendering logic
  const showButtons = useCallback(
    (showingDev: boolean) => {
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

      return (
        <Button
          sx={{ ml: 1 }}
          variant="outlined"
          color={choice === LL.UPDATE_AVAILABLE() ? 'success' : 'warning'}
          size="small"
          onClick={() => showFirmwareDialog(showingDev)}
        >
          {choice}
        </Button>
      );
    },
    [
      usingDevVersion,
      devUpgradeAvailable,
      stableUpgradeAvailable,
      me.admin,
      LL,
      showFirmwareDialog
    ]
  );

  const content = useMemo(() => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />;
    }

    return (
      <>
        <Box p={2} border="1px solid grey" borderRadius={2}>
          <Typography mb={1} variant="h6" color="primary">
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
              <Typography mt={4} mb={1} variant="h6" color="primary">
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
                      <Typography color="secondary">
                        {LL.STORED_VERSIONS()}
                      </Typography>
                    </Grid>
                    <Grid size={{ xs: 8, md: 10 }}>
                      {otherPartitions.map((partition) => (
                        <Typography key={partition.partition} mb={1}>
                          {partition.version}
                          <IconButton
                            onClick={() =>
                              setPartitionVersionInfo(partition.partition)
                            }
                            aria-label={LL.FIRMWARE_VERSION_INFO()}
                          >
                            <InfoOutlinedIcon
                              color="primary"
                              sx={{ fontSize: 18 }}
                            />
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
                    {latestVersion?.name}
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
                    {latestDevVersion?.name}
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
            <Typography mt={2} color="warning">
              <WarningIcon color="warning" sx={{ verticalAlign: 'middle', mr: 2 }} />
              {LL.INTERNET_CONNECTION_REQUIRED()}
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
                downloadOnly={downloadOnly}
                platform={platform}
                LL={LL}
                onClose={closeInstallDialog}
                onInstall={installFirmwareURL}
              />
              <InstallPartitionDialog
                openInstallPartitionDialog={openInstallPartitionDialog}
                version={partitionVersion?.name || ''}
                partition={partition}
                LL={LL}
                onClose={closeInstallPartitionDialog}
                onInstall={installPartitionFirmware}
              />
              <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
                {LL.UPLOAD()}
              </Typography>
              <SingleUpload text={LL.UPLOAD_DROP_TEXT()} doRestart={doRestart} />
            </>
          )}
        </Box>
      </>
    );
  }, [
    data,
    error,
    loadData,
    LL,
    platform,
    internetLive,
    latestVersion,
    latestDevVersion,
    showVersionInfo,
    locale,
    openInstallDialog,
    fetchDevVersion,
    downloadOnly,
    me.admin,
    showButtons,
    handleVersionInfoClose,
    closeInstallDialog,
    installFirmwareURL,
    doRestart,
    otherPartitions,
    setPartitionVersionInfo,
    showPartitionDialog,
    partitionVersion,
    partition,
    firmwareSize,
    closeInstallPartitionDialog,
    installPartitionFirmware
  ]);

  return <SectionContent>{restarting ? <SystemMonitor /> : content}</SectionContent>;
};

export default memo(Version);
