import { Theme } from "@material-ui/core";
import { APStatus, APNetworkStatus } from "./types";

export const apStatusHighlight = ({ status }: APStatus, theme: Theme) => {
  switch (status) {
    case APNetworkStatus.ACTIVE:
      return theme.palette.success.main;
    case APNetworkStatus.INACTIVE:
      return theme.palette.info.main;
    case APNetworkStatus.LINGERING:
      return theme.palette.warning.main;
    default:
      return theme.palette.warning.main;
  }
}

export const apStatus = ({ status }: APStatus) => {
  switch (status) {
    case APNetworkStatus.ACTIVE:
      return "Active";
    case APNetworkStatus.INACTIVE:
      return "Inactive";
    case APNetworkStatus.LINGERING:
      return "Lingering until idle";
    default:
      return "Unknown";
  }
};
