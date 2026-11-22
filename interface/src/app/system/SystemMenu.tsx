import { memo, useMemo } from 'react';
import type { ReactNode } from 'react';
import { useNavigate, useParams } from 'react-router';

import { Icon, Loader, Message, Select, cx } from 'components/ui';
import type { IconName } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import { ChangesBar } from '../main/ChangesBar';
import { ValueRow } from './ValueRow';
import type { SectionId } from './catalog';
import { buildMenus, groupItems } from './model';
import type { SysItem } from './model';
import { useFavorites } from './useFavorites';
import { useEdits, useSystemData } from './useSystemData';
import { SystemBar, useSystemView } from './view';

const menuIcon = (id: string): IconName => {
  if (/^hc\d$/.test(id)) return 'device_thermostat';
  switch (id) {
    case 'dhw':
      return 'water_drop';
    case 'boiler':
    case 'ahs':
    case 'hs':
      return 'local_fire_department';
    case 'heatpump':
      return 'heat_pump';
    case 'solar':
    case 'src':
      return 'solar_power';
    case 'mixer':
      return 'valve';
    case 'ventilation':
      return 'mode_fan';
    case 'pool':
      return 'pool';
    case 'service':
      return 'build';
    case 'energy':
      return 'speed';
    case 'thermostat':
      return 'tune';
    case 'fav':
      return 'star';
    default:
      return 'developer_board';
  }
};

