import { useContext, useState } from 'react';

import { Message } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

import {
  FAULT_PACK_LIST_URL,
  installPackFile,
  removePack,
  useFaultPacks
} from './faultPacks';

export const brandName = (brand: string): string =>
  brand.charAt(0).toUpperCase() + brand.slice(1);

// The fault code packs that are stored on the gateway, and installing a pack from a file
export const FaultPackManager = ({ missing }: { missing: readonly string[] }) => {
  const { LL, locale } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);
  const { info } = useFaultPacks([], locale);
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState('');

  const supported = !!info && info.max_size > 0;
  const run = async (action: () => Promise<void>) => {
    setBusy(true);
    setError('');
    try {
      await action();
    } catch (e) {
      setError(e instanceof Error ? e.message : String(e));
    } finally {
      setBusy(false);
    }
  };

  return (
    <div className="sys-fault">
      <div className="sys-fault-head">{LL.SYS_FAULT_PACK_TITLE()}</div>

      {missing.map((b) => (
        <p key={b}>{LL.SYS_FAULT_PACK_MISSING(brandName(b))}</p>
      ))}

      {info && !supported && (
        <Message level="warning">{LL.SYS_FAULT_PACK_UNSUPPORTED()}</Message>
      )}

      {info?.packs.map((p) => (
        <p
          key={`${p.brand}.${p.lang}`}
          className="sys-sub"
          style={{ margin: '4px 0' }}
        >
          {LL.SYS_FAULT_SOURCE(
            brandName(p.brand),
            p.lang.toUpperCase(),
            p.version ?? '',
            p.entries ?? 0
          )}
          {me.admin && (
            <>
              {' · '}
              <a
                href="#"
                onClick={(e) => {
                  e.preventDefault();
                  void run(() => removePack(p.brand, p.lang));
                }}
              >
                {LL.SYS_FAULT_PACK_REMOVE()}
              </a>
            </>
          )}
        </p>
      ))}

      {me.admin && supported && (
        <p style={{ margin: '12px 0 4px' }}>
          <label className="sys-fault-lbl" htmlFor="fault-file">
            {LL.SYS_FAULT_PACK_FILE()}
          </label>
          <input
            id="fault-file"
            type="file"
            accept=".json,application/json"
            disabled={busy}
            onChange={(e) => {
              const file = e.target.files?.[0];
              e.target.value = '';
              if (file) void run(() => installPackFile(file));
            }}
          />
        </p>
      )}

      <p className="sys-sub" style={{ margin: '8px 0 0' }}>
        {LL.SYS_FAULT_PACK_DISCLAIMER()}
      </p>
      <p className="sys-sub" style={{ margin: '8px 0 0' }}>
        {LL.SYS_FAULT_PACK_HELP()}{' '}
        <a href={FAULT_PACK_LIST_URL} target="_blank" rel="noopener noreferrer">
          {LL.SYS_FAULT_PACK_DOWNLOAD()}
        </a>
      </p>

      {error && <p>{LL.SYS_FAULT_PACK_ERROR(error)}</p>}
    </div>
  );
};
