// npm install axios
// node api_test.js
const axios = require('axios');

async function testAPI(ip = "ems-esp.local", apiPath = "system", loopCount = 1, delayMs = 1000) {
    const baseUrl = `http://${ip}`;
    const url = `${baseUrl}/${apiPath}`;
    const results = [];
    
    for (let i = 0; i < loopCount; i++) {
        let logMessage = '';
        if (loopCount > 1) {
            logMessage = `- Request ${i + 1}/${loopCount} -`;
        }
        
        try {
            const response = await axios.get(url, {
                timeout: 5000,
                headers: {
                    'Content-Type': 'application/json'
                }
            });
            logMessage += (logMessage ? ' ' : '') + `URL: ${url}, Status: ${response.status}`;
        } catch (error) {
            console.error('Error:', error.message);
            // if (error.response) {
            //     console.error('Response status:', error.response.status);
            //     console.error('Response data:', error.response.data);
            // }
            throw error;
        }

        // if successful make another request to the /api/system/info endpoint to fetch the freeMem
        const response = await axios.get(`${baseUrl}/api/system/info`);
        const freeMem = response.data?.freeMem || response.data?.system?.freeMem;
        const uptime = response.data?.uptime || response.data?.system?.uptime;
        if (freeMem !== undefined) {
            logMessage += (logMessage ? ' ' : '') + `(uptime: ${uptime}, freeMem: ${freeMem})`;
        } else {
            logMessage += (logMessage ? ' ' : '') + 'freeMem not found in response';
        }
        console.log(logMessage);

        // Delay before next request (except for the last one)
        if (i < loopCount - 1) {
            await new Promise(resolve => setTimeout(resolve, delayMs));
        }
    }
    
    return loopCount === 1 ? results[0] : results;
}

// Run the test
// Examples:
// testAPI("192.168.1.65", "api/system") - single call
// testAPI("192.168.1.65", "api/system", 5) - 5 calls with 1000ms delay
// testAPI("192.168.1.65", "api/system", 10, 2000) - 10 calls with 2000ms delay
testAPI("192.168.1.65", "status", 20000, 5)
    .then(() => {
        console.log('Test completed successfully');
        process.exit(0);
    })
    .catch((error) => {
        console.error('Test failed:', error);
        process.exit(1);
    });