#!/usr/bin/env node

const axios = require('axios');

const url = 'http://10.10.10.135/api/system/commands';
const queryParams = {
  entity: 'commands',
  id: 0
};

const totalRequests = 1000000;
const requestsPerCount = 100;

let requestCount = 0;

function fetchData() {
  axios
    .get(url, { params: queryParams })
    .then((response) => {
      requestCount++;

      if (requestCount % requestsPerCount === 0) {
        console.log(`Requests completed: ${requestCount}`);
      }

      if (requestCount < totalRequests) {
        fetchData();
      } else {
        console.log('All requests completed.');
      }
    })
    .catch((error) => {
      console.error('Error making request:', error.message);
    });
}

// Start making requests
console.log(`Starting test`);
fetchData();
