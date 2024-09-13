#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GyverButton.h>
#include "RF24.h"
#include "bitmap.h"
#include <WiFi.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SPIClass *sp = nullptr;
SPIClass *hp = nullptr;
RF24 radio(16, 15, 16000000);
RF24 radio1(22, 21, 16000000);
int ble_channels[] = {2, 26, 80};
int menu_number = 0;
const char jam_text[] = "xxxxxxxxxxxxxxxx";
GButton butt1(25);
const char *ssid = "jammer";
const char *password = "W0rthlesS0ul";
WebServer server(80);

void handleRoot() {
  String html = "";
  html += "<!DOCTYPE html>\n";
  html += "<html lang=\"ru\">\n";
  html += "<head>\n";
  html += "    <meta charset=\"UTF-8\">\n";
  html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  html += "    <style>\n";
  html += "        body {\n";
  html += "            font-family: Arial, sans-serif;\n";
  html += "            display: flex;\n";
  html += "            flex-direction: column;\n";
  html += "            align-items: center;\n";
  html += "        }\n";
  html += "        .container {\n";
  html += "            display: flex;\n";
  html += "            flex-direction: column;\n";
  html += "            align-items: center;\n";
  html += "        }\n";
  html += "        .header {\n";
  html += "            display: flex;\n";
  html += "            align-items: center;\n";
  html += "            justify-content: center;\n";
  html += "            margin-bottom: 20px;\n";
  html += "        }\n";
  html += "        .header img {\n";
  html += "            width: 100px;\n";
  html += "            height: auto;\n";
  html += "            margin-bottom: 10px;\n";
  html += "        }\n";
  html += "        .text {\n";
  html += "            display: flex;\n";
  html += "            flex-direction: column;\n";
  html += "        }\n";
  html += "        .buttons {\n";
  html += "            display: flex;\n";
  html += "            flex-direction: column;\n";
  html += "            align-items: center;\n";
  html += "        }\n";
  html += "        .button {\n";
  html += "            width: 128px;\n";
  html += "            height: 32px;\n";
  html += "            margin: 5px;\n";
  html += "            border: none;\n";
  html += "            cursor: pointer;\n";
  html += "            overflow: hidden;\n";
  html += "        }\n";
  html += "    </style>\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "    <div class=\"container\">\n";
  html += "        <div class=\"header\">\n";
  html += "            <img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAADICAYAAACtWK6eAAAACXBIWXMAABJ0AAASdAHeZh94AAAGnGlUWHRYTUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4bXB0az0iQWRvYmUgWE1QIENvcmUgOS4wLWMwMDEgNzkuYzAyMDRiMiwgMjAyMy8wMi8wOS0wNjoyNjoxNCAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIiB4bWxuczpkYz0iaHR0cDovL3B1cmwub3JnL2RjL2VsZW1lbnRzLzEuMS8iIHhtbG5zOnBob3Rvc2hvcD0iaHR0cDovL25zLmFkb2JlLmNvbS9waG90b3Nob3AvMS4wLyIgeG1sbnM6eG1wTU09Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9tbS8iIHhtbG5zOnN0RXZ0PSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvc1R5cGUvUmVzb3VyY2VFdmVudCMiIHhtcDpDcmVhdG9yVG9vbD0iQWRvYmUgUGhvdG9zaG9wIDI0LjYgKDIwMjMwNTI0Lm0uMjE4NSAwZTcxMzlkKSAgKFdpbmRvd3MpIiB4bXA6Q3JlYXRlRGF0ZT0iMjAyNC0wOS0xM1QyMDoxNjo0OCswMzowMCIgeG1wOk1vZGlmeURhdGU9IjIwMjQtMDktMTNUMjA6MjA6MTUrMDM6MDAiIHhtcDpNZXRhZGF0YURhdGU9IjIwMjQtMDktMTNUMjA6MjA6MTUrMDM6MDAiIGRjOmZvcm1hdD0iaW1hZ2UvcG5nIiBwaG90b3Nob3A6Q29sb3JNb2RlPSIzIiB4bXBNTTpJbnN0YW5jZUlEPSJ4bXAuaWlkOjU5YzJlOWIwLTQxM2EtYzM0NS1hOGIyLTBlZTIwZWI5ZmNlYiIgeG1wTU06RG9jdW1lbnRJRD0ieG1wLmRpZDo3MDVlZTJjZS03ZjIwLWE5NDMtYWZlNC0wOTI0YzJhMmQwMDEiIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDo3MDVlZTJjZS03ZjIwLWE5NDMtYWZlNC0wOTI0YzJhMmQwMDEiPiA8cGhvdG9zaG9wOkRvY3VtZW50QW5jZXN0b3JzPiA8cmRmOkJhZz4gPHJkZjpsaT54bXAuZGlkOjcwNWVlMmNlLTdmMjAtYTk0My1hZmU0LTA5MjRjMmEyZDAwMTwvcmRmOmxpPiA8L3JkZjpCYWc+IDwvcGhvdG9zaG9wOkRvY3VtZW50QW5jZXN0b3JzPiA8eG1wTU06SGlzdG9yeT4gPHJkZjpTZXE+IDxyZGY6bGkgc3RFdnQ6YWN0aW9uPSJjcmVhdGVkIiBzdEV2dDppbnN0YW5jZUlEPSJ4bXAuaWlkOjcwNWVlMmNlLTdmMjAtYTk0My1hZmU0LTA5MjRjMmEyZDAwMSIgc3RFdnQ6d2hlbj0iMjAyNC0wOS0xM1QyMDoxNjo0OCswMzowMCIgc3RFdnQ6c29mdHdhcmVBZ2VudD0iQWRvYmUgUGhvdG9zaG9wIDI0LjYgKDIwMjMwNTI0Lm0uMjE4NSAwZTcxMzlkKSAgKFdpbmRvd3MpIi8+IDxyZGY6bGkgc3RFdnQ6YWN0aW9uPSJzYXZlZCIgc3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlpZDo1OWMyZTliMC00MTNhLWMzNDUtYThiMi0wZWUyMGViOWZjZWIiIHN0RXZ0OndoZW49IjIwMjQtMDktMTNUMjA6MjA6MTUrMDM6MDAiIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9IkFkb2JlIFBob3Rvc2hvcCAyNC42ICgyMDIzMDUyNC5tLjIxODUgMGU3MTM5ZCkgIChXaW5kb3dzKSIgc3RFdnQ6Y2hhbmdlZD0iLyIvPiA8L3JkZjpTZXE+IDwveG1wTU06SGlzdG9yeT4gPC9yZGY6RGVzY3JpcHRpb24+IDwvcmRmOlJERj4gPC94OnhtcG1ldGE+IDw/eHBhY2tldCBlbmQ9InIiPz4cqzpoAAAh6klEQVR4nO2deXRU5f3wv89d597ZJ7sBwhrZBBJB+IlCKbsKFqtF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\" alt=\"Image\">\n";
  html += "        </div>\n";
  html += "        <div class=\"buttons\">\n";
  html += "            <button class=\"button\" onclick=\"location.href='/bluetooth_jam'\">\n";
  html += "                <img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAIAAAAAgCAIAAABVQOdyAAAACXBIWXMAAC4jAAAuIwF4pT92AAAFY0lEQVR4nOxazUobURidn8xfEpvSLITixrWv4UP4Dr6Cb+AbBLrQQlG6qFCwq7qxG7MKaS1YbauLtgakoLgwik5P5zgf1zvpdCbJJGByELlz/+85937f/Ylh5IFtWWEY5ioyxTBRr9UgwNvNTd91x92XiYRlmmEEaGBZ1ri7M3mwIhMkGpjj7s/jhG2amOmuZVVLJT1JEYAaBHlsEYqsrKxkiSwU6S0uLy+fnp4iz8bGhpaEUpeXlxx7q9VaXFxkpOYXGSNoNBr5+lculVjSsSxtjjtxkqpBNQgy1oz8q6urWSJTsLS0tL29nT0/ATbBbJYWwT5SQS6yoS01CfGIRBLiIcDR0ZFE9sxGLCws5OioGVt5AgL4ti2p1sNU0eDpzEyWyociQHLAeZtObxGpkIoCcI73bFrC/xIgbw/vUYop5jLsdrvPPK8U+1uR5/z8fG5uDqmiQZZ9EXLu7u6yZgQ4NYQO1TJIGBRgosmSBzUiPNcBpvZlBHyyQmRDZmRAJM0IWyTUbiBD0j6gHrTYM0mYRUNoApX0pHsgAWD33djQS78D2/YiDUq2zZizs7N6ve77vgzsw87Ol4ODdLeMbBjbUgRUzgGoAiQnKQ0CecGYEQAvnJtgAfwig1RISRigoWAYOWmLOaNFTjKl2hlEUhj2jQVlHTC/gAX/JYBATF8OYKbf3NyoGlAAOxbg7u4OeWrRsYDonJzgPxZQugBCMQIYZ7oAGGH4ENqAUYNWIR2gmF2QrrWSEgYwP6gTa6bX1ZiltFx5aOi/PiAv+ffQbBHJVT1EtVqVMEwQVsCs75sFCKA5sUIFkAmL/yiIsat7IY3rMLKTQzZBKlSXiwn+8sULtqoB7OMvS4XhQxNEiyEUIAkLH1OGRkZMkNgTLnlSjDBYRpJqgkAWIqUIN5RkkP5A8zpGQoBWBNnnaAZEndqojeHknmegXZAGrgOal57Izj5Hy5UbRi5U6KC/lQ04PaQ4YfGowhRjoBM1oBOWqZp0wsIdN45hL29P0LtKWS4gMSOywUckslEbbdeP/iRj+hfAiM5l7WZz9927AdmfYiC839rSNJiyP2rMKF4XeLO+Pu4eTRjKQTAyAWCsueuf4h4jNkFwwnSYkw5z6oTHCxwFcOgd1jZ0inzA9L+6uiLRWAG1J08C1x2NBmF0XWNEG3McC3h0kHshIz4x8fQbxld7+ORJ4r85ucFPbtLl/IUa5DJOrSH35X5/sCwLluf48PDnt29k+eLiwlDeAzqdTq1Wa7fbosGHnZ0SiqVeRWSHCIBDEChbjaAeTYUmxINEhsmayJCSEw4Gn6g5VB5ewD5vVVFQPY2jBhGvn5u1PgAW9/f3T4+P0eqv799B96zvI96NBdjb2zNN0/M8WQc0U8PqgAggkLtozlnSKmdUMi6fvETijWZ6TsrGsLyxyCeTWMOInu3+Pkba9sHHjz+/fiX7n9ttz7Z5zSmXcc1mE+H5+XkRgNfRQ18BRnzjLw2pAkh+7VOtIT2n+qm2IuhZeYGAGel2uz8i9vEffQrUF7H4qQAzpVKpSC9fNhqVaIkMC0IfjQZMgVwcFSoAb3hU9Ky8QEAAmPvfnQ6s/6dWq+Y4rvLzEzfxJkwH4Cae7weE0KcuBT6PFCcAbX3y8nKkAgAVx7m9vb2+vk6+rgSK0Se2Xr1KfwPoD0If3yPpWhkuTgB6DvpnQN7xRy0ALH7d9596npv46VX5oQCY+8/r9SL6ECq7IHUbwzcDoxgBjOhKnDFUYjwCpGCmXBb21xsN6FFEK5yJg16jP0rIo/zrtTXXcYpoAtaGO0XtNzlT/AWdMCyPM2yvK1Afm6bQQSdchNedWPwBAAD//6v3aTcAAAAGSURBVAMAkd3n1KZOOCoAAAAASUVORK5CYII=\">\n";
  html += "            </button>\n";
  html += "            <button class=\"button\" onclick=\"location.href='/drone_jam'\">\n";
  html += "                <img src=\"data:image/png;base64,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\">\n";
  html += "            </button>\n";
  html += "            <button class=\"button\" onclick=\"location.href='/wifi_jam'\">\n";
  html += "                <img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAIAAAAAgCAYAAADaInAlAAAACXBIWXMAAC4jAAAuIwF4pT92AAAEvklEQVR4nOya200zMRCFE0Ju5A4IAuKm/5U2KIIeaIEO6IAKqIAGaIACKIAnnhAIAuzvz+REjnECJCTewI40Yu3YZnfPGc9lncvlckmmf1qj30CmGQEWR29ubpLr6+uhvtPTU9uP3N/ff/g95Tp+QLFYTJaWlob68vn8oB+17eXl97a5pq1xKXjAH9Xb21sLttpnZ2cWeEDn+vz8PLm8vIx+n1MToFQqJYVCwYI4qUCKFDzgTBUyYPWx7+PHCIAVA3y1XJ4YeFeenp6SZbNeCh50Jirrj30fUxOgXq8n6+vrScUA3+v1xoIKScpmHMDmzdyCaTdqtaT38DB2Hmszz3cpsZTtHP2sDytXnwA/Ojoa+6wLRIp3qz84OEju7u6Sx8fHDw9T6vv7arVqiTLKt9PfbreT1U4n2e12R74cxkGE2A9/cXFh7wcwaR8fHw/ukWv6BDRjXQJwLf8PObiWLhQBAKJmrDckWDWDCPi+uzAxBGv/29uza728vAzWtW7BxAeFyK7h5OTE3g+giRCAiU8X4AJUhPDBDYG9UARoGIt+fn4egPP6+pq0mk1r8S7wivwH1wDYjxdwA7WVFXsN8O4/UEzBX9aW8D/T4AoA++rqyl4DPsDT1pYPkK5L+HUEcC3+wfjwaqUyZJmAjrV2Go2RW7ovZTMn5CZYC+t3JfYLIGWDBNr+2RXI62X1iHaDX0mANyfgc3N+AOR6e2Pjy8D7sru1NfTPWBMypQV8VG7At3QVdESEX0sAtmxA8X1yu9UaCey2AVZk6eAu+sWfUeK6Ev2fUqDAFEsBG3EtHTeA+BnBryOA3wGQqwbUkNRNUKjqnz+PPsDd3twMzi0boqW1MiiwlQ2gcgNU9v4UARommPMF4BTIfWVRxjW9mIHagsrGKXjoTEMEwB18sFyTyrm1fcUGFHXIArDsYt+FNAzoLsDUDUJkSsFDZxoiAL7aFRdQWa+byoXEt3IKQ/7vs3oYP5DL9JsEQK2FG22ZIJB2t9u1VcI3p1bwmbBrdDoduwOwqzRNTKGdY5YPo5Lt4eFh7Je6SDr6R5WIQ7K5tmbzfb4HrHtWLtnf34/9cJlOQ4BQardlgIcY2iHQnZ0d+42gVqkMDzbuIi2pXqYTEKDppYNs6Z8t2PS+K8yTACrcqI0rIMVTns+16x4Yr4Mc/hjSP/X5BzwmnYdSVNJ94q7cNNNf109B506ACmVhAyBS8oo5ygjqhhQEeozlN/ooJ1PybRsyzDPq9wlAXg8wOqkjoPzx+poHYGoTT9CnGoFbJJp0HuADLOOpQOqe9DGKdV1CMSYqAVC/8AOgtUB6V+yTQsSJkfP7BHBVluee3tF4twAEiKE+N7uYZp57nEx97ocnhCLUHN/b9yaMK/nG9vchAsg6XRk3/it9k86TdYfkMwKnhgByCSGJXeTxX6DAx6Lwz2rHJAB9bO2+ZgSYAQEQtyb/U0BOMw8SjKpTLAQB2Obd0z2uLEc+Bey/QJ3VJ9jS8a+YBND5AgWKKPe1UDsAVt70PhjxoYdTQWQDaSIAAZlIwG8igSxw3gRAAVv9IsNCEQClzKu0UEfHYm//KNurH2VnOgMCpE0J8mQ9yqkz/UMEUH6dgf9HCZDp5PofAAD//6xSXnEAAAAGSURBVAMAgZeRcCOE3iIAAAAASUVORK5CYII=\">\n";
  html += "            </button>";
  html += "        </div>\n";
  html += "    </div>\n";
  html += "</body>\n";
  html += "</html>";
  server.send(200, "text/html", html);

}
void bluetooth_jammer() {
  server.send(200, "text/plain", "bluetooth & ble jam");
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_bluetooth_jam, 128, 32, WHITE);
  display.display();
  HSPI_init();
  VSPI_init();
  while (true){
    radio.setChannel(random(40));
    radio1.setChannel(random(40, 80));
  }
}
void drone_jammer() {
  server.send(200, "text/plain", "drone jam");
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_drone_jam, 128, 32, WHITE);
  display.display();
  HSPI_init();
  VSPI_init();
  while (true){
    radio.setChannel(random(62));
    radio1.setChannel(random(62, 125));
  }
}
void wifi_jammer() {
  server.send(200, "text/plain", "wifi jam");
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_wifi_jam, 128, 32, WHITE);
  display.display();
  HSPI_init();
  VSPI_init();
  while (true){
    for (int channel = 0; channel < 13; channel++) {
      for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
        radio.setChannel(i);
        radio1.setChannel(i);
        radio.writeFast(&jam_text, sizeof(jam_text));
        radio1.writeFast(&jam_text, sizeof(jam_text));
      }
    }
  }
}
void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/bluetooth_jam", bluetooth_jammer);
  server.on("/drone_jam", drone_jammer);
  server.on("/wifi_jam", wifi_jammer);
  server.begin();
  butt1.setTimeout(200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_logo, 128, 32, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap_bluetooth_jammer, 128, 32, WHITE);
  display.display();
}

