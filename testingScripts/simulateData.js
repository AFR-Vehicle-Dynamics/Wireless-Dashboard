const WebSocket = require('ws');

const ws = new WebSocket('ws://127.0.0.1:8080');

const messages = [
    { raw: "1000", angle: "0", sample: "1" },
    { raw: "1050", angle: "1100", sample: "2" },
    { raw: "1100", angle: "1100", sample: "3" },
    { raw: "950",  angle: "2048", sample: "4" },
    { raw: "1200", angle: "1100", sample: "5" },
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