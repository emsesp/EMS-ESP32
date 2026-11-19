import { type FC, type ReactNode, useState } from 'react';

import { type MenuGroup, menuRoutes, routeTrail, routes } from 'app/routes';
import {
  Button,
  ButtonLink,
  Checkbox,
  Chip,
  Dialog,
  FormSection,
  Hero,
  Icon,
  IconButton,
  type IconName,
  KeyValueList,
  LanguageGrid,
  LanguageSelect,
  LinkList,
  Listbox,
  Loader,
  Message,
  Meter,
  ProgressBar,
  SaveBar,
  SearchField,
  Section,
  Segmented,
  Select,
  Stat,
  StatRow,
  Switch,
  TabNav,
  Table,
  Tag,
  TextArea,
  TextField,
  Tile,
  Tiles,
  ToggleButton,
  ToggleGroup,
  toast
} from 'components/ui';
import { icons } from 'components/ui/icons';
import { useI18nContext } from 'i18n/i18n-react';

import './Gallery.css';

// Dev-only page at /ui (see index.tsx). Add a Sample for every new component in components/ui.

const TOKENS = [
  'bg',
  'paper',
  'paper-2',
  'drawer',
  'border',
  'divider',
  'text',
  'text-2',
  'text-3',
  'primary',
  'secondary',
  'info',
  'success',
  'success-main',
  'warning',
  'error',
  'lightblue',
  'appbar',
  'row',
  'row-odd',
  'row-hover'
];

const MENU_GROUPS: MenuGroup[] = ['main', 'modules', 'system', 'account'];

const LISTBOX_SAMPLE: { value: string; label: string; icon: IconName; n: number }[] =
  [
    {
      value: '8',
      label: 'GBx72/Trendline/Cerapur/Greenstar Si',
      icon: 'local_fire_department',
      n: 132
    },
    {
      value: '16',
      label: 'RC310/Moduline 3000/1010H/CW400',
      icon: 'thermostat',
      n: 87
    },
    { value: '32', label: 'MM100', icon: 'valve', n: 9 },
    { value: '48', label: 'SM100', icon: 'solar_power', n: 0 }
  ];

const ListboxSample = () => {
  const [value, setValue] = useState<string>();
  return (
    <Listbox
      label="EMS Device"
      placeholder="Select a device…"
      value={value}
      onChange={setValue}
      options={LISTBOX_SAMPLE.map((o) => ({
        value: o.value,
        label: o.label,
        icon: <Icon name={o.icon} />,
        meta: o.n,
        disabled: o.n === 0
      }))}
    />
  );
};

const TRAIL_EXAMPLES = [
  '/dashboard',
  '/status/log',
  '/settings/network/scan',
  '/settings/security/apikeys'
];

const Sample: FC<{ title: string; children: ReactNode }> = ({ title, children }) => (
  <section className="gal-sample">
    <h2 className="h6">{title}</h2>
    {children}
  </section>
);

const COLORS = ['primary', 'secondary', 'warning', 'error'] as const;

