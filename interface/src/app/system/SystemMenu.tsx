import { memo, useContext, useEffect, useMemo } from 'react';
import type { ReactNode } from 'react';
import { Link, useNavigate, useParams } from 'react-router';

import { Icon, Listbox, Loader, Message, cx } from 'components/ui';
import type { IconName } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

import { ChangesBar } from '../main/ChangesBar';
import { FaultPanel } from './Faults';
import Home from './Home';
import SensorsOverview, { useSensors } from './SensorsOverview';
import {
  BoilerTile,
  DhwTile,
  HcTile,
  SummaryTile,
  hasSummary,
  heatingCircuits
} from './Tiles';
import { ValueRow } from './ValueRow';
import type { SectionId } from './catalog';
import { buildMenus, groupItems } from './model';
import type { SysItem } from './model';
import { matchEntries, useModuleIndex } from './moduleSearch';
import type { ModulePage } from './moduleSearch';
import { useFavorites } from './useFavorites';
import { useEdits, useSystemData } from './useSystemData';
import { SystemBar, useSystemView } from './view';
import { useViewLevels } from './viewLevels';

// The Dashboard is for looking, not for changing the system: the pages of the sensors, the scheduler, the custom
// entities and the commands stay in the main menu. The search finds their entries and opens these pages.
const MODULE_PAGES: Record<ModulePage, { path: string; icon: IconName }> = {
  sensors: { path: '/sensors', icon: 'sensors' },
  scheduler: { path: '/scheduler', icon: 'more_time' },
  customentities: { path: '/customentities', icon: 'playlist_add' },
  commands: { path: '/commands', icon: 'playlist_play' }
};

