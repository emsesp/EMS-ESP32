import { useState } from 'react';

import {
  Button,
  ConfirmDialog,
  Dialog,
  Icon,
  IconButton,
  KeyValueList,
  Message,
  Section,
  Select,
  Table,
  Tag,
  toast
} from 'components/ui';

import FirmwareUpload from './FirmwareUpload';
import {
  HttpError,
  downloadCoreDump,
  eraseCoreDump,
  factoryReset,
  restart,
  setBootPartition
} from './api';
import type { RecoveryPartition, RecoveryStatus } from './types';

const formatMB = (bytes: number) => `${(bytes / 1048576).toFixed(2)} MB`;

const formatInstallDate = (epoch: number) =>
  epoch > 0 ? new Date(epoch * 1000).toLocaleString() : 'unknown';

const formatUptime = (seconds: number) => {
  const h = Math.floor(seconds / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;
  return `${h}h ${m}m ${s}s`;
};

const ETH_STATE_TEXT: Record<
  Exclude<RecoveryStatus['eth_state'], undefined | 'connected'>,
  string
> = {
  disabled: 'disabled, the clock mode conflicts with PSRAM',
  failed: 'failed to start, no PHY found',
  no_link: 'no cable connected',
  connecting: 'waiting for an IP address'
};

const failedToStart = (p: RecoveryPartition) =>
  p.ota_state === 'aborted' || p.ota_state === 'invalid';

// why the device is running the recovery, if it can be worked out
const recoveryReason = (status: RecoveryStatus, failed: RecoveryPartition[]) => {
  if (status.reason === 'crash') {
    return 'EMS-ESP crashed repeatedly and switched to the recovery.';
  }
  if (status.reason === 'request') {
    return undefined; // started on purpose from EMS-ESP, e.g. a long press of the button
  }
  if (failed.length > 0) {
    return `The firmware in ${failed.map((p) => p.label).join(' and ')} failed to start.`;
  }
  if (status.reset_reason === 'panic' || status.reset_reason === 'watchdog') {
    return 'EMS-ESP crashed while starting.';
  }
  return undefined;
};

const wifiText = (status: RecoveryStatus) => {
  if (!status.wifi_ssid) return 'not configured';
  if (status.wifi_connected) {
    return `${status.wifi_ssid}, ${status.wifi_ip ?? ''} (${status.wifi_rssi ?? 0} dBm)${status.static_ip ? ', static IP' : ''}`;
  }
  if (!status.wifi_started) {
    return status.eth_state === 'connected'
      ? 'not used, Ethernet is connected'
      : 'waiting for Ethernet';
  }
  return `connecting to ${status.wifi_ssid}`;
};

interface DashboardProps {
  status: RecoveryStatus;
  onRestarting: (target: string) => void;
  onSignOut: () => void;
  onChanged: () => void;
  onBusy: (busy: boolean) => void;
}

const Dashboard = ({
  status,
  onRestarting,
  onSignOut,
  onChanged,
  onBusy
}: DashboardProps) => {
  const [confirmBoot, setConfirmBoot] = useState<RecoveryPartition>();
  const [infoPartition, setInfoPartition] = useState<RecoveryPartition>();
  const [confirmFactoryReset, setConfirmFactoryReset] = useState(false);
  const [confirmEraseCoreDump, setConfirmEraseCoreDump] = useState(false);
  const [pickedTarget, setPickedTarget] = useState<string>();
  const [uploading, setUploading] = useState(false);
  const uploadTarget = pickedTarget ?? status.upload_target;

  const setBusy = (busy: boolean) => {
    setUploading(busy);
    onBusy(busy);
  };

  const handleError = (error: unknown) => {
    if (error instanceof HttpError && error.status === 401) {
      toast.warning('Session expired, please sign in again');
      onSignOut();
    } else {
      toast.error((error as Error).message);
    }
  };

  const doBoot = async (partition: RecoveryPartition) => {
    setConfirmBoot(undefined);
    try {
      await setBootPartition(partition.label);
      onRestarting(partition.label);
    } catch (error) {
      if (error instanceof HttpError && error.status === 400) {
        toast.error(`${partition.label} does not contain a bootable firmware`);
      } else {
        handleError(error);
      }
    }
  };

  const doRestart = async (target: string) => {
    try {
      await restart();
      onRestarting(target);
    } catch (error) {
      handleError(error);
    }
  };

  const doDownloadCoreDump = async () => {
    try {
      await downloadCoreDump();
    } catch (error) {
      handleError(error);
    }
  };

  const doEraseCoreDump = async () => {
    setConfirmEraseCoreDump(false);
    try {
      await eraseCoreDump();
      toast.success('Crash dump erased');
      onChanged();
    } catch (error) {
      handleError(error);
    }
  };

  const doFactoryReset = async () => {
    setConfirmFactoryReset(false);
    try {
      await factoryReset();
      onRestarting(status.boot);
    } catch (error) {
      handleError(error);
    }
  };

  const otaPartitions = status.partitions.filter((p) => !p.factory);
  const hasFirmware = otaPartitions.some((p) => p.valid);
  const reason = recoveryReason(
    status,
    otaPartitions.filter((p) => p.valid && failedToStart(p))
  );

  return (
    <>
      <div className="rc-msgs">
        {reason && <Message level="error">{reason}</Message>}
        <Message level={hasFirmware ? 'info' : 'warning'}>
          {hasFirmware
            ? 'This device is running the EMS-ESP Recovery. Start one of the installed firmwares, or upload a new one.'
            : 'No EMS-ESP firmware is installed. Upload a firmware to continue.'}
        </Message>
      </div>

      {status.coredump && (
        <Section icon="bug_report" title="Crash Dump">
          <p className="rc-p muted">
            The last crash left this summary. Copy it, or download the crash dump,
            and add it to a GitHub issue so the crash can be decoded. See{' '}
            <a
              href="https://docs.emsesp.org/FAQ#what-to-do-when-ems-esp-crashes"
              target="_blank"
              rel="noreferrer"
            >
              what to do when EMS-ESP crashes
            </a>
            .
          </p>
          <pre className="summary mono rc-dump">{status.coredump}</pre>
          <div className="rc-actions">
            <Button
              icon="download"
              disabled={uploading}
              onClick={() => void doDownloadCoreDump()}
            >
              Download crash dump
            </Button>
            <Button
              color="error"
              icon="delete"
              disabled={uploading}
              onClick={() => setConfirmEraseCoreDump(true)}
            >
              Erase crash dump
            </Button>
          </div>
        </Section>
      )}

      <Section icon="memory" title="Firmware">
        <Table stack className="rc-tbl">
          <tbody>
            {status.partitions.map((p) => (
              <tr key={p.label} className={p.valid ? undefined : 'dim'}>
                <td>
                  <span className="rc-part">
                    <Icon name={p.factory ? 'build' : 'memory'} className="lb" />
                    {p.factory ? `Recovery (${p.label})` : p.label}
                  </span>
                </td>
                <td data-label="Version">
                  {p.valid &&
                    (p.version ? (
                      <span className="mono">v{p.version}</span>
                    ) : (
                      <span className="muted">unknown</span>
                    ))}
                </td>
                <td data-label="Size" className="num">
                  {p.valid
                    ? `${formatMB(p.image_size)} of ${formatMB(p.size)}`
                    : formatMB(p.size)}
                </td>
                <td className="rc-tags">
                  {p.valid &&
                    p.official !== undefined &&
                    (p.official ? (
                      <Tag kind="ok" icon="verified">
                        official
                      </Tag>
                    ) : (
                      <Tag kind="warn">custom</Tag>
                    ))}
                  {p.label === status.running && <Tag kind="ok">running</Tag>}
                  {p.label === status.boot && p.label !== status.running && (
                    <Tag kind="type">next boot</Tag>
                  )}
                  {p.label === uploadTarget && <Tag>upload target</Tag>}
                  {!p.valid && <Tag kind="warn">empty</Tag>}
                  {p.valid && failedToStart(p) && (
                    <Tag kind="err">failed to start</Tag>
                  )}
                  {p.valid && p.ota_state === 'pending' && (
                    <Tag kind="warn">unconfirmed</Tag>
                  )}
                </td>
                <td className="act">
                  <div className="rc-row-act">
                    {p.valid && (
                      <IconButton
                        icon="info"
                        label="Firmware version info"
                        disabled={uploading}
                        onClick={() => setInfoPartition(p)}
                      />
                    )}
                    {!p.factory && p.valid && (
                      <Button
                        size="sm"
                        icon="play_arrow"
                        disabled={uploading}
                        onClick={() => setConfirmBoot(p)}
                      >
                        Start
                      </Button>
                    )}
                  </div>
                </td>
              </tr>
            ))}
          </tbody>
        </Table>
      </Section>

      <Section icon="upload_file" title="Upload Firmware">
        <p className="rc-p muted">
          After the upload, start the firmware from the list above when you are ready.
          To enable the MD5 check, upload the matching .md5 file first.
        </p>
        <Select
          label="Write to"
          className="rc-target"
          disabled={uploading}
          value={uploadTarget}
          onChange={(e) => setPickedTarget(e.currentTarget.value)}
          options={otaPartitions.map((p) => ({
            value: p.label,
            label:
              p.label +
              (p.valid
                ? ` (replaces ${p.version ? 'v' + p.version : 'installed firmware'})`
                : ' (empty)')
          }))}
        />
        <FirmwareUpload
          target={uploadTarget}
          chip={status.chip}
          onUploaded={onChanged}
          onUnauthorized={onSignOut}
          onBusy={setBusy}
        />
      </Section>

      <Section icon="developer_board" title="System">
        <KeyValueList
          rows={[
            {
              label: 'Recovery version',
              value: <span className="mono">v{status.recovery_version}</span>
            },
            {
              label: 'Chip',
              value: `${status.chip}, ${formatMB(status.flash_size)} flash${status.psram_size ? ', ' + formatMB(status.psram_size) + ' PSRAM' : ''}`
            },
            {
              label: 'Free heap',
              value: `${Math.round(status.free_heap / 1024)} KB`
            },
            { label: 'Uptime', value: formatUptime(status.uptime) },
            {
              label: 'MAC address',
              value: <span className="mono">{status.mac}</span>
            },
            {
              label: 'Access point',
              value: `${status.ap_ssid} on ${status.ap_ip} (${status.ap_clients} connected)`
            },
            status.eth_state && {
              label: 'Ethernet',
              value:
                status.eth_state === 'connected'
                  ? `connected, ${status.eth_ip ?? ''}${status.static_ip ? ' (static IP)' : ''}`
                  : ETH_STATE_TEXT[status.eth_state]
            },
            { label: 'WiFi', value: wifiText(status) },
            { label: 'Hostname', value: status.hostname }
          ]}
        />
        <div className="rc-actions rc-system-actions">
          <Button
            color="warning"
            icon="restart_alt"
            disabled={uploading}
            onClick={() => void doRestart(status.boot)}
          >
            Restart{status.boot !== status.running ? ` (boots ${status.boot})` : ''}
          </Button>
          {!status.disable_reset && (
            <Button
              color="error"
              icon="delete_forever"
              disabled={uploading}
              onClick={() => setConfirmFactoryReset(true)}
            >
              Factory Reset
            </Button>
          )}
        </div>
      </Section>

      <ConfirmDialog
        open={confirmFactoryReset}
        title="Factory reset"
        label="Factory Reset"
        icon="delete_forever"
        color="error"
        onClose={() => setConfirmFactoryReset(false)}
        onConfirm={doFactoryReset}
      >
        Are you sure you want to reset EMS-ESP to its factory defaults?
      </ConfirmDialog>

      <ConfirmDialog
        open={confirmEraseCoreDump}
        title="Erase crash dump"
        label="Erase"
        icon="delete"
        color="error"
        onClose={() => setConfirmEraseCoreDump(false)}
        onConfirm={doEraseCoreDump}
      >
        Are you sure you want to erase the crash dump? Download it first if you want
        to report the crash.
      </ConfirmDialog>

      <ConfirmDialog
        open={!!confirmBoot}
        title="Start firmware"
        label="Start"
        icon="play_arrow"
        onClose={() => setConfirmBoot(undefined)}
        onConfirm={() => confirmBoot && doBoot(confirmBoot)}
      >
        <p>
          Restart and run
          {confirmBoot?.version ? ` EMS-ESP v${confirmBoot.version}` : ''} from{' '}
          {confirmBoot?.label}?
        </p>
        {confirmBoot && failedToStart(confirmBoot) && (
          <Message level="warning">
            This firmware failed to start before. If it fails again the device falls
            back to the other firmware, or to the recovery.
          </Message>
        )}
      </ConfirmDialog>

      <Dialog
        open={!!infoPartition}
        title="Firmware version information"
        onClose={() => setInfoPartition(undefined)}
        actions={
          <Button
            color="neutral"
            icon="close"
            onClick={() => setInfoPartition(undefined)}
          >
            Close
          </Button>
        }
      >
        {infoPartition && (
          <KeyValueList
            rows={[
              {
                label: 'Version',
                value: infoPartition.version ? (
                  <span className="mono">v{infoPartition.version}</span>
                ) : (
                  'unknown'
                )
              },
              {
                label: 'Type',
                value: infoPartition.factory ? 'Recovery App' : 'EMS-ESP Firmware'
              },
              infoPartition.official !== undefined && {
                label: 'Build',
                value: infoPartition.official
                  ? 'Official, signed by EMS-ESP'
                  : 'Custom, not signed by EMS-ESP'
              },
              { label: 'Partition', value: infoPartition.label },
              {
                label: 'Size',
                value: `${formatMB(infoPartition.image_size)} of ${formatMB(infoPartition.size)}`
              },
              {
                label: 'Install Date',
                value: formatInstallDate(infoPartition.install_date)
              }
            ]}
          />
        )}
      </Dialog>
    </>
  );
};

export default Dashboard;
