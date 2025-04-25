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
            opacity: 0;
            animation: fadeIn 1s forwards;
        }

        @keyframes fadeIn {
            from {
                opacity: 0;
            }
            to {
                opacity: 1;
            }
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
            transform: translateY(20px);
            animation: slideIn 0.5s forwards;
        }

        @keyframes slideIn {
            from {
                transform: translateY(20px);
                opacity: 0;
            }
            to {
                transform: translateY(0);
                opacity: 1;
            }
        }

        .header {
            margin-bottom: 20px;
            animation: logoMovement 1s infinite alternate;
        }

        @keyframes logoMovement {
            0% { transform: translateY(0); }
            100% { transform: translateY(-5px); }
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

        .button, .dropdown-button {
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover, .dropdown-button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px) scale(1.05); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active, .dropdown-button:active {
            transform: translateY(2px); 
        }

        .button:focus, .dropdown-button:focus {
            outline: none; 
        }

        .settings-button {
            background-color: #28a745;
        }

        .ota-button {
            background-color: #ffc107; 
        }

        .dropdown {
            position: absolute;
            top: 50px;
            right: 0;
            background-color: rgba(30, 30, 30, 0.9);
            border-radius: 8px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            display: none;
            flex-direction: column;
            gap: 5px;
            z-index: 10;
        }

        .dropdown.show {
            display: flex;
            animation: dropdownExpand 0.6s forwards;
        }

        @keyframes dropdownExpand {
            0% {
                opacity: 0;
                transform: scaleY(0);
            }
        }

        .hidden {
            opacity: 0;
            transform: translateY(-20px);
            transition: opacity 0.2s ease, transform 0.2s ease;
        }
    </style>
    <script>
        function toggleDropdown() {
            const dropdown = document.getElementById('settingsDropdown');
            dropdown.classList.toggle('show');

            const items = dropdown.querySelectorAll('.dropdown-button');

            if (dropdown.classList.contains('show')) {
                items.forEach((item, index) => {
                    setTimeout(() => {
                        item.classList.remove('hidden');
                        item.style.opacity = 1;
                        item.style.transform = 'translateY(0)';
                    }, index * 50);
                });
            } else {
                items.forEach((item) => {
                    item.classList.add('hidden');
                    item.style.opacity = 0;
                    item.style.transform = 'translateY(-20px)';
                });
            }
        }

        window.addEventListener('click', function(event) {
            const dropdown = document.getElementById('settingsDropdown');
            const settingsButton = document.querySelector('.settings-button');
            if (!settingsButton.contains(event.target) && !dropdown.contains(event.target)) {
                dropdown.classList.remove('show');
                const items = dropdown.querySelectorAll('.dropdown-button');
                items.forEach((item) => {
                    item.classList.add('hidden');
                    item.style.opacity = 0;
                    item.style.transform = 'translateY(-20px)';
                });
            }
        });
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <img src="data:image/png;base64,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" alt="Logo"></img>
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
                <button onclick="location.href='/setting_bluetooth_jam'" class="dropdown-button hidden">Bluetooth Jam</button>
                <button onclick="location.href='/setting_drone_jam'" class="dropdown-button hidden">Drone Jam</button>
                <button onclick="location.href='/setting_separate_together'" class="dropdown-button hidden">Separate or Together</button>
                <button onclick="location.href='/setting_misc_jam'" class="dropdown-button hidden">Misc Jam</button>
                <button onclick="location.href='/setting_logo'" class="dropdown-button hidden">Logo</button>
                <button onclick="location.href='/setting_buttons'" class="dropdown-button hidden">Buttons</button>
                <button onclick="location.href='/wifi_settings'" class="dropdown-button hidden">WiFi Settings</button>

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
            transition: background-color 0.5s;
            opacity: 0;
            animation: fadeIn 1s forwards;
        }

        @keyframes fadeIn {
            from {
                opacity: 0;
            }
            to {
                opacity: 1;
            }
        }

        .container {
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 400px;
            position: relative;
            transform: translateY(20px);
            animation: slideIn 0.5s forwards;
        }

        @keyframes slideIn {
            from {
                transform: translateY(20px);
                opacity: 0;
            }
            to {
                transform: translateY(0);
                opacity: 1;
            }
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
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
        }

        .file-upload label:hover {
            background-color: #0056b3;
            transform: translateY(-4px) scale(1.05);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
        }

        .button:hover {
            background-color: #218838;
            transform: translateY(-4px) scale(1.05);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(2px);
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

const char* html_pls_reboot = R"rawliteral(
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
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">Please reboot</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
        }, 1000);
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
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">Jamming of selected channels</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
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
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">Bluetooth Jamming</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
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
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">Drone Jamming</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
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
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">WiFi Jamming</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
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
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">BLE Jamming</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
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
            margin-bottom: 15px;
            display: inline-block;
        }

        .circle-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .circle {
            width: 10px;
            height: 10px;
            margin: 0 5px;
            border-radius: 50%;
            background: linear-gradient(45deg, #007bff, #00c3ff);
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.5),
                        0 0 20px rgba(0, 195, 255, 0.3);
            animation: pulse 1.2s infinite;
            transition: transform 0.2s ease-in-out;
        }

        .circle:nth-child(2) {
            animation-delay: 0.4s;
        }

        .circle:nth-child(3) {
            animation-delay: 0.8s;
        }

        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
            }
            50% {
                transform: scale(1.5);
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="textElement" class="text">Zigbee Jamming</div>
        <div class="circle-container">
            <div class="circle"></div>
            <div class="circle"></div>
            <div class="circle"></div>
        </div>
    </div>

    <script>
        const textElement = document.getElementById('textElement');
        const originalText = textElement.textContent;
        const specialChars = '*&^%$#@!)(_+-';

        setInterval(() => {
            const words = originalText.split(' ');
            const randomWordIndex = Math.floor(Math.random() * words.length);
            const randomWord = words[randomWordIndex];

            if (randomWord) {
                const randomCharIndex = Math.floor(Math.random() * randomWord.length);

                const randomChar = specialChars[Math.floor(Math.random() * specialChars.length)];

                const modifiedWord = randomWord.split('');
                modifiedWord[randomCharIndex] = randomChar;
                words[randomWordIndex] = modifiedWord.join('');

                textElement.textContent = words.join(' ');

                setTimeout(() => {
                    modifiedWord[randomCharIndex] = randomWord[randomCharIndex];
                    words[randomWordIndex] = modifiedWord.join('');
                    textElement.textContent = words.join(' ');
                }, 999);
            }
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

const char* html_access_point = R"rawliteral(
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .dropdown-button:active {
            transform: translateY(1px); 
        }

        .dropdown-button:focus {
            outline: none;
        }
		.warning {
            margin-top: 15px;
            font-size: 14px;
            color: #ffcc00;
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
            <h1>Control Access Point:</h1>
        </div>

        <div class="buttons">
            <button class="button" onclick="location.href='/access_point_off'">Disable the access point</button>
        </div>
		<div class="warning">⚠️ After disconnecting the access point, the last item on the screen will be to enable the access point.</div>
    </div>
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            <h1>Choose jamming type:</h1>
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

const char* html_buttons_settings = R"rawliteral(
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            <h1>Choose control mode:</h1>
        </div>

        <div class="buttons">
            <button class="button" onclick="location.href='/button_method_0'">Single-button control (25 pin)</button>
            <button class="button" onclick="location.href='/button_method_1'">Two-button control (25, 26 pin)</button>
            <button class="button" onclick="location.href='/button_method_2'">Three-button control (25, 26, 27 pin)</button>
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            <h1>Choose jamming type:</h1>
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            <h1>Choose jamming type:</h1>
        </div>
        <div class="buttons">
            <button class="button" onclick="location.href='/misc_method_0'">Channel Switching (fast mode; used in Bluetooth Jam)</button>
            <button class="button" onclick="location.href='/misc_method_1'">Packet Sending (slow mode; used in WiFi Jam)</button>
        </div>
    </div>
</body>
</html>
)rawliteral";

const char* html_wifi_settings = R"rawliteral(
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
            opacity: 0;
            animation: fadeIn 1s forwards;
        }

        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            padding: 30px;
            border-radius: 10px;
            background: #1e1e1e;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.3);
            width: 90%;
            max-width: 350px;
            position: relative;
            transform: translateY(20px);
            animation: slideIn 0.5s forwards;
        }

        @keyframes slideIn {
            from {
                transform: translateY(20px);
                opacity: 0;
            }
            to {
                transform: translateY(0);
                opacity: 1;
            }
        }

        .input {
            background-color: #333333;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 16px;
            width: 100%;
            box-sizing: border-box;
            margin: 0 auto 15px;
            text-align: center;
            transition: background-color 0.3s, transform 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
        }

        label {
            display: block;
            margin-bottom: 8px;
            font-size: 14px;
            color: #aaaaaa;
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
            width: calc(100% - 20px);
            box-sizing: border-box;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
            margin: 5px auto;
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
        }

        .button:active {
            transform: translateY(1px);
        }

        .button.reset {
            background-color: #dc3545;
        }

        .button.reset:hover {
            background-color: #c82333;
        }

        .warning {
            margin-top: 15px;
            font-size: 14px;
            color: #ffcc00;
            text-align: center;
            padding: 10px;
            border-radius: 5px;
            background-color: rgba(255, 204, 0, 0.1);
        }

        .notification {
            position: fixed;
            top: -100px;
            left: 50%;
            transform: translateX(-50%);
            padding: 15px 25px;
            border-radius: 0 0 8px 8px;
            color: white;
            font-weight: bold;
            z-index: 1000;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);
            transition: top 0.5s ease-out;
            width: 80%;
            max-width: 400px;
            text-align: center;
            opacity: 0;
            animation: notificationFadeIn 0.3s forwards;
        }

        @keyframes notificationFadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }

        .success {
            background: linear-gradient(135deg, #28a745, #218838);
        }

        .error {
            background: linear-gradient(135deg, #dc3545, #c82333);
        }
    </style>
</head>
<body>
    <div id="notification" class="notification" style="display: none;"></div>

    <div class="container">
        <h1>WiFi Settings</h1>
        <form id="wifiForm">
            <input type="text" id="ssid" name="ssid" placeholder="Enter SSID" class="input">
            <input type="password" id="password" name="password" placeholder="Enter password" class="input">
            <button type="button" onclick="validateAndSave()" class="button">Save Settings</button>
            <button type="button" onclick="confirmReset()" class="button reset">Reset to Default</button>
        </form>
        <div class="warning">⚠️ Leave both fields empty to disable access point</div>
    </div>

    <script>
        function showNotification(message, isSuccess) {
            const notification = document.getElementById("notification");
            notification.textContent = message;
            notification.className = isSuccess ? "notification success" : "notification error";
            notification.style.display = "block";

            setTimeout(() => {
                notification.style.top = "20px";
            }, 10);

            setTimeout(() => {
                notification.style.top = "-100px";
                setTimeout(() => {
                    notification.style.display = "none";
                }, 500);
            }, 3000);
        }

        function validateAndSave() {
            const ssid = document.getElementById("ssid").value;
            const password = document.getElementById("password").value;
            
            if (ssid === "" && password === "") {
                if (confirm("Are you sure you want to disable the access point?")) {
                    showNotification("Disabling access point...", true);
                    setTimeout(() => {
                        location.href = "/access_point_off";
                    }, 1000);
                }
                return;
            }

            if (ssid.length > 0 && password.length === 0) {
                showNotification("Password must be at least 8 characters long", false);
                return;
            }

            if (password.length > 0 && password.length < 8) {
                showNotification("Password must be at least 8 characters long", false);
                return;
            }

            const formData = new FormData();
            formData.append("ssid", ssid);
            formData.append("password", password);

            showNotification("Saving settings...", true);

            fetch("/save_wifi_settings", {
                method: "POST",
                body: formData
            })
            .then(response => {
                if (response.ok) {
                    showNotification("Settings saved successfully! Rebooting...", true);
                    setTimeout(() => {
                        window.location.reload();
                    }, 2000);
                } else {
                    showNotification("Error saving settings", false);
                }
            })
            .catch(error => {
                console.error("Error:", error);
                showNotification("Connection error", false);
            });
        }

        function confirmReset() {
            if (confirm("Are you sure you want to reset to default settings?")) {
                showNotification("Resetting to default settings...", true);
                setTimeout(() => {
                    location.href = "/reset_wifi_settings";
                }, 1000);
            }
        }
        
        document.addEventListener('keypress', function(e) {
            if (e.key === 'Enter') {
                validateAndSave();
            }
        });
    </script>
</body>
</html>
)rawliteral";


const char* html_logo_setings = R"rawliteral(
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            <h1>Choose logo settings:</h1>
        </div>
        <div class="buttons">
            <button class="button" onclick="location.href='/logo_on'">Show logo on startup</button>
            <button class="button" onclick="location.href='/logo_off'">Hide logo on startup</button>
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            <h1>Choose jamming type:</h1>
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3; 
            transform: translateY(-4px); 
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .dropdown-button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            display: flex;
            flex-direction: column;
            align-items: center;
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
            transition: background-color 0.3s, transform 0.2s, box-shadow 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            display: flex;
            flex-direction: column;
            align-items: center;
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
            transition: background-color 0.3s, transform 0.2s;
        }

        .input:focus {
            outline: none;
            background-color: #444444;
            box-shadow: 0 0 8px rgba(0, 123, 255, 0.5);
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
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
        }

        .button:hover {
            background-color: #0056b3;
            transform: translateY(-4px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.4);
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
            if (stopValue < startValue) {
                alert('Stop value must not be less than Start value.');
                return;
            }
            location.href = `/misc_jam?start=${startValue}&stop=${stopValue}`;
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Set Start/Stop Values:</h1>
        <input id="startInput" class="input" type="number" placeholder="Start (0-125)" max="125" min="0" />
        <input id="stopInput" class="input" type="number" placeholder="Stop (0-125)" max="125" min="0" />
        <button class="button" onclick="validateAndRedirect()">Jam</button>
    </div>
</body>
</html>
)rawliteral";