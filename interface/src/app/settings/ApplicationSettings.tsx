import {
  Fragment,
  type ReactNode,
  useCallback,
  useContext,
  useEffect,
  useRef,
  useState
} from 'react';

import { API, getBoardProfile, readSettings, writeSettings } from 'api/app';
import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import { BOARD_PROFILES } from 'app/main/types';
import type {
  APIcall,
  BoardProfile,
  BoardProfileKey,
  Settings
} from 'app/main/types';
import { VALIDATION_LIMITS, createSettingsValidator } from 'app/main/validators';
import {
  Button,
  FormSection,
  Icon,
  type IconName,
  Listbox,
  Loader,
  SearchField,
  Section,
  type SelectOption,
  Switch,
  TextField,
  toast
} from 'components/ui';
import 'components/ui/Chip.css';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

import './ApplicationSettings.css';
import FormPage, { useFormPage } from './FormPage';

// highest GPIO number on the supported chips (ESP32-S3)
const GPIO_MAX = 48;

const ENTITY_LANGUAGES: Record<string, string> = {
  cs: 'Česky',
  de: 'Deutsch',
  en: 'English',
  fr: 'Français',
  it: 'Italiano',
  hu: 'Magyar',
  nl: 'Nederlands',
  no: 'Norsk',
  pl: 'Polski',
  sk: 'Slovenčina',
  sv: 'Svenska',
  tr: 'Türk'
};

const EMS_BUS_IDS: SelectOption[] = [
  [0x0a, 'Terminal (0x0A)'],
  [0x0b, 'Service Key (0x0B)'],
  [0x0d, 'Modem (0x0D)'],
  [0x0e, 'Converter (0x0E)'],
  [0x0f, 'Time Module (0x0F)'],
  [0x48, 'Gateway 1 (0x48)'],
  [0x49, 'Gateway 2 (0x49)'],
  [0x4a, 'Gateway 3 (0x4A)'],
  [0x4b, 'Gateway 4 (0x4B)'],
  [0x4c, 'Gateway 5 (0x4C)'],
  [0x4d, 'Gateway 7 (0x4D)']
].map(([value, label]) => ({ value: String(value), label: String(label) }));

const SYSLOG_LEVELS: SelectOption[] = [
  ['-1', 'OFF'],
  ['3', 'ERR'],
  ['4', 'WARN'],
  ['5', 'NOTICE'],
  ['6', 'INFO'],
  ['9', 'ALL']
].map(([value = '', label = '']) => ({ value, label }));

const opts = (labels: string[], first = 0): SelectOption[] =>
  labels.map((label, i) => ({ value: String(first + i), label }));

const PHY_NAMES = ['LAN8720', 'TLK110', 'RTL8201'];
const ETH_CLOCKS = opts(['GPIO0_IN', 'GPIO0_OUT', 'GPIO16_OUT', 'GPIO17_OUT']);

// firmware returns 205 when one of these changes (ChangeFlags::RESTART in WebSettingsService.cpp)
const RESTART_KEYS = [
  'board_profile',
  'phy_type',
  'eth_power',
  'eth_phy_addr',
  'eth_clock_mode',
  'ems_bus_id',
  'telnet_enabled',
  'low_clock'
] as const;

interface Item {
  key: string;
  label: string;
  help?: string | undefined;
  node: ReactNode;
}

interface Group {
  id: string;
  icon: IconName;
  title: string;
  desc?: string;
  items: (Item | false)[];
}

