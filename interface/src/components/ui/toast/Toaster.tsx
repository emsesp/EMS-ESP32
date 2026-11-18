import { memo, useEffect, useRef, useSyncExternalStore } from 'react';

import { useI18nContext } from 'i18n/i18n-react';

import Icon from '../Icon';
import IconButton from '../IconButton';
import type { IconName } from '../icons';
import './Toaster.css';
import {
  type ToastItem,
  type ToastSeverity,
  getSnapshot,
  removeToast,
  subscribe
} from './store';

const AUTO_CLOSE_MS = 3000;

const ICONS: Record<ToastSeverity, IconName> = {
  success: 'check_circle',
  error: 'error',
  warning: 'warning',
  info: 'info'
};

// auto-closes, but the countdown stops while the browser tab is hidden
const Toast = memo(({ item }: { item: ToastItem }) => {
  const { LL } = useI18nContext();

  useEffect(() => {
    let remaining = AUTO_CLOSE_MS;
    let started = Date.now();
    let timer: ReturnType<typeof setTimeout> | undefined;
    const start = () => {
      started = Date.now();
      timer = setTimeout(() => removeToast(item.id), remaining);
    };
    const onVisibility = () => {
      if (document.hidden) {
        clearTimeout(timer);
        remaining -= Date.now() - started;
      } else {
        start();
      }
    };
    if (!document.hidden) start();
    document.addEventListener('visibilitychange', onVisibility);
    return () => {
      clearTimeout(timer);
      document.removeEventListener('visibilitychange', onVisibility);
    };
  }, [item.id]);

  return (
    <div
      className={`toast ${item.severity}`}
      role={item.severity === 'error' ? 'alert' : 'status'}
      onClick={() => removeToast(item.id)}
    >
      <Icon name={ICONS[item.severity]} size="sm" />
      <span className="grow">{item.message}</span>
      <IconButton icon="close" label={LL.CLOSE()} />
    </div>
  );
});

const supportsPopover =
  typeof HTMLElement !== 'undefined' && 'showPopover' in HTMLElement.prototype;

// A manual popover sits in the top layer, so toasts show above an open modal <dialog>.
// Re-showing it moves it above a dialog opened later.
const Toaster = memo(() => {
  const toasts = useSyncExternalStore(subscribe, getSnapshot);
  const ref = useRef<HTMLDivElement>(null);
  const count = useRef(0);

  useEffect(() => {
    const el = ref.current;
    if (el && supportsPopover) {
      const isOpen = el.matches(':popover-open');
      if (toasts.length > count.current && isOpen) el.hidePopover();
      if (toasts.length && (!isOpen || toasts.length > count.current)) {
        el.showPopover();
      }
      if (!toasts.length && isOpen) el.hidePopover();
    }
    count.current = toasts.length;
  }, [toasts]);

  return (
    <div ref={ref} className="toaster" popover="manual">
      {toasts.map((item) => (
        <Toast key={item.id} item={item} />
      ))}
    </div>
  );
});

export default Toaster;
