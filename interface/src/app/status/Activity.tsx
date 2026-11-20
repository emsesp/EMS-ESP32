import { useContext } from 'react';

import { readActivity } from 'api/app';
import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import {
  ButtonLink,
  Hero,
  Loader,
  ProgressBar,
  Section,
  SortHeader,
  Table,
  byNumber,
  useTableSort
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { Translation } from 'i18n/i18n-types';
import { useInterval } from 'utils';

import type { Stat } from '../main/types';
import './Status.css';
import { busInfo, nf } from './status';

const qualityColor = (q: number) =>
  q === 100 ? 'success' : q >= 95 ? 'warning' : 'error';

const Activity = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);
  const { data, send: loadData, error } = useRequest(readActivity);
  const { data: system, send: loadSystem } = useRequest(readSystemStatus);

  useInterval(() => {
    void loadData();
    void loadSystem();
  });

  const sort = useTableSort(data?.stats ?? [], {
    s: byNumber((st) => st.s),
    f: byNumber((st) => st.f),
    q: byNumber((st) => st.q)
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void loadData()} />;
  }

  const name = (id: number) =>
    LL.STATUS_NAMES[id.toString() as keyof Translation['STATUS_NAMES']]();
  const bus = system && busInfo(system.bus_status, system.bus_uptime, LL);

  return (
    <>
      {system && bus && (
        <Hero
          state={bus.state}
          icon="swap_horiz"
          title={bus.text}
          text={`${system.num_devices} ${LL.DEVICES()}`}
          actions={
            me.admin && (
              <ButtonLink to="/settings/application" color="secondary" icon="tune">
                {LL.EMS_BUS_SETTINGS()}
              </ButtonLink>
            )
          }
        />
      )}
      <Section icon="monitoring" title={LL.DATA_TRAFFIC()}>
        <div className="tbl-wrap">
          <Table className="st-activity">
            <thead>
              <tr>
                <th />
                <SortHeader className="r" {...sort.header('s')}>
                  {LL.SUCCESS()}
                </SortHeader>
                <SortHeader className="r" {...sort.header('f')}>
                  {LL.FAIL()}
                </SortHeader>
                <SortHeader className="r" {...sort.header('q')}>
                  {LL.QUALITY()}
                </SortHeader>
              </tr>
            </thead>
            <tbody>
              {sort.rows.map((stat: Stat) => {
                const active = stat.q !== 0 && stat.s + stat.f !== 0;
                return (
                  <tr key={stat.id}>
                    <td>{name(stat.id)}</td>
                    <td className="r num">{nf.format(stat.s)}</td>
                    <td className={stat.f ? 'r num warnc' : 'r num muted'}>
                      {nf.format(stat.f)}
                    </td>
                    <td className="r num">
                      {active && (
                        <span className="st-quality">
                          <ProgressBar
                            value={stat.q}
                            color={qualityColor(stat.q)}
                            label={LL.QUALITY()}
                          />
                          <span>{stat.q}%</span>
                        </span>
                      )}
                    </td>
                  </tr>
                );
              })}
            </tbody>
          </Table>
        </div>
        <p className="cap st-help">{LL.ACTIVITY_HELP()}</p>
      </Section>
    </>
  );
};

export default Activity;
