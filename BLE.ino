#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CSR_IoT_Lab";
const char* password = "#IOT@1984";

WebServer server(80);

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Media Control</title>
  <style>
    body {
      text-align: center;
      font-family: sans-serif;
      margin-top: 50px;
    }

    button {
      font-size: 22px;
      padding: 15px 40px;
      margin: 10px;
      border-radius: 10px;
    }

    .slider-container {
      margin-top: 40px;
    }

    .vol-label {
      font-size: 20px;
      margin-right: 10px;
    }

    #volumeValue {
      font-size: 20px;
      margin-left: 10px;
    }

    input[type=range] {
      -webkit-appearance: none;
      width: 90%;
      height: 10px;
      background: #ccc;
      border-radius: 5px;
      outline: none;
      background-image: linear-gradient(#4caf50, #4caf50);
      background-repeat: no-repeat;
    }

    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      height: 30px;
      width: 30px;
      background: #333;
      border-radius: 50%;
      cursor: pointer;
      margin-top: -10px;
    }

    input[type=range]::-moz-range-thumb {
      height: 30px;
      width: 30px;
      background: #333;
      border-radius: 50%;
      cursor: pointer;
    }

    input[type=range]::-moz-range-track {
      height: 10px;
      background: #ccc;
      border-radius: 5px;
    }

    input[type=range]::-moz-range-progress {
      background-color: #4caf50;
      height: 10px;
      border-radius: 5px;
    }
  </style>

</head>
<body>
  <h1>&#127926 Media Control</h1>
  <button onclick="fetch('/next')">&#9193 Next</button>
  <button onclick="fetch('/play')">&#x23ef Play/Pause</button>
  <button onclick="fetch('/prev')">&#9194 Previous</button>

  <div class="slider-container">
    <label class="vol-label" for="volume">&#128266 Volume:</label>
    <input type="range" id="volume" min="0" max="100" value="50">
    <span id="volumeValue">?</span>
  </div>

  <script>
    const slider = document.getElementById("volume");
    const display = document.getElementById("volumeValue");

    function updateSliderStyle(val) {
      const percent = (val - slider.min) / (slider.max - slider.min) * 100;
      slider.style.background = `linear-gradient(to right, #4caf50 ${percent}%, #ccc ${percent}%)`;
    }

    slider.oninput = () => {
      display.textContent = slider.value;
      fetch(`/volume?value=${slider.value}`)
        .then(res => res.text())
        .then(data => console.log("ESP32 says:", data));
      updateSliderStyle(slider.value);
    };

    updateSliderStyle(slider.value); // initial fill
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() { server.send(200, "text/html", htmlPage); });
  server.on("/play", []() { Serial.println("play"); server.send(200, "OK"); });
  server.on("/next", []() { Serial.println("next"); server.send(200, "OK"); });
  server.on("/prev", []() { Serial.println("prev"); server.send(200, "OK"); });
  server.on("/volume", []() {
    if (server.hasArg("value")) {
      String volume = server.arg("value");
      Serial.print("Volume set to: ");
      Serial.println(volume);
    }
    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}