const ApplicationSettings = () => {
  const { LL } = useI18nContext();
  const {
    data: hardware,
    error: hardwareError,
    send: readHardware
  } = useRequest(readSystemStatus);
  const form = useFormPage<Settings>({
    read: readSettings,
    update: writeSettings,
    validator: createSettingsValidator,
    restartKeys: RESTART_KEYS
  });
  const latest = useRef(form);
  latest.current = form;

  // the drawer reads the system name from the authentication context
  const { setSystemName } = useContext(AuthenticatedContext);
  const savedName = form.origData?.system_name;
  useEffect(() => {
    if (savedName !== undefined) setSystemName(savedName);
  }, [savedName, setSystemName]);

  const [query, setQuery] = useState('');

  // the group cards scroll to just below the sticky search card, whose height depends on wrapping
  const observer = useRef<ResizeObserver>(undefined);
  const indexRef = useCallback((el: HTMLDivElement | null) => {
    observer.current?.disconnect();
    if (!el) return;
    observer.current = new ResizeObserver(() =>
      el.parentElement?.style.setProperty('--app-index-h', `${el.offsetHeight}px`)
    );
    observer.current.observe(el);
  }, []);

  const { send: sendAPI } = useRequest((call: APIcall) => API(call), {
    immediate: false
  });
  const { loading: loadingProfile, send: readBoardProfile } = useRequest(
    (profile: string) => getBoardProfile(profile),
    { immediate: false }
  );

  const changeBoardProfile = (profile: string) => {
    form.set({ board_profile: profile });
    if (profile === 'CUSTOM') return;
    readBoardProfile(profile)
      .then((res) => {
        const p = res as BoardProfile;
        latest.current.set({
          led_gpio: p.led_gpio,
          led_type: p.led_type,
          dallas_gpio: p.dallas_gpio,
          rx_gpio: p.rx_gpio,
          tx_gpio: p.tx_gpio,
          pbutton_gpio: p.pbutton_gpio,
          phy_type: p.phy_type,
          eth_power: p.eth_power,
          eth_phy_addr: p.eth_phy_addr,
          eth_clock_mode: p.eth_clock_mode
        });
      })
      .catch((error: Error) => toast.error(error.message));
  };

  const sendTestEmail = () =>
    sendAPI({
      device: 'system',
      cmd: 'sendmail',
      data: '"Email notification test successful!"',
      id: 0
    })
      .then(() => toast.success(LL.TEST_EMAIL_SUCCESSFUL()))
      .catch((error: Error) => toast.error(error.message));

  const jump = (id: string) =>
    document
      .getElementById(`app-${id}`)
      ?.scrollIntoView({ behavior: 'smooth', block: 'start' });

  type K = keyof Settings;
  const sw = (
    key: K,
    label: string,
    help?: string,
    warn?: boolean,
    disabled?: boolean
  ) => ({
    key,
    label,
    help,
    node: (
      <Switch
        label={label}
        helperText={help}
        warn={!!warn}
        disabled={!!disabled}
        {...form.check(key)}
      />
    )
  });
  const num = (
    key: K,
    label: string,
    extra: {
      unit?: string;
      min?: number;
      max?: number;
      help?: string;
      disabled?: boolean;
    } = {}
  ) => ({
    key,
    label,
    help: extra.help,
    node: (
      <TextField
        label={label}
        unit={extra.unit}
        min={extra.min}
        max={extra.max}
        helperText={extra.help}
        disabled={extra.disabled}
        {...form.number(key)}
      />
    )
  });
  const txt = (
    key: K,
    label: string,
    extra: { mono?: boolean; type?: string } = {}
  ) => ({
    key,
    label,
    node: <TextField label={label} {...extra} {...form.text(key)} />
  });
  const sel = (key: K, label: string, options: SelectOption[]) => ({
    key,
    label,
    node: <Listbox label={label} options={options} {...form.select(key)} />
  });

  const groups = (data: Settings): Group[] => {
    if (!hardware) return [];
    const custom = data.board_profile === 'CUSTOM';
    const noPsram = hardware.psram ? undefined : LL.IS_REQUIRED('PSRAM');
    const gpio = { min: 0, max: GPIO_MAX };
    const port = {
      min: VALIDATION_LIMITS.PORT_MIN,
      max: VALIDATION_LIMITS.PORT_MAX
    };

    const profiles: SelectOption[] = (
      hardware.model.startsWith('BBQKees')
        ? [hardware.board]
        : Object.keys(BOARD_PROFILES)
    ).map((code) => ({
      value: code,
      label: BOARD_PROFILES[code as BoardProfileKey] ?? code
    }));
    if (custom || data.developer_mode) {
      profiles.push({ value: 'CUSTOM', label: `${LL.CUSTOM()}…` });
    }
    if (!profiles.some((p) => p.value === data.board_profile)) {
      profiles.unshift({ value: data.board_profile, label: data.board_profile });
    }

    const pins: [string, ReactNode][] = [
      ['Rx', data.rx_gpio],
      ['Tx', data.tx_gpio],
      [LL.BUTTON(), data.pbutton_gpio],
      [LL.TEMPERATURE(), data.dallas_gpio || '–'],
      ['LED', data.led_gpio || '–'],
      ['PHY', PHY_NAMES[data.phy_type - 1] ?? '–']
    ];

    return [
      {
        id: 'general',
        icon: 'badge',
        title: LL.GENERAL(),
        items: [
          txt('system_name', LL.SYSTEM_NAME()),
          sel(
            'locale',
            LL.LANGUAGE_ENTITIES(),
            Object.entries(ENTITY_LANGUAGES)
              .filter(
                ([code]) =>
                  !hardware.languages ||
                  hardware.languages.includes(code) ||
                  code === data.locale
              )
              .map(([code, name]) => ({
                value: code,
                label: `${name} (${code.toUpperCase()})`
              }))
          ),
          sw('auto_fw_check', LL.AUTO_FW_CHECK()),
          sw('fahrenheit', LL.CONVERT_FAHRENHEIT())
        ]
      },
      {
        id: 'ems',
        icon: 'swap_horiz',
        title: LL.EMS_BUS(0),
        desc: LL.EMS_BUS_DESC(),
        items: [
          sel('tx_mode', LL.TX_MODE(), [
            ...(data.developer_mode ? [{ value: '0', label: LL.OFF() }] : []),
            ...opts(['EMS', 'EMS+', 'HT3', LL.HARDWARE(), LL.AUTO()], 1)
          ]),
          sel('ems_bus_id', LL.ID_OF(LL.EMS_BUS(0)), EMS_BUS_IDS),
          sw('readonly_mode', LL.READONLY()),
          sw('write_protection', LL.WRITE_PROTECTION()),
          data.write_protection &&
            num('write_limit', LL.WRITE_LIMIT(), {
              min: VALIDATION_LIMITS.WRITE_LIMIT_MIN,
              max: VALIDATION_LIMITS.WRITE_LIMIT_MAX
            }),
          data.write_protection &&
            data.write_limit !== 0 &&
            sw('write_limit_refuse', LL.WRITE_LIMIT_REFUSE()),
          sw('boiler_heatingoff', LL.HEATINGOFF()),
          sw('remote_timeout_en', LL.REMOTE_TIMEOUT_EN()),
          data.remote_timeout_en &&
            num('remote_timeout', LL.REMOTE_TIMEOUT(), {
              unit: LL.HOURS(),
              min: VALIDATION_LIMITS.REMOTE_TIMEOUT_MIN,
              max: VALIDATION_LIMITS.REMOTE_TIMEOUT_MAX
            }),
          sw('shower_timer', LL.ENABLE_SHOWER_TIMER()),
          data.shower_timer &&
            num('shower_min_duration', LL.MIN_DURATION(), {
              unit: LL.SECONDS(),
              min: VALIDATION_LIMITS.SHOWER_MIN_DURATION_MIN,
              max: VALIDATION_LIMITS.SHOWER_MIN_DURATION_MAX
            }),
          sw(
            'shower_alert',
            LL.ENABLE_SHOWER_ALERT(),
            undefined,
            false,
            !data.shower_timer
          ),
          data.shower_alert &&
            num('shower_alert_trigger', LL.TRIGGER_TIME(), {
              unit: LL.MINUTES(),
              min: VALIDATION_LIMITS.SHOWER_ALERT_TRIGGER_MIN,
              max: VALIDATION_LIMITS.SHOWER_ALERT_TRIGGER_MAX,
              disabled: !data.shower_timer
            }),
          data.shower_alert &&
            num('shower_alert_coldshot', LL.COLD_SHOT_DURATION(), {
              unit: LL.SECONDS(),
              min: VALIDATION_LIMITS.SHOWER_ALERT_COLDSHOT_MIN,
              max: VALIDATION_LIMITS.SHOWER_ALERT_COLDSHOT_MAX,
              disabled: !data.shower_timer
            })
        ]
      },
      {
        id: 'board',
        icon: 'developer_board',
        title: LL.BOARD(),
        desc: LL.BOARD_DESC(),
        items: [
          {
            key: 'board_profile',
            label: LL.BOARD_PROFILE(),
            node: (
              <Listbox
                label={LL.BOARD_PROFILE()}
                options={profiles}
                fullWidth
                {...form.select('board_profile')}
                disabled={loadingProfile}
                onChange={changeBoardProfile}
              />
            )
          },
          !custom && {
            key: 'board_pins gpio',
            label: LL.BOARD_PINS(),
            node: (
              <div className="field full">
                <span className="app-label">{LL.BOARD_PINS()}</span>
                <div className="app-pins">
                  {pins.map(([name, value]) => (
                    <span key={name} className="app-pin">
                      <span className="cap">{name}</span>
                      {value}
                    </span>
                  ))}
                </div>
              </div>
            )
          },
          custom && num('rx_gpio', LL.GPIO_OF('Rx'), gpio),
          custom && num('tx_gpio', LL.GPIO_OF('Tx'), gpio),
          custom && num('pbutton_gpio', LL.GPIO_OF(LL.BUTTON()), gpio),
          custom &&
            num('dallas_gpio', LL.GPIO_OF(LL.TEMPERATURE()), {
              ...gpio,
              help: LL.ZERO_DISABLES()
            }),
          custom &&
            num('led_gpio', LL.GPIO_OF('LED'), {
              ...gpio,
              help: LL.ZERO_DISABLES()
            }),
          custom &&
            data.led_gpio !== 0 &&
            sel('led_type', LL.LED_TYPE(), opts(['LED', 'RGB-LED'])),
          custom &&
            sel('phy_type', LL.PHY_TYPE(), opts([LL.DISABLED(1), ...PHY_NAMES])),
          custom &&
            data.phy_type !== 0 &&
            num('eth_power', LL.GPIO_OF(LL.PHY_POWER()), {
              min: -1,
              max: GPIO_MAX,
              help: LL.MINUS_ONE_DISABLES()
            }),
          custom &&
            data.phy_type !== 0 &&
            num('eth_phy_addr', LL.ADDRESS_OF('PHY I²C'), { min: 0, max: 31 }),
          custom &&
            data.phy_type !== 0 &&
            sel('eth_clock_mode', LL.PHY_CLOCK(), ETH_CLOCKS)
        ]
      },
      {
        id: 'sensors',
        icon: 'sensors',
        title: LL.SENSORS(),
        items: [
          sw('analog_enabled', LL.ENABLE_ANALOG()),
          data.dallas_gpio !== 0 && sw('dallas_parasite', LL.ENABLE_PARASITE())
        ]
      },
      {
        id: 'format',
        icon: 'text_format',
        title: LL.FORMATTING_OPTIONS(),
        desc: LL.FORMAT_DESC(),
        items: [
          sel('bool_dashboard', LL.BOOLEAN_FORMAT_DASHBOARD(), [
            ...opts([LL.ONOFF(), LL.ONOFF_CAP(), 'true/false'], 1),
            { value: '5', label: '1/0' }
          ]),
          sel(
            'bool_format',
            LL.BOOLEAN_FORMAT_API(),
            opts(
              [
                LL.ONOFF(),
                LL.ONOFF_CAP(),
                '"true"/"false"',
                'true/false',
                '"1"/"0"',
                '1/0'
              ],
              1
            )
          ),
          sel('enum_format', LL.ENUM_FORMAT(), opts([LL.VALUE(5), LL.INDEX()], 1))
        ]
      },
      {
        id: 'services',
        icon: 'hub',
        title: LL.SERVICES(),
        items: [
          sw('telnet_enabled', LL.ENABLE_TELNET()),
          sw('modbus_enabled', LL.ENABLE_MODBUS(), noPsram, false, !hardware.psram),
          data.modbus_enabled && num('modbus_port', LL.PORT(), port),
          data.modbus_enabled &&
            num('modbus_max_clients', LL.AP_MAX_CLIENTS(), {
              min: VALIDATION_LIMITS.MODBUS_MAX_CLIENTS_MIN,
              max: VALIDATION_LIMITS.MODBUS_MAX_CLIENTS_MAX
            }),
          data.modbus_enabled &&
            num('modbus_timeout', LL.TIMEOUT(), {
              unit: LL.SECONDS(),
              min: VALIDATION_LIMITS.MODBUS_TIMEOUT_MIN,
              max: VALIDATION_LIMITS.MODBUS_TIMEOUT_MAX
            }),
          sw('syslog_enabled', LL.ENABLE_SYSLOG()),
          data.syslog_enabled && txt('syslog_host', LL.HOST(), { mono: true }),
          data.syslog_enabled && num('syslog_port', LL.PORT(), port),
          data.syslog_enabled && sel('syslog_level', LL.LOG_LEVEL(), SYSLOG_LEVELS),
          data.syslog_enabled &&
            num('syslog_mark_interval', LL.MARK_INTERVAL(), {
              unit: LL.SECONDS(),
              min: VALIDATION_LIMITS.SYSLOG_MARK_INTERVAL_MIN,
              max: VALIDATION_LIMITS.SYSLOG_MARK_INTERVAL_MAX
            }),
          sw('email_enabled', LL.ENABLE_EMAIL(), noPsram, false, !hardware.psram),
          data.email_enabled &&
            txt('email_server', LL.SMTP_SERVER(), { mono: true }),
          data.email_enabled && num('email_port', LL.PORT(), port),
          data.email_enabled &&
            sel(
              'email_security',
              LL.SECURITY(0),
              opts([LL.OFF(), 'SSL', 'StartTLS'])
            ),
          data.email_enabled && txt('email_login', LL.USERNAME(0)),
          data.email_enabled &&
            txt('email_pass', LL.PASSWORD(), { type: 'password' }),
          data.email_enabled && txt('email_sender', LL.EMAIL_FROM()),
          data.email_enabled && txt('email_recp', LL.EMAIL_TO()),
          data.email_enabled && txt('email_subject', LL.EMAIL_SUBJECT()),
          data.email_enabled && {
            key: 'email_test',
            label: LL.SEND_TEST_EMAIL(),
            node: (
              <div className="field app-test">
                <Button
                  icon="send"
                  disabled={form.dirtyFlags.length !== 0}
                  onClick={() => void sendTestEmail()}
                >
                  {LL.SEND_TEST_EMAIL()}
                </Button>
              </div>
            )
          }
        ]
      },
      {
        id: 'advanced',
        icon: 'science',
        title: LL.ADVANCED(),
        items: [
          sw('trace_raw', LL.LOG_HEX()),
          data.led_gpio !== 0 && sw('hide_led', LL.HIDE_LED()),
          sw('low_clock', LL.UNDERCLOCK_CPU()),
          sw('developer_mode', LL.DEVELOPER_MODE()),
          sw('disable_reset', LL.DISABLE_RESET())
        ]
      }
    ];
  };

  const q = query.trim().toLowerCase();
  // keys match by word start, so "led" finds led_gpio but not telnet_enabled
  const matches = (item: Item) =>
    !q ||
    `${item.label} ${item.help ?? ''}`.toLowerCase().includes(q) ||
    (q.includes('_')
      ? item.key.includes(q)
      : item.key.split(/[_ ]/).some((w) => w.startsWith(q)));

  return (
    <FormPage form={form}>
      {(data) => {
        if (!hardware) {
          return (
            <Loader
              error={hardwareError?.message}
              onRetry={() => void readHardware()}
            />
          );
        }
        const shown = groups(data).map((g) => ({
          ...g,
          items: g.items.filter((i): i is Item => !!i && matches(i))
        }));
        return (
          <div>
            <div ref={indexRef} className="section app-index">
              <SearchField
                label={LL.SEARCH_SETTINGS()}
                value={query}
                onChange={(e) => setQuery(e.currentTarget.value)}
              />
              <nav className="app-jumps">
                {shown.map((g) => (
                  <button
                    key={g.id}
                    type="button"
                    className="chip"
                    disabled={!g.items.length}
                    onClick={() => jump(g.id)}
                  >
                    <Icon name={g.icon} size="xs" />
                    {g.title}
                  </button>
                ))}
              </nav>
            </div>
            {shown.map(
              (g) =>
                g.items.length > 0 && (
                  <FormSection
                    key={g.id}
                    id={`app-${g.id}`}
                    className="app-group"
                    icon={g.icon}
                    title={g.title}
                    desc={g.desc}
                  >
                    {g.items.map((i) => (
                      <Fragment key={i.key}>{i.node}</Fragment>
                    ))}
                  </FormSection>
                )
            )}
            {shown.every((g) => !g.items.length) && (
              <Section className="muted">{LL.NO_SETTINGS_MATCH()}</Section>
            )}
          </div>
        );
      }}
    </FormPage>
  );
};

export default ApplicationSettings;
