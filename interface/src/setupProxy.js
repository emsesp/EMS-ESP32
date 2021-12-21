const pkg = require('../package.json');
const { createProxyMiddleware } = require('http-proxy-middleware');
const target = process.env.PROXY || pkg.proxy;

module.exports = function (app) {
  app.use(
    createProxyMiddleware('/api', {
      target
    })
  );

  // app.use(
  //   createProxyMiddleware('/ws', {
  //     target: target,
  //     // target: target.replace(/^http(s?):\/\//, 'ws$1://'),
  //     ws: true
  //   })
  // );
};
