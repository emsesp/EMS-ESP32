import { SensorsTile } from './SensorsOverview';
import type { Sensors } from './SensorsOverview';
import {
  BoilerTile,
  CustomTile,
  DhwTile,
  HcTile,
  NotesTile,
  SummaryTile,
  heatingCircuits,
  summaryMenus,
  typeNameOf
} from './Tiles';
import type { SysItem } from './model';
import type { Edits } from './useSystemData';

// Home, the first page of the Dashboard: the most important things as tiles, easy to operate. The favorites
// have their own page in the menu.
const Home = ({
  items,
  edits,
  sensors
}: {
  items: readonly SysItem[];
  edits: Edits;
  sensors: Sensors;
}) => {
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
        <SensorsTile sensors={sensors} kind="temps" to="/dashboard/sensors" />
        <SensorsTile sensors={sensors} kind="analogs" to="/dashboard/sensors" />
        <CustomTile items={items} to="/dashboard/custom" />
        <NotesTile items={items} />
      </div>
    </>
  );
};

export default Home;
