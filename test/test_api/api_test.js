// npm install axios
// node api_test.js
const axios = require('axios');

async function testAPI(ip = "ems-esp.local", apiPath = "system", loopCount = 1, delayMs = 1000) {
    const baseUrl = `http://${ip}/api`;
    const url = `${baseUrl}/${apiPath}`;
    const results = [];
    
    for (let i = 0; i < loopCount; i++) {
        if (loopCount > 1) {
            console.log(`\n--- Request ${i + 1} of ${loopCount} ---`);
        }
        
        try {
            const response = await axios.get(url, {
                timeout: 5000,
                headers: {
                    'Content-Type': 'application/json'
                }
            });
            
            console.log('Status:', response.status);
            console.log('Data:', JSON.stringify(response.data, null, 2));
            
            results.push(response.data);
            
            // Delay before next request (except for the last one)
            if (i < loopCount - 1) {
                await new Promise(resolve => setTimeout(resolve, delayMs));
            }
        } catch (error) {
            console.error('Error:', error.message);
            if (error.response) {
                console.error('Response status:', error.response.status);
                console.error('Response data:', error.response.data);
            }
            throw error;
        }
    }
    
    return loopCount === 1 ? results[0] : results;
}

// Run the test
// Examples:
// testAPI("192.168.1.65", "system") - single call
// testAPI("192.168.1.65", "system", 5) - 5 calls with 1000ms delay
// testAPI("192.168.1.65", "system", 10, 2000) - 10 calls with 2000ms delay
testAPI("192.168.1.65", "system", 1000)
    .then(() => {
        console.log('Test completed successfully');
        process.exit(0);
    })
    .catch((error) => {
        console.error('Test failed:', error);
        process.exit(1);
    });