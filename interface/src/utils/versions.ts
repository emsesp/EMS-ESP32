import type { RemoteVersionInfo, VersionsResponse } from 'types';

// true if version a is newer than b, like 1.0.1 and 1.0.0. A prerelease, like 1.1.0-dev.1, is older than the release
export const isNewerVersion = (a: string, b: string) => {
  const parse = (v: string) => {
    const dash = v.indexOf('-');
    const numbers = (text: string) =>
      text.split('.').map((n) => Number.parseInt(n, 10) || 0);
    return {
      main: numbers(dash < 0 ? v : v.slice(0, dash)),
      pre: dash < 0 ? [] : numbers(v.slice(dash + 1)),
      isPre: dash >= 0
    };
  };
  const x = parse(a);
  const y = parse(b);
  for (let i = 0; i < Math.max(x.main.length, y.main.length); i++) {
    const diff = (x.main[i] ?? 0) - (y.main[i] ?? 0);
    if (diff !== 0) {
      return diff > 0;
    }
  }
  if (x.isPre !== y.isPre) {
    return !x.isPre;
  }
  for (let i = 0; i < Math.max(x.pre.length, y.pre.length); i++) {
    const diff = (x.pre[i] ?? 0) - (y.pre[i] ?? 0);
    if (diff !== 0) {
      return diff > 0;
    }
  }
  return false;
};

// the release with the recovery app, from the channel we're on, or the other one if that has none
export const onlineRecoveryRelease = (
  versions?: VersionsResponse
): { info: RemoteVersionInfo & { recovery: string }; dev: boolean } | undefined => {
  const releases = [
    { info: versions?.dev, dev: true },
    { info: versions?.stable, dev: false }
  ];
  if (versions?.current?.type !== 'dev') releases.reverse();
  const release = releases.find((r) => r.info?.recovery);
  return release?.info?.recovery
    ? {
        info: release.info as RemoteVersionInfo & { recovery: string },
        dev: release.dev
      }
    : undefined;
};

// a newer recovery app than the installed one, false if none is installed as that's an install, not an update
export const recoveryUpgradeable = (versions?: VersionsResponse) => {
  const installed = versions?.current?.recovery;
  const online = onlineRecoveryRelease(versions)?.info.recovery;
  return !!installed && !!online && isNewerVersion(online, installed);
};

// shows the yellow dot in the menu
export const updateAvailable = (versions?: VersionsResponse) =>
  (versions?.current?.upgradeable ?? false) || recoveryUpgradeable(versions);
