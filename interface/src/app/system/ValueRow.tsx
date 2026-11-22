import { memo } from 'react';

import { Button, Icon, Segmented, Select, cx } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import { formatValue } from '../main/deviceValue';
import { isFavorite, rangeOf, str } from './model';
import type { SysItem } from './model';
import type { Edits } from './useSystemData';

// value of a number field rounded to the step, without float noise
const round = (v: number, step: number): number => {
  const decimals = (String(step).split('.')[1] ?? '').length;
  return Number((Math.round(v / step) * step).toFixed(decimals));
};

// short name as the API knows it, e.g. hc1/seltemp
const shortId = (item: SysItem): string => {
  const short = (item.key.split(':')[1] ?? '').split('/').pop() ?? '';
  return item.tag ? `${item.tag}/${short}` : short;
};

export const useFormat = () => {
  const { LL } = useI18nContext();
  return (v: unknown, u?: number): string => formatValue(LL, v, u);
};

interface ControlProps {
  item: SysItem;
  value: unknown;
  onChange: (v: unknown) => void;
  onRun: () => void;
  // tile: a tile of the Home page, full width and without the slider
  variant?: 'row' | 'tile';
}

// the control that fits the kind of value: read-only text, command button, choice,
// number with steps, or free text
export const ValueControl = ({
  item,
  value,
  onChange,
  onRun,
  variant = 'row'
}: ControlProps) => {
  const { LL } = useI18nContext();
  const format = useFormat();
  const { dv } = item;
  const tile = variant === 'tile';

  if (!dv.c) {
    const text = format(dv.v, dv.u);
    return <span className="sys-ro">{text === '' ? '–' : text}</span>;
  }

  if (dv.v === undefined && dv.l === undefined) {
    return (
      <Button color="neutral" icon="play_arrow" onClick={onRun}>
        {LL.RUN()}
      </Button>
    );
  }

  if (dv.l && dv.l.length > 0) {
    const current = str(value);
    const options = dv.l.map((o) => ({ value: o, label: o }));
    if (dv.l.length <= 3 || (tile && dv.l.length <= 6)) {
      return (
        <Segmented
          hideLabel
          fullWidth={tile}
          label={item.label}
          options={options}
          value={current}
          onChange={onChange}
        />
      );
    }
    return (
      <Select
        hideLabel
        label={item.label}
        options={options}
        value={dv.l.includes(current) ? current : ''}
        onChange={(e) => onChange(e.target.value)}
      />
    );
  }

  const range = rangeOf(item);
  if (typeof value === 'number' && range) {
    const set = (v: number) =>
      onChange(round(Math.min(range.max, Math.max(range.min, v)), range.step));
    return (
      <>
        {!tile && (
          <input
            type="range"
            className="sys-range"
            aria-label={item.label}
            min={range.min}
            max={range.max}
            step={range.step}
            value={value}
            onChange={(e) => set(Number(e.target.value))}
          />
        )}
        <span className="sys-step">
          <button
            type="button"
            aria-label="−"
            onClick={() => set(value - range.step)}
          >
            −
          </button>
          <span>{format(value, dv.u)}</span>
          <button
            type="button"
            aria-label="+"
            onClick={() => set(value + range.step)}
          >
            +
          </button>
        </span>
      </>
    );
  }

  return (
    <input
      className="input"
      aria-label={item.label}
      defaultValue={str(value)}
      key={str(item.dv.v)}
      title={dv.h ?? ''}
      onBlur={(e) => {
        if (e.target.value !== str(item.dv.v)) onChange(e.target.value);
      }}
    />
  );
};

interface RowProps {
  item: SysItem;
  edits: Edits;
  // undefined when the favorite can't be changed (no administrator)
  onFavorite?: (item: SysItem) => void;
  // moves the value between Normal and All values; only on the pages of the menu, not on the favorites
  onLevel?: (item: SysItem) => void;
  // where the value lives, shown in search results and favorites
  where?: string;
}

export const ValueRow = memo(
  ({ item, edits, onFavorite, onLevel, where }: RowProps) => {
    const { LL } = useI18nContext();
    const format = useFormat();
    const changed = edits.pending.has(item.uid);
    const isFav = isFavorite(item);
    return (
      <div className={cx('sys-row', changed && 'pend')}>
        <div className="sys-lab">
          <button
            type="button"
            className={cx('sys-star', isFav && 'on')}
            aria-label={LL.FAVORITES()}
            aria-pressed={isFav}
            disabled={!onFavorite}
            onClick={() => onFavorite?.(item)}
          >
            <Icon name="star" size="sm" />
          </button>
          {onLevel && (
            <button
              type="button"
              className={cx('sys-star', 'sys-eye', item.level === 2 && 'expert')}
              aria-label={
                item.level === 1
                  ? LL.SYS_LEVEL_MOVE_ALL()
                  : LL.SYS_LEVEL_MOVE_NORMAL()
              }
              title={
                item.level === 1
                  ? LL.SYS_LEVEL_MOVE_ALL()
                  : LL.SYS_LEVEL_MOVE_NORMAL()
              }
              onClick={() => onLevel(item)}
            >
              <Icon
                name={item.level === 1 ? 'visibility' : 'visibility_off'}
                size="sm"
              />
            </button>
          )}
          <span className="txt">
            <b>{item.label}</b>
            <small>
              {where ? `${where} · ` : ''}
              {item.dv.c ?? shortId(item)}
              {changed && ` · ${LL.SYS_CHANGED_WAS(format(item.dv.v, item.dv.u))}`}
            </small>
          </span>
        </div>
        <div className="sys-ctl">
          <ValueControl
            item={item}
            value={edits.value(item)}
            onChange={(v) => edits.set(item, v)}
            onRun={() => void edits.run(item, LL.WRITE_CMD_SENT())}
          />
        </div>
      </div>
    );
  }
);
