import { useMatch, useResolvedPath } from 'react-router';

export const useRouterTab = () => {
  const routerTabPathMatch = useMatch(useResolvedPath(':tab').pathname);
  const routerTab = routerTabPathMatch?.params?.tab || false;

  return { routerTab } as const;
};
