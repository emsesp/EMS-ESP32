import { useContext, useState } from 'react';
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
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval, usePersistState } from 'utils';

import { readDashboard, writeDeviceValue } from '../../api/app';
import './Dashboard.css';
import DeviceIcon from './DeviceIcon';
import DevicesDialog from './DevicesDialog';
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

const Dashboard = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  const [showAll, setShowAll] = usePersistState(true, 'showAll');
  // per device open/closed; devices not in here follow showAll
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
      toast.error(String(error.error?.message || 'An error occurred'));
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
        <td className="r num db-val">{value}</td>
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
    const expanded = isOpen(device.id);
    const link = pageLink(device.t, device.id);
    return (
      <section key={device.id} className="db-card">
        <button
          type="button"
          className="db-head"
          aria-expanded={expanded}
          onClick={() => setOpen({ ...open, [device.id]: !expanded })}
        >
          <DeviceIcon type_id={device.t ?? 0} size="sm" className="lb" />
          <span className="db-name">{device.n || typeName(device.t)}</span>
          <span className="lb">({items.length})</span>
          <Icon
            name="chevron_right"
            size="sm"
            className={cx('db-chev', expanded && 'open')}
          />
        </button>
        {expanded && (
          <>
            <Table className="db-tbl">
              <tbody>{items.map((di) => renderValue(device, di))}</tbody>
            </Table>
            {link && (
              <div className="db-foot">
                <Link to={link.to}>
                  {link.label}
                  <Icon name="chevron_right" size="xs" />
                </Link>
              </div>
            )}
          </>
        )}
      </section>
    );
  };

  return (
    <>
      <div className="db-bar">
        <p className="cap grow">{LL.DASHBOARD_1()}</p>
        <ToggleGroup>
          <ToggleButton on={showAll} icon="unfold_more" onClick={() => setAll(true)}>
            {LL.ALLVALUES()}
          </ToggleButton>
          <ToggleButton
            on={!showAll}
            icon="unfold_less"
            onClick={() => setAll(false)}
          >
            {LL.COMPACT()}
          </ToggleButton>
        </ToggleGroup>
      </div>

      {data.connected && !hasFavEntities && (
        <Message level="warning" className="db-msg">
          {LL.NO_DATA_1()} <Link to="/customizations">{LL.CUSTOMIZATIONS()}</Link>{' '}
          {LL.NO_DATA_2()} {LL.NO_DATA_3()} <Link to="/devices">{LL.DEVICES()}</Link>
          .
        </Message>
      )}

      {devices.length > 0 ? (
        <div className="db-cards">{devices.map(renderCard)}</div>
      ) : (
        <p className="tbl-empty">{LL.DASHBOARD_EMPTY()}</p>
      )}

      {selected?.dv && (
        <DevicesDialog
          open
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
