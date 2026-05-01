// Dashboard_UI.h
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    body { font-family: sans-serif; color: #333; }
    canvas { margin: auto; display: block; }

    section {
      display: flex;
      flex-flow: row;
      flex-wrap: wrap;
      gap: 10px;
      justify-content: space-around;
    }

    .steering { position: relative; height: 200px; width: 200px; margin: 20px auto; }
    #Steering_angle { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); font-size: 1.5rem; font-weight: bold; }
    .thermal { border-radius: 8px; }
    .thermal-data { font-size: 1.1rem; font-family: monospace; }

    /* Recorder styles */
    #recorder {
      border: 1px solid #ccc;
      border-radius: 6px;
      padding: 10px 14px;
      margin: 10px;
      display: flex;
      align-items: center;
      gap: 12px;
      flex-wrap: wrap;
      background: #f9f9f9;
    }

    #recIndicator {
      width: 10px; height: 10px;
      border-radius: 50%;
      background: #ccc;
      flex-shrink: 0;
    }

    #recIndicator.recording {
      background: red;
      animation: blink 1s infinite;
    }

    @keyframes blink {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.3; }
    }

    #recorder label { font-size: 0.85rem; color: #555; }
    #recorder input[type="text"] { font-size: 0.85rem; padding: 2px 5px; width: 130px; }
    #recorder button { font-size: 0.85rem; padding: 3px 10px; cursor: pointer; }
    #recStats { font-size: 0.85rem; color: #555; font-family: monospace; }
  </style>
