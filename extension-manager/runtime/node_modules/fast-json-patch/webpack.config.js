var webpack = require('webpack');
const package = require('./package.json');
const path = require('path');

module.exports = env => {
  if(env && env.NODE_ENV === "test") {
    return [
      {
        entry: './test/spec/webpack/importSpec.src.js',
        mode: 'production',
        output: {
          path: path.resolve(__dirname, 'test/spec/webpack'),
          filename: 'importSpec.build.js'
        },
        target: 'node',
        resolve: {
          extensions: ['.js']
        }
      }
    ];
  }
  else {
    return [
      {
        entry: './index.js',
        mode: 'production',
        optimization: {
          minimize: false
        },
        output: {
          filename: 'fast-json-patch.js',
          library: 'jsonpatch',
          libraryTarget: 'var'
        },
        resolve: {
          extensions: ['.js']
        },
        plugins: [
          new webpack.BannerPlugin('fast-json-patch, version: ' + package['version'])
        ]
      },
      {
        entry: './index.js',
        mode: 'production',
        output: {
          filename: 'fast-json-patch.min.js',
          library: 'jsonpatch',
          libraryTarget: 'var'
        },
        resolve: {
          extensions: ['.js']
        },
        plugins: [
          new webpack.BannerPlugin('fast-json-patch, version: ' + package['version'])
        ]
      }
    ];
  }
};




