#!/bin/bash
set -e

### Functions

is_git() {
    command -v git >/dev/null 2>&1 || return 1
    command git rev-parse >/dev/null 2>&1 || return 1

    return 0
}

stat_bytes() {
	filesize=`du -k "$1" | cut -f1;`
	echo 'size:' $filesize 'bytes'
}

# Available environments
list_envs() {
    grep env: platformio.ini | sed 's/\[env:\(.*\)\]/\1/g'
}

print_available() {
    echo "--------------------------------------------------------------"
    echo "Available environments:"
    for environment in $available; do
        echo "-> $environment"
    done
}

print_environments() {
    echo "--------------------------------------------------------------"
    echo "Current environments:"
    for environment in $environments; do
        echo "-> $environment"
    done
}

set_default_environments() {
    # Hook to build in parallel when using travis
    if [[ "${TRAVIS_BUILD_STAGE_NAME}" = "Release" ]] && ${par_build}; then
        environments=$(echo ${available} | \
            awk -v par_thread=${par_thread} -v par_total_threads=${par_total_threads} \
            '{ for (i = 1; i <= NF; i++) if (++j % par_total_threads == par_thread ) print $i; }')
        return
    fi

    # Only build travis target
    if [[ "${TRAVIS_BUILD_STAGE_NAME}" = "Test" ]]; then
        environments=$travis
        return
    fi

    # Fallback to all available environments
    environments=$available
}

build_webui() {
    cd ./tools/webfilesbuilder

    # Build system uses gulpscript.js to build web interface
    if [ ! -e node_modules/gulp/bin/gulp.js ]; then
        echo "--------------------------------------------------------------"
        echo "Installing dependencies..."
        npm ci
    fi

    # Recreate web interface - "node ./tools/webfilesbuilder/node_modules/gulp/bin/gulp.js --cwd ./tools/webfilesbuilder"
    echo "--------------------------------------------------------------"
    echo "Building web interface..."
    node node_modules/gulp/bin/gulp.js || exit

    cd ../..
}

build_environments() {
    echo "--------------------------------------------------------------"
    echo "Building firmware images..."
    # don't move to firmware folder until Travis fixed (see https://github.com/travis-ci/dpl/issues/846#issuecomment-547157406)
    # mkdir -p $destination

    for environment in $environments; do
        echo "* EMS-ESP-$version-$environment.bin"
        platformio run --silent --environment $environment || exit 1
        stat_bytes .pio/build/$environment/firmware.bin
        # mv .pio/build/$environment/firmware.bin $destination/EMS-ESP-$version-$environment.bin
        # mv .pio/build/$environment/firmware.bin EMS-ESP-$version-$environment.bin
        mv .pio/build/$environment/firmware.bin EMS-ESP-dev-$environment.bin
    done
    echo "--------------------------------------------------------------" 
}


####### MAIN

destination=firmware
version_file=./src/version.h
version=$(grep -E '^#define APP_VERSION' $version_file | awk '{print $3}' | sed 's/"//g')

if ${TRAVIS:-false}; then
    git_revision=${TRAVIS_COMMIT::7}
    git_tag=${TRAVIS_TAG}
elif is_git; then
    git_revision=$(git rev-parse --short HEAD)
    git_tag=$(git tag --contains HEAD)
else
    git_revision=unknown
    git_tag=
fi

echo $git_tag

if [[ -n $git_tag ]]; then
    new_version=${version/-*}
    sed -i -e "s@$version@$new_version@" $version_file
    version=$new_version
    trap "git checkout -- $version_file" EXIT
fi

par_build=false
par_thread=${BUILDER_THREAD:-0}
par_total_threads=${BUILDER_TOTAL_THREADS:-4}
if [ ${par_thread} -ne ${par_thread} -o \
    ${par_total_threads} -ne ${par_total_threads} ]; then
    echo "Parallel threads should be a number."
    exit
fi
if [ ${par_thread} -ge ${par_total_threads} ]; then
    echo "Current thread is greater than total threads. Doesn't make sense"
    exit
fi

# travis platformio target is used for nightly Test
travis=$(list_envs | grep travis | sort)

# get all taregts, excluding travis and debug
available=$(list_envs | grep -Ev -- 'travis|debug|release' | sort)

export PLATFORMIO_BUILD_FLAGS="${PLATFORMIO_BUILD_FLAGS}"

# get command line Parameters
# l prints environments
# 2 does parallel builds
# d uses next arg as destination folder
while getopts "lpd:" opt; do
  case $opt in
    l)
        print_available
        exit
        ;;
    p)
        par_build=true
        ;;
    d)
        destination=$OPTARG
        ;;
    esac
done

shift $((OPTIND-1))

# Welcome message
echo "--------------------------------------------------------------"
echo "EMS-ESP FIRMWARE BUILDER"
echo "Building for version ${version}" ${git_revision:+($git_revision)}

# Environments to build
environments=$@

if [ $# -eq 0 ]; then
    set_default_environments
fi

if ${CI:-false}; then
    print_environments
fi

# for debugging
echo "* git_revision = $git_revision"
echo "* git_tag = $git_tag"
echo "* TRAVIS_COMMIT = $TRAVIS_COMMIT"
echo "* TRAVIS_TAG = $TRAVIS_TAG"
echo "* TRAVIS_BRANCH = $TRAVIS_BRANCH"
echo "* TRAVIS_BUILD_STAGE_NAME = $TRAVIS_BUILD_STAGE_NAME"

build_webui
build_environments
