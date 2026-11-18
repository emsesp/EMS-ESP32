import { type ReactNode, useCallback, useRef, useState } from 'react';

import type { AlovaGenerics, Method } from 'alova';
import { useRequest } from 'alova/client';
import { API } from 'api/app';
import SystemMonitor from 'app/status/SystemMonitor';

import BlockNavigation from 'components/routing/BlockNavigation';
import { Button, Loader, SaveBar } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { APIcall } from 'app/main/types';
import { formFields, useRest } from 'utils';
import { ValidationError, validate } from 'validators';
import type Schema from 'validators/schema';
import type { ValidateFieldsError } from 'validators/schema';

export interface FormPageOptions<D> {
  read: () => Method<AlovaGenerics>;
  update: (value: D) => Method<AlovaGenerics>;
  validator?: (data: D) => Schema;
  // fields that only take effect after a restart (marked in the label and the save bar)
  restartKeys?: readonly (keyof D & string)[];
}

const same = (a: unknown, b: unknown) =>
  a === b ||
  (typeof a === 'object' && JSON.stringify(a) === JSON.stringify(b)) ||
  (typeof a === 'number' && isNaN(a) && typeof b === 'number' && isNaN(b));

// state of a settings form: values, dirty fields, errors, save and cancel
export const useFormPage = <D extends object>({
  read,
  update,
  validator,
  restartKeys = []
}: FormPageOptions<D>) => {
  const rest = useRest<D>({ read, update });
  const { data, origData, dirtyFlags, setDirtyFlags, updateDataValue } = rest;
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  type K = keyof D & string;

  // latest values, so two changes in the same tick both count
  const latest = useRef({ data, dirtyFlags });
  latest.current = { data, dirtyFlags };

  // change one or more fields
  const set = useCallback(
    (patch: Partial<D>) => {
      const base = latest.current.data;
      if (!base) return;
      const next = { ...base, ...patch };
      updateDataValue(next);
      const orig = (origData ?? {}) as Record<string, unknown>;
      const dirty = new Set(latest.current.dirtyFlags);
      for (const k of Object.keys(patch)) {
        if (same(orig[k], (next as Record<string, unknown>)[k])) dirty.delete(k);
        else dirty.add(k);
      }
      setDirtyFlags([...dirty]);
      latest.current = { data: next, dirtyFlags: [...dirty] };
      setFieldErrors((errs) => {
        if (!errs) return errs;
        const left = { ...errs };
        for (const k of Object.keys(patch)) delete left[k];
        return left;
      });
    },
    [origData, setDirtyFlags, updateDataValue]
  );

  const { text, number, check, select, segmented } = formFields(
    data as D | undefined,
    set,
    (name: K) => ({
      changed: dirtyFlags.includes(name),
      restart: restartKeys.includes(name),
      error: fieldErrors?.[name]?.[0]?.message
    })
  );

  const cancel = () => {
    if (origData) updateDataValue(origData);
    setDirtyFlags([]);
    setFieldErrors(undefined);
  };

  const save = async () => {
    if (!data) return;
    setFieldErrors(undefined);
    try {
      if (validator) await validate(validator(data), data);
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
      setTimeout(() =>
        document.querySelector<HTMLElement>('[aria-invalid="true"]')?.focus()
      );
      return;
    }
    await rest.saveData();
  };

  return {
    ...rest,
    set,
    text,
    number,
    check,
    select,
    segmented,
    cancel,
    save,
    fieldErrors,
    needsRestart: dirtyFlags.some((k) => restartKeys.includes(k as K))
  };
};

export type FormState<D extends object> = ReturnType<typeof useFormPage<D>>;

interface FormPageProps<D extends object> {
  form: FormState<D>;
  children: (data: D) => ReactNode;
}

// loader, the unsaved-changes guard, the save bar and the restart prompt around a settings form
const FormPage = <D extends object>({ form, children }: FormPageProps<D>) => {
  const { LL } = useI18nContext();
  const [restarting, setRestarting] = useState(false);
  const { send: sendAPI, loading: restartSending } = useRequest(
    (call: APIcall) => API(call),
    { immediate: false }
  );

  if (restarting) return <SystemMonitor />;
  if (!form.data) {
    return (
      <Loader error={form.errorMessage} onRetry={() => void form.loadData()} />
    );
  }

  const changes = form.dirtyFlags.length;

  return (
    <>
      {form.blocker && <BlockNavigation blocker={form.blocker} />}
      {children(form.data)}
      {changes > 0 ? (
        <SaveBar
          message={
            <>
              {LL.CHANGED_SETTINGS({ num: changes })}
              {form.needsRestart && (
                <span className="warnc"> · {LL.NEEDS_RESTART()}</span>
              )}
            </>
          }
        >
          <Button color="secondary" icon="undo" disabled={form.saving} onClick={form.cancel}>
            {LL.CANCEL()}
          </Button>
          <Button
            variant="contained"
            icon="check"
            disabled={form.saving}
            onClick={() => void form.save()}
          >
            {LL.SAVE()}
          </Button>
        </SaveBar>
      ) : (
        form.restartNeeded && (
          <SaveBar warning icon="restart_alt" message={LL.SAVED_RESTART()}>
            <Button
              variant="contained"
              color="warning"
              icon="restart_alt"
              disabled={restartSending}
              onClick={() =>
                void sendAPI({ device: 'system', cmd: 'restart', id: 0 }).then(() =>
                  setRestarting(true)
                )
              }
            >
              {LL.RESTART_NOW()}
            </Button>
          </SaveBar>
        )
      )}
    </>
  );
};

export default FormPage;
