const char* html = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .header {
            margin-bottom: 20px;
        }

        .header img {
            width: 100px; 
            height: auto;
            margin-bottom: 10px;
        }

        .buttons {
            display: flex;
            flex-direction: column;
            gap: 15px; 
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s; 
            font-size: 16px;
            width: 100%;
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-2px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .button:active {
            transform: translateY(1px); 
        }

        .button:focus {
            outline: none; 
        }

        .settings-button {
            background-color: #28a745;
        }

        .ota-button {
            background-color: #ffc107;
        }

        .ota-button:hover {
            background-color: #e0a800;
        }

        .dropdown {
            position: absolute;
            top: 50px;
            right: 0;
            background-color: rgba(30, 30, 30, 0.8);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
        }

        .dropdown-button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
        }

        .dropdown-button:hover {
            background-color: #0056b3;
        }

        .dropdown-button:active {
            transform: translateY(1px); 
        }

        .dropdown-button:focus {
            outline: none;
        }
    </style>
    <script>
        function toggleDropdown() {
            const dropdown = document.getElementById('settingsDropdown');
            dropdown.style.display = dropdown.style.display === 'flex' ? 'none' : 'flex';
        }
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <img src="data:image/png;base64,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"></img>
        </div>

        <div class="buttons">
            <button onclick="location.href='/bluetooth_jam'" class="button">Bluetooth jammer</button>
            <button onclick="location.href='/drone_jam'" class="button">Drone jammer</button>
            <button onclick="location.href='/wifi_select'" class="button">WiFi jammer</button>
            <button onclick="location.href='/ble_jam'" class="button">BLE jammer</button>
            <button onclick="location.href='/zigbee_jam'" class="button">Zigbee jammer</button>
            <button onclick="location.href='/misc_jammer'" class="button">Misc jammer</button>
            <button class="button settings-button" onclick="toggleDropdown()">Settings</button>
            <button onclick="location.href='/OTA'" class="button ota-button">OTA</button>
            <div id="settingsDropdown" class="dropdown">
                <button onclick="location.href='/setting_bluetooth_jam'" class="dropdown-button">Bluetooth Jam</button>
                <button onclick="location.href='/setting_drone_jam'" class="dropdown-button">Drone Jam</button>
                <button onclick="location.href='/setting_separate_together'" class="dropdown-button">Separate or Together</button>
                <button onclick="location.href='/setting_misc_jam'" class="dropdown-button">Misc Jam</button>
            </div>
        </div>
    </div>
</body>
</html>
)rawliteral";

const char* html_ota = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 400px;
        }

        h2 {
            margin-bottom: 20px;
        }

        .file-upload {
            position: relative;
            margin-top: 10px;
        }

        .file-upload input[type="file"] {
            display: none;
        }

        .file-upload label {
            display: inline-block;
            background-color: #007bff;
            color: white;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }

        .file-upload label:hover {
            background-color: #0056b3;
        }

        .button {
            background-color: #28a745;
            border: none;
            color: white;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin-top: 20px;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }

        .button:hover {
            background-color: #218838;
        }

        .hint {
            margin-top: 10px;
            font-size: 14px;
            color: #cccccc;
        }

        .warning {
            margin-top: 15px;
            font-size: 14px;
            color: #ffcc00;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Firmware Update</h2>
        <form enctype='multipart/form-data' action='/update' method='POST'>
            <div class="file-upload">
                <label for="file-upload">Choose File</label>
                <input type='file' id="file-upload" name='update' accept=".bin" required>
            </div>
            <button class="button" type="submit">Upload</button>
            <div class="hint">Please select a .bin file for update.</div>
            <div class="warning">⚠️ Do not select bootloader or partition files!</div>
        </form>
    </div>
</body>
</html>
)rawliteral";

