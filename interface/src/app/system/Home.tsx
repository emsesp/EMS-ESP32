import { useI18nContext } from 'i18n/i18n-react';

import {
  BoilerTile,
  DhwTile,
  HcTile,
  NotesTile,
  SummaryTile,
  heatingCircuits,
  summaryMenus,
  typeNameOf
} from './Tiles';
import { ValueRow } from './ValueRow';
import type { SysItem } from './model';
import type { useFavorites } from './useFavorites';
import type { Edits } from './useSystemData';

// Home, the first page of the Dashboard: the most important things as tiles, easy to operate, and the
// favorites of all devices
const Home = ({
  items,
  edits,
  fav
}: {
  items: readonly SysItem[];
  edits: Edits;
  fav: ReturnType<typeof useFavorites>;
}) => {
  const { LL } = useI18nContext();
  return (
    <>
      <div className="sys-tiles">
        {heatingCircuits(items).map((tag) => (
          <HcTile
            key={tag}
            tag={tag}
            items={items}
            edits={edits}
            to={`/dashboard/${tag}`}
          />
        ))}
        <DhwTile items={items} edits={edits} to="/dashboard/dhw" />
        <BoilerTile items={items} to="/dashboard/boiler" />
        {summaryMenus().map((menu) => (
          <SummaryTile
            key={menu}
            menu={menu}
            title={typeNameOf(items, menu)}
            items={items}
            to={`/dashboard/${menu}`}
          />
        ))}
        <NotesTile items={items} />
      </div>

      <div className="sys-home-fav">
        <section className="section">
          <div className="sys-sec-head">{LL.FAVORITES()}</div>
          {fav.items.map((i) => (
            <ValueRow
              key={i.uid}
              item={i}
              edits={edits}
              where={i.typeName}
              {...(fav.canToggle(i) && fav.toggle ? { onFavorite: fav.toggle } : {})}
            />
          ))}
          {fav.items.length === 0 && (
            <div className="sys-empty">{LL.SYS_FAV_NONE()}</div>
          )}
        </section>
      </div>
    </>
  );
};

export default Home;
