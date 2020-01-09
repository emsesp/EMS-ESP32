### Using the PlatformIO IDE

- Download [Git](https://git-scm.com/download/win) (install using the default settings)
- Download and install [Visual Studio Code](https://code.visualstudio.com/docs/?dv=win) (VSC)
- Restart the PC (if using Windows) to apply the new PATH settings. It should now detect Git
- Install the VSC extension "PlatformIO IDE"
- Git clone this repo with "git clone" from the command palette and using the URL https://github.com/proddy/EMS-ESP.git
- Install NodeJS from [here](https://nodejs.org/en/download/).
- Test if Node is installed correctly by `node --version` (both v10 and v12 are ok)
- Install the modules in the `tools/webfilesbuilder` folder with:
```
cd tools/webfilesbuilder
npm ci
```
- Check you can build the web files by typing `node node_modules/gulp/bin/gulp.js` from the `webfilesbuilder` folder.
- Finally use the build targets in PlatformIO's to build & upload.

If you seeing fatal errors in the npm install and came from an earlier build then you'll may need to refresh the library, like
1. Remove the local `node_modules`
2. Clean npm cache by `npm cache verify`
3. Update the global npm by `sudo npm ci -g npm`
4. Reinstall the local `node_modules` by `npm ci`

### Using Linux CLI

- Install python:
```sh
sudo apt install python-pip
```
- Install PlatformIO (see  https://docs.platformio.org/en/latest/installation.html):
```sh
python -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/develop/scripts/get-platformio.py)"
```
- Edit `~/.profile` and add: `export PATH=$PATH:~/.platformio/penv/bin` to the end, then `exit`, and start a new shell
- Update platformio and its libraries
```sh
platformio platform update
platformio lib update
```
- Grab the EMS-ESP source code:
```sh
git clone https://github.com/proddy/EMS-ESP.git
```
 (use `git clone --branch=dev https://github.com/proddy/EMS-ESP.git` for dev builds)
- Install NodeJS. See instructions [here](https://tecadmin.net/install-latest-nodejs-npm-on-ubuntu/).
- Install the EMS-ESP's web builder components
```sh
cd EMS-ESP/tools/webfilesbuilder
npm ci
cd ../..
```
- Build the firmware image:
```sh
pio run
```
- Upload to firmware image. This uses OTA as default. Change `platformio.ini` if using USB
```sh
pio run -t upload
```

