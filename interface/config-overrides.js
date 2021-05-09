const ManifestPlugin = require('webpack-manifest-plugin')
const WorkboxWebpackPlugin = require('workbox-webpack-plugin')
const MiniCssExtractPlugin = require('mini-css-extract-plugin')
const CompressionPlugin = require('compression-webpack-plugin')
const ProgmemGenerator = require('./progmem-generator.js')

module.exports = function override(config, env) {
  const hosted = process.env.REACT_APP_HOSTED

  if (env === 'production' && !hosted) {
    console.log('Custom webpack...')

    // rename the output file, we need it's path to be short for LittleFS
    config.output.filename = 'js/[id].[chunkhash:4].js'
    config.output.chunkFilename = 'js/[id].[chunkhash:4].js'

    // take out the manifest and service worker plugins
    config.plugins = config.plugins.filter(
      (plugin) => !(plugin instanceof ManifestPlugin),
    )
    config.plugins = config.plugins.filter(
      (plugin) => !(plugin instanceof WorkboxWebpackPlugin.GenerateSW),
    )

    // shorten css filenames
    const miniCssExtractPlugin = config.plugins.find(
      (plugin) => plugin instanceof MiniCssExtractPlugin,
    )
    miniCssExtractPlugin.options.filename = 'css/[id].[contenthash:4].css'
    miniCssExtractPlugin.options.chunkFilename =
      'css/[id].[contenthash:4].c.css'

    // build progmem data files
    config.plugins.push(
      new ProgmemGenerator({
        outputPath: '../lib/framework/WWWData.h',
        bytesPerLine: 20,
      }),
    )

    // add compression plugin, compress javascript
    config.plugins.push(
      new CompressionPlugin({
        filename: '[path].gz[query]',
        algorithm: 'gzip',
        test: /\.(js)$/,
        deleteOriginalAssets: true,
      }),
    )
  }
  return config
}
