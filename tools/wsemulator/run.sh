#!/bin/sh

node  $PWD/../webfilesbuilder/node_modules/gulp/bin/gulp.js --cwd $PWD/../webfilesbuilder

open -na Google\ Chrome --args --disable-web-security --remote-debugging-port=9222 --user-data-dir="/tmp/chrome_dev" $PWD/../../src/websrc/temp/index.html 

node wserver.js
