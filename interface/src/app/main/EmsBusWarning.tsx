import { Message } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

const EmsBusWarning = ({ className }: { className: string }) => {
  const { LL } = useI18nContext();
  return (
    <Message level="error" className={className}>
      {LL.EMS_BUS_WARNING()}.{' '}
      <a
        target="_blank"
        rel="noreferrer"
        href="https://docs.emsesp.org/Troubleshooting#ems-bus-is-not-connecting"
      >
        {LL.ONLINE_HELP()}
      </a>
    </Message>
  );
};

export default EmsBusWarning;
