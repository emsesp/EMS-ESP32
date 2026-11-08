// preact/compat shim used as the `react` alias.
//
// react-router v8 statically imports/uses a couple of React 19 APIs that
// preact/compat doesn't implement. Aliasing `react` straight to preact/compat
// therefore (a) fails Rolldown's static export analysis during dependency
// optimization / build and (b) could crash at runtime. We re-export everything
// from preact/compat and add the missing pieces:
//
//   - useOptimistic: called unconditionally at the top of `RouterProvider`
//     (lib/components.js). The optimistic setter is only invoked when
//     `unstable_useTransitions === true`, which this app doesn't enable, so a
//     passthrough state + no-op setter is correct.
//   - use: only invoked (on a promise) in react-router's RSC server-SSR path
//     (lib/rsc/server.ssr.js), which a client-only `createBrowserRouter` app
//     never executes. It still must exist as an export to avoid Rolldown's
//     IMPORT_IS_UNDEFINED warning; we provide a Suspense-style promise unwrap
//     just in case it is ever reached.

// preact/compat's type declarations use CommonJS `export =`, which TypeScript
// won't let us `export *` from, but Rolldown/Vite correctly re-export its named
// members at runtime (this is what makes react-router's `React.useState` etc.
// resolve through the alias).
// @ts-expect-error -- CJS `export =` interop; valid at bundle time.
export * from 'preact/compat';
export { default } from 'preact/compat';

export function useOptimistic<S>(passthrough: S): [S, (action: unknown) => void] {
  return [passthrough, () => {}];
}

interface TrackedThenable<T> extends PromiseLike<T> {
  status?: 'pending' | 'fulfilled' | 'rejected';
  value?: T;
  reason?: unknown;
}

// React 19's `use`, narrowed to the promise-unwrap (Suspense) behavior that
// react-router relies on.
export function use<T>(usable: TrackedThenable<T>): T {
  switch (usable.status) {
    case 'fulfilled':
      return usable.value as T;
    case 'rejected':
      throw usable.reason;
    default:
      if (usable.status === undefined) {
        usable.status = 'pending';
        void usable.then(
          (value) => {
            usable.status = 'fulfilled';
            usable.value = value;
          },
          (reason) => {
            usable.status = 'rejected';
            usable.reason = reason;
          }
        );
      }
      // eslint-disable-next-line @typescript-eslint/only-throw-error -- Suspense throws the thenable
      throw usable;
  }
}
