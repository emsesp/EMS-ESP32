import { memo, useEffect, useRef, useState, useSyncExternalStore } from 'react';

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

// auto-closes; the countdown (and its bar) stops while the pointer or focus is on
// the toast, or the browser tab is hidden
const Toast = memo(({ item }: { item: ToastItem }) => {
  const { LL } = useI18nContext();
  const ref = useRef<HTMLDivElement>(null);
  const remaining = useRef(AUTO_CLOSE_MS);
  const [hover, setHover] = useState(false);
  const [focus, setFocus] = useState(false);
  const [hidden, setHidden] = useState(document.hidden);
  const paused = hover || focus || hidden;

  useEffect(() => {
    const el = ref.current;
    const onVisibility = () => setHidden(document.hidden);
    const onFocusIn = () => setFocus(true);
    const onFocusOut = (e: FocusEvent) =>
      setFocus(el?.contains(e.relatedTarget as Node | null) ?? false);
    document.addEventListener('visibilitychange', onVisibility);
    el?.addEventListener('focusin', onFocusIn);
    el?.addEventListener('focusout', onFocusOut);
    return () => {
      document.removeEventListener('visibilitychange', onVisibility);
      el?.removeEventListener('focusin', onFocusIn);
      el?.removeEventListener('focusout', onFocusOut);
    };
  }, []);

  useEffect(() => {
    if (paused) return;
    const started = Date.now();
    const timer = setTimeout(() => removeToast(item.id), remaining.current);
    return () => {
      clearTimeout(timer);
      remaining.current -= Date.now() - started;
    };
  }, [paused, item.id]);

  return (
    <div
      ref={ref}
      className={`toast ${item.severity}${paused ? ' paused' : ''}`}
      role={item.severity === 'error' ? 'alert' : 'status'}
      onClick={() => removeToast(item.id)}
      onMouseEnter={() => setHover(true)}
      onMouseLeave={() => setHover(false)}
    >
      <Icon name={ICONS[item.severity]} />
      <span className="grow">{item.message}</span>
      <IconButton icon="close" label={LL.CLOSE()} plain />
      <span
        className="toast-bar"
        style={{ animationDuration: `${AUTO_CLOSE_MS}ms` }}
      />
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
  const lastId = useRef(0);

  useEffect(() => {
    const el = ref.current;
    const newest = toasts.at(-1)?.id ?? 0;
    const added = newest > lastId.current;
    if (el && supportsPopover) {
      const isOpen = el.matches(':popover-open');
      if (added && isOpen) el.hidePopover();
      if (toasts.length && (!isOpen || added)) el.showPopover();
      if (!toasts.length && isOpen) el.hidePopover();
    }
    lastId.current = Math.max(lastId.current, newest);
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
