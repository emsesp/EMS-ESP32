import formidable from "formidable";

export default () => {
    return {
        name: "vite:mockserver",
        configureServer: async (server) => {

            // catch any file uploads
            server.middlewares.use(async (req, res, next) => {

                if (req.url.startsWith("/rest/uploadFile")) {

                    let progress = 0;
                    const file_size = req.headers['content-length'];

                    req.on('data', async (chunk) => {
                        progress += chunk.length;
                        const percentage = (progress / file_size) * 100;
                        console.log(`Progress: ${Math.round(percentage)}%`);
                        // await new Promise((resolve) => setTimeout(() => resolve(), 3000)); // slow it down
                    });

                    const form = formidable({});
                    let fields;
                    let files;
                    try {
                        [fields, files] = await form.parse(req);
                    } catch (err) {
                        console.error(err);
                        res.writeHead(err.httpCode || 400, {
                            "Content-Type": "text/plain",
                        });
                        res.end(String(err));
                        return;
                    }

                    const upploaded_file = files.file[0];
                    const file_name = upploaded_file.originalFilename;
                    const file_extension = file_name.substring(file_name.lastIndexOf('.') + 1);

                    console.log("Filename: " + file_name);
                    console.log("Extension: " + file_extension);
                    console.log("File size: " + file_size);

                    if (file_extension === 'bin' || file_extension === 'json') {
                        console.log("File uploaded successfully!");
                    } else if (file_extension === 'md5') {
                        // return res.json({ md5: 'ef4304fc4d9025a58dcf25d71c882d2c' });
                        console.log("MD5 hash generated successfully!");
                        res.end(
                            JSON.stringify({
                                md5: 'ef4304fc4d9025a58dcf25d71c882d2c',
                            }),
                        );
                    } else {
                        res.statusCode = 400;
                        console.log("Invalid file extension!");
                    }

                    res.end();

                } else {
                    next(); // call next middleware
                }
            });
        },
    };
};
