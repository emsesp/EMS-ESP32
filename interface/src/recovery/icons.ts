// The icons the recovery app shows, including those of the shared ui components it uses.
// vite.config.ts hands this module to components/ui/Icon.tsx in the recovery build instead of
// the full registry, so an icon missing here renders empty.
import type { IconName } from 'components/ui/icons';
import bug_report from 'components/ui/icons/bug_report.svg?raw';
import build from 'components/ui/icons/build.svg?raw';
import cancel from 'components/ui/icons/cancel.svg?raw';
import check_circle from 'components/ui/icons/check_circle.svg?raw';
import close from 'components/ui/icons/close.svg?raw';
import icon_delete from 'components/ui/icons/delete.svg?raw';
import delete_forever from 'components/ui/icons/delete_forever.svg?raw';
import developer_board from 'components/ui/icons/developer_board.svg?raw';
import download from 'components/ui/icons/download.svg?raw';
import error from 'components/ui/icons/error.svg?raw';
import info from 'components/ui/icons/info.svg?raw';
import login from 'components/ui/icons/login.svg?raw';
import logout from 'components/ui/icons/logout.svg?raw';
import memory from 'components/ui/icons/memory.svg?raw';
import play_arrow from 'components/ui/icons/play_arrow.svg?raw';
import refresh from 'components/ui/icons/refresh.svg?raw';
import restart_alt from 'components/ui/icons/restart_alt.svg?raw';
import upload from 'components/ui/icons/upload.svg?raw';
import upload_file from 'components/ui/icons/upload_file.svg?raw';
import verified from 'components/ui/icons/verified.svg?raw';
import visibility from 'components/ui/icons/visibility.svg?raw';
import visibility_off from 'components/ui/icons/visibility_off.svg?raw';
import warning from 'components/ui/icons/warning.svg?raw';

export const icons: Partial<Record<IconName, string>> = {
  build,
  bug_report,
  cancel,
  check_circle,
  close,
  delete: icon_delete,
  delete_forever,
  developer_board,
  download,
  error,
  info,
  login,
  logout,
  memory,
  play_arrow,
  refresh,
  restart_alt,
  upload,
  upload_file,
  verified,
  visibility,
  visibility_off,
  warning
};
