const WebSocket = require('ws');
const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', (ws) => {
    console.log('Client connected');
    ws.send(JSON.stringify({ message: 'connected', status: 'ok' }));

    ws.on('message', (data) => {
        console.log('Received:', data.toString());
        
        // Broadcast to ALL connected clients (including the browser)
        wss.clients.forEach((client) => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(data.toString());
            }
        });
    });

    ws.on('close', () => console.log('Client disconnected'));
});

console.log('WebSocket server running on ws://127.0.0.1:8080');