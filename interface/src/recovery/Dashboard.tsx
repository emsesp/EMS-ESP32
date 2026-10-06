import { type ReactNode, useState } from 'react';

import CancelIcon from '@mui/icons-material/Cancel';
import HealthAndSafetyIcon from '@mui/icons-material/HealthAndSafety';
import LogoutIcon from '@mui/icons-material/Logout';
import MemoryIcon from '@mui/icons-material/Memory';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import RestartAltIcon from '@mui/icons-material/RestartAlt';
import {
  Avatar,
  Box,
  Button,
  Chip,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  MenuItem,
  TextField,
  Typography
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import ButtonRow from 'components/ButtonRow';
import MessageBox from 'components/MessageBox';
import SectionContent from 'components/SectionContent';
import { toast } from 'components/toast';

import FirmwareUpload from './FirmwareUpload';
import { HttpError, restart, setBootPartition } from './api';
import type { RecoveryPartition, RecoveryStatus } from './types';

const formatMB = (bytes: number) => `${(bytes / 1048576).toFixed(2)} MB`;

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

const SectionTitle = ({ children }: { children: ReactNode }) => (
  <Typography sx={{ pb: 1 }} variant="h6" color="primary">
    {children}
  </Typography>
);

const InfoRow = ({ label, value }: { label: string; value: ReactNode }) => (
  <Box sx={{ display: 'flex', py: 0.5 }}>
    <Typography sx={{ width: 160, flexShrink: 0 }} color="text.secondary">
      {label}
    </Typography>
    <Typography>{value}</Typography>
  </Box>
);

interface DashboardProps {
  status: RecoveryStatus;
  onRestarting: (target: string) => void;
  onSignOut: () => void;
}

const Dashboard = ({ status, onRestarting, onSignOut }: DashboardProps) => {
  const [confirmBoot, setConfirmBoot] = useState<RecoveryPartition>();
  const [pickedTarget, setPickedTarget] = useState<string>();
  const uploadTarget = pickedTarget ?? status.upload_target;

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

  const otaPartitions = status.partitions.filter((p) => !p.factory);
  const hasFirmware = otaPartitions.some((p) => p.valid);
  const reason = recoveryReason(
    status,
    otaPartitions.filter((p) => p.valid && failedToStart(p))
  );

  return (
    <>
      <SectionContent>
        {reason && <MessageBox sx={{ mb: 2 }} level="error" message={reason} />}
        <MessageBox level={hasFirmware ? 'info' : 'warning'}>
          {hasFirmware
            ? 'This device is running the EMS-ESP Recovery. Start one of the installed firmwares, or upload a new one.'
            : 'No EMS-ESP firmware is installed. Upload a firmware to continue.'}
        </MessageBox>
      </SectionContent>

      <SectionContent>
        <SectionTitle>Firmware</SectionTitle>
        <List dense>
          {status.partitions.map((p, index) => (
            <Box key={p.label}>
              {index > 0 && <Divider component="li" />}
              <ListItem
                secondaryAction={
                  !p.factory &&
                  p.valid && (
                    <Button
                      startIcon={<PlayArrowIcon />}
                      variant="outlined"
                      color="primary"
                      size="small"
                      onClick={() => setConfirmBoot(p)}
                    >
                      Start
                    </Button>
                  )
                }
              >
                <ListItemAvatar>
                  <Avatar
                    sx={{
                      bgcolor: p.factory
                        ? '#5d89f7'
                        : p.valid
                          ? '#2e586a'
                          : '#5f5f5f'
                    }}
                  >
                    {p.factory ? <HealthAndSafetyIcon /> : <MemoryIcon />}
                  </Avatar>
                </ListItemAvatar>
                <ListItemText
                  primary={
                    <Box
                      sx={{
                        display: 'flex',
                        alignItems: 'center',
                        gap: 1,
                        flexWrap: 'wrap'
                      }}
                    >
                      <span>{p.factory ? `Recovery (${p.label})` : p.label}</span>
                      {p.label === status.running && (
                        <Chip size="small" color="success" label="running" />
                      )}
                      {p.label === status.boot && p.label !== status.running && (
                        <Chip size="small" color="secondary" label="next boot" />
                      )}
                      {p.label === uploadTarget && (
                        <Chip
                          size="small"
                          variant="outlined"
                          label="upload target"
                        />
                      )}
                      {!p.valid && (
                        <Chip
                          size="small"
                          color="warning"
                          variant="outlined"
                          label="empty"
                        />
                      )}
                      {p.valid && failedToStart(p) && (
                        <Chip size="small" color="error" label="failed to start" />
                      )}
                      {p.valid && p.ota_state === 'pending' && (
                        <Chip
                          size="small"
                          color="warning"
                          variant="outlined"
                          label="unconfirmed"
                        />
                      )}
                    </Box>
                  }
                  secondary={
                    p.valid
                      ? `${p.version ? 'v' + p.version + ' · ' : ''}${formatMB(p.image_size)} of ${formatMB(p.size)}`
                      : formatMB(p.size)
                  }
                />
              </ListItem>
            </Box>
          ))}
        </List>
      </SectionContent>

      <SectionContent>
        <SectionTitle>Upload Firmware</SectionTitle>
        <Typography sx={{ pb: 2 }} variant="body2" color="text.secondary">
          The firmware is started automatically after the upload. To enable the MD5
          check, upload the matching .md5 file first.
        </Typography>
        <TextField
          select
          size="small"
          sx={{ mb: 2, minWidth: 260 }}
          label="Write to"
          value={uploadTarget}
          onChange={(e) => setPickedTarget(e.target.value)}
        >
          {otaPartitions.map((p) => (
            <MenuItem key={p.label} value={p.label}>
              {p.label}
              {p.valid
                ? ` (replaces ${p.version ? 'v' + p.version : 'installed firmware'})`
                : ' (empty)'}
            </MenuItem>
          ))}
        </TextField>
        <FirmwareUpload
          target={uploadTarget}
          chip={status.chip}
          onUploaded={() => void doRestart(uploadTarget)}
          onUnauthorized={onSignOut}
        />
      </SectionContent>

      <SectionContent>
        <SectionTitle>System</SectionTitle>
        <InfoRow label="Recovery version" value={'v' + status.recovery_version} />
        <InfoRow
          label="Chip"
          value={`${status.chip}, ${formatMB(status.flash_size)} flash${status.psram_size ? ', ' + formatMB(status.psram_size) + ' PSRAM' : ''}`}
        />
        <InfoRow
          label="Free heap"
          value={`${Math.round(status.free_heap / 1024)} KB`}
        />
        <InfoRow label="Uptime" value={formatUptime(status.uptime)} />
        <InfoRow label="MAC address" value={status.mac} />
        <InfoRow
          label="Access point"
          value={`${status.ap_ssid} on ${status.ap_ip} (${status.ap_clients} connected)`}
        />
        {status.eth_state && (
          <InfoRow
            label="Ethernet"
            value={
              status.eth_state === 'connected'
                ? `connected, ${status.eth_ip ?? ''}${status.static_ip ? ' (static IP)' : ''}`
                : ETH_STATE_TEXT[status.eth_state]
            }
          />
        )}
        <InfoRow
          label="WiFi"
          value={
            !status.wifi_ssid
              ? 'not configured'
              : status.wifi_connected
                ? `${status.wifi_ssid}, ${status.wifi_ip ?? ''} (${status.wifi_rssi ?? 0} dBm)${status.static_ip ? ', static IP' : ''}`
                : !status.wifi_started
                  ? status.eth_state === 'connected'
                    ? 'not used, Ethernet is connected'
                    : 'waiting for Ethernet'
                  : `connecting to ${status.wifi_ssid}`
          }
        />
        <InfoRow label="Hostname" value={status.hostname} />

        <ButtonRow>
          <Button
            startIcon={<RestartAltIcon />}
            variant="outlined"
            color="error"
            onClick={() => void doRestart(status.boot)}
          >
            Restart{status.boot !== status.running ? ` (boots ${status.boot})` : ''}
          </Button>
          <Button
            startIcon={<LogoutIcon />}
            variant="outlined"
            color="secondary"
            onClick={onSignOut}
          >
            Sign Out
          </Button>
        </ButtonRow>
      </SectionContent>

      <Dialog
        sx={dialogStyle}
        open={!!confirmBoot}
        onClose={() => setConfirmBoot(undefined)}
      >
        <DialogTitle>Start Firmware</DialogTitle>
        <DialogContent dividers>
          Restart and run
          {confirmBoot?.version ? ` EMS-ESP v${confirmBoot.version}` : ''} from{' '}
          {confirmBoot?.label}?
          {confirmBoot && failedToStart(confirmBoot) && (
            <MessageBox
              sx={{ mt: 2 }}
              level="warning"
              message="This firmware failed to start before. If it fails again the device falls back to the other firmware, or to the recovery."
            />
          )}
        </DialogContent>
        <DialogActions>
          <Button
            startIcon={<CancelIcon />}
            variant="outlined"
            color="secondary"
            onClick={() => setConfirmBoot(undefined)}
          >
            Cancel
          </Button>
          <Button
            startIcon={<PlayArrowIcon />}
            variant="outlined"
            color="primary"
            onClick={() => confirmBoot && void doBoot(confirmBoot)}
          >
            Start
          </Button>
        </DialogActions>
      </Dialog>
    </>
  );
};

export default Dashboard;
