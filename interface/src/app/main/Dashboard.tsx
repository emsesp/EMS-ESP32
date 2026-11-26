import { Fragment, type ReactNode, useContext, useState } from 'react';
import { Link } from 'react-router';

import { useRequest } from 'alova/client';
import {
  Icon,
  Loader,
  Message,
  Table,
  ToggleButton,
  ToggleGroup,
  cx,
  rowAction,
  toast
} from 'components/ui';
import 'components/ui/Chip.css';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval, usePersistState } from 'utils';

import { readDashboard, writeDeviceValue } from '../../api/app';
import './Dashboard.css';
import DeviceIcon from './DeviceIcon';
import DevicesDialog from './DevicesDialog';
import EmsBusWarning from './EmsBusWarning';
import { formatValue } from './deviceValue';
import {
  type DashboardItem,
  DeviceEntityMask,
  DeviceType,
  type DeviceValue
} from './types';
import { deviceValueItemValidation } from './validators';

const hasMask = (id: string, mask: number) =>
  (parseInt(id.slice(0, 2), 16) & mask) === mask;

// a value without v is a command that can only be run
const isCommand = (dv: DeviceValue) => dv.v === '' || dv.v === undefined;

// values a card shows in Summary mode
const PEEK = 3;

// puts the links where the translated sentence has their marker characters
const withLinks = (text: string, links: Record<string, ReactNode>) =>
  text
    .split(/([\uE000\uE001])/)
    .map((part, i) => <Fragment key={i}>{links[part] ?? part}</Fragment>);

// the firmware's boolean renderings (bool_dashboard setting, all 12 languages)
const BOOL_PAIRS = new Set([
  'off|on',
  'aus|an',
  'uit|aan',
  'av|på',
  'wyłączono|włączono',
  'wył.|wł.',
  'kapalı|açık',
  'vyp|zap',
  'ki|be',
  'false|true',
  '0|1'
]);

// writeable booleans come with their two renderings in l, off first
const boolState = (dv: DeviceValue) => {
  const l = dv.l;
  if (l?.length !== 2 || !BOOL_PAIRS.has(l.join('|').toLowerCase())) return;
  const v = String(dv.v);
  return v === l[1] ? true : v === l[0] ? false : undefined;
};

const enum Group {
  DEVICES,
  SENSORS,
  AUTOMATION
}

const groupOf = (t?: number) => {
  switch (t) {
    case DeviceType.TEMPERATURESENSOR:
    case DeviceType.ANALOGSENSOR:
      return Group.SENSORS;
    case DeviceType.SCHEDULER:
    case DeviceType.COMMAND:
      return Group.AUTOMATION;
    default:
      return Group.DEVICES;
  }
};

