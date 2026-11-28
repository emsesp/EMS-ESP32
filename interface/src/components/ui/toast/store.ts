export type ToastSeverity = 'success' | 'error' | 'info' | 'warning';

// a button on the toast, e.g. to undo what the toast reports
export interface ToastAction {
  label: string;
  onClick: () => void;
}

export interface ToastItem {
  id: number;
  severity: ToastSeverity;
  message: string;
  action?: ToastAction | undefined;
}

const MAX_TOASTS = 3;

let toasts: ToastItem[] = [];
let nextId = 1;
const listeners = new Set<() => void>();

const emit = () => {
  for (const listener of listeners) listener();
};

export const subscribe = (listener: () => void): (() => void) => {
  listeners.add(listener);
  return () => {
    listeners.delete(listener);
  };
};

export const getSnapshot = (): ToastItem[] => toasts;

const add = (
  severity: ToastSeverity,
  message: string,
  action?: ToastAction
): number => {
  const id = nextId++;
  // a repeated message replaces the earlier one, so it moves to the end with a fresh timer
  const rest = toasts.filter(
    (t) => t.severity !== severity || t.message !== message
  );
  toasts = [...rest, { id, severity, message, action }].slice(-MAX_TOASTS);
  emit();
  return id;
};

export const removeToast = (id: number): void => {
  const next = toasts.filter((t) => t.id !== id);
  if (next.length !== toasts.length) {
    toasts = next;
    emit();
  }
};

// Imperative API mirroring the subset of react-toastify used across the app.
export const toast = {
  success: (message: string) => add('success', message),
  error: (message: string) => add('error', message),
  info: (message: string, action?: ToastAction) => add('info', message, action),
  warning: (message: string) => add('warning', message)
};
