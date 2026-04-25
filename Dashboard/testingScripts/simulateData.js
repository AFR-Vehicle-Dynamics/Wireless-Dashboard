const WebSocket = require("ws");

const wss = new WebSocket.Server({ port: 8081 });

console.log("Access the AFR Track Simulator at: ws://localhost:8080/ws");

let sampleCount = 0;
let engineTemp = 85.0;

wss.on("connection", (ws) => {
  console.log("Dashboard linked. Starting lap simulation...");

  const sendTelemetry = setInterval(() => {
    sampleCount++;
    const time = Date.now() / 1000;

    const rng = Math.random();

    if (rng < 0.02) {
      console.log("Simulating sensor disconnect (Raw: 0)...");
      ws.send(JSON.stringify({ raw: 0, steer: 0, sample: sampleCount }));
      return;
    }

    if (rng < 0.04) {
      console.log("Simulating malformed packet (Bad JSON)...");
      ws.send('{"raw": 2048, "mangled": ');
      return;
    }

    // Simulate steering and lateral Gs moving together
    const steerInput = 2048 + Math.sin(time * 1.5) * 1500;
    const lateralG = (Math.sin(time * 1.5) * 2.2).toFixed(2);

    // Engine heat soak: temps slowly rise then fluctuate
    engineTemp += 0.01;
    const coolantIn = (engineTemp + Math.random()).toFixed(1);

    const data = {
      sample: sampleCount,

      // Engine
      c1: coolantIn,
      air: (28.4 + Math.random() * 0.5).toFixed(1),
      c2: (coolantIn - 12.5).toFixed(1),

      // IMU & Dynamics
      pitch: (Math.sin(time * 3) * 2.5 + (Math.random() - 0.5)).toFixed(2),
      roll: (Math.cos(time * 3) * 1.5).toFixed(2),
      yaw: sampleCount % 360,

      xaccel: (Math.random() * 0.3).toFixed(2),
      yaccel: lateralG,
      zaccel: (1.0 + (Math.random() - 0.5) * 0.1).toFixed(2),

      // Hardware Pots
      steer: Math.floor(steerInput),
      raw: Math.floor(2000 + Math.cos(time * 5) * 400),
    };

    ws.send(JSON.stringify(data));
  }, 100); // 100ms = 10Hz Sampling Rate

  ws.on("close", () => {
    clearInterval(sendTelemetry);
    console.log("Dashboard disconnected.");
  });
});
