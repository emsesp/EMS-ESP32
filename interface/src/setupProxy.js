const { createProxyMiddleware } = require('http-proxy-middleware');

module.exports = function (app) {
    app.use(
        '/rest/*',
        createProxyMiddleware({
            target: 'http://localhost:3080',
            secure: false,
            changeOrigin: true
        })
    );
};
