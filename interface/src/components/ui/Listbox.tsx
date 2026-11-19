import {
  type FC,
  type KeyboardEvent,
  type ReactNode,
  useEffect,
  useId,
  useRef,
  useState
} from 'react';

import './Field.css';
import Icon from './Icon';
import './Listbox.css';
import { cx } from './cx';

export interface ListboxOption {
  value: string;
  label: string;
  icon?: ReactNode;
  // shown muted on the right of the option, e.g. a count
  meta?: ReactNode;
  disabled?: boolean;
}

export interface ListboxProps {
  label: string;
  // label is for screen readers only
  hideLabel?: boolean;
  value: string | undefined;
  options: ListboxOption[];
  onChange: (value: string) => void;
  placeholder?: string;
  disabled?: boolean;
  fullWidth?: boolean;
  className?: string;
}

// a select whose options can have an icon and a count, which a native <select> can't show
const Listbox: FC<ListboxProps> = ({
  label,
  hideLabel,
  value,
  options,
  onChange,
  placeholder,
  disabled,
  fullWidth,
  className
}) => {
  const id = useId();
  const [open, setOpen] = useState(false);
  const [active, setActive] = useState(-1);
  const rootRef = useRef<HTMLDivElement>(null);
  const triggerRef = useRef<HTMLButtonElement>(null);
  const listRef = useRef<HTMLUListElement>(null);
  const typed = useRef({ text: '', at: 0 });

  const selected = options.find((o) => o.value === value);
  const enabled = options.flatMap((o, i) => (o.disabled ? [] : [i]));

  const show = () => {
    const i = options.findIndex((o) => o.value === value && !o.disabled);
    setActive(i >= 0 ? i : (enabled[0] ?? -1));
    setOpen(true);
  };

  const hide = () => {
    setOpen(false);
    triggerRef.current?.focus();
  };

  const pick = (i: number) => {
    const o = options[i];
    if (!o || o.disabled) return;
    hide();
    if (o.value !== value) onChange(o.value);
  };

  useEffect(() => {
    if (!open) return;
    listRef.current?.focus();
    const onPointerDown = (e: PointerEvent) => {
      if (!rootRef.current?.contains(e.target as Node)) setOpen(false);
    };
    document.addEventListener('pointerdown', onPointerDown);
    return () => document.removeEventListener('pointerdown', onPointerDown);
  }, [open]);

  useEffect(() => {
    if (open && active >= 0) {
      listRef.current?.children[active]?.scrollIntoView({ block: 'nearest' });
    }
  }, [open, active]);

  const move = (delta: number) => {
    const pos = enabled.indexOf(active) + delta;
    const next = enabled[Math.min(enabled.length - 1, Math.max(0, pos))];
    if (next !== undefined) setActive(next);
  };

  const typeAhead = (key: string) => {
    const now = Date.now();
    const t = typed.current;
    t.text = now - t.at > 600 ? key : t.text + key;
    t.at = now;
    const q = t.text.toLowerCase();
    const i = enabled.find((n) => options[n]?.label.toLowerCase().startsWith(q));
    if (i !== undefined) setActive(i);
  };

  const onListKeyDown = (e: KeyboardEvent<HTMLUListElement>) => {
    switch (e.key) {
      case 'ArrowDown':
        move(1);
        break;
      case 'ArrowUp':
        move(-1);
        break;
      case 'Home':
        setActive(enabled[0] ?? -1);
        break;
      case 'End':
        setActive(enabled[enabled.length - 1] ?? -1);
        break;
      case 'Enter':
      case ' ':
        pick(active);
        break;
      case 'Escape':
        hide();
        break;
      case 'Tab':
        setOpen(false);
        return;
      default:
        if (e.key.length !== 1 || e.ctrlKey || e.metaKey || e.altKey) return;
        typeAhead(e.key);
    }
    e.preventDefault();
  };

  const optionId = (i: number) => `${id}-o${i}`;

  return (
    <div
      ref={rootRef}
      className={cx(
        'field',
        'lbx',
        fullWidth && 'full',
        disabled && 'disabled',
        className
      )}
    >
      <label htmlFor={id} className={cx(hideLabel && 'sr-only')}>
        {label}
      </label>
      <button
        ref={triggerRef}
        id={id}
        type="button"
        className="input lbx-trigger"
        disabled={disabled}
        aria-haspopup="listbox"
        aria-expanded={open}
        aria-controls={open ? `${id}-list` : undefined}
        onClick={() => (open ? hide() : show())}
        onKeyDown={(e) => {
          if (e.key === 'ArrowDown' || e.key === 'ArrowUp') {
            e.preventDefault();
            show();
          }
        }}
      >
        {selected?.icon && <span className="lbx-icon">{selected.icon}</span>}
        <span className={cx('lbx-text', !selected && 'lbx-ph')}>
          {selected?.label ?? placeholder}
        </span>
        <Icon name="chevron_right" size="sm" className="lbx-chev" />
      </button>
      {open && (
        <ul
          ref={listRef}
          id={`${id}-list`}
          role="listbox"
          tabIndex={-1}
          className="lbx-list"
          aria-label={label}
          aria-activedescendant={active >= 0 ? optionId(active) : undefined}
          onKeyDown={onListKeyDown}
        >
          {options.map((o, i) => (
            <li
              key={o.value}
              id={optionId(i)}
              role="option"
              aria-selected={o.value === value}
              aria-disabled={o.disabled || undefined}
              className={cx(
                'lbx-opt',
                i === active && 'active',
                o.value === value && 'sel'
              )}
              onPointerMove={() => {
                if (!o.disabled && i !== active) setActive(i);
              }}
              onClick={() => pick(i)}
            >
              {o.icon && <span className="lbx-icon">{o.icon}</span>}
              <span className="lbx-text">{o.label}</span>
              {o.meta !== undefined && <span className="count">{o.meta}</span>}
            </li>
          ))}
        </ul>
      )}
    </div>
  );
};

export default Listbox;
