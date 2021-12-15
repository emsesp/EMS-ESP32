import { useMatch, useResolvedPath } from 'react-router-dom';

export const useRouterTab = () => {
  const routerTabPath = useResolvedPath(':tab');
  const routerTabPathMatch = useMatch(routerTabPath.pathname);

  const routerTab = routerTabPathMatch?.params?.tab || false;
  return { routerTab } as const;
};
