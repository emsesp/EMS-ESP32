import { handleRequest } from './Handler';

export default {
  port: 3080,
  fetch: (request) => handleRequest(request)
};
