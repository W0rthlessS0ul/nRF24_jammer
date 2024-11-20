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

        .settings-button {
            background-color: #28a745;
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
            <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAADICAYAAACtWK6eAAAACXBIWXMAABJ0AAASdAHeZh94AAAGnGlUWHRYTUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4bXB0az0iQWRvYmUgWE1QIENvcmUgOS4wLWMwMDEgNzkuYzAyMDRiMiwgMjAyMy8wMi8wOS0wNjoyNjoxNCAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIiB4bWxuczpkYz0iaHR0cDovL3B1cmwub3JnL2RjL2VsZW1lbnRzLzEuMS8iIHhtbG5zOnBob3Rvc2hvcD0iaHR0cDovL25zLmFkb2JlLmNvbS9waG90b3Nob3AvMS4wLyIgeG1sbnM6eG1wTU09Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9tbS8iIHhtbG5zOnN0RXZ0PSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvc1R5cGUvUmVzb3VyY2VFdmVudCMiIHhtcDpDcmVhdG9yVG9vbD0iQWRvYmUgUGhvdG9zaG9wIDI0LjYgKDIwMjMwNTI0Lm0uMjE4NSAwZTcxMzlkKSAgKFdpbmRvd3MpIiB4bXA6Q3JlYXRlRGF0ZT0iMjAyNC0wOS0xM1QyMDoxNjo0OCswMzowMCIgeG1wOk1vZGlmeURhdGU9IjIwMjQtMDktMTNUMjA6MjA6MTUrMDM6MDAiIHhtcDpNZXRhZGF0YURhdGU9IjIwMjQtMDktMTNUMjA6MjA6MTUrMDM6MDAiIGRjOmZvcm1hdD0iaW1hZ2UvcG5nIiBwaG90b3Nob3A6Q29sb3JNb2RlPSIzIiB4bXBNTTpJbnN0YW5jZUlEPSJ4bXAuaWlkOjU5YzJlOWIwLTQxM2EtYzM0NS1hOGIyLTBlZTIwZWI5ZmNlYiIgeG1wTU06RG9jdW1lbnRJRD0ieG1wLmRpZDo3MDVlZTJjZS03ZjIwLWE5NDMtYWZlNC0wOTI0YzJhMmQwMDEiIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDo3MDVlZTJjZS03ZjIwLWE5NDMtYWZlNC0wOTI0YzJhMmQwMDEiPiA8cGhvdG9zaG9wOkRvY3VtZW50QW5jZXN0b3JzPiA8cmRmOkJhZz4gPHJkZjpsaT54bXAuZGlkOjcwNWVlMmNlLTdmMjAtYTk0My1hZmU0LTA5MjRjMmEyZDAwMTwvcmRmOmxpPiA8L3JkZjpCYWc+IDwvcGhvdG9zaG9wOkRvY3VtZW50QW5jZXN0b3JzPiA8eG1wTU06SGlzdG9yeT4gPHJkZjpTZXE+IDxyZGY6bGkgc3RFdnQ6YWN0aW9uPSJjcmVhdGVkIiBzdEV2dDppbnN0YW5jZUlEPSJ4bXAuaWlkOjcwNWVlMmNlLTdmMjAtYTk0My1hZmU0LTA5MjRjMmEyZDAwMSIgc3RFdnQ6d2hlbj0iMjAyNC0wOS0xM1QyMDoxNjo0OCswMzowMCIgc3RFdnQ6c29mdHdhcmVBZ2VudD0iQWRvYmUgUGhvdG9zaG9wIDI0LjYgKDIwMjMwNTI0Lm0uMjE4NSAwZTcxMzlkKSAgKFdpbmRvd3MpIi8+IDxyZGY6bGkgc3RFdnQ6YWN0aW9uPSJzYXZlZCIgc3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlpZDo1OWMyZTliMC00MTNhLWMzNDUtYThiMi0wZWUyMGViOWZjZWIiIHN0RXZ0OndoZW49IjIwMjQtMDktMTNUMjA6MjA6MTUrMDM6MDAiIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9IkFkb2JlIFBob3Rvc2hvcCAyNC42ICgyMDIzMDUyNC5tLjIxODUgMGU3MTM5ZCkgIChXaW5kb3dzKSIgc3RFdnQ6Y2hhbmdlZD0iLyIvPiA8L3JkZjpTZXE+IDwveG1wTU06SGlzdG9yeT4gPC9yZGY6RGVzY3JpcHRpb24+IDwvcmRmOlJERj4gPC94OnhtcG1ldGE+IDw/eHBhY2tldCBlbmQ9InIiPz4cqzpoAAAh6klEQVR4nO2deXRU5f3wv89d597ZJ7sBwhrZBBJB+IlCKbsKFqtF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"></img>
        </div>

        <div class="buttons">
            <button onclick="location.href='/bluetooth_jam'" class="button">Bluetooth jammer</button>
            <button onclick="location.href='/drone_jam'" class="button">Drone jammer</button>
            <button onclick="location.href='/wifi_jam'" class="button">WiFi jammer</button>
            <button onclick="location.href='/ble_jam'" class="button">BLE jammer</button>
            <button onclick="location.href='/zigbee_jam'" class="button">Zigbee jammer</button>
            <button onclick="location.href='/misc_jammer'" class="button">Misc jammer</button>
            <button class="button settings-button" onclick="toggleDropdown()">Settings</button>
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