import { useContext, useEffect, useState } from 'react';
import { Link, useBlocker } from 'react-router';

import * as SystemApi from 'api/system';
import { API, callAction } from 'api/app';
import { REQUEST_FAILED } from 'api/endpoints';
import { askPassword, passwordCancelledMessage } from 'api/passwordPrompt';
import { readPasswordConfirmed } from 'api/security';

import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import SystemConfirmDialog from 'app/status/SystemConfirmDialog';
import SystemMonitor from 'app/status/SystemMonitor';
import BlockNavigation from 'components/routing/BlockNavigation';
import {
  Button,
  ButtonLink,
  ConfirmDialog,
  Dialog,
  FileUpload,
  Icon,
  IconButton,
  KeyValueList,
  Loader,
  Message,
  Section,
  StatusDot,
  Table,
  Tag,
  ToggleButton,
  ToggleGroup,
  toast
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { TranslationFunctions } from 'i18n/i18n-types';
import type { SystemStatus } from 'types';
import { prettyDateTime } from 'utils/time';
import {
  CUSTOM_BUILD_URL,
  isNewerVersion,
  onlineRecoveryRelease
} from 'utils/versions';

import './Version.css';

const STABLE_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/';
const STABLE_RELNOTES_URL =
  'https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md';
const DEV_URL = 'https://github.com/emsesp/EMS-ESP32/releases/download/latest/';
const DEV_RELNOTES_URL =
  'https://github.com/emsesp/EMS-ESP32/blob/dev/CHANGELOG_LATEST.md';
const UPGRADE_HELP_URL = 'https://docs.emsesp.org/FAQ#upgrading-the-firmware';
const MAX_VERSION_POLLS = 5; // approx 15 seconds of waiting
// the factory partition holds the recovery app; it is labelled boot in the partition tables
const RECOVERY_PARTITION = 'boot';

type Channel = 'stable' | 'dev';
type Partition = SystemStatus['partitions'][number];

const getFlashSize = (data: SystemStatus): string => {
  if (data.flash_chip_size >= 32768) return '32MB';
  if (data.flash_chip_size >= 16384) return '16MB';
  return '4MB';
};

const getPlatform = (data: SystemStatus): string =>
  `${data.esp_platform}-${getFlashSize(data)}${data.psram ? '+' : ''}`;

const firmwareURL = (version: string, dev: boolean, platform: string) => {
  const filename = `EMS-ESP-${version.replaceAll('.', '_')}-${platform}.bin`;
  return dev ? `${DEV_URL}${filename}` : `${STABLE_URL}v${version}/${filename}`;
};

const upgradeMessage = (type: number, LL: TranslationFunctions) =>
  type === 2
    ? LL.UPGRADE_IMPORTANT_MESSAGES_2()
    : type === 3
      ? LL.UPGRADE_IMPORTANT_MESSAGES_3()
      : undefined;

const uploadPercent = ({ loaded, total }: { loaded: number; total: number }) =>
  total === 0 || loaded === 0
    ? 0
    : loaded <= total
      ? Math.round((loaded * 100) / total)
      : Math.round((total * 100) / loaded);

const Version = () => {
  const { LL, locale } = useI18nContext();
  const { me, versions, refreshVersions } = useContext(AuthenticatedContext);

  const [restarting, setRestarting] = useState(false);
  const [installingRecovery, setInstallingRecovery] = useState(false);
  const [confirm, setConfirm] = useState<'restart' | 'format'>();
  const [channel, setChannel] = useState<Channel>();
  // the install dialog, with the upgrade message type for the target version
  const [install, setInstall] = useState<{ dev: boolean; messages: number }>();
  // restart from another partition, or into the recovery app
  const [confirmRecovery, setConfirmRecovery] = useState(false);
  const [confirmReinstall, setConfirmReinstall] = useState(false);
  const [info, setInfo] = useState<Partition>();
  const [installURL, setInstallURL] = useState<string>();
  const [polls, setPolls] = useState(0);

  const [md5, setMd5] = useState<string>();
  // the file the device stopped because it isn't an official build
  const [customFile, setCustomFile] = useState<File>();
  const [uploadWarning, setUploadWarning] = useState<{ file: File; type: number }>();

  const usingDevVersion = versions?.current?.type === 'dev';
  const upgradeAvailable = versions?.current?.upgradeable ?? false;
  const internetLive = Boolean(versions?.stable || versions?.dev);
  const checking = !internetLive && polls < MAX_VERSION_POLLS;

  useEffect(() => {
    if (!checking) return;
    const timer = setTimeout(() => {
      setPolls((n) => n + 1);
      void refreshVersions();
    }, 3000);
    return () => clearTimeout(timer);
  }, [checking, polls, refreshVersions]);

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  // a 400 means the device refused to boot from the partition, as the image in it doesn't pass verification
  const { send: sendSetPartition, loading: switching } = useRequest(
    (partition: string) => callAction({ action: 'setPartition', param: partition }),
    { immediate: false }
  ).onError((error) => {
    const message = String(error.error?.message || LL.ERROR());
    if (message !== REQUEST_FAILED) {
      toast.error(message);
    } else if (error.args[0] === RECOVERY_PARTITION) {
      toast.error(LL.RECOVERY_START_FAILED());
    } else {
      toast.error(LL.PARTITION_START_FAILED());
    }
  });

  // custom: the user agreed to install a build that isn't an official EMS-ESP build
  const { send: sendUploadURL, loading: installing } = useRequest(
    (url: string, custom = false) =>
      callAction({ action: 'uploadURL', param: url, custom }),
    { immediate: false }
  );
  const installCustom = () =>
    installURL ? sendUploadURL(installURL, true) : Promise.resolve();

  const { send: sendAPI, loading: sending } = useRequest(
    (call: APIcall) => API(call),
    { immediate: false }
  );

  const { send: sendUpgradeMessages } = useRequest(
    (version: string) =>
      callAction({ action: 'upgradeImportantMessages', param: version }),
    { immediate: false }
  );
  // undefined when the check failed
  const upgradeMessages = (version: string) =>
    sendUpgradeMessages(version)
      .then(
        (d) =>
          (d as { upgradeImportantMessageType: number }).upgradeImportantMessageType
      )
      .catch((e: Error) => {
        toast.error(e.message || LL.ERROR());
        return undefined;
      });

  const {
    loading: uploading,
    uploading: uploadProgress,
    send: sendUpload,
    abort: cancelUpload
  } = useRequest(SystemApi.uploadFile, {
    immediate: false,
    // alova keeps the last progress after a finished request; reset so "Install anyway"
    // doesn't reopen the bar at ~100% from the refused upload
    middleware: async (context, next) => {
      context.proxyStates.uploading.v = { loaded: 0, total: 0 };
      return next();
    }
  }).onSuccess(({ data: response }) => {
      let payload: unknown = response;
      if (typeof payload === 'string' && payload.length > 0) {
        try {
          payload = JSON.parse(payload);
        } catch {
          payload = response;
        }
      }
      const result =
        payload && typeof payload === 'object'
          ? (payload as {
              official?: boolean;
              md5?: string;
              recovery?: boolean;
              md5_ok?: boolean;
            })
          : {};
      if (result.official) {
        toast.success(LL.OFFICIAL_BUILD());
      }
      if (result.md5 !== undefined) {
        setMd5(result.md5);
        toast.success(LL.UPLOAD_MD5_RECEIVED());
      } else if ('recovery' in result) {
        // written to the factory partition, EMS-ESP keeps running
        toast.success(LL.RECOVERY_INSTALLED());
        setMd5(undefined);
        void loadData();
        void refreshVersions();
      } else {
        if ('md5_ok' in result) {
          toast.success(LL.UPLOAD_MD5_MATCHED());
        }
        void doRestart();
      }
    }
  );

  const doRestart = async () => {
    setConfirm(undefined);
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
        setConfirm(undefined);
        setRestarting(true);
      })
      .catch((error: Error) => {
        toast.error(error.message);
      });
  };

  const blocker = useBlocker(uploading);

  const upload = async (file: File, custom = false) => {
    setUploadWarning(undefined);
    setCustomFile(undefined);
    // asked before sending, otherwise the whole file is sent first
    const confirmed = await readPasswordConfirmed()
      .then((state) => state.confirmed || askPassword())
      .catch(() => false);
    if (!confirmed) {
      toast.warning(passwordCancelledMessage());
      return;
    }
    await sendUpload(file, custom).catch((error: Error) => {
      if (error.message.includes('The user aborted a request')) {
        toast.warning(LL.UPLOAD_ABORTED());
      } else if (error.message === 'Precondition Failed') {
        // 412, see OtaUpdater
        setCustomFile(file);
      } else {
        toast.warning(LL.FW_UPLOAD_FAILED());
      }
    });
  };

  const startUpload = async (file: File) => {
    const type = await upgradeMessages(file.name);
    if (type === undefined) return;
    if (type === 0) {
      await upload(file);
    } else {
      setUploadWarning({ file, type });
    }
  };

  const showInstall = async (dev: boolean) => {
    setInstall({ dev, messages: 0 });
    const target = dev ? versions?.dev?.version : versions?.stable?.version;
    if (target) {
      const type = await upgradeMessages(target);
      if (type) {
        setInstall((cur) => (cur?.dev === dev ? { dev, messages: type } : cur));
      }
    }
  };

  const installFirmwareURL = async (url: string) => {
    setInstallURL(url);
    const started = await sendUploadURL(url)
      .then(() => true)
      .catch((error: Error) => {
        toast.error(error.message);
        return false;
      });
    if (started) {
      setInstall(undefined);
      await doRestart();
    }
  };

  const restartIntoRecovery = async () => {
    const switched = await sendSetPartition(RECOVERY_PARTITION)
      .then(() => true)
      .catch(() => false); // onError shows the message
    if (switched) {
      setConfirmRecovery(false);
      setRestarting(true);
    }
  };

  const platform = data ? getPlatform(data) : '';

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
    if (!onlineRecovery) {
      toast.error(LL.RECOVERY_NOT_FOUND());
      return;
    }
    setInstallURL(onlineRecovery.url);
    await sendUploadURL(onlineRecovery.url)
      .then(() => setInstallingRecovery(true))
      .catch((error: Error) => {
        toast.error(error.message);
      });
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
    return <Loader error={error?.message} onRetry={() => void loadData()} />;
  }

  const isDevBuild = versions?.current
    ? usingDevVersion
    : data.emsesp_version.includes('dev');
  const isDebugBuild = data.build_flags?.includes('DEBUG') ?? false;
  const running = data.partitions.find((p) => p.partition === data.partition);
  // 4MB boards have no factory partition, so no recovery app
  const hasRecoverySlot =
    data.recovery_version !== undefined && getFlashSize(data) !== '4MB';
  const recoveryUpdateAvailable =
    !!onlineRecovery &&
    (!data.recovery_version ||
      isNewerVersion(onlineRecovery.version, data.recovery_version));

  const channelName = (dev: boolean) => (dev ? LL.DEVELOPMENT() : LL.STABLE());
  const shown: Channel = channel ?? (usingDevVersion ? 'dev' : 'stable');
  const target = shown === 'dev' ? versions?.dev : versions?.stable;
  const isSwitch = (shown === 'dev') !== usingDevVersion;

  const installTarget = install
    ? install.dev
      ? versions?.dev?.version
      : versions?.stable?.version
    : undefined;
  const installBinURL =
    installTarget && install
      ? firmwareURL(installTarget, install.dev, platform)
      : '';

  const infoButton = (p: Partition | undefined) =>
    p && (
      <IconButton
        icon="info"
        label={LL.FIRMWARE_VERSION_INFO()}
        onClick={() => setInfo(p)}
      />
    );

  const updateCard = () => {
    if (checking) {
      return (
        <p className="cap fw-checking">{LL.INTERNET_CONNECTION_CHECK()}&hellip;</p>
      );
    }
    if (!target) {
      return <Message level="info">{LL.FW_UPDATES_OFFLINE()}</Message>;
    }
    const isNew = isSwitch || upgradeAvailable;
    // custom build newer than the official release for this channel: no "latest" /
    // reinstall for the older GitHub build
    const customAhead =
      data.official_build === false &&
      !isNew &&
      isNewerVersion(data.emsesp_version, target.version);
    if (customAhead) {
      return (
        <div className="fw-update">
          <Icon name="check_circle" className="fw-big" />
          <div className="grow">
            <div className="fw-update-title">{LL.USING_CUSTOM_FIRMWARE()}</div>
          </div>
        </div>
      );
    }
    const title = isSwitch
      ? shown === 'dev'
        ? LL.SWITCH_TO_DEV(target.version)
        : LL.SWITCH_TO_STABLE(target.version)
      : upgradeAvailable
        ? LL.FW_VERSION_AVAILABLE({ version: target.version })
        : LL.LATEST_VERSION(channelName(usingDevVersion));
    return (
      <div className="fw-update">
        <Icon
          name={isNew ? 'new_releases' : 'check_circle'}
          className={isNew ? 'fw-big new' : 'fw-big'}
        />
        <div className="grow">
          <div className="fw-update-title">{title}</div>
          <div className="cap">
            {channelName(shown === 'dev')} {target.version}
            {target.date && ` · ${prettyDateTime(locale, new Date(target.date))}`}
            {' · '}
            <a
              href={shown === 'dev' ? DEV_RELNOTES_URL : STABLE_RELNOTES_URL}
              target="_blank"
              rel="noreferrer"
            >
              {LL.FW_RELEASE_NOTES()}
            </a>
          </div>
        </div>
        {me.admin &&
          (isNew ? (
            <Button
              color={isSwitch ? 'warning' : 'primary'}
              icon={isSwitch ? 'swap_horiz' : 'system_update'}
              onClick={() => void showInstall(shown === 'dev')}
            >
              {LL.INSTALL()} <span className="fw-ver">{target.version}</span>
            </Button>
          ) : (
            <Button
              color="primary"
              icon="refresh"
              onClick={() => setConfirmReinstall(true)}
            >
              {LL.REINSTALL()}
            </Button>
          ))}
      </div>
    );
  };

  const updateStatus = (available: boolean) =>
    internetLive &&
    (available ? (
      <Tag kind="warn">
        <StatusDot state="warn" />
        {LL.UPDATE_AVAILABLE()}
      </Tag>
    ) : (
      <span className="cap">{LL.NO_UPDATES_AVAILABLE()}</span>
    ));

  const recoveryActions = () => (
    <div className="fw-act">
      {data.recovery_version && updateStatus(recoveryUpdateAvailable)}
      {me.admin && internetLive && recoveryUpdateAvailable && (
        <Button
          size="sm"
          icon="system_update"
          disabled={installing}
          onClick={() => void installRecovery()}
        >
          {data.recovery_version ? LL.UPDATE() : LL.INSTALL()}
        </Button>
      )}
    </div>
  );

  const uploadMessage = uploadWarning && upgradeMessage(uploadWarning.type, LL);
  const installMessage = install && upgradeMessage(install.messages, LL);

  return (
    <>
      <Section icon="inventory_2" title={LL.INSTALLED_VERSIONS()}>
        <Table stack className="fw-tbl">
          <tbody>
            <tr>
              <td>{LL.EMSESP_APP()}</td>
              <td data-label={LL.VERSION()}>
                <span className="fw-ver-cell">
                  <span className="mono">{data.emsesp_version}</span>
                  {infoButton(running)}
                </span>
              </td>
              <td>
                <div className="fw-tags">
                  {data.official_build !== undefined &&
                    (data.official_build ? (
                      <Tag kind="ok" icon="verified">
                        {LL.OFFICIAL_BUILD()}
                      </Tag>
                    ) : (
                      <Tag kind="warn">{LL.CUSTOM_BUILD()}</Tag>
                    ))}
                  <Tag kind={isDevBuild ? 'type' : 'muted'}>
                    {channelName(isDevBuild)}
                  </Tag>
                  {isDebugBuild && <Tag kind="err">DEBUG</Tag>}
                </div>
              </td>
              <td className="act">
                <div className="fw-act">{updateStatus(upgradeAvailable)}</div>
              </td>
            </tr>

            {hasRecoverySlot && (
              <tr>
                <td>{LL.RECOVERY_APP()}</td>
                <td data-label={LL.VERSION()}>
                  {data.recovery_version ? (
                    <span className="mono">{data.recovery_version}</span>
                  ) : (
                    <span className="muted">{LL.RECOVERY_NOT_INSTALLED()}</span>
                  )}
                </td>
                <td>
                  {data.recovery_version &&
                    data.recovery_official !== undefined &&
                    (data.recovery_official ? (
                      <Tag kind="ok" icon="verified">
                        {LL.OFFICIAL_BUILD()}
                      </Tag>
                    ) : (
                      <Tag kind="warn">{LL.CUSTOM_BUILD()}</Tag>
                    ))}
                </td>
                <td className="act">{recoveryActions()}</td>
              </tr>
            )}
          </tbody>
        </Table>
      </Section>

      <Section
        icon="system_update"
        title={LL.FW_UPDATES()}
        actions={
          internetLive && (
            <ToggleGroup label={LL.RELEASE_TYPE()}>
              <ToggleButton
                on={shown === 'stable'}
                onClick={() => setChannel('stable')}
              >
                {LL.STABLE()}
              </ToggleButton>
              <ToggleButton on={shown === 'dev'} onClick={() => setChannel('dev')}>
                {LL.DEVELOPMENT()}
              </ToggleButton>
            </ToggleGroup>
          )
        }
      >
        {updateCard()}
      </Section>

      {me.admin && (
        <>
          <Section icon="upload_file" title={LL.UPLOAD()}>
            <FileUpload
              text={md5 ? LL.UPLOAD_MD5_RECEIVED() : LL.UPLOAD_FIRMWARE_TEXT()}
              accept={['.bin', '.md5']}
              onUpload={(file) => void startUpload(file)}
              progress={uploading ? uploadPercent(uploadProgress) : undefined}
              onCancel={cancelUpload}
            />
            {customFile && !uploading && (
              <Message
                level="warning"
                className="fw-msg"
                actions={
                  <Button
                    size="sm"
                    color="warning"
                    icon="warning"
                    onClick={() => void upload(customFile, true)}
                  >
                    {LL.UPLOAD_INSTALL_ANYWAY()}
                  </Button>
                }
              >
                {LL.UPLOAD_NOT_OFFICIAL()}.{' '}
                <a href={CUSTOM_BUILD_URL} target="_blank" rel="noreferrer">
                  {LL.UPLOAD_NOT_OFFICIAL_HELP()}
                </a>
              </Message>
            )}
            {md5 && <p className="cap mono fw-md5">MD5: {md5}</p>}
          </Section>

          <Section
            icon="power_settings_new"
            title={data.disable_reset ? LL.RESTART() : LL.FW_RESTART_RESET()}
          >
            <div className="fw-actions">
              <Button
                color="warning"
                icon="power_settings_new"
                onClick={() => setConfirm('restart')}
              >
                {LL.RESTART()}
              </Button>
              {!data.disable_reset && (
                <Button
                  color="error"
                  icon="delete_forever"
                  onClick={() => setConfirm('format')}
                >
                  {LL.FACTORY_RESET()}
                </Button>
              )}
              {hasRecoverySlot && data.recovery_version && (
                <Button
                  color="secondary"
                  icon="build"
                  onClick={() => setConfirmRecovery(true)}
                >
                  {LL.RESTART_INTO_RECOVERY()}
                </Button>
              )}
            </div>
          </Section>
        </>
      )}

      <Dialog
        open={info !== undefined}
        title={LL.FIRMWARE_VERSION_INFO()}
        onClose={() => setInfo(undefined)}
        actions={
          <Button color="neutral" icon="close" onClick={() => setInfo(undefined)}>
            {LL.CLOSE()}
          </Button>
        }
      >
        {info && (
          <KeyValueList
            rows={[
              {
                label: LL.VERSION(),
                value: <span className="mono">{info.version}</span>
              },
              {
                label: LL.PARTITION(),
                value: info.partition,
                extra: info.partition === data.partition && LL.ACTIVE()
              },
              info.partition === data.partition && {
                label: LL.PLATFORM(),
                value: platform
              },
              info.size > 0 && { label: LL.FW_SIZE(), value: `${info.size} KB` },
              !!info.install_date && {
                label: LL.FW_INSTALL_DATE(),
                value: prettyDateTime(locale, new Date(info.install_date))
              }
            ]}
          />
        )}
      </Dialog>

      {me.admin && (
        <>
          <Dialog
            open={install !== undefined}
            title={LL.INSTALL_FIRMWARE({ channel: channelName(!!install?.dev) })}
            onClose={() => setInstall(undefined)}
            size="md"
            actions={
              <>
                <Button
                  color="neutral"
                  icon="cancel"
                  onClick={() => setInstall(undefined)}
                >
                  {LL.CANCEL()}
                </Button>
                {installBinURL && (
                  <ButtonLink href={installBinURL} color="secondary" icon="download">
                    {LL.DOWNLOAD()}
                  </ButtonLink>
                )}
                <Button
                  color="warning"
                  icon="system_update"
                  disabled={!installBinURL || installing || sending}
                  onClick={() => void installFirmwareURL(installBinURL)}
                >
                  {LL.INSTALL()}
                </Button>
              </>
            }
          >
            <p>{LL.INSTALL_VERSION(installTarget)}</p>
            {installMessage && (
              <Message level="warning" className="fw-msg">
                {installMessage}
              </Message>
            )}
            <p>
              <Link to="/settings/downloadUpload">
                {LL.DOWNLOAD_SYSTEM_BACKUP()}
              </Link>
              {' · '}
              <a href={UPGRADE_HELP_URL} target="_blank" rel="noreferrer">
                {LL.ONLINE_HELP()}
              </a>
            </p>
          </Dialog>

          <ConfirmDialog
            open={confirmRecovery}
            title={LL.RESTART_INTO_RECOVERY()}
            label={LL.RESTART()}
            icon="restart_alt"
            color="warning"
            disabled={switching}
            onClose={() => setConfirmRecovery(false)}
            onConfirm={restartIntoRecovery}
          >
            {LL.FW_RECOVERY_TEXT()}
          </ConfirmDialog>

          <ConfirmDialog
            open={confirmReinstall && !!target}
            title={LL.REINSTALL()}
            label={LL.REINSTALL()}
            icon="refresh"
            color="warning"
            disabled={installing || sending}
            onClose={() => setConfirmReinstall(false)}
            onConfirm={async () => {
              if (!target) return;
              setConfirmReinstall(false);
              await installFirmwareURL(
                firmwareURL(target.version, shown === 'dev', platform)
              );
            }}
          >
            {target && LL.REINSTALL_CONFIRM(target.version)}
          </ConfirmDialog>

          <ConfirmDialog
            open={uploadWarning !== undefined}
            title={LL.UPGRADE_IMPORTANT_MESSAGES()}
            label={LL.UPLOAD()}
            icon="upload"
            onClose={() => setUploadWarning(undefined)}
            onConfirm={() => uploadWarning && upload(uploadWarning.file)}
          >
            {uploadMessage && (
              <Message level="warning" className="fw-msg">
                {uploadMessage}
              </Message>
            )}
            <p>
              <a href={UPGRADE_HELP_URL} target="_blank" rel="noreferrer">
                {LL.ONLINE_HELP()}
              </a>
            </p>
          </ConfirmDialog>

          <SystemConfirmDialog
            confirm={confirm}
            disabled={sending}
            onClose={() => setConfirm(undefined)}
            onConfirm={(cmd) => (cmd === 'format' ? doFormat() : doRestart())}
          />
        </>
      )}
      <BlockNavigation blocker={blocker} busy />
    </>
  );
};

export default Version;
