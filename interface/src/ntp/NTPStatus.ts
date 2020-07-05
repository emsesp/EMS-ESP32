import { Theme } from "@material-ui/core";
import { NTPStatus, NTPSyncStatus } from "./types";

export const isNtpActive = ({ status }: NTPStatus) => status === NTPSyncStatus.NTP_ACTIVE;

export const ntpStatusHighlight = ({ status }: NTPStatus, theme: Theme) => {
  switch (status) {
    case NTPSyncStatus.NTP_INACTIVE:
      return theme.palette.info.main;
    case NTPSyncStatus.NTP_ACTIVE:
      return theme.palette.success.main;
    default:
      return theme.palette.error.main;
  }
}

export const ntpStatus = ({ status }: NTPStatus) => {
  switch (status) {
    case NTPSyncStatus.NTP_INACTIVE:
      return "Inactive";
    case NTPSyncStatus.NTP_ACTIVE:
      return "Active";
    default:
      return "Unknown";
  }
}