const char* html_upload_progress = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        #progress {
            width: 100%;
            background-color: #ddd;
            border-radius: 5px;
            overflow: hidden;
        }

        #progress-bar {
            width: 0;
            height: 30px;
            background-color: #007bff;
            text-align: center;
            color: white;
            transition: width 0.2s;
        }

        #status {
            margin-top: 10px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Uploading Firmware...</h2>
        <div id="progress">
            <div id="progress-bar">0%</div>
        </div>
        <div id="status"></div>
    </div>
    <script>
        function updateProgress(percent) {
            const progressBar = document.getElementById("progress-bar");
            progressBar.style.width = percent + '%';
            progressBar.innerHTML = percent + '%';
        }
    </script>
</body>
</html>
)rawliteral";

const char* html_misc_jam = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        .text {
            font-size: 24px;
            color: #007bff;
            border-radius: 8px;
        }

        .dots {
            font-size: 24px;
            color: #007bff;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="text">Jamming of selected channels</div>
        <div class="dots" id="dots">.</div>
    </div>

    <script>
        const dotsElement = document.getElementById('dots');
        let dots = 1;

        setInterval(() => {
            dots = (dots % 3) + 1;
            dotsElement.innerText = '.'.repeat(dots);
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

const char* html_bluetooth_jam = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        .text {
            font-size: 24px;
            color: #007bff;
            border-radius: 8px;
        }

        .dots {
            font-size: 24px;
            color: #007bff;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="text">Bluetooth jam</div>
        <div class="dots" id="dots">.</div>
    </div>

    <script>
        const dotsElement = document.getElementById('dots');
        let dots = 1;

        setInterval(() => {
            dots = (dots % 3) + 1;
            dotsElement.innerText = '.'.repeat(dots);
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

const char* html_drone_jam = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        .text {
            font-size: 24px;
            color: #007bff;
            border-radius: 8px;
        }

        .dots {
            font-size: 24px;
            color: #007bff;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="text">Drone jam</div>
        <div class="dots" id="dots">.</div>
    </div>

    <script>
        const dotsElement = document.getElementById('dots');
        let dots = 1;

        setInterval(() => {
            dots = (dots % 3) + 1;
            dotsElement.innerText = '.'.repeat(dots);
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

const char* html_wifi_jam = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        .text {
            font-size: 24px;
            color: #007bff;
            border-radius: 8px;
        }

        .dots {
            font-size: 24px;
            color: #007bff;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="text">WiFi jam</div>
        <div class="dots" id="dots">.</div>
    </div>

    <script>
        const dotsElement = document.getElementById('dots');
        let dots = 1;

        setInterval(() => {
            dots = (dots % 3) + 1;
            dotsElement.innerText = '.'.repeat(dots);
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

const char* html_ble_jam = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        .text {
            font-size: 24px;
            color: #007bff;
            border-radius: 8px;
        }

        .dots {
            font-size: 24px;
            color: #007bff;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="text">BLE jam</div>
        <div class="dots" id="dots">.</div>
    </div>

    <script>
        const dotsElement = document.getElementById('dots');
        let dots = 1;

        setInterval(() => {
            dots = (dots % 3) + 1;
            dotsElement.innerText = '.'.repeat(dots);
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

const char* html_zigbee_jam = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
        }

        .text {
            font-size: 24px;
            color: #007bff;
            border-radius: 8px;
        }

        .dots {
            font-size: 24px;
            color: #007bff;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="text">Zigbee jam</div>
        <div class="dots" id="dots">.</div>
    </div>

    <script>
        const dotsElement = document.getElementById('dots');
        let dots = 1;

        setInterval(() => {
            dots = (dots % 3) + 1;
            dotsElement.innerText = '.'.repeat(dots);
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

const char* html_bluetooth_setings = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .header {
            margin-bottom: 20px;
        }

        .buttons {
            display: flex;
            flex-direction: column;
            gap: 15px; 
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s; 
            font-size: 16px;
            width: 100%;
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-2px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .button:active {
            transform: translateY(1px); 
        }

        .button:focus {
            outline: none; 
        }

        .dropdown {
            position: absolute;
            top: 50px;
            right: 0;
            background-color: rgba(30, 30, 30, 0.8);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
        }

        .dropdown-button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            text-align: center;
        }

        .dropdown-button:hover {
            background-color: #0056b3;
        }

        .dropdown-button:active {
            transform: translateY(1px); 
        }

        .dropdown-button:focus {
            outline: none;
        }
    </style>
    <script>
        function toggleDropdown() {
            const dropdown = document.getElementById('settingsDropdown');
            dropdown.style.display = dropdown.style.display === 'flex' ? 'none' : 'flex';
        }
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Select the channel scanning type:</h1>
        </div>

        <div class="buttons">
            <button class="button" onclick="location.href='/bluetooth_method_0'">Brute Force By List (List 21)</button>
            <button class="button" onclick="location.href='/bluetooth_method_1'">Randomize Values (0-79)</button>
            <button class="button" onclick="location.href='/bluetooth_method_2'">Brute Force By Values (0-79)</button>
        </div>
    </div>
</body>
</html>
)rawliteral";

const char* html_drone_setings = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .header {
            margin-bottom: 20px;
        }

        .buttons {
            display: flex;
            flex-direction: column;
            gap: 15px; 
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s; 
            font-size: 16px;
            width: 100%;
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-2px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .button:active {
            transform: translateY(1px); 
        }

        .button:focus {
            outline: none; 
        }

        .dropdown {
            position: absolute;
            top: 50px;
            right: 0;
            background-color: rgba(30, 30, 30, 0.8);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
        }

        .dropdown-button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            text-align: center;
        }

        .dropdown-button:hover {
            background-color: #0056b3;
        }

        .dropdown-button:active {
            transform: translateY(1px); 
        }

        .dropdown-button:focus {
            outline: none;
        }
    </style>
    <script>
        function toggleDropdown() {
            const dropdown = document.getElementById('settingsDropdown');
            dropdown.style.display = dropdown.style.display === 'flex' ? 'none' : 'flex';
        }
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Select the channel scanning type:</h1>
        </div>

        <div class="buttons">
            <button class="button" onclick="location.href='/drone_method_0'">Randomize Values (0-125)</button>
            <button class="button" onclick="location.href='/drone_method_1'">Brute Force By Values (0-125)</button>
        </div>
    </div>
</body>
</html>
)rawliteral";

const char* html_misc_setings = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .header {
            margin-bottom: 20px;
        }

        .buttons {
            display: flex;
            flex-direction: column;
            gap: 15px; 
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s; 
            font-size: 16px;
            width: 100%;
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-2px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .button:active {
            transform: translateY(1px); 
        }

        .button:focus {
            outline: none; 
        }

        .dropdown {
            position: absolute;
            top: 50px;
            right: 0;
            background-color: rgba(30, 30, 30, 0.8);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
        }

        .dropdown-button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            text-align: center;
        }

        .dropdown-button:hover {
            background-color: #0056b3;
        }

        .dropdown-button:active {
            transform: translateY(1px); 
        }

        .dropdown-button:focus {
            outline: none;
        }
    </style>
    <script>
        function toggleDropdown() {
            const dropdown = document.getElementById('settingsDropdown');
            dropdown.style.display = dropdown.style.display === 'flex' ? 'none' : 'flex';
        }
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Select the channel scanning type:</h1>
        </div>

        <div class="buttons">
            <button class="button" onclick="location.href='/misc_method_0'">Channel Switching (fast mode; used in Bluetooth Jam)</button>
            <button class="button" onclick="location.href='/misc_method_1'">Packet Sending (slow mode; used in WiFi Jam)</button>
        </div>
    </div>
</body>
</html>
)rawliteral";

const char* html_separate_or_together = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .header {
            margin-bottom: 20px;
        }

        .buttons {
            display: flex;
            flex-direction: column;
            gap: 15px; 
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s; 
            font-size: 16px;
            width: 100%;
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-2px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .button:active {
            transform: translateY(1px); 
        }

        .button:focus {
            outline: none; 
        }

        .dropdown {
            position: absolute;
            top: 50px;
            right: 0;
            background-color: rgba(30, 30, 30, 0.8);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
        }

        .dropdown-button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            text-align: center;
        }

        .dropdown-button:hover {
            background-color: #0056b3;
        }

        .dropdown-button:active {
            transform: translateY(1px); 
        }

        .dropdown-button:focus {
            outline: none;
        }
    </style>
    <script>
        function toggleDropdown() {
            const dropdown = document.getElementById('settingsDropdown');
            dropdown.style.display = dropdown.style.display === 'flex' ? 'none' : 'flex';
        }
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Select the channel scanning type:</h1>
        </div>

        <div class="buttons">
            <button class="button" onclick="location.href='/separate_or_together_method_0'">Modules on Different Channels</button>
            <button class="button" onclick="location.href='/separate_or_together_method_1'">Modules on Same Channel</button>
        </div>
    </div>
</body>
</html>
)rawliteral";

const char* html_wifi_select = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .header {
            margin-bottom: 20px;
        }

        .buttons {
            display: flex;
            flex-direction: column;
            gap: 15px; 
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s; 
            font-size: 16px;
            width: 100%;
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-2px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .button:active {
            transform: translateY(1px); 
        }

        .button:focus {
            outline: none; 
        }

        .dropdown {
            position: absolute;
            top: 50px;
            right: 0;
            background-color: rgba(30, 30, 30, 0.8);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
        }

        .dropdown-button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
            text-align: center;
        }

        .dropdown-button:hover {
            background-color: #0056b3;
        }

        .dropdown-button:active {
            transform: translateY(1px); 
        }

        .dropdown-button:focus {
            outline: none;
        }
    </style>
    <script>
        function toggleDropdown() {
            const dropdown = document.getElementById('settingsDropdown');
            dropdown.style.display = dropdown.style.display === 'flex' ? 'none' : 'flex';
        }
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Select Jamming Type:</h1>
        </div>

        <div class="buttons">
            <button class="button" onclick="location.href='/wifi_jam'">Jam All Channels</button>
            <button class="button" onclick="location.href='/wifi_channel'">Select Channel To Jam</button>
        </div>
    </div>
</body>
</html>
)rawliteral";

const char* html_wifi_channel = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-2px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }
    </style>
    <script>
        function validateAndRedirect() {
            const channelValue = parseInt(document.getElementById('channelInput').value);

            if (isNaN(channelValue) || channelValue < 0 || channelValue > 12) {
                alert('Channel must be a number between 0 and 12.');
                return;
            }
            location.href = `/wifi_selected_jam?channel=${channelValue}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Jam Channel:</h1>
        <input id="channelInput" class="input" type="number" placeholder="Channel (0-12)" max="12" min="0" />
        <button class="button" onclick="validateAndRedirect()">Submit</button>
    </div>
</body>
</html>
)rawliteral";

const char* html_misc_jammer = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: 'Arial', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #121212;
            color: #ffffff;
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 15px;
            text-align: center;
        }

        .button {
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            cursor: pointer;
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
            font-size: 16px;
            width: 100%;
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-2px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button:focus {
            outline: none;
        }
    </style>
    <script>
        function validateAndRedirect() {
            const startValue = parseInt(document.getElementById('startInput').value);
            const stopValue = parseInt(document.getElementById('stopInput').value);

            if (isNaN(startValue) || startValue < 0 || startValue > 125) {
                alert('Start value must be a number between 0 and 125.');
                return;
            }
            if (isNaN(stopValue) || stopValue < 0 || stopValue > 125) {
                alert('Stop value must be a number between 0 and 125.');
                return;
            }
            location.href = `/misc_jam?start=${startValue}&stop=${stopValue}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Set Start and Stop Values:</h1>
        <input id="startInput" class="input" type="number" placeholder="Start (0-125)" max="125" min="0" />
        <input id="stopInput" class="input" type="number" placeholder="Stop (0-125)" max="125" min="0" />
        <button class="button" onclick="validateAndRedirect()">Jam</button>
    </div>
</body>
</html>
)rawliteral";