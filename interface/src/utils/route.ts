export const routeMatches = (route: string, pathname: string) => pathname.startsWith(route + '/') || pathname === route;
