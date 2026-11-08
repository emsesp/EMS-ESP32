import { memo, useEffect, useRef, useState, useSyncExternalStore } from 'react';

import CloseIcon from '@mui/icons-material/Close';
import Alert from '@mui/material/Alert';
import Grow from '@mui/material/Grow';
import IconButton from '@mui/material/IconButton';
import LinearProgress from '@mui/material/LinearProgress';
import Stack from '@mui/material/Stack';

import {
  type ToastItem,
  type ToastSeverity,
  getSnapshot,
  removeToast,
  subscribe
} from './toastStore';

const AUTO_CLOSE_MS = 3000;
const TICK_MS = 50;

// Semantic accent colors users already expect:
// success → green, error → red, warning → amber, info → blue.
const ACCENT: Record<ToastSeverity, string> = {
  success: '#16a34a',
  error: '#dc2626',
  warning: '#f59e0b',
  info: '#2563eb'
};

// Single toast row: owns its auto-dismiss timer + countdown progress bar, pauses
// while the window is unfocused (matching react-toastify's pauseOnFocusLoss).
const ToastRow = memo(({ item }: { item: ToastItem }) => {
  const [open, setOpen] = useState(true);
  const [remaining, setRemaining] = useState(AUTO_CLOSE_MS);
  const remainingRef = useRef(AUTO_CLOSE_MS);

  useEffect(() => {
    let paused = document.hidden;
    const onVisibility = () => {
      paused = document.hidden;
    };
    document.addEventListener('visibilitychange', onVisibility);

    const timer = setInterval(() => {
      if (paused) return;
      remainingRef.current = Math.max(0, remainingRef.current - TICK_MS);
      setRemaining(remainingRef.current);
      if (remainingRef.current === 0) setOpen(false);
    }, TICK_MS);

    return () => {
      clearInterval(timer);
      document.removeEventListener('visibilitychange', onVisibility);
    };
  }, []);

  const accent = ACCENT[item.severity];

  return (
    <Grow in={open} onExited={() => removeToast(item.id)}>
      <Alert
        severity={item.severity}
        variant="standard"
        onClick={() => setOpen(false)}
        action={
          <IconButton
            size="small"
            aria-label="close"
            onClick={(e) => {
              e.stopPropagation();
              setOpen(false);
            }}
            sx={{ color: '#9ca3af', '&:hover': { color: '#374151' } }}
          >
            <CloseIcon fontSize="small" />
          </IconButton>
        }
        sx={{
          position: 'relative',
          width: 'fit-content',
          minWidth: 300,
          maxWidth: 360,
          cursor: 'pointer',
          overflow: 'hidden',
          borderRadius: '8px',
          bgcolor: '#f3f4f6',
          color: '#1f2937',
          boxShadow: '0 4px 12px rgba(0, 0, 0, 0.18)',
          borderLeft: `4px solid ${accent}`,
          alignItems: 'center',
          '& .MuiAlert-icon': { color: accent, alignItems: 'center' },
          // '& .MuiAlert-message': {
          //   py: '8px',
          //   color: '#1f2937'
          // },
          '& .MuiAlert-action': { alignItems: 'center', pt: 0, mr: '-4px' }
        }}
      >
        {item.message}
        <LinearProgress
          variant="determinate"
          value={(remaining / AUTO_CLOSE_MS) * 100}
          sx={{
            position: 'absolute',
            left: 0,
            right: 0,
            bottom: 0,
            height: 3,
            backgroundColor: 'transparent',
            '& .MuiLinearProgress-bar': { backgroundColor: accent, opacity: 0.55 }
          }}
        />
      </Alert>
    </Grow>
  );
});

const Toaster = memo(() => {
  const toasts = useSyncExternalStore(subscribe, getSnapshot);

  return (
    <Stack
      spacing={1}
      sx={{
        position: 'fixed',
        bottom: 16,
        left: 16,
        zIndex: (theme) => theme.zIndex.snackbar,
        pointerEvents: 'none',
        '& > *': { pointerEvents: 'auto' }
      }}
    >
      {toasts.map((item) => (
        <ToastRow key={item.id} item={item} />
      ))}
    </Stack>
  );
});

export default Toaster;