const Dashboard = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  const [showAll, setShowAll] = usePersistState(true, 'showAll');
  // per device all values or summary; devices not in here follow showAll
  const [open, setOpen] = useState<Record<number, boolean>>({});
  const [deviceCount, setDeviceCount] = useState(0);
  const [selected, setSelected] = useState<DashboardItem>();

  const {
    data,
    send: fetchDashboard,
    error
  } = useRequest(readDashboard).onSuccess((event) => {
    if (event.data.nodes.length !== deviceCount) {
      setDeviceCount(event.data.nodes.length);
      setOpen({});
    }
  });

  const { loading: submitting, send: sendDeviceValue } = useRequest(
    (data: { id: number; c: string; v: unknown }) => writeDeviceValue(data),
    { immediate: false }
  )
    .onSuccess(() => {
      toast.success(LL.WRITE_CMD_SENT());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || LL.ERROR()));
    });

  useInterval(() => {
    if (!selected) {
      void fetchDashboard();
    }
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void fetchDashboard()} />;
  }

  const deviceValueDialogSave = async (dv: DeviceValue) => {
    if (!selected) {
      return;
    }
    // a command run from the dialog is sent by DevicesDialog itself
    if (dv.v !== undefined) {
      await sendDeviceValue({ id: selected.parentNode.id, c: dv.c ?? '', v: dv.v });
    }
    setSelected(undefined);
    void fetchDashboard();
  };

  const canEdit = (dv?: DeviceValue): dv is DeviceValue =>
    !!(me.admin && dv?.c && !hasMask(dv.id, DeviceEntityMask.DV_READONLY));

  const setAll = (value: boolean) => {
    setShowAll(value);
    setOpen({});
  };

  const isOpen = (id: number) => open[id] ?? showAll;

  const typeName = (t?: number) => {
    switch (t) {
      case DeviceType.CUSTOM:
        return LL.CUSTOM_ENTITIES(0);
      case DeviceType.ANALOGSENSOR:
        return LL.ANALOG_SENSORS();
      case DeviceType.TEMPERATURESENSOR:
        return LL.TEMP_SENSORS();
      case DeviceType.COMMAND:
        return LL.COMMANDS();
      case DeviceType.SCHEDULER:
        return LL.SCHEDULER();
      default:
        return '';
    }
  };

  const pageLink = (
    t: number | undefined,
    id: number
  ): { to: string; label: string } | undefined => {
    switch (t) {
      case DeviceType.CUSTOM:
        return me.admin
          ? { to: '/customentities', label: LL.CUSTOM_ENTITIES(0) }
          : undefined;
      case DeviceType.ANALOGSENSOR:
      case DeviceType.TEMPERATURESENSOR:
        return { to: '/sensors', label: LL.SENSORS() };
      case DeviceType.COMMAND:
        return me.admin ? { to: '/commands', label: LL.COMMANDS() } : undefined;
      case DeviceType.SCHEDULER:
        return me.admin ? { to: '/scheduler', label: LL.SCHEDULER() } : undefined;
      default:
        return { to: `/devices?device=${id}`, label: LL.DEVICES() };
    }
  };

  const devices = data.nodes.filter((di) => di.nodes?.length);
  const hasFavEntities = data.nodes.some((di) => di.id <= 90);

  const renderValue = (device: DashboardItem, di: DashboardItem) => {
    const dv = di.dv;
    if (!dv) {
      return null;
    }
    const name = dv.id.slice(2);
    const value = formatValue(LL, dv.v, dv.u);
    const editable = canEdit(dv);
    const command = editable && isCommand(dv);
    const locked = me.admin && hasMask(dv.id, DeviceEntityMask.DV_READONLY);
    const on = boolState(dv);
    return (
      <tr
        key={di.id}
        {...(editable
          ? rowAction(
              () => setSelected({ ...di, parentNode: device }),
              command
                ? `${LL.RUN()}: ${name}`
                : `${LL.CHANGE_VALUE()}: ${name} = ${value}`
            )
          : {})}
      >
        <td>{name}</td>
        <td className="r num db-val">
          {on === undefined ? (
            value
          ) : (
            <span className={cx('switch', 'db-sw', on && 'on')} aria-hidden="true" />
          )}
        </td>
        <td className="db-act">
          {editable && (
            <Icon name={command ? 'play_arrow' : 'edit'} size="xs" className="lb" />
          )}
          {locked && <Icon name="lock" size="xs" label={LL.MASK_READONLY()} />}
        </td>
      </tr>
    );
  };

  const renderCard = (device: DashboardItem) => {
    const items = device.nodes ?? [];
    const full = isOpen(device.id);
    const isCommands = device.t === DeviceType.COMMAND;
    // the summary leaves out commands and empty values
    const peek = items.filter((di) => di.dv && !isCommand(di.dv)).slice(0, PEEK);
    const hidden = isCommands ? 0 : items.length - peek.length;
    const shown = hidden && !full ? peek : items;
    const link = pageLink(device.t, device.id);
    const name = device.n || typeName(device.t);
    return (
      <section key={device.id} className="db-card">
        <div className={cx('db-top', shown.length > 0 && 'open')}>
          <div className="db-head">
            <DeviceIcon type_id={device.t ?? 0} size="sm" className="lb" />
            <span className="db-name" title={name}>
              {name}
            </span>
            <span className="count primary">{items.length}</span>
          </div>
          {link && (
            <Link
              to={link.to}
              className="db-link"
              aria-label={link.label}
              title={link.label}
            >
              <Icon name="open_in_new" size="xs" />
            </Link>
          )}
        </div>
        {isCommands ? (
          <div className="db-chips">
            {items.map((di) => renderCommand(device, di))}
          </div>
        ) : (
          shown.length > 0 && (
            <Table className="db-tbl">
              <tbody>{shown.map((di) => renderValue(device, di))}</tbody>
            </Table>
          )
        )}
        {hidden > 0 && !(full && showAll) && (
          <button
            type="button"
            className="db-more"
            aria-expanded={full}
            onClick={() => setOpen({ ...open, [device.id]: !full })}
          >
            {full ? LL.SHOW_LESS() : LL.SHOW_MORE({ n: hidden })}
          </button>
        )}
      </section>
    );
  };

  const renderCommand = (device: DashboardItem, di: DashboardItem) => {
    const name = di.dv?.id.slice(2) ?? '';
    return canEdit(di.dv) ? (
      <button
        key={di.id}
        type="button"
        className="chip outline"
        aria-label={`${LL.RUN()}: ${name}`}
        onClick={() => setSelected({ ...di, parentNode: device })}
      >
        <Icon name="play_arrow" size="xs" className="lb" />
        {name}
      </button>
    ) : (
      <span key={di.id} className="chip outline">
        {name}
      </span>
    );
  };

  const groups = [
    { g: Group.DEVICES, title: LL.DEVICES() },
    { g: Group.SENSORS, title: LL.SENSORS() },
    { g: Group.AUTOMATION, title: LL.AUTOMATION() }
  ]
    .map(({ g, title }) => ({
      title,
      items: devices.filter((d) => groupOf(d.t) === g)
    }))
    .filter(({ items }) => items.length > 0);

  return (
    <>
      <div className="db-bar">
        <p className="cap grow">{LL.DASHBOARD_1()}</p>
        <ToggleGroup>
          <ToggleButton
            on={!showAll}
            icon="unfold_less"
            onClick={() => setAll(false)}
          >
            {LL.SUMMARY()}
          </ToggleButton>
          <ToggleButton on={showAll} icon="unfold_more" onClick={() => setAll(true)}>
            {LL.ALLVALUES()}
          </ToggleButton>
        </ToggleGroup>
      </div>

      {!data.connected && <EmsBusWarning className="db-msg" />}
      {data.connected && !hasFavEntities && (
        <Message level="warning" className="db-msg">
          {withLinks(LL.NO_DATA({ customizations: '\uE000', devices: '\uE001' }), {
            '\uE000': <Link to="/customizations">{LL.CUSTOMIZATIONS()}</Link>,
            '\uE001': <Link to="/devices">{LL.DEVICES()}</Link>
          })}
        </Message>
      )}

      {devices.length > 0 ? (
        groups.map(({ title, items }) => (
          <section key={title} className="db-group" aria-label={title}>
            <h2 className="db-gtitle">{title}</h2>
            <div className="db-cards">{items.map(renderCard)}</div>
          </section>
        ))
      ) : (
        <p className="tbl-empty">{LL.DASHBOARD_EMPTY()}</p>
      )}

      {selected?.dv && (
        <DevicesDialog
          onClose={() => setSelected(undefined)}
          onSave={deviceValueDialogSave}
          selectedItem={selected.dv}
          writeable={true}
          validator={deviceValueItemValidation(selected.dv)}
          progress={submitting}
        />
      )}
    </>
  );
};

export default Dashboard;
