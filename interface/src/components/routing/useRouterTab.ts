import { useMatch, useResolvedPath } from 'react-router-dom';

export const useRouterTab = () => {
  // const loc = useLocation().pathname;
  // const routerTab = loc.substring(0, loc.lastIndexOf('/')) ? loc : false;

  const routerTabPath = useResolvedPath(':tab');
  const routerTabPathMatch = useMatch(routerTabPath.pathname);
  const routerTab = routerTabPathMatch?.params?.tab || false;

  return { routerTab } as const;
};
