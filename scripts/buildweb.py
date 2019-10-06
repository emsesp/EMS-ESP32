Import("env")

env.Execute("node ./tools/webfilesbuilder/node_modules/gulp/bin/gulp.js --silent --cwd ./tools/webfilesbuilder")
