const WebSocket = require('ws');

const ws = new WebSocket('ws://127.0.0.1:8080');

const messages = [
    {"raw":0,"steer":0,"air":-31.5,"c1":-31.5,"c2":-31.5,"pitch":-5.62,"roll":-4.52,"yaw":-41.40,"sample":1},
    {"raw":0,"steer":0,"air":-31.5,"c1":-31.5,"c2":-31.5,"pitch":-4.62,"roll":-3.52,"yaw":-42.40,"sample":2},
    {"raw":0,"steer":0,"air":-31.5,"c1":-31.5,"c2":-31.5,"pitch":-6.62,"roll":-1.52,"yaw":-44.40,"sample":3},
    {"raw":0,"steer":0,"air":-31.5,"c1":-31.5,"c2":-31.5,"pitch":-7.62,"roll":2.52,"yaw":-40.40,"sample":4},
    {"raw":0,"steer":0,"air":-31.5,"c1":-31.5,"c2":-31.5,"pitch":-9.62,"roll":2.52,"yaw":-50.40,"sample":5},
];

ws.on('open', () => {
    console.log('Connected to server');

    messages.forEach((msg, index) => {
        setTimeout(() => {
            console.log('Sending:', msg);
            ws.send(JSON.stringify(msg));

            // Close after last message
            if (index === messages.length - 1) {
                setTimeout(() => ws.close(), 500);
            }
        }, index * 1000); // sends one message every 1 second
    });
});

ws.on('message', (data) => {
    console.log('Response:', data.toString());
});