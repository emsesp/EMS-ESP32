import type { FC } from 'react';

import { useI18nContext } from 'i18n/i18n-react';

import Button from './Button';
import './Loader.css';
import Message from './Message';
import { cx } from './cx';

export interface LoaderProps {
  // shown instead of the spinner when loading failed
  error?: string | undefined;
  onRetry?: () => void;
  // full viewport height, centred (app start-up)
  page?: boolean;
}

// placeholder for a page whose data hasn't loaded yet
const Loader: FC<LoaderProps> = ({ error, onRetry, page }) => {
  const { LL } = useI18nContext();

  if (error) {
    return (
      <div className={cx('loader', page && 'page')}>
        <Message
          level="error"
          actions={
            onRetry && (
              <Button size="sm" icon="refresh" onClick={onRetry}>
                {LL.RETRY()}
              </Button>
            )
          }
        >
          {error}
        </Message>
      </div>
    );
  }

  return (
    <div className={cx('loader', page && 'page')} role="status">
      <span className="spinner" />
      <span className="sr-only">{LL.PLEASE_WAIT()}</span>
    </div>
  );
};

export default Loader;