const menuIcon = (id: string): IconName => {
  if (/^hc\d$/.test(id)) return 'device_thermostat';
  switch (id) {
    case 'sensors':
      return 'sensors';
    case 'custom':
      return 'playlist_add';
    case 'home':
      return 'assessment';
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
  const { me } = useContext(AuthenticatedContext);
  const view = useSystemView();
  const { level, q } = view;
  // names of sensors, scheduler entries, custom entities and commands, read when the first search starts
  const moduleIndex = useModuleIndex(q.trim() !== '', me.admin);
  const data = useSystemData();
  const edits = useEdits(data);
  const sensors = useSensors();
  const navigate = useNavigate();
  const menu = useParams()['*'];
  const fav = useFavorites(data);
  const favs = fav.items;
  // the values with the level the user chose, for the lists of values
  const lv = useViewLevels(data.items);
  const leveled = lv.items;

  const menus = useMemo(() => buildMenus(data.items), [data.items]);
  const current: string =
    menu && (menu === 'fav' || menu === 'sensors')
      ? menu
      : (menus.find((m) => m.id === menu)?.id ??
        // while the devices load there are no menus yet; keep the page of the address
        (data.items.length === 0 && menu ? menu : 'home'));

  // a page starts at the top, also when it was opened from a tile far down on Home
  useEffect(() => {
    window.scrollTo(0, 0);
  }, [current]);

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
      case 'home':
        return LL.SYS_HOME();
      case 'sensors':
        return LL.SENSORS();
      case 'custom':
        return LL.CUSTOM_ENTITIES(0);
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
    leveled.filter((i) => i.menu === id && (level === 2 || i.level === 1)).length;

  const searching = q.trim() !== '';
  // what else the search finds: the pages of the menu and the entries of the modules
  const needle = q.trim().toLowerCase();
  const pageHits = searching
    ? [...menus.map((m) => m.id), 'sensors'].filter((id) =>
        menuName(id).toLowerCase().includes(needle)
      )
    : [];
  const moduleName = (page: ModulePage): string =>
    page === 'sensors'
      ? LL.SENSORS()
      : page === 'scheduler'
        ? LL.SCHEDULER()
        : page === 'commands'
          ? LL.COMMANDS()
          : LL.CUSTOM_ENTITIES(0);
  const moduleHits = searching ? matchEntries(moduleIndex, needle) : [];
  let list: SysItem[];
  if (searching) {
    const needle = q.trim().toLowerCase();
    list = leveled.filter(
      (i) =>
        (level === 2 || i.level === 1) &&
        `${i.label} ${i.key} ${i.dv.c ?? ''}`.toLowerCase().includes(needle)
    );
  } else if (current === 'fav') {
    list = favs;
  } else {
    list = leveled.filter((i) => i.menu === current);
  }
  const { groups, hidden } = groupItems(
    list,
    searching || current === 'fav' ? 2 : level
  );

  const go = (id: string) => {
    view.setQ('');
    void navigate(`/dashboard/${id}`);
  };

  const navItem = (id: string, count?: number): ReactNode => (
    <button
      key={id}
      type="button"
      className={cx(
        'sys-nav-item',
        current === id && !searching && 'on',
        id === 'fav' && 'fav'
      )}
      onClick={() => go(id)}
    >
      <Icon name={menuIcon(id)} size="sm" />
      <span className="grow">{menuName(id)}</span>
      {count !== undefined && <span className="n">{count}</span>}
    </button>
  );

  const groupTitle = (g: 'heating' | 'plant' | 'overview') =>
    g === 'heating'
      ? LL.SYS_GROUP_HEATING()
      : g === 'plant'
        ? LL.SYS_GROUP_PLANT()
        : LL.SYS_GROUP_OVERVIEW();

  // the tile of the page (as on the Dashboard overview), above its values
  let tiles: ReactNode = null;
  if (heatingCircuits(data.items).includes(current)) {
    tiles = <HcTile tag={current} items={data.items} edits={edits} />;
  } else if (current === 'dhw') {
    tiles = <DhwTile items={data.items} edits={edits} />;
  } else if (current === 'boiler') {
    tiles = <BoilerTile items={data.items} />;
  } else if (hasSummary(current)) {
    tiles = (
      <SummaryTile menu={current} title={menuName(current)} items={data.items} />
    );
  }
  if (tiles) tiles = <div className="sys-tiles sys-tiles-one">{tiles}</div>;

  // pages that are not lists of values: Home and the sensors
  const own = !searching && (current === 'home' || current === 'sensors');

  let body: ReactNode;
  if (!searching && current === 'sensors') {
    body = <SensorsOverview sensors={sensors} />;
  } else if (data.loading && data.items.length === 0) {
    body = <Loader />;
  } else if (data.items.length === 0) {
    body = (
      <Message level={data.error ? 'error' : 'info'}>
        {data.error ?? LL.SYS_NO_DEVICES()}
      </Message>
    );
  } else if (!searching && current === 'home') {
    body = <Home items={data.items} edits={edits} sensors={sensors} />;
  } else {
    body = (
      <>
        {pageHits.length + moduleHits.length > 0 && (
          <section className="section">
            <div className="sys-sec-head">
              <span>{LL.SYS_SEARCH_PLACES()}</span>
            </div>
            {pageHits.map((id) => (
              <button
                key={id}
                type="button"
                className="sys-row sys-hit"
                onClick={() => go(id)}
              >
                <span className="sys-lab">
                  <Icon name={menuIcon(id)} size="sm" />
                  <span className="txt">
                    <b>{menuName(id)}</b>
                  </span>
                </span>
              </button>
            ))}
            {moduleHits.map((e, i) => (
              <button
                key={`${e.page}-${i}`}
                type="button"
                className="sys-row sys-hit"
                onClick={() => {
                  view.setQ('');
                  void navigate(MODULE_PAGES[e.page].path);
                }}
              >
                <span className="sys-lab">
                  <Icon name={MODULE_PAGES[e.page].icon} size="sm" />
                  <span className="txt">
                    <b>{e.name}</b>
                    <small>
                      {moduleName(e.page)}
                      {e.hint ? ` · ${e.hint}` : ''}
                    </small>
                  </span>
                </span>
              </button>
            ))}
          </section>
        )}
        {!searching && tiles}
        {!searching && current === 'service' && <FaultPanel items={data.items} />}
        {groups.map((g) => (
          <section
            key={`${g.title}|${g.section}|${g.items[0]?.devId}`}
            className="section"
          >
            <div className="sys-sec-head">
              <span>
                {g.title ? `${g.title} · ` : ''}
                {sectionName(g.section)}
              </span>
              {g.items[0] && (
                <Link
                  to={`/devices?device=${g.items[0].devId}`}
                  className="sys-sec-link"
                  aria-label={LL.DEVICES()}
                  title={LL.DEVICES()}
                >
                  <Icon name="open_in_new" size="xs" />
                </Link>
              )}
            </div>
            {g.items.map((i) => (
              <ValueRow
                key={i.uid}
                item={i}
                edits={edits}
                {...(fav.canToggle(i) && fav.toggle
                  ? { onFavorite: fav.toggle }
                  : {})}
                {...(current !== 'fav' &&
                // in All values only the values the user moved have the eye, to bring them back
                (level === 1 || lv.changed(i)) &&
                lv.toggle &&
                lv.canToggle(i)
                  ? { onLevel: lv.toggle }
                  : {})}
                {...(searching || current === 'fav'
                  ? { where: menuName(i.menu) }
                  : {})}
              />
            ))}
          </section>
        ))}
        {groups.length === 0 && pageHits.length + moduleHits.length === 0 && (
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
      <SystemBar view={view} showLevel={searching || !(own || current === 'fav')} />
      <div className="sys-layout">
        <nav className="sys-nav">
          {navItem('home')}
          {navItem('fav', favs.length)}
          {(['heating', 'plant', 'overview'] as const).map((g) => {
            const ms = menus.filter((m) => m.group === g);
            if (!ms.length) return null;
            return (
              <div key={g}>
                <div className="sys-nav-group">{groupTitle(g)}</div>
                {ms.map((m) => navItem(m.id, visibleCount(m.id)))}
                {g === 'overview' &&
                  navItem('sensors', sensors.temps.length + sensors.analogs.length)}
              </div>
            );
          })}
        </nav>
        <div className="sys-main">
          <div className="sys-nav-select">
            <Listbox
              hideLabel
              fullWidth
              label={LL.SYS_MENU()}
              value={current}
              options={[
                { value: 'home', label: LL.SYS_HOME() },
                { value: 'fav', label: LL.FAVORITES() },
                ...menus.map((m) => ({ value: m.id, label: menuName(m.id) })),
                { value: 'sensors', label: menuName('sensors') }
              ]}
              onChange={go}
            />
          </div>
          <h1 className="sys-title" style={{ margin: '0 0 2px' }}>
            {searching ? LL.SYS_SEARCH_RESULTS() : menuName(current)}
          </h1>
          <p className="sys-sub" style={{ margin: '0 0 12px' }}>
            {searching
              ? LL.SYS_SEARCH_HELP()
              : current === 'fav'
                ? LL.SYS_FAV_HELP()
                : current === 'home'
                  ? LL.SYS_HOME_HELP()
                  : current === 'sensors'
                    ? LL.SYS_SENSORS_HELP()
                    : LL.SYS_MENU_HELP({ name: menuName(current) })}
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
