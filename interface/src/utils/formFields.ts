import type { ChangeEvent } from 'react';

import { extractEventValue, numberValue } from './binding';

export interface FieldMeta {
  changed?: boolean;
  restart?: boolean;
  error?: string | undefined;
}

const str = (v: unknown) =>
  typeof v === 'string' || typeof v === 'number' || typeof v === 'boolean'
    ? String(v)
    : '';

// prop builders for the components/ui fields, bound to one object of values
export const formFields = <D extends object>(
  values: D | undefined,
  set: (patch: Partial<D>) => void,
  meta: (name: keyof D & string) => FieldMeta = () => ({})
) => {
  type K = keyof D & string;
  const v = (name: K) => (values as Record<string, unknown> | undefined)?.[name];
  const patch = (name: K, value: unknown) => set({ [name]: value } as Partial<D>);
  // a select keeps numbers as numbers
  const parse = (name: K, s: string) =>
    typeof v(name) === 'number' ? Number(s) : s;

  return {
    // TextField / TextArea
    text: (name: K) => ({
      name,
      ...meta(name),
      value: str(v(name)),
      onChange: (e: ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) =>
        patch(name, e.currentTarget.value)
    }),
    // TextField type="number"; keeps partly typed numbers like "1." as text
    number: (name: K) => {
      const n = v(name);
      return {
        name,
        ...meta(name),
        type: 'number',
        value: typeof n === 'string' ? n : numberValue(n as number | undefined),
        onChange: (e: ChangeEvent<HTMLInputElement>) =>
          patch(name, extractEventValue(e))
      };
    },
    // Switch / Checkbox
    check: (name: K) => {
      const { changed, restart } = meta(name);
      return {
        name,
        changed,
        restart,
        checked: !!v(name),
        onChange: (checked: boolean) => patch(name, checked)
      };
    },
    select: (name: K) => ({
      name,
      ...meta(name),
      value: str(v(name)),
      onChange: (e: ChangeEvent<HTMLSelectElement>) =>
        patch(name, parse(name, e.currentTarget.value))
    }),
    segmented: (name: K) => {
      const { changed, restart } = meta(name);
      return {
        changed,
        restart,
        value: str(v(name)),
        onChange: (s: string) => patch(name, parse(name, s))
      };
    }
  };
};