const SystemMenu = memo(() => {
  const { LL } = useI18nContext();
  const view = useSystemView();
  const { level, q } = view;
  const data = useSystemData();
  const edits = useEdits(data);
  const navigate = useNavigate();
  const menu = useParams()['*'];
  const fav = useFavorites(data);
  const favs = fav.items;

  const menus = useMemo(() => buildMenus(data.items), [data.items]);
  const current =
    menu === 'fav'
      ? 'fav'
      : (menus.find((m) => m.id === menu)?.id ?? menus[0]?.id ?? 'fav');

  // Menu names: the heating circuits and groups are translated here, the device types
  // (boiler, thermostat, mixer, ...) come translated from the firmware
  const menuName = (id: string): string => {
    const hc = /^hc(\d)$/.exec(id);
    if (hc) return LL.SYS_HC(hc[1] ?? '');
    switch (id) {
      case 'dhw':
        return LL.SYS_DHW();
      case 'ahs':
        return LL.SYS_AHS();
      case 'hs':
        return LL.SYS_HS();
      case 'src':
        return LL.SYS_SRC();
      case 'service':
        return LL.SYS_SERVICE();
      case 'energy':
        return LL.SYS_ENERGY();
      case 'fav':
        return LL.FAVORITES();
      default:
        return (
          data.items.find((i) => i.menu === id && i.devType === id)?.typeName ??
          data.items.find((i) => i.menu === id)?.typeName ??
          LL.SYS_OTHER_DEVICES()
        );
    }
  };

  const sectionName = (s: SectionId): string => {
    const names: Record<SectionId, string> = {
      mode: LL.SYS_SEC_MODE(),
      temps: LL.SYS_SEC_TEMPS(),
      status: LL.SYS_SEC_STATUS(),
      service: LL.SYS_SERVICE(),
      energy: LL.SYS_ENERGY(),
      advanced: LL.SYS_SEC_ADVANCED(),
      measure: LL.SYS_SEC_MEASURE(),
      other: LL.SYS_SEC_OTHER()
    };
    return names[s];
  };

  const visibleCount = (id: string) =>
    data.items.filter((i) => i.menu === id && (level === 2 || i.level === 1)).length;

  const searching = q.trim() !== '';
  let list: SysItem[];
  if (searching) {
    const needle = q.trim().toLowerCase();
    list = data.items.filter(
      (i) =>
        (level === 2 || i.level === 1) &&
        `${i.label} ${i.key} ${i.dv.c ?? ''}`.toLowerCase().includes(needle)
    );
  } else if (current === 'fav') {
    list = favs;
  } else {
    list = data.items.filter((i) => i.menu === current);
  }
  const { groups, hidden } = groupItems(
    list,
    searching || current === 'fav' ? 2 : level
  );

  const go = (id: string) => {
    view.setQ('');
    void navigate(`/system/${id}`);
  };

  const navItem = (id: string, count: number): ReactNode => (
    <button
      key={id}
      type="button"
      className={cx('sys-nav-item', current === id && !searching && 'on')}
      onClick={() => go(id)}
    >
      <Icon name={menuIcon(id)} size="sm" />
      <span className="grow">{menuName(id)}</span>
      <span className="n">{count}</span>
    </button>
  );

  const groupTitle = (g: 'heating' | 'plant' | 'overview') =>
    g === 'heating'
      ? LL.SYS_GROUP_HEATING()
      : g === 'plant'
        ? LL.SYS_GROUP_PLANT()
        : LL.SYS_GROUP_OVERVIEW();

  let body: ReactNode;
  if (data.loading && data.items.length === 0) {
    body = <Loader />;
  } else if (data.items.length === 0) {
    body = (
      <Message level={data.error ? 'error' : 'info'}>
        {data.error ?? LL.SYS_NO_DEVICES()}
      </Message>
    );
  } else {
    body = (
      <>
        {groups.map((g) => (
          <section
            key={`${g.title}|${g.section}|${g.items[0]?.devId}`}
            className="section"
          >
            <div className="sys-sec-head">
              {g.title ? `${g.title} · ` : ''}
              {sectionName(g.section)}
            </div>
            {g.items.map((i) => (
              <ValueRow
                key={i.uid}
                item={i}
                edits={edits}
                {...(fav.canToggle(i) && fav.toggle
                  ? { onFavorite: fav.toggle }
                  : {})}
                {...(searching || current === 'fav'
                  ? { where: menuName(i.menu) }
                  : {})}
              />
            ))}
          </section>
        ))}
        {groups.length === 0 && (
          <section className="section">
            <div className="sys-empty">
              {searching
                ? LL.SYS_NO_RESULTS()
                : current === 'fav'
                  ? LL.SYS_FAV_NONE()
                  : LL.SYS_NOTHING_HERE()}
            </div>
          </section>
        )}
        {hidden > 0 && !searching && current !== 'fav' && (
          <p className="sys-hint">
            {LL.SYS_HIDDEN({ num: hidden })} ·{' '}
            <a
              href="#"
              onClick={(e) => {
                e.preventDefault();
                view.setLevel(2);
              }}
            >
              {LL.ALLVALUES()}
            </a>
          </p>
        )}
      </>
    );
  }

  return (
    <>
      <SystemBar page="system" view={view} />
      <div className="sys-layout">
        <nav className="sys-nav">
          {navItem('fav', favs.length)}
          {(['heating', 'plant', 'overview'] as const).map((g) => {
            const ms = menus.filter((m) => m.group === g);
            if (!ms.length) return null;
            return (
              <div key={g}>
                <div className="sys-nav-group">{groupTitle(g)}</div>
                {ms.map((m) => navItem(m.id, visibleCount(m.id)))}
              </div>
            );
          })}
        </nav>
        <div className="sys-main">
          <div className="sys-nav-select">
            <Select
              hideLabel
              fullWidth
              label={LL.SYS_MENU()}
              value={current}
              options={[
                { value: 'fav', label: LL.FAVORITES() },
                ...menus.map((m) => ({ value: m.id, label: menuName(m.id) }))
              ]}
              onChange={(e) => go(e.target.value)}
            />
          </div>
          <h1 className="sys-title" style={{ margin: '0 0 2px' }}>
            {searching ? LL.SYS_SEARCH_RESULTS() : menuName(current)}
          </h1>
          <p className="sys-sub" style={{ margin: '0 0 12px' }}>
            {current === 'fav' && !searching
              ? LL.SYS_FAV_HELP()
              : LL.SYS_MENU_HELP()}
          </p>
          {body}
        </div>
      </div>
      {edits.pending.size > 0 && (
        <ChangesBar
          message={LL.UNSAVED_CHANGES({ num: edits.pending.size })}
          saving={edits.saving}
          onDiscard={edits.discard}
          onSave={() => void edits.apply(LL.WRITE_CMD_SENT())}
        />
      )}
    </>
  );
});

export default SystemMenu;
