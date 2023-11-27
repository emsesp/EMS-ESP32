import { useLocation } from 'react-router-dom';

export const useRouterTab = () => {
  const loc = useLocation().pathname;
  const routerTab = loc.substring(0, loc.lastIndexOf('/')) ? loc : false;

  return { routerTab } as const;
};
