import {
  type CSSProperties,
  type FC,
  type KeyboardEvent,
  useEffect,
  useId,
  useRef,
  useState
} from 'react';

import './Button.css';
import Icon from './Icon';
import './Listbox.css';
import { cx } from './cx';
import type { IconName } from './icons';

export interface MenuItem {
  label: string;
  icon?: IconName;
  onSelect: () => void;
}

export interface MenuProps {
  // accessible name; the trigger text unless iconOnly
  label: string;
  icon: IconName;
  // a round icon button instead of a labelled button
  iconOnly?: boolean;
  color?: 'primary' | 'secondary';
  items: MenuItem[];
  disabled?: boolean;
  className?: string;
}

const GAP = 4;
const MARGIN = 8;

// the list is fixed to the viewport, right aligned to the trigger when there is no room
const placeMenu = (trigger: HTMLElement): CSSProperties => {
  const r = trigger.getBoundingClientRect();
  const below = window.innerHeight - r.bottom - GAP - MARGIN;
  const up = below < 160 && r.top > below;
  const x =
    r.left + 200 > window.innerWidth - MARGIN
      ? { right: window.innerWidth - r.right }
      : { left: r.left };
  return {
    ...x,
    ...(up ? { bottom: window.innerHeight - r.top + GAP } : { top: r.bottom + GAP })
  };
};

// a button that opens a short list of actions
const Menu: FC<MenuProps> = ({
  label,
  icon,
  iconOnly,
  color = 'primary',
  items,
  disabled,
  className
}) => {
  const id = useId();
  const [pos, setPos] = useState<CSSProperties>();
  const [active, setActive] = useState(0);
  const rootRef = useRef<HTMLDivElement>(null);
  const triggerRef = useRef<HTMLButtonElement>(null);
  const listRef = useRef<HTMLUListElement>(null);
  const open = pos !== undefined;

  const show = () => {
    if (!triggerRef.current) return;
    setActive(0);
    setPos(placeMenu(triggerRef.current));
  };

  const hide = (focus = true) => {
    setPos(undefined);
    if (focus) triggerRef.current?.focus();
  };

  useEffect(() => {
    if (!open) return;
    listRef.current?.focus();
    const onPointerDown = (e: PointerEvent) => {
      if (!rootRef.current?.contains(e.target as Node)) hide(false);
    };
    const onScroll = () => hide(false);
    document.addEventListener('pointerdown', onPointerDown);
    window.addEventListener('resize', onScroll);
    document.addEventListener('scroll', onScroll, true);
    return () => {
      document.removeEventListener('pointerdown', onPointerDown);
      window.removeEventListener('resize', onScroll);
      document.removeEventListener('scroll', onScroll, true);
    };
  }, [open]);

  const pick = (i: number) => {
    const item = items[i];
    if (!item) return;
    hide();
    item.onSelect();
  };

  const onKeyDown = (e: KeyboardEvent<HTMLUListElement>) => {
    switch (e.key) {
      case 'ArrowDown':
        setActive((a) => (a + 1) % items.length);
        break;
      case 'ArrowUp':
        setActive((a) => (a + items.length - 1) % items.length);
        break;
      case 'Home':
        setActive(0);
        break;
      case 'End':
        setActive(items.length - 1);
        break;
      case 'Enter':
      case ' ':
        pick(active);
        break;
      case 'Escape':
        e.stopPropagation();
        hide();
        break;
      case 'Tab':
        hide(false);
        return;
      default:
        return;
    }
    e.preventDefault();
  };

  return (
    <div ref={rootRef} className={cx('menu', className)}>
      <button
        ref={triggerRef}
        type="button"
        className={iconOnly ? 'icon-btn plain' : cx('btn', color)}
        disabled={disabled}
        aria-haspopup="menu"
        aria-expanded={open}
        aria-controls={open ? `${id}-menu` : undefined}
        aria-label={iconOnly ? label : undefined}
        title={iconOnly ? label : undefined}
        onClick={() => (open ? hide() : show())}
      >
        <Icon name={icon} size={iconOnly ? undefined : 'sm'} />
        {!iconOnly && label}
      </button>
      {open && (
        <ul
          ref={listRef}
          id={`${id}-menu`}
          role="menu"
          tabIndex={-1}
          className="lbx-list"
          style={pos}
          aria-label={label}
          aria-activedescendant={`${id}-i${active}`}
          onKeyDown={onKeyDown}
        >
          {items.map((item, i) => (
            <li
              key={item.label}
              id={`${id}-i${i}`}
              role="menuitem"
              className={cx('lbx-opt', i === active && 'active')}
              onPointerMove={() => setActive(i)}
              onClick={() => pick(i)}
            >
              {item.icon && <Icon name={item.icon} size="sm" />}
              {item.label}
            </li>
          ))}
        </ul>
      )}
    </div>
  );
};

export default Menu;
