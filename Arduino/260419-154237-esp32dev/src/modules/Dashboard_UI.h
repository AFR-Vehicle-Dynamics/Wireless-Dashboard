// Dashboard_UI.h
const char index_html[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
  <head>
    <meta charset="UTF-8" />
    <title>AFR Telemetry Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
      :root {
        /* AFR Team Colors */
        --afr-blue: #005279;
        --afr-gold: #f5a103;
      }

      /* General page styling */
      body {
        font-family: sans-serif;
        color: black;
        background-color: #f4f4f4;
        margin: 0;
      }

      /* Status bar for connection info */
      #status-bar {
        padding: 10px 20px;
        background: var(--afr-blue);
        color: #fff;
        font-weight: bold;
        border-bottom: 2px solid var(--afr-gold);
      }

      /* Responsive grid layout */
      .dashboard-grid {
        display: grid;
        gap: 15px;
        padding: 15px;

        /* Default to 3 equal columns for desktop*/
        grid-template-columns: repeat(3, 1fr);
      }

      /* Tablet: If the screen is smaller than 900px, switch to 2 columns */
      @media (max-width: 900px) {
        .dashboard-grid {
          grid-template-columns: repeat(2, 1fr);
        }
      }

      /* Phone: If the screen is smaller than 600px, switch to 1 column */
      @media (max-width: 600px) {
        .dashboard-grid {
          grid-template-columns: 1fr;
        }
      }

      /* Individual card styling */
      .card {
        padding: 15px;
        background-color: #fff;
        display: flex;
        flex-direction: column;

        border: 1px solid #ddd;
        border-top: 5px solid var(--afr-gold);
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
      }

      .card-header {
        font-size: 0.8rem;
        font-weight: bold;
        text-transform: uppercase;
        color: #333;
        display: flex;
        justify-content: space-between;
        align-items: center;
        border-bottom: 1px solid #eee;
        padding-bottom: 5px;
      }

      .card-value {
        font-size: 3rem;
        font-weight: bold;
        text-align: center;
        color: black;
        margin: 10px 0;
      }

      /* Ensures charts stay inside the cards */
      .chart-container {
        width: 100%;
        height: 180px;
        display: flex;
        justify-content: center;
        align-items: center;
        overflow: hidden;
      }

      button {
        background-color: var(--afr-gold);
        border: none;
        border-radius: 3px;
        font-weight: bold;
        color: black;
        cursor: pointer;
        padding: 4px 8px;
      }
    </style>
  </head>
  <body>
    <div id="status-bar">
      AFR Telemetry - Status: <span id="conn-status">Disconnected</span>
    </div>

    <div class="dashboard-grid">
      <div class="card" id="engine-card">
        <div class="card-header">Engine Temperatures</div>
        <div class="card-value"><span id="coolant_in_val">--</span>&deg;C</div>
        <div
          style="
            display: flex;
            justify-content: space-around;
            margin-bottom: 10px;
            font-weight: bold;
          "
        >
          <div>Air: <span id="air_temp_val">--</span>&deg;C</div>
          <div>Out: <span id="coolant_out_val">--</span>&deg;C</div>
        </div>
        <div class="chart-container">
          <canvas id="thermal_chart"></canvas>
        </div>
      </div>

      <div class="card" id="imu-card">
        <div class="card-header">
          IMU Data (Pitch)
          <button onclick="offsetYaw()">Reset Yaw</button>
        </div>
        <div class="card-value"><span id="pitch_val">--</span>&deg;</div>
        <div class="chart-container">
          <canvas id="imu_chart"></canvas>
        </div>
      </div>

      <div class="card" id="gforce-card">
        <div class="card-header">G-Force Data (Lateral)</div>
        <div class="card-value"><span id="accel_val">--</span> G</div>
        <div class="chart-container">
          <canvas id="accel_chart"></canvas>
        </div>
      </div>

      <div class="card" id="steering-card">
        <div class="card-header">Steering Input</div>
        <div class="chart-container" style="position: relative; height: 250px">
          <div
            style="
              position: absolute;
              top: 60%;
              left: 50%;
              transform: translate(-50%, -50%);
              text-align: center;
            "
          >
            <span
              id="steer_val"
              style="font-size: 3.5rem; font-weight: bold; line-height: 1"
              >--</span
            >

            <span
              style="
                font-size: 1.5rem;
                font-weight: bold;
                vertical-align: 1.5rem;
              "
              >&deg;</span
            >
          </div>
          <canvas id="steering_chart"></canvas>
        </div>
      </div>

      <div class="card" id="suspension-card">
        <div class="card-header">Suspension Travel</div>
        <div class="card-value"><span id="susp_val">--</span> mm</div>
        <div class="chart-container">
          <canvas id="suspension_chart"></canvas>
        </div>
      </div>
    </div>

    <script>
      const maxDataPoints = 150;
      let currentYaw = 0;
      let yawOffset = 0;

      function offsetYaw() {
        yawOffset = currentYaw;
        console.log("Yaw offset set to " + yawOffset);
      }

      // thermal chart settings
      const thermalData = {
        labels: [],
        datasets: [
          {
            label: "In (C1)",
            data: [],
            borderColor: "#FF6384",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
          {
            label: "Air",
            data: [],
            borderColor: "#36A2EB",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
          {
            label: "Out (C2)",
            data: [],
            borderColor: "#4BC0C0",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
        ],
      };

      // imu (pitch) chart settings
      const imuData = {
        labels: [],
        datasets: [
          {
            label: "Pitch",
            data: [],
            borderColor: "#FF6384",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
          {
            label: "Roll",
            data: [],
            borderColor: "#36A2EB",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
          {
            label: "Yaw",
            data: [],
            borderColor: "#4BC0C0",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
        ],
      };

      // acceleration chart settings
      const accelData = {
        labels: [],
        datasets: [
          {
            label: "X",
            data: [],
            borderColor: "#FF6384",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
          {
            label: "Y",
            data: [],
            borderColor: "#36A2EB",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
          {
            label: "Z",
            data: [],
            borderColor: "#4BC0C0",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
        ],
      };

      // steering angle chart settings
      const steeringData = {
        labels: [],
        datasets: [
          {
            data: [0, 4096, 4096],
            backgroundColor: ["#FF6384", "#36A2EB", "#eee"], // red, blue, and grey spot
            borderWidth: 0,
          },
        ],
      };

      // linear pot chart settings
      const linearPotData = {
        labels: [],
        datasets: [
          {
            label: "Shock 1",
            data: [],
            borderColor: "#005279",
            borderWidth: 2,
            fill: false,
            pointRadius: 0,
          },
        ],
      };

      // create thermal graph
      const thermalCtx = document
        .getElementById("thermal_chart")
        .getContext("2d");
      const thermalChart = new Chart(thermalCtx, {
        type: "line",
        data: thermalData,
        options: {
          responsive: true,
          maintainAspectRatio: false,
          animation: false,
        },
      });

      // create imu pitch graph
      const imuCtx = document.getElementById("imu_chart").getContext("2d");
      const imuChart = new Chart(imuCtx, {
        type: "line",
        data: imuData,
        options: {
          responsive: true,
          maintainAspectRatio: false,
          animation: false,
          scales: { y: { min: -90, max: 90 } },
        },
      });

      // create acceleration graph
      const accelCtx = document.getElementById("accel_chart").getContext("2d");
      const accelChart = new Chart(accelCtx, {
        type: "line",
        data: accelData,
        options: {
          responsive: true,
          maintainAspectRatio: false,
          animation: false,
          scales: { y: { min: -3, max: 3 } },
        },
      });

      // create steering doughnut gauge
      const steeringCtx = document
        .getElementById("steering_chart")
        .getContext("2d");
      const steeringChart = new Chart(steeringCtx, {
        type: "doughnut",
        data: steeringData,
        options: {
          cutout: "82%",
          circumference: 270,
          rotation: -135,
          maintainAspectRatio: false,
          layout: {
            padding: 0,
          },
          plugins: {
            legend: {
              display: false,
            },
          },
        },
      });

      // create linear pot graph
      const lpCtx = document
        .getElementById("suspension_chart")
        .getContext("2d");
      const lpChart = new Chart(lpCtx, {
        type: "line",
        data: linearPotData,
        options: {
          responsive: false,
          maintainAspectRatio: false,
          animation: false,
          scales: { y: { beginAtZero: true, max: 50 } },
        },
      });

      // WebSocket (Connection) Logic
      // Automatically detects if we are on the car's IP or testing locally
      const host = window.location.hostname || "127.0.0.1";
      const gateway = `ws://${host}:8081/ws`;

      var ws = new WebSocket(gateway);

      // runs every time the ESP32 sends new data
      ws.onmessage = function (e) {
        try {
          const data = JSON.parse(e.data);

          // if sensor is unplugged (returns -1), show "No data" and grey out
          if (data.raw <= 0) {
            document.getElementById("conn-status").innerText = "No data";
            document.getElementById("steer_val").innerText = "No data";
          } else {
            // update the text labels with real numbers
            document.getElementById("conn-status").innerText = "Connected";

            const steerDeg = (((data.steer - 2048) / 2048) * 135).toFixed(1);
            const suspMM = ((data.raw / 4095) * 50).toFixed(1);

            // update readouts
            document.getElementById("coolant_in_val").innerText =
              data.c1 === -31.5 ? "--" : data.c1;
            document.getElementById("air_temp_val").innerText =
              data.air === -31.5 ? "--" : data.air;
            document.getElementById("coolant_out_val").innerText =
              data.c2 === -31.5 ? "--" : data.c2;
            document.getElementById("pitch_val").innerText = data.pitch;
            document.getElementById("accel_val").innerText =
              data.yaccel === -1 ? "--" : data.yaccel;
            document.getElementById("steer_val").innerText = steerDeg;
            document.getElementById("susp_val").innerText = suspMM;

            // push data to charts
            thermalData.labels.push(data.sample);
            thermalData.datasets[0].data.push(data.c1);
            thermalData.datasets[1].data.push(data.air);
            thermalData.datasets[2].data.push(data.c2);

            imuData.labels.push(data.sample);
            imuData.datasets[0].data.push(data.pitch);
            imuData.datasets[1].data.push(data.roll);
            imuData.datasets[2].data.push(data.yaw - yawOffset);
            currentYaw =
              imuData.datasets[2].data[imuData.datasets[2].data.length - 1];

            accelData.labels.push(data.sample);
            accelData.datasets[0].data.push(data.xaccel);
            accelData.datasets[1].data.push(data.yaccel);
            accelData.datasets[2].data.push(data.zaccel);

            linearPotData.labels.push(data.sample);
            linearPotData.datasets[0].data.push(suspMM);

            // update steering
            steeringData.datasets[0].data[0] = data.raw;
            steeringData.datasets[0].data[1] = 4096 - data.raw;
            steeringData.datasets[0].data[2] = 4096;
          }

          // updates graph data
          const datasets = [thermalData, imuData, accelData, linearPotData];
          datasets.forEach((d) => {
            if (d.labels.length > maxDataPoints) {
              d.labels.shift();
              d.datasets.forEach((s) => s.data.shift());
            }
          });

          // refresh charts on screen
          thermalChart.update();
          imuChart.update();
          accelChart.update();
          steeringChart.update();
          lpChart.update();
        } catch (err) {
          console.log("Malformed packet skipped: ", err);
        }
      };

      // basic connection status updates
      ws.onopen = () => {
        document.getElementById("conn-status").innerText = "Connected";
      };
      ws.onerror = () => {
        document.getElementById("conn-status").innerText = "No Connection";
      };
    </script>
  </body>
</html>

)rawliteral";