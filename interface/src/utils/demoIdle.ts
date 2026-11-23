import { useSyncExternalStore } from 'react';

// Hosted demo (demo.emsesp.org, built with `vite build --mode hosted`) only:
// stop polling the REST API while the tab is hidden or nobody has touched the
// page for IDLE_MS. Every poll is a billed Cloudflare Worker request, and
// forgotten demo tabs were otherwise polling around the clock.
// On a real device this is a no-op: `demoPaused()` is always false.

export const DEMO_IDLE_ENABLED = import.meta.env.MODE === 'hosted';

const IDLE_MS = 10 * 60 * 1000;
const CHECK_MS = 15 * 1000;
const ACTIVITY_EVENTS = [
  'pointerdown',
  'pointermove',
  'keydown',
  'wheel',
  'touchstart',
  'scroll'
] as const;

let lastActivity = Date.now();
let paused = false;
const listeners = new Set<() => void>();

const compute = () =>
  document.visibilityState === 'hidden' || Date.now() - lastActivity > IDLE_MS;

const update = () => {
  const next = compute();
  if (next !== paused) {
    paused = next;
    listeners.forEach((l) => l());
  }
};

if (DEMO_IDLE_ENABLED && typeof window !== 'undefined') {
  const onActivity = () => {
    lastActivity = Date.now();
    if (paused) update();
  };
  ACTIVITY_EVENTS.forEach((e) =>
    window.addEventListener(e, onActivity, { passive: true, capture: true })
  );
  document.addEventListener('visibilitychange', () => {
    if (document.visibilityState === 'visible') lastActivity = Date.now();
    update();
  });
  setInterval(update, CHECK_MS);
}

/** True when polling should be skipped (hosted demo only). */
export const demoPaused = () => DEMO_IDLE_ENABLED && paused;

/** Subscribe to paused/resumed changes; returns an unsubscribe function. */
export const subscribeDemoPaused = (listener: () => void) => {
  listeners.add(listener);
  return () => listeners.delete(listener);
};

/** Wake the demo up again (e.g. from the banner's Resume button). */
export const resumeDemo = () => {
  lastActivity = Date.now();
  update();
};

export const useDemoPaused = () =>
  useSyncExternalStore(subscribeDemoPaused, demoPaused, () => false);
