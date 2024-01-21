import { handleRequest } from './handler';

export default {
  port: 3080,
  fetch: (request) => handleRequest(request)
};
