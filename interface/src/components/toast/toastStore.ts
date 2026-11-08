export type ToastSeverity = 'success' | 'error' | 'info' | 'warning';

export interface ToastItem {
  id: number;
  severity: ToastSeverity;
  message: string;
}

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

const add = (severity: ToastSeverity, message: string): number => {
  const id = nextId++;
  toasts = [...toasts, { id, severity, message }];
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
  info: (message: string) => add('info', message),
  warning: (message: string) => add('warning', message)
};
