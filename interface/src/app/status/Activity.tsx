import {
  Body,
  Cell,
  Header,
  HeaderCell,
  HeaderRow,
  Row,
  Table
} from '@table-library/react-table-library/table';
import { useTheme as tableTheme } from '@table-library/react-table-library/theme';
import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { Translation } from 'i18n/i18n-types';
import { useInterval } from 'utils';

import { readActivity } from '../../api/app';
import type { Stat } from '../main/types';

const SystemActivity = () => {
  const { data, send: loadData, error } = useRequest(readActivity);

  useInterval(() => {
    void loadData();
  });

  const { LL } = useI18nContext();

  useLayoutTitle(LL.DATA_TRAFFIC());

  const stats_theme = tableTheme({
    Table: `
      --data-table-library_grid-template-columns: repeat(1, minmax(0, 1fr)) 90px 90px 80px;
    `,
    BaseRow: `
      font-size: 14px;
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      color: #90CAF9;

      .th {
        height: 36px;
        border-bottom: 1px solid #565656;
      }
    `,
    Row: `
      .td {
        padding: 8px;
        border-top: 1px solid #565656;
        border-bottom: 1px solid #565656;
      }

      &:nth-of-type(odd) .td {
        background-color: #303030;
      }
      &:nth-of-type(even) .td {
        background-color: #1e1e1e;
      }
    `,
    BaseCell: `
      &:not(:first-of-type) {
        text-align: center;
      }
    `
  });

  const showName = (id: number) => {
    const name: keyof Translation['STATUS_NAMES'] = id;
    return LL.STATUS_NAMES[name]();
  };

  const showQuality = (stat: Stat) => {
    if (stat.q === 0 || stat.s + stat.f === 0) {
      return;
    }
    if (stat.q === 100) {
      return <div style={{ color: '#00FF7F' }}>{stat.q}%</div>;
    }
    if (stat.q >= 95) {
      return <div style={{ color: 'orange' }}>{stat.q}%</div>;
    } else {
      return <div style={{ color: 'red' }}>{stat.q}%</div>;
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <Table
        data={{ nodes: data.stats }}
        theme={stats_theme}
        layout={{ custom: true }}
      >
        {(tableList: Stat[]) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell resize />
                <HeaderCell stiff>{LL.SUCCESS()}</HeaderCell>
                <HeaderCell stiff>{LL.FAIL()}</HeaderCell>
                <HeaderCell stiff>{LL.QUALITY()}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((stat: Stat) => (
                <Row key={stat.id} item={stat}>
                  <Cell>{showName(stat.id)}</Cell>
                  <Cell stiff>{Intl.NumberFormat().format(stat.s)}</Cell>
                  <Cell stiff>{Intl.NumberFormat().format(stat.f)}</Cell>
                  <Cell stiff>{showQuality(stat)}</Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default SystemActivity;
