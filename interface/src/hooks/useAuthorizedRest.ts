import { redirectingAuthorizedFetch } from '../authentication';
import useRest, { RestRequestOptions } from './useRest';

const useAuthorizedRest = <D>({
  endpoint
}: Omit<RestRequestOptions, 'fetchFunction'>) =>
  useRest<D>({
    endpoint,
    fetchFunction: redirectingAuthorizedFetch
  });

export default useAuthorizedRest;
