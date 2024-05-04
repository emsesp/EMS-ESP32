import express from 'express';
import multer from 'multer';

const rest_server = express();
const port = 3082;
const upload = multer({ dest: './uploads' });

const UPLOAD_FILE_ENDPOINT = '/rest/uploadFile';

// delay functions, 2 different types
const delay = (ms) => new Promise((res) => setTimeout(res, ms));
function delay_blocking(milliseconds) {
  var start = new Date().getTime();
  while (true) {
    if (new Date().getTime() - start > milliseconds) {
      break;
    }
  }
}

function progress_middleware(req, _res, next) {
  let progress = 0;
  const file_size = req.headers['content-length'];
  console.log('Uploading file. Size ' + file_size + ' bytes');

  // set event listener
  req.on('data', async (chunk) => {
    progress += chunk.length;
    const percentage = (progress / file_size) * 100;
    console.log(`Progress: ${Math.round(percentage)}%`);
    // await delay(1000); // slow it down
    delay_blocking(1000); // slow it down
  });
  next(); // invoke next middleware which is multer
}

rest_server.post(UPLOAD_FILE_ENDPOINT, progress_middleware, upload.single('file'), (req, res) => {
  if (req.file) {
    const filename = req.file.originalname;
    const ext = filename.substring(filename.lastIndexOf('.') + 1);
    console.log(req.file);

    if (ext === 'bin' || ext === 'json') {
      console.log('Received firmware or json file, extension: ' + ext);
      return res.sendStatus(200);
    } else if (ext === 'md5') {
      return res.json({ md5: 'ef4304fc4d9025a58dcf25d71c882d2c' });
    }
  }
  console.log('Invalid file extension');
  return res.sendStatus(400);
});

// start server
rest_server.listen(port, () => console.log(`EMS-ESP File Upload mock server running on http://localhost:${port}/`));