const Gallery = () => {
  const { LL } = useI18nContext();
  const [dialogOpen, setDialogOpen] = useState(false);
  const [text, setText] = useState('ems-esp');
  const [toggle, setToggle] = useState(true);
  const [seg, setSeg] = useState('lost');

  return (
    <div className="gal">
      <h1 className="gal-title">UI components</h1>
      <p className="cap">
        Dev-only gallery of components/ui. Not part of the production build.
      </p>

      <Sample title="Tokens">
        <div className="gal-grid">
          {TOKENS.map((t) => (
            <div key={t} className="row">
              <span className="gal-swatch" style={{ background: `var(--${t})` }} />
              <span className="mono">--{t}</span>
            </div>
          ))}
        </div>
      </Sample>

      <Sample title="Text and layout utilities">
        <div className="row wrap">
          <span className="h6">.h6 heading</span>
          <span>body 13px</span>
          <span className="cap">.cap caption</span>
          <span className="mono">.mono 192.168.1.4</span>
          <span className="muted">.muted</span>
          <span className="lb">.lb lightblue</span>
        </div>
        <div className="row gal-demo">
          <span>.row</span>
          <span className="grow gal-fill">.grow</span>
          <span>end</span>
        </div>
      </Sample>

      <Sample title={`Icon (${Object.keys(icons).length})`}>
        <div className="row wrap">
          <Icon name="settings" />
          <Icon name="settings" size="sm" />
          <Icon name="settings" size="xs" />
          <span className="gal-big">
            <Icon name="settings" />
          </span>
          <span className="lb">
            <Icon name="info" label="Information" />
          </span>
          <span className="cap">
            default 22px, sm, xs, 36px via .parent .icon, colour from parent, with
            label
          </span>
        </div>
        <div className="gal-grid gal-icons">
          {(Object.keys(icons) as IconName[]).map((name) => (
            <div key={name} className="row">
              <Icon name={name} />
              <span className="cap">{name}</span>
            </div>
          ))}
        </div>
      </Sample>

      <Sample title="Button, IconButton">
        <div className="row wrap gal-demo">
          {COLORS.map((color) => (
            <Button key={color} color={color} icon="check">
              {color}
            </Button>
          ))}
          <Button size="sm" icon="undo">
            small
          </Button>
          <Button icon="check" disabled>
            disabled
          </Button>
        </div>
        <div className="row gal-demo">
          <IconButton icon="settings" label="Settings" />
          <IconButton icon="close" label="Close" plain />
          <IconButton icon="delete_forever" label="Delete" disabled />
        </div>
      </Sample>

      <Sample title="TextField, Select, LanguageSelect, Listbox">
        <div className="gal-grid">
          <TextField
            name="text"
            label="Hostname"
            helperText="shown in the menu"
            value={text}
            onInput={(e) => setText(e.currentTarget.value)}
          />
          <TextField
            name="err"
            label="Port"
            value="99999"
            error="must be 1 to 65535"
          />
          <TextField
            name="pw"
            type="password"
            label={LL.PASSWORD()}
            value="secret"
          />
          <TextField name="off" label="Disabled" value="read only" disabled />
          <Select
            label="Tx mode"
            options={['EMS', 'EMS+', 'HT3', 'Hardware'].map((v) => ({
              value: v,
              label: v
            }))}
          />
          <LanguageSelect />
          <ListboxSample />
        </div>
      </Sample>

      <Sample title="FormSection, Switch, Checkbox, Segmented, TextArea, field flags (settings forms)">
        <FormSection
          icon="tune"
          title="Behaviour"
          desc="A line of text under the title."
        >
          <TextField
            name="port"
            label="Port"
            type="number"
            unit="s"
            value="1883"
            changed
            restart
          />
          <TextField
            name="ssid"
            label="SSID"
            mono
            value="home"
            action={
              <Button size="sm" icon="search">
                Scan
              </Button>
            }
          />
          <Switch
            label="Enable MQTT"
            helperText="a help line under the label"
            checked={toggle}
            onChange={setToggle}
            changed={!toggle}
          />
          <Switch
            label="Read-only mode"
            helperText="a warning help line"
            warn
            checked={false}
            onChange={() => undefined}
          />
          <Switch label="Disabled" checked disabled onChange={() => undefined} />
          <Segmented
            label="Start the Access Point"
            fullWidth
            options={[
              { value: 'always', label: 'Always' },
              { value: 'lost', label: 'When the network is lost' },
              { value: 'never', label: 'Never' }
            ]}
            value={seg}
            onChange={setSeg}
          />
          <TextArea
            name="ca"
            label="Root CA"
            placeholder="-----BEGIN CERTIFICATE-----"
          />
          <Checkbox label="A checkbox" checked={toggle} onChange={setToggle} />
        </FormSection>
      </Sample>

      <Sample title="LinkList, Tag, TabNav, LanguageGrid">
        <TabNav
          items={[
            { to: '/ui', label: 'This page', icon: 'settings' },
            { to: '/settings', label: 'Settings', icon: 'tune' }
          ]}
        />
        <div className="row wrap gal-demo">
          {(
            [
              'plain',
              'muted',
              'new',
              'mod',
              'sys',
              'type',
              'ok',
              'warn',
              'err'
            ] as const
          ).map((k) => (
            <Tag key={k} kind={k}>
              {k}
            </Tag>
          ))}
          <Tag icon="lock">with icon</Tag>
        </div>
        <LinkList
          items={[
            {
              to: '/settings',
              icon: 'settings',
              title: 'Grid card',
              text: 'a route',
              badge: true
            },
            {
              href: 'https://emsesp.org',
              icon: 'menu_book',
              color: '#72caf9',
              title: 'External',
              text: 'opens a new tab',
              note: 'emsesp.org'
            }
          ]}
        />
        <LinkList
          layout="rows"
          items={[
            {
              to: '/settings/ap',
              icon: 'wifi_tethering',
              title: 'Rows',
              text: 'one card'
            },
            {
              to: '/settings/ntp',
              icon: 'schedule',
              color: '#c5572c',
              title: 'NTP',
              value: 'value at the right',
              state: 'ok'
            }
          ]}
        />
        <LanguageGrid />
      </Sample>

      <Sample title="Message">
        <div className="grid">
          <Message level="info">Info message.</Message>
          <Message
            level="warning"
            actions={
              <Button icon="close" color="warning">
                {LL.CLOSE()}
              </Button>
            }
          >
            Warning with an action.
          </Message>
          <Message level="error">Error message.</Message>
          <Message level="success">Success message.</Message>
        </div>
      </Sample>

      <Sample title="Dialog, toast">
        <div className="row wrap">
          <Button icon="open_in_new" onClick={() => setDialogOpen(true)}>
            Open dialog
          </Button>
          <Button
            icon="check_circle"
            color="secondary"
            onClick={() => toast.success(LL.UPDATED_OF(LL.SETTINGS(1)))}
          >
            success
          </Button>
          <Button
            icon="info"
            color="secondary"
            onClick={() => toast.info('Information')}
          >
            info
          </Button>
          <Button
            icon="warning"
            color="warning"
            onClick={() => toast.warning(LL.INVALID_LOGIN())}
          >
            warning
          </Button>
          <Button
            icon="error"
            color="error"
            onClick={() => toast.error('Something failed')}
          >
            error
          </Button>
        </div>
        <Dialog
          open={dialogOpen}
          title={LL.BLOCK_NAVIGATE_1()}
          onClose={() => setDialogOpen(false)}
          actions={
            <>
              <Button
                icon="cancel"
                color="secondary"
                onClick={() => setDialogOpen(false)}
              >
                {LL.CANCEL()}
              </Button>
              <Button icon="check" onClick={() => toast.info('Toast above dialog')}>
                Toast
              </Button>
            </>
          }
        >
          <p>{LL.BLOCK_NAVIGATE_2()}</p>
          <div className="form">
            <TextField name="d" label="Field in a dialog" fullWidth />
          </div>
        </Dialog>
      </Sample>

      <Sample title="SaveBar (inline here; in the app it docks to the bottom of the page)">
        <SaveBar message={LL.BLOCK_NAVIGATE_1()}>
          <Button color="secondary" icon="undo">
            {LL.RESET()}
          </Button>
          <Button icon="check">{LL.APPLY_CHANGES(2)}</Button>
        </SaveBar>
        <SaveBar warning icon="restart_alt" message={LL.RESTART_TEXT(0)}>
          <Button color="warning" icon="restart_alt">
            {LL.RESTART()}
          </Button>
        </SaveBar>
      </Sample>

      <Sample title="Hero, Section, KeyValueList, ButtonLink (status pages)">
        <Hero
          state="ok"
          icon="check_circle"
          title={LL.STATUS_RUNNING()}
          text="E32V2 · 2 days 4 hours"
          actions={
            <ButtonLink to="/ui" color="secondary" icon="settings">
              {LL.SETTINGS(0)}
            </ButtonLink>
          }
        />
        <Hero state="warn" icon="warning" title={LL.TX_ISSUES()} />
        <Hero state="err" icon="error" title={LL.DISCONNECTED()} />
        <Hero state="off" icon="wifi_tethering_off" title={LL.INACTIVE(0)} />
        <Section
          icon="info"
          title={LL.DETAILS()}
          actions={<Chip state="ok">live</Chip>}
        >
          <KeyValueList
            rows={[
              { label: LL.HOSTNAME(), value: 'ems-esp' },
              { label: 'SSID', value: 'home', extra: '-41 dBm' },
              false,
              { label: LL.NETWORK_DNS(), value: '10.10.10.1' }
            ]}
          />
        </Section>
      </Sample>

      <Sample title="Tiles, StatusDot, Chip">
        <Tiles>
          <Tile
            to="/ui"
            icon="memory"
            color="#72caf9"
            title={LL.HARDWARE()}
            text="143 KB"
            state="ok"
          />
          <Tile
            to="/ui"
            icon="new_releases"
            color="#c5572c"
            title="Firmware"
            text="v3.10.0"
            state="warn"
            badge
          />
          <Tile
            to="/ui"
            icon="list_alt"
            color="#40828f"
            title={LL.LOG_OF(LL.SYSTEM(0))}
            text={LL.NUM_ERRORS({ num: 2 })}
            state="err"
          />
        </Tiles>
        <div className="row wrap">
          {(['ok', 'warn', 'err', 'off'] as const).map((s) => (
            <Chip key={s} state={s}>
              {s}
            </Chip>
          ))}
        </div>
      </Sample>

      <Sample title="Meter, ProgressBar, StatRow">
        <div className="meters">
          <Meter label={LL.HEAP()} used={169} total={312} unit="KB" />
          <Meter label="PSRAM" used={3900} total={4096} unit="KB" />
          <Meter
            label={LL.FILE_SYSTEM()}
            used={24}
            total={2048}
            unit="KB"
            note="note"
          />
        </div>
        <div className="row">
          <ProgressBar className="grow" value={100} color="success" />
          <ProgressBar className="grow" value={95} color="warning" />
          <ProgressBar className="grow" value={20} color="error" />
        </div>
        <StatRow>
          <Stat value="1" label={LL.MQTT_QUEUE()} />
          <Stat value="3" label={LL.ERRORS_OF('MQTT')} warn />
        </StatRow>
      </Sample>

      <Sample title="ToggleGroup, SearchField, Table, Loader">
        <div className="row wrap">
          <ToggleGroup>
            <ToggleButton
              on={toggle}
              icon="density_small"
              onClick={() => setToggle(!toggle)}
            >
              {LL.COMPACT()}
            </ToggleButton>
            <ToggleButton
              on={!toggle}
              icon="pause"
              onClick={() => setToggle(!toggle)}
            >
              {LL.PAUSE()}
            </ToggleButton>
          </ToggleGroup>
          <SearchField label={LL.FILTER_LINES()} />
        </div>
        <Table>
          <thead>
            <tr>
              <th>{LL.NAME(0)}</th>
              <th className="r">{LL.SUCCESS()}</th>
            </tr>
          </thead>
          <tbody>
            <tr>
              <td>EMS Reads (Tx)</td>
              <td className="r num">9,026</td>
            </tr>
            <tr>
              <td>EMS Writes (Tx)</td>
              <td className="r num">33</td>
            </tr>
          </tbody>
        </Table>
        <Loader />
        <Loader error="Problem loading" onRetry={() => toast.info(LL.RETRY())} />
      </Sample>

      <Sample title="Route table: drawer groups">
        <div className="gal-grid">
          {MENU_GROUPS.map((group) => (
            <div key={group}>
              <div className="lb">{group}</div>
              {menuRoutes(group).map((r) => (
                <div key={r.path} className="row gal-nav">
                  <Icon name={r.icon} className="muted" />
                  <span>{r.title(LL)}</span>
                  {r.admin && <span className="cap">admin</span>}
                  {r.badge && <span className="gal-dot" title={r.badge} />}
                </div>
              ))}
            </div>
          ))}
        </div>
      </Sample>

      <Sample title="Route table: breadcrumbs">
        {TRAIL_EXAMPLES.map((path) => (
          <div key={path} className="row gal-nav">
            <span className="mono gal-path">{path}</span>
            <span>
              {routeTrail(path)
                .map((r) => r.title(LL))
                .join(' › ')}
            </span>
          </div>
        ))}
      </Sample>

      <Sample title={`Route table: all routes (${routes.length})`}>
        <div className="gal-scroll">
          <table className="gal-table">
            <thead>
              <tr>
                <th>path</th>
                <th>title</th>
                <th>icon</th>
                <th>parent</th>
                <th>menu</th>
                <th>admin</th>
                <th>badge</th>
              </tr>
            </thead>
            <tbody>
              {routes.map((r) => (
                <tr key={r.path}>
                  <td className="mono">
                    {r.path}
                    {r.wildcard && '/*'}
                  </td>
                  <td>{r.title(LL)}</td>
                  <td>
                    <Icon name={r.icon} size="sm" />{' '}
                    <span className="cap">{r.icon}</span>
                  </td>
                  <td className="mono">{r.parent}</td>
                  <td>{r.menu}</td>
                  <td>{r.admin && 'yes'}</td>
                  <td>{r.badge}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </Sample>
    </div>
  );
};

export default Gallery;