void VSPI_init() {
  sp = new SPIClass(VSPI);
  sp->begin();
  radio1.begin(sp);
  radio1.setAutoAck(false);
  radio1.stopListening();
  radio1.setRetries(0, 0);
  radio1.setPALevel(RF24_PA_MAX, true);
  radio1.setDataRate(RF24_2MBPS);
  radio1.setCRCLength(RF24_CRC_DISABLED);
  radio1.startConstCarrier(RF24_PA_MAX, 80);
}

void HSPI_init() {
  hp = new SPIClass(HSPI);
  hp->begin();
  radio.begin(hp);
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.startConstCarrier(RF24_PA_MAX, 40); 
}

void loop() {
  butt1.tick();
  server.handleClient();  
  if (butt1.isSingle()) {
    menu_number = (menu_number + 1) % 3;
    display.clearDisplay();
    const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jammer :
                            (menu_number == 1) ? bitmap_drone_jammer : bitmap_wifi_jammer;
    display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
    display.display();
  }

  if (butt1.isHolded()) {
    display.clearDisplay();
    const uint8_t* bitmap = (menu_number == 0) ? bitmap_bluetooth_jam :
                            (menu_number == 1) ? bitmap_drone_jam : bitmap_wifi_jam;
    display.drawBitmap(0, 0, bitmap, 128, 32, WHITE);
    display.display();
    HSPI_init();
    VSPI_init();
    
    while (true) {
      if (menu_number == 0) {
        radio.setChannel(random(40));
        radio1.setChannel(random(40, 80));
      } else if (menu_number == 1) {
        radio.setChannel(random(62));
        radio1.setChannel(random(62, 125));
      } else if (menu_number == 2) {
        for (int channel = 0; channel < 13; channel++) {
          for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
            radio.setChannel(i);
            radio1.setChannel(i);
            radio.writeFast(&jam_text, sizeof(jam_text));
            radio1.writeFast(&jam_text, sizeof(jam_text));
          }
        }
      }
    }
  }
}