</head>
<body>

  <div id="data" style="font-size: 1.2rem;">Status: No data</div>

  <!-- Recorder Bar -->
  <div id="recorder">
    <div id="recIndicator"></div>

    <button id="btnRecord" onclick="toggleRecord()">⏺ Record</button>
    <button id="btnSave" onclick="saveCSV()" disabled>⬇ Save CSV</button>
    <button id="btnClear" onclick="clearData()" disabled>✕ Clear</button>

    <span id="recStats">0 samples</span>

    <label>Filename: <input type="text" id="cfgFilename" value="recording"></label>
    <label>Keys (blank = all): <input type="text" id="cfgKeys" placeholder="e.g. steer, pitch, roll"></label>
  </div>

  <section>
    <div class="thermal">
      <h3>Engine Data</h3>
      <p class="thermal-data">
        Air: <span id="air_t">No data</span> | 
        In: <span id="c1_t">No data</span> |
        Out: <span id="c2_t">No data</span>
      </p>
    </div>

    <div class="section-divider">
      <h3 style="text-align: center;">Steering Angle</h3>
      <div class="steering">
        <p id="Steering_angle">No data</p>
        <canvas id="Steering_chart" width="300" height="300"></canvas>
      </div>
    </div>
  </section>

  <section>
    <div class="IMU">
      <h3 style="display: inline-block;">IMU Data</h3>
      <button onclick="offsetYaw()">reset yaw</button>
      <canvas id="IMU_chart" width="800" height="300"></canvas>
      <canvas id="Accel_chart" width="800" height="300"></canvas>
    </div>

    <div class="Suspension">
      <h3>Suspension Travel</h3>
      <canvas id="LP_chart" width="800" height="300"></canvas>
    </div>
  </section>

  <script>
    // ─── Chart Setup ────────────────────────────────────────────────────────
    const maxDataPoints = 50;
    const LPctx       = document.getElementById("LP_chart").getContext("2d");
    const Steeringctx = document.getElementById("Steering_chart").getContext("2d");
    const IMUctx      = document.getElementById("IMU_chart").getContext("2d");
    const Accelctx    = document.getElementById("Accel_chart").getContext("2d");

    const linearPotData = {
      labels: [],
      datasets: [{ label: "Shock 1", data: [], borderColor: "#4CAF50", borderWidth: 2, fill: false, pointRadius: 0 }],
    };

    const steeringData = {
      labels: [],
      datasets: [{ data: [0, 4096, 4096], backgroundColor: ["#FF6384", "#36A2EB", "#eee"], borderWidth: 0 }],
    };

    const imuData = {
      labels: [],
      datasets: [
        { label: "Pitch", data: [], borderColor: "#f55442", borderWidth: 2, fill: false, pointRadius: 0 },
        { label: "Roll",  data: [], borderColor: "#42f542", borderWidth: 2, fill: false, pointRadius: 0 },
        { label: "Yaw",   data: [], borderColor: "#4272f5", borderWidth: 2, fill: false, pointRadius: 0 },
      ],
    };

    const accelData = {
      labels: [],
      datasets: [
        { label: "X", data: [], borderColor: "#f55442", borderWidth: 2, fill: false, pointRadius: 0 },
        { label: "Y", data: [], borderColor: "#42f542", borderWidth: 2, fill: false, pointRadius: 0 },
        { label: "Z", data: [], borderColor: "#4272f5", borderWidth: 2, fill: false, pointRadius: 0 },
      ],
    };

    const chart         = new Chart(LPctx,       { type: "line",    data: linearPotData, options: { responsive: false, animation: false, scales: { y: { beginAtZero: true, max: 4096 } } } });
    const Steeringchart = new Chart(Steeringctx, { type: "doughnut", data: steeringData,  options: { cutout: "80%", circumference: 270, rotation: -135, radius: "100" } });
    const IMUchart      = new Chart(IMUctx,      { type: "line",    data: imuData,        options: { responsive: false, animation: false, scales: { y: { min: -90, max: 90 } } } });
    const Accelchart    = new Chart(Accelctx,    { type: "line",    data: accelData,      options: { responsive: false, animation: false, scales: { y: { min: -5,  max: 5  } } } });

    // ─── Yaw Offset ─────────────────────────────────────────────────────────
    let currentYaw = 0, yawOffset = 0;
    function offsetYaw() { yawOffset = currentYaw; console.log("Yaw offset set to " + yawOffset); }

    // ─── Recorder ────────────────────────────────────────────────────────────
    let recording = false;
    let samples   = [];
    let headers   = [];

    const recIndicator = document.getElementById('recIndicator');
    const btnRecord    = document.getElementById('btnRecord');
    const btnSave      = document.getElementById('btnSave');
    const btnClear     = document.getElementById('btnClear');
    const recStats     = document.getElementById('recStats');

    function toggleRecord() {
      if (recording) {
        // Stop
        recording = false;
        recIndicator.classList.remove('recording');
        btnRecord.textContent = '⏺ Record';
        btnSave.disabled  = samples.length === 0;
        btnClear.disabled = samples.length === 0;
      } else {
        // Start — reset buffer
        samples = []; headers = [];
        recording = true;
        recIndicator.classList.add('recording');
        btnRecord.textContent = '⏹ Stop';
        btnSave.disabled  = true;
        btnClear.disabled = true;
        recStats.textContent = '0 samples';
      }
    }

    function recordSample(data) {
      if (!recording) return;

      const keysRaw = document.getElementById('cfgKeys').value.trim();
      const filterKeys = keysRaw ? keysRaw.split(',').map(k => k.trim()).filter(Boolean) : null;

      // Flatten one level (all values in the JSON are already flat here)
      const row = {};
      row._timestamp = new Date().toISOString();

      const src = filterKeys
        ? Object.fromEntries(filterKeys.map(k => [k, data[k] ?? '']))
        : data;

      Object.assign(row, src);

      // Build/extend header list
      Object.keys(row).forEach(k => { if (!headers.includes(k)) headers.push(k); });

      samples.push(row);
      recStats.textContent = samples.length + ' samples';
    }

    function saveCSV() {
      if (!samples.length) return;

      const cols = ['_timestamp', ...headers.filter(h => h !== '_timestamp')];
      const esc  = v => { const s = String(v ?? ''); return (s.includes(',') || s.includes('"') || s.includes('\n')) ? `"${s.replace(/"/g,'""')}"` : s; };

      const csv = [
        cols.map(esc).join(','),
        ...samples.map(row => cols.map(c => esc(row[c])).join(','))
      ].join('\r\n');

      const a = document.createElement('a');
      a.href     = URL.createObjectURL(new Blob([csv], { type: 'text/csv' }));
      a.download = (document.getElementById('cfgFilename').value.trim() || 'recording') + '.csv';
      document.body.appendChild(a); a.click(); document.body.removeChild(a);
    }

    function clearData() {
      samples = []; headers = [];
      recStats.textContent = '0 samples';
      btnSave.disabled  = true;
      btnClear.disabled = true;
    }

    // ─── WebSocket ───────────────────────────────────────────────────────────
    const host    = window.location.hostname || '127.0.0.1';
    const gateway = `ws://${host}:8080/ws`;
    var ws        = new WebSocket(gateway);

    ws.onmessage = function (e) {
      const data = JSON.parse(e.data);

      // Pass raw data object to recorder
      recordSample(data);

      if (data.raw === -1) {
        document.getElementById("data").innerText = "Status: No data";
        document.getElementById("Steering_angle").innerText = "No data";
        steeringData.datasets[0].backgroundColor = ["#eee", "#eee", "#eee"];
      } else {
        document.getElementById("data").innerText = "Status: Connected";
        document.getElementById("Steering_angle").innerText = data.steer + "\u00B0";
        steeringData.datasets[0].backgroundColor = ["#FF6384", "#36A2EB", "#eee"];

        linearPotData.labels.push(data.sample);
        linearPotData.datasets[0].data.push(data.raw);

        imuData.labels.push(data.sample);
        imuData.datasets[0].data.push(data.pitch);
        imuData.datasets[1].data.push(data.roll);
        imuData.datasets[2].data.push(data.yaw - yawOffset);
        currentYaw = imuData.datasets[2].data.at(-1);

        accelData.labels.push(data.sample);
        accelData.datasets[0].data.push(data.xaccel);
        accelData.datasets[1].data.push(data.yaccel);
        accelData.datasets[2].data.push(data.zaccel);

        if (linearPotData.labels.length > maxDataPoints) {
          linearPotData.labels.shift();
          linearPotData.datasets[0].data.shift();
        }
        if (imuData.labels.length > maxDataPoints) {
          imuData.labels.shift();
          imuData.datasets[0].data.shift();
          imuData.datasets[1].data.shift();
          imuData.datasets[2].data.shift();
          accelData.labels.shift();
          accelData.datasets[0].data.shift();
          accelData.datasets[1].data.shift();
          accelData.datasets[2].data.shift();
        }

        steeringData.datasets[0].data[0] = data.raw;
        steeringData.datasets[0].data[1] = 4096 - data.raw;
        steeringData.datasets[0].data[2] = 4096;
      }

      document.getElementById("air_t").innerText = data.air === -31.5 ? "No data" : data.air + "\u00B0C";
      document.getElementById("c1_t").innerText  = data.c1  === -31.5 ? "No data" : data.c1  + "\u00B0C";
      document.getElementById("c2_t").innerText  = data.c2  === -31.5 ? "No data" : data.c2  + "\u00B0C";

      chart.update();
      Steeringchart.update();
      IMUchart.update();
      Accelchart.update();
    };

    ws.onopen  = function() { document.getElementById("data").innerText = "Status: Connected"; };
    ws.onerror = function() { document.getElementById("data").innerText = "Status: No data"; };
  </script>
</body>
</html>


)rawliteral";