var gulp = require('gulp');
var fs = require('fs');
var concat = require('gulp-concat');
var gzip = require('gulp-gzip');
var flatmap = require('gulp-flatmap');
var path = require('path');
var htmlmin = require('gulp-htmlmin');
var uglify = require('gulp-uglify');
var pump = require('pump');


gulp.task('myespjs-concat', function () {
    return gulp.src(['../../src/websrc/myesp.js', '../../src/custom.js'])
        .pipe(concat({
            path: 'myesp.js',
            stat: {
                mode: 0666
            }
        }))
        .pipe(gulp.dest('../../src/websrc/temp/js'))
});

gulp.task('myespjsminify', ["myespjs-concat"], function (cb) {
    pump([
        gulp.src('../../src/websrc/temp/js/myesp.js'),
        uglify(),
        gulp.dest('../../src/websrc/temp/js/ugly'),
    ],
        cb
    );
});

gulp.task("myespjsgz", ["myespjsminify"], function () {
    return gulp.src("../../src/websrc/temp/js/ugly/myesp.js")
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/js'));
});

gulp.task('myespjsgzh', ["myespjsgz"], function () {
    var source = "../../src/websrc/temp/gzipped/js/" + "myesp.js.gz";
    var destination = "../../src/webh/" + "myesp.js.gz.h";

    var wstream = fs.createWriteStream(destination);
    wstream.on('error', function (err) {
        console.log(err);
    });

    var data = fs.readFileSync(source);

    wstream.write('#define myesp_js_gz_len ' + data.length + '\n');
    wstream.write('const uint8_t myesp_js_gz[] PROGMEM = {')

    for (i = 0; i < data.length; i++) {
        if (i % 1000 == 0) wstream.write("\n");
        wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
        if (i < data.length - 1) wstream.write(',');
    }

    wstream.write('\n};')
    wstream.end();
});

gulp.task("scripts", ["scripts-concat"], function () {

    var source = "../../src/websrc/temp/gzipped/js/" + "required.js.gz";
    var destination = "../../src/webh/" + "required.js.gz.h";

    var wstream = fs.createWriteStream(destination);
    wstream.on('error', function (err) {
        console.log(err);
    });

    var data = fs.readFileSync(source);

    wstream.write('#define required_js_gz_len ' + data.length + '\n');
    wstream.write('const uint8_t required_js_gz[] PROGMEM = {')

    for (i = 0; i < data.length; i++) {
        if (i % 1000 == 0) wstream.write("\n");
        wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
        if (i < data.length - 1) wstream.write(',');
    }

    wstream.write('\n};')
    wstream.end();

});

gulp.task('scripts-concat', ["myespjsgzh"], function () {
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
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/js/'));
});

gulp.task('styles-concat', function () {
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
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/css/'));
});

gulp.task("styles", ["styles-concat"], function () {

    var source = "../../src/websrc/temp/gzipped/css/" + "required.css.gz";
    var destination = "../../src/webh/" + "required.css.gz.h";

    var wstream = fs.createWriteStream(destination);
    wstream.on('error', function (err) {
        console.log(err);
    });

    var data = fs.readFileSync(source);

    wstream.write('#define required_css_gz_len ' + data.length + '\n');
    wstream.write('const uint8_t required_css_gz[] PROGMEM = {')

    for (i = 0; i < data.length; i++) {
        if (i % 1000 == 0) wstream.write("\n");
        wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
        if (i < data.length - 1) wstream.write(',');
    }

    wstream.write('\n};')
    wstream.end();

});

gulp.task("fontgz", function () {
    return gulp.src("../../src/websrc/3rdparty/fonts/*.*")
        .pipe(gulp.dest("../../src/websrc/temp/fonts/"))
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/fonts/'));
});

gulp.task("fonts", ["fontgz"], function () {
    return gulp.src("../../src/websrc/temp/gzipped/fonts/*.*")
        .pipe(flatmap(function (stream, file) {
            var filename = path.basename(file.path);
            var wstream = fs.createWriteStream("../../src/webh/" + filename + ".h");
            wstream.on("error", function (err) {
                gutil.log(err);
            });
            var data = file.contents;
            wstream.write("#define " + filename.replace(/\.|-/g, "_") + "_len " + data.length + "\n");
            wstream.write("const uint8_t " + filename.replace(/\.|-/g, "_") + "[] PROGMEM = {")

            for (i = 0; i < data.length; i++) {
                if (i % 1000 == 0) wstream.write("\n");
                wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
                if (i < data.length - 1) wstream.write(',');
            }

            wstream.write("\n};")
            wstream.end();

            return stream;
        }));
});

gulp.task('html-concat', function () {
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
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/'));
});

gulp.task('htmlsprep', ["html-concat"], function () {
    return gulp.src('../../src/websrc/index.html')
        .pipe(htmlmin({ collapseWhitespace: true, minifyJS: true }))
        .pipe(gulp.dest('../../src/websrc/temp/'))
        .pipe(gzip({
            append: true
        }))
        .pipe(gulp.dest('../../src/websrc/temp/gzipped/'));
});

gulp.task("htmls", ["htmlsprep"], function () {
    return gulp.src("../../src/websrc/temp/gzipped/*.gz")
        .pipe(flatmap(function (stream, file) {
            var filename = path.basename(file.path);
            var wstream = fs.createWriteStream("../../src/webh/" + filename + ".h");
            wstream.on("error", function (err) {
                gutil.log(err);
            });
            var data = file.contents;
            wstream.write("#define " + filename.replace(/\.|-/g, "_") + "_len " + data.length + "\n");
            wstream.write("const uint8_t " + filename.replace(/\.|-/g, "_") + "[] PROGMEM = {")

            for (i = 0; i < data.length; i++) {
                if (i % 1000 == 0) wstream.write("\n");
                wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
                if (i < data.length - 1) wstream.write(',');
            }

            wstream.write("\n};")
            wstream.end();

            return stream;
        }));
});

gulp.task('default', ['scripts', 'styles', "fonts", "htmls"]);