/*
EMS-ESP web server file system builder

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*eslint quotes: ['error', 'single']*/
/*eslint-env es6*/

const gulp = require('gulp');
const fs = require('fs');
const concat = require('gulp-concat');
const gzip = require('gulp-gzip');
const flatmap = require('gulp-flatmap');
const path = require('path');
const htmlmin = require('gulp-htmlmin');
const uglify = require('gulp-uglify');
const pump = require('pump');
const through = require('through2');

// file name includes extension
var buildHeader = function (name) {

    return through.obj(function (source, encoding, callback) {

        var parts = source.path.split(path.sep);
        var filename = parts[parts.length - 1];
        var extension = filename.split('.')[1];

        console.info('Creating file: ' + filename);

        // var safename = name.split('.').join('_');
        var safename = name.replace(/\.|-/g, "_");

        var destination = "../../src/webh/" + filename + ".h";

        // check for woff files which should be fonts
        if (extension === "woff") {
            extension = "fonts";
        }

        // html files go into root
        if (extension === "html") {
            var source = "../../src/websrc/temp/gzipped/" + name + ".gz";
        } else {
            var source = "../../src/websrc/temp/gzipped/" + extension + "/" + name + ".gz";
        }

        var wstream = fs.createWriteStream(destination);
        wstream.on('error', function (err) {
            console.log(err);
        });

        var data = fs.readFileSync(source);

        wstream.write('#define ' + safename + '_gz_len ' + data.length + '\n');
        wstream.write('const uint8_t ' + safename + '_gz[] PROGMEM = {');

        for (i = 0; i < data.length; i++) {
            if (i % 1000 == 0) wstream.write("\n");
            wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
            if (i < data.length - 1) wstream.write(',');
        }

        wstream.write('\n};')
        wstream.end();

        callback(null, destination);

    });
};

gulp.task('myespjs', function () {
    return gulp.src(['../../src/websrc/myesp.js', '../../src/custom.js'])
        .pipe(concat({
            path: 'myesp.js',
            stat: {
                mode: 0666
            }
        }))
        .pipe(gulp.dest('../../src/websrc/temp/js'))
        .pipe(uglify())
        .pipe(gulp.dest('../../src/websrc/temp/js/ugly'))
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/js'))
        .pipe(buildHeader('myesp.js'));
});

gulp.task('requiredjs', function () {
    return gulp.src(['../../src/websrc/3rdparty/js/jquery-1.12.4.min.js', '../../src/websrc/3rdparty/js/bootstrap-3.3.7.min.js', '../../src/websrc/3rdparty/js/footable-3.1.6.min.js'])
        .pipe(concat({
            path: 'required.js',
            stat: {
                mode: 0666
            }
        }))
        .pipe(gulp.dest('../../src/websrc/temp/js/'))
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/js/'))
        .pipe(buildHeader('required.js'));
});


gulp.task('requiredcss', function () {
    return gulp.src(['../../src/websrc/3rdparty/css/bootstrap-3.3.7.min.css', '../../src/websrc/3rdparty/css/footable.bootstrap-3.1.6.min.css', '../../src/websrc/3rdparty/css/sidebar.css'])
        .pipe(concat({
            path: 'required.css',
            stat: {
                mode: 0666
            }
        }))
        .pipe(gulp.dest('../../src/websrc/temp/css/'))
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/css/'))
        .pipe(buildHeader('required.css'));
});

gulp.task("fontwoff", function () {
    return gulp.src("../../src/websrc/3rdparty/fonts/*.*")
        .pipe(gulp.dest("../../src/websrc/temp/fonts/"))
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/fonts/'))
        .pipe(buildHeader('glyphicons-halflings-regular.woff'));
});

gulp.task('myesphtml', function () {
    return gulp.src(['../../src/websrc/myesp.htm', '../../src/custom.htm'])
        .pipe(concat({
            path: 'myesp.html',
            stat: {
                mode: 0666
            }
        }))
        .pipe(htmlmin({ collapseWhitespace: true, minifyJS: true }))
        .pipe(gulp.dest('../../src/websrc/temp/'))
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/'))
        .pipe(buildHeader('myesp.html'));

});

gulp.task('indexhtml', function () {
    return gulp.src('../../src/websrc/index.html')
        .pipe(htmlmin({ collapseWhitespace: true, minifyJS: true }))
        .pipe(gulp.dest('../../src/websrc/temp/'))
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/'))
        .pipe(buildHeader('index.html'));
});

gulp.task('default', gulp.parallel('myespjs', 'requiredjs', 'requiredcss', 'fontwoff', 'myesphtml', 'indexhtml'));
