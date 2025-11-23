// npm install axios
// node api_test.js
const axios = require('axios');

async function testAPI(ip = "ems-esp.local", apiPath = "system") {
    const baseUrl = `http://${ip}/api`;
    const url = `${baseUrl}/${apiPath}`;
    
    try {
        const response = await axios.get(url, {
            timeout: 5000,
            headers: {
                'Content-Type': 'application/json'
            }
        });
        
        console.log('Status:', response.status);
        console.log('Data:', JSON.stringify(response.data, null, 2));
        
        return response.data;
    } catch (error) {
        console.error('Error:', error.message);
        if (error.response) {
            console.error('Response status:', error.response.status);
            console.error('Response data:', error.response.data);
        }
        throw error;
    }
}

// Run the test
testAPI("192.168.1.223", "system")
    .then(() => {
        console.log('Test completed successfully');
        process.exit(0);
    })
    .catch((error) => {
        console.error('Test failed:', error);
        process.exit(1);
    });