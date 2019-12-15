$ScriptDir = Split-Path $script:MyInvocation.MyCommand.Path

# build web
$webfilesbuilder = $ScriptDir + "\..\webfilesbuilder"
node $webfilesbuilder\node_modules\gulp\bin\gulp.js --cwd $webfilesbuilder

# run chrome
$pathToChrome = 'C:\Program Files (x86)\Google\Chrome\Application\chrome.exe'
$tempFolder = '--user-data-dir=c:\temp'
$startmode = '--remote-debugging-port=9222 --disable-web-security --disable-gpu'
$startPage = $ScriptDir + "\..\..\src\websrc\temp\index.html"
Start-Process -FilePath $pathToChrome -ArgumentList $tempFolder, $startmode, $startPage

# run ws fake server
node wserver.js
