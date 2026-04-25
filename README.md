# Usage
### Config
Set host Wifi network in `src/dashboardServer.h` \
All pins are set in `src/config/pins.h` \ 
### Viewing
Serial output will show IP address, ex:
``` 
WiFi connected! 
IP address: 192.168.1.112 
```
View dashboard on port 8080, ex: `192.168.1.112:8080`
### Local Testing
The dashboard now automatically detects if you are testing locally or on the car. \
Run bash:`node server.js` to host websocket on port `8080` for `dashboard.html`, note changes made to must be copied over to the `.ino` file \
View site from LiveServer \
Send json data in form `{"raw": "1000", "sample": "1" }` using tool like Postman \
OR run `node /testScripts/simulateData.js`

# Arduino Dependencies 
**Async TCP** by ESP32Async \
**ESP Async WebServer** by ESP32Async