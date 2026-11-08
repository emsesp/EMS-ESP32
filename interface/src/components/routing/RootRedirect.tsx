import { memo, useContext, useEffect, useRef } from 'react';
import { Navigate } from 'react-router';

import { toast } from 'components/toast';
import { AuthenticationContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

type RootRedirectKind = 'unauthorized' | 'fileUpdated';

// Shows a one-shot toast and bounces back to "/". Used by the /unauthorized and
// /fileUpdated routes. Resolves its own i18n message so it can be used directly
// as a static route element.
const RootRedirect = ({ kind }: { kind: RootRedirectKind }) => {
  const { LL } = useI18nContext();
  const { signOut } = useContext(AuthenticationContext);
  const hasShownToast = useRef(false);

  useEffect(() => {
    // Guard against StrictMode double-invoke / re-renders.
    if (hasShownToast.current) return;
    hasShownToast.current = true;

    if (kind === 'unauthorized') {
      signOut(false);
      toast.success(LL.PLEASE_SIGNIN());
    } else {
      toast.success(LL.UPLOAD_SUCCESSFUL());
    }
    // Run once on mount.
  }, []);

  return <Navigate to="/" replace />;
};

export default memo(RootRedirect);
