# EMS-ESP REST API

OpenAPI is an open standard specification for describing REST APIs. From the [OpenAPI Specification](https://github.com/OAI/OpenAPI-Specification):

> The OpenAPI Specification (OAS) defines a standard, programming language-agnostic interface description for HTTP APIs, which allows both humans and computers to discover and understand the capabilities of a service without requiring access to source code, additional documentation, or inspection of network traffic. When properly defined via OpenAPI, a consumer can understand and interact with the remote service with a minimal amount of implementation logic. Similar to what interface descriptions have done for lower-level programming, the OpenAPI Specification removes guesswork in calling a service.

## Specification

- All data is sent and received as JSON.
- The URL is `http://<hostname>/api` with the default `<hostname>` being `ems-esp`. Note that it may also be ems-esp.local on some environments depending on your domain and dns setup.
- Only HTTP for now. HTTPS will be supported soon, with self-signed certificates or user's own.
- The EMS-ESP REST API will eventually be fully described in an OpenAPI 3.0 compliant document, available via [postman](https://www.postman.com/collections/479af3935991ac030130).
- Read instructions to fetch data use HTTP `GET` and operations that change data use HTTP `POST`, `PUT` or `PATCH`.
- To keep backwards compatibility with earlier versions, parameters can also be provided as HTML query parameters (name-value pairs) as opposed to URI path parameters. For example `POST http://ems-esp/api/boiler/wwtemp` with a JSON body of `{value: 50}` is the same as the URL `http://ems-esp/api?device=boiler?cmd=wwtemp?data=50`.
- Unless explicitly bypassed in the WebUI some operations required admin privileges in the form of an Access Token which can be generated from the Web UI's Security tab. An Access Token is a string 152 characters long. Token's do not expire. The token needs to be either embedded into the HTTP Header as `"Authorization: Bearer {ACCESS_TOKEN}"` or as query parameter `?access_token={ACCESS_TOKEN}`. To test you can use a command line instruction like

  ```bash
    % curl -i -H "Authorization: Bearer {ACCESS_TOKEN}" -X GET http://ems-esp/api/system/settings
    % curl -i -H "Authorization: Bearer {ACCESS_TOKEN}" -H "Content-Type: application/json" -d '{ "name": "wwtemp", "value":60}' http://ems-esp/api/boiler
    % curl -i http://ems-esp.local/api/system/settings\?access_token\="{ACCESS_TOKEN}"
  ```

## Error handling

- Requests that require authentication will return `403 Forbidden`
- Authenticating with invalid credentials will return `401 Unauthorized`

    ```html
    HTTP/1.1 401 Unauthorized
    {"message": "Bad credentials"}
    ```

- Sending invalid JSON will result in a `400 Bad Request` response.

    ```html
    HTTP/1.1 400 Bad Request
    {"message":"Problems parsing JSON"}
    ```

- Sending invalid fields will result in a `422 Unprocessable Entity` response.

    ```html
    HTTP/1.1 422 Unprocessable Entity
    {"message":"Invalid command"}
    ```

## Endpoints

### types

- `<string>` is a character string
- `<integer>` is a positive integer value
- `<number>` is an integer or floating point number
- `<boolean>` is either 1, 0, "1", "0", "on", "off", "true" or "false"
  
### descriptions

- `{device}` is boiler, thermostat, mixer, solar, heatpump, dallassensor
- `{name}` is the short name of the parameter, e.g. **curflowtemp**
- `{hc}` is the heating circuit number as a string, like **hc1**, **hc2** etc
- `{value}` is either a `<string>`, `<integer>`, `<number>` or `<boolean>`

## Device Endpoints

| Method | Endpoint | Description | Access Token required | JSON body data |
| - | - | - | - | - |
| GET | `/{device}` | return all device details and values | | |
| GET | `/{device}/{name}` | return a specific parameter and all its properties (name, fullname, value, type, min, max, unit, writeable) | | |
| GET | `/device={device}?cmd={name}?data={value}[?hc=<number>` | Using HTTP query parameters. This is to keep compatibility with v2. Unless bypassed in the EMS-ESP settings make sure you include `access_token={ACCESS_TOKEN}` | x |
| POST/PUT | `/{device}[/{hc\|wwc}][/{name}]` | sets a specific value to a parameter name. If no hc is selected and one is required for the device, the default will be used. The `hc` or `wwc` are only used with the `mixer` and `thermostat` devices, and optional | x | `{ ["name" : <string>] , ["hc": <number>], "value": <value> }` |

## System Endpoints

| Method | Endpoint | Description | Access Token required | JSON body data |
| - | - | - | - | - |
| GET | `/system/info` | list system information | | | |
| GET | `/system/settings` | list all settings, except passwords | |
| POST/PUT | `/system/pin` | switch a GPIO state to HIGH or LOW | x | `{ "id":<gpio>, "value":<boolean> }` |
| POST/PUT | `/system/send` | send a telegram to the EMS bus | x | `{ "value" : <string> }` |
| POST/PUT | `/system/publish` | force an MQTT publish | x | `{ "value" : <device> \| "ha" }` |
| POST/PUT | `/system/fetch` | fetch all EMS data from all devices | x | `{ "value" : <device> \| "all" }` |

## Examples

### From command line
```bash
# GETS do not need authentication
% curl http://ems-esp.local/api/thermostat/temp

# POSTS (with -d) need authentication tokens
% curl http://ems-esp.local/api/thermostat/temp \
  -H 'Content-Type: application/json' \
  -H 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ8.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjEuMWIwIn0.qeGT53Aom4rDYeIT1Pr4BSMdeWyf4_zN9ue2c51ZnM0' \
  -d '{ "value" : 22.5 }'

# GET with authentication using query parameter with token
% curl http://ems-esp.local/api/system/settings\?access_token\="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ8.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjEuMWIwIn0.qeGT53Aom4rDYeIT1Pr4BSMdeWyf4_zN9ue2c51ZnM0"
```

### Using Python
```python
import requests
import json

url = "http://ems-esp.local/api/thermostat/temp"

token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ8.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjEuMWIwIn0.qeGT53Aom4rDYeIT1Pr4BSMdeWyf4_zN9ue2c51ZnM0"

body = json.dumps({ "value": 22.5 })

headers = { 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + token }

response = requests.post(url, headers=headers, data=body, verify=False)

print(response.json())
```


