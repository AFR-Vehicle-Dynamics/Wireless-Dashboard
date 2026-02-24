# Local Testing
Run `bash: node server.js` to host websocket on port `8080` for `dashboard.html`, note changes made to html must be copied over to the `.ino` file \
Switch websocket to the local server: `var ws = new WebSocket('ws://127.0.0.1:8080');` \
View site using LiveServer

#### Sending testing WebSocket data
Send json data in form `{raw: "1000", angle: "800", sample: "1" }` using tool like Postman or browser console OR run `node /testScripts/simulateData.js` to send a set of data to populate graphs. \
JSON data for analog sensors ranges 0-4096, must be sent as strings.

### TODO:
- Add CSS
- Create visualization for rest of sensors
- 2-way communication 