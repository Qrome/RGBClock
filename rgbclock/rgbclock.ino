/** The MIT License (MIT)

Copyright (c) 2022 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**********************************************
* Edit Settings.h for personalization
***********************************************/

#include "Settings.h"

#define VERSION "1.0"

#define HOSTNAME "RGBClock-" 
#define CONFIG "/conf.txt"

/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (_time_ / SECS_PER_HOUR)

boolean isClockOn = false;

// Time 
TimeClient timeClient(UtcOffset);
long lastEpoch = 0;
long firstEpoch = 0;
long displayOffEpoch = 0;
String lastMinute = "xx";
String lastSecond = "xx";
String lastReportStatus = "";

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second

uint32_t HOUR_COLOR   = pixels.Color(255, 0, 0);
uint32_t MINUTE_COLOR = pixels.Color(0, 255, 0);
uint32_t SECOND_COLOR = pixels.Color(0, 0, 255);
uint32_t fire_color   = pixels.Color(80,35, 0);
uint32_t off_color    = pixels.Color(0, 0, 0);

//declairing prototypes
void configModeCallback (WiFiManager *myWiFiManager);
int8_t getWifiQuality();

ESP8266WebServer server(WEBSERVER_PORT);
ESP8266HTTPUpdateServer serverUpdater;

static const char WEB_ACTIONS[] PROGMEM =  "<a class='w3-bar-item w3-button' href='/'><i class='fa fa-home'></i> Home</a>"
                      "<a class='w3-bar-item w3-button' href='/configure'><i class='fa fa-cog'></i> Configure</a>"
                      "<a class='w3-bar-item w3-button' href='/systemreset' onclick='return confirm(\"Do you want to reset to default settings?\")'><i class='fa fa-undo'></i> Reset Settings</a>"
                      "<a class='w3-bar-item w3-button' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fa fa-wifi'></i> Forget WiFi</a>"
                      "<a class='w3-bar-item w3-button' href='/update'><i class='fa fa-wrench'></i> Firmware Update</a>"
                      "<a class='w3-bar-item w3-button' href='https://github.com/Qrome' target='_blank'><i class='fa fa-question-circle'></i> About</a>";

static const char CLOCK_FORM[] PROGMEM = "<p>UTC Time Offset <select <select class='w3-option w3-padding' name='utcoffset'>%UTCOFFSET%</select>"
                      "<a class='w3-bar-item w3-button' alt='About UTC Offset' href='https://en.wikipedia.org/wiki/UTC_offset' target='_blank'><i class='fa fa-question-circle'></i></a></p>"
                      "<p>Clock Sync Refresh (minutes) <select class='w3-option w3-padding' name='refresh'>%OPTIONS%</select></p>"
                      "<p><label>Brightness %</label>"
                      "<input type='range' value='%BRIGHT%' min='1' max='100' oninput='this.nextElementSibling.value = this.value' name='bright'><output>%BRIGHT%</output></p>";
                            
static const char THEME_FORM[] PROGMEM =   "<p>Theme Color <select class='w3-option w3-padding' name='theme'>%THEME_OPTIONS%</select></p>"
                      "<button class='w3-button w3-block w3-grey w3-section w3-padding' type='submit'>Save</button></form>";

static const char UTC_OFFSETS[] PROGMEM = "<option>-12.00</option>"
                      "<option>-11.00</option>"
                      "<option>-10.00</option>"
                      "<option>-9.00</option>"
                      "<option>-8.00</option>"
                      "<option>-7.00</option>"
                      "<option>-6.00</option>"
                      "<option>-5.00</option>"
                      "<option>-4.00</option>"
                      "<option>-3.00</option>"
                      "<option>-2.00</option>"
                      "<option>-1.00</option>"
                      "<option>0.00</option>"
                      "<option>1.00</option>"
                      "<option>2.00</option>"
                      "<option>3.00</option>"
                      "<option>3.50</option>"
                      "<option>4.00</option>"
                      "<option>4.50</option>"
                      "<option>5.00</option>"
                      "<option>5.50</option>"
                      "<option>5.75</option>"
                      "<option>6.00</option>"
                      "<option>6.50</option>"
                      "<option>7.00</option>"
                      "<option>8.00</option>"
                      "<option>8.75</option>"
                      "<option>9.00</option>"
                      "<option>9.50</option>"
                      "<option>10.00</option>"
                      "<option>10.50</option>"
                      "<option>11.00</option>"
                      "<option>11.50</option>"
                      "<option>12.00</option>"
                      "<option>12.75</option>"
                      "<option>13.00</option>"
                      "<option>14.00</option>";

static const char COLOR_THEMES[] PROGMEM = "<option>red</option>"
                      "<option>pink</option>"
                      "<option>purple</option>"
                      "<option>deep-purple</option>"
                      "<option>indigo</option>"
                      "<option>blue</option>"
                      "<option>light-blue</option>"
                      "<option>cyan</option>"
                      "<option>teal</option>"
                      "<option>green</option>"
                      "<option>light-green</option>"
                      "<option>lime</option>"
                      "<option>khaki</option>"
                      "<option>yellow</option>"
                      "<option>amber</option>"
                      "<option>orange</option>"
                      "<option>deep-orange</option>"
                      "<option>blue-grey</option>"
                      "<option>brown</option>"
                      "<option>grey</option>"
                      "<option>dark-grey</option>"
                      "<option>black</option>"
                      "<option>w3schools</option>";
                            

void setup() {  
  Serial.begin(115200);
  SPIFFS.begin();
  delay(10);
  
  //New Line to clear from start garbage
  Serial.println();
  
  // Initialize digital pin for LED (little blue light on the Wemos D1 Mini)
  pinMode(externalLight, OUTPUT);

  readSettings();
 
  pixels.begin();
  pixels.fill(pixels.Color(0, 0, 0), 0, NUMPIXELS);
  pixels.show();

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);
  
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  if (!wifiManager.autoConnect((const char *)hostname.c_str())) {// new addition
    delay(3000);
    WiFi.disconnect(true);
    ESP.reset();
    delay(5000);
  }
  
  // print the received signal strength:
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(getWifiQuality());
  Serial.println("%");

  if (ENABLE_OTA) {
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.setHostname((const char *)hostname.c_str()); 
    if (OTA_Password != "") {
      ArduinoOTA.setPassword(((const char *)OTA_Password.c_str()));
    }
    ArduinoOTA.begin();
  }

  server.on("/", displayStatus);
  server.on("/systemreset", handleSystemReset);
  server.on("/forgetwifi", handleWifiReset);
  server.on("/updateconfig", handleUpdateConfig);
  server.on("/configure", handleConfigure);
  server.on("/resetadmin", handleResetAdmin);
  server.onNotFound(redirectHome);
  serverUpdater.setup(&server, "/update", www_username, www_password);
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  String webAddress = "http://" + WiFi.localIP().toString() + ":" + String(WEBSERVER_PORT) + "/";
  Serial.println("Use this URL : " + webAddress);

  IPAddress ip = WiFi.localIP(); //IPAddress(192, 168, 0, 9);
  displayIP(ip[3]);

  applyBrightness();
  
  flashLED(1, 100);
  Serial.println("*** Leaving setup()");
}

//************************************************************
// Main Loop
//************************************************************
void loop() {
  
   //Get Time Update
  if((getMinutesFromLastRefresh() >= minutesBetweenDataRefresh) || lastEpoch == 0) {
    getUpdateTime();
  }

  updateLED();

  server.handleClient();
  
  if (ENABLE_OTA) {
    ArduinoOTA.handle();
  }
}

void updateLED() {
  int hotSecondsLED = timeClient.getSecondsInt()%12;
  int secondsLED = timeClient.getSecondsInt()/5;
  int minutesLED = timeClient.getMinutesInt()/5;
  int hoursLED = timeClient.get12HoursInt();

  if (timeClient.getMinutesInt()%5 >= 3) {
    minutesLED += 1;
    if (minutesLED == 12) {
      minutesLED = 0;
    }
  }

  pixels.fill(pixels.Color(0, 0, 0), 0, NUMPIXELS); // set all off

  pixels.setPixelColor(hotSecondsLED, pixels.Color(5, 5, 5));
  
  if (secondsLED == minutesLED && minutesLED == hoursLED) {
    pixels.setPixelColor(secondsLED, pixels.Color(255, 255, 255));
  } else if (secondsLED == minutesLED) {
    pixels.setPixelColor(secondsLED, pixels.Color(0, 255, 255));
    pixels.setPixelColor(hoursLED, HOUR_COLOR);
  } else if (secondsLED == hoursLED) {
    pixels.setPixelColor(secondsLED, pixels.Color(255, 0, 255));
    pixels.setPixelColor(minutesLED, MINUTE_COLOR);
  } else if (minutesLED == hoursLED) {
    pixels.setPixelColor(minutesLED, pixels.Color(255, 255, 0));
    pixels.setPixelColor(secondsLED, SECOND_COLOR);
  } else {
    pixels.setPixelColor(secondsLED, SECOND_COLOR);
    pixels.setPixelColor(minutesLED, MINUTE_COLOR);
    pixels.setPixelColor(hoursLED, HOUR_COLOR);
  }
  pixels.show();
}

void displayIP(byte octet) {
  Serial.println("Last octet: " + String(octet));
  String ip = String(octet);
  char digits[ip.length()];
  ip.toCharArray(digits, ip.length()+1);

  for (int inx = 0; inx < 50; inx++) {
    if (ip.length() == 3) {
      if (digits[0] == digits[1] && digits[1] == digits[2]) {
        pixels.setPixelColor(String(digits[0]).toInt(), pixels.Color(255, 255, 255));
      } else if (digits[0] == digits[1]) {
        pixels.setPixelColor(String(digits[0]).toInt(), pixels.Color(255, 255, 0));
        pixels.setPixelColor(String(digits[2]).toInt(), SECOND_COLOR);
      } else if (digits[1] == digits[2]) {
        pixels.setPixelColor(String(digits[1]).toInt(), pixels.Color(0, 255, 255));
        pixels.setPixelColor(String(digits[0]).toInt(), HOUR_COLOR);
      } else if (digits[0] == digits[2]) {
        pixels.setPixelColor(String(digits[0]).toInt(), pixels.Color(255, 0, 255));
        pixels.setPixelColor(String(digits[1]).toInt(), HOUR_COLOR);
      } else {
        pixels.setPixelColor(String(digits[2]).toInt(), SECOND_COLOR);
        pixels.setPixelColor(String(digits[1]).toInt(), MINUTE_COLOR);
        pixels.setPixelColor(String(digits[0]).toInt(), HOUR_COLOR);
      }
    } else if (ip.length() == 2) {
      if (digits[0] == digits[1]) {
        pixels.setPixelColor(String(digits[0]).toInt(), pixels.Color(255, 255, 0));
      } else {
        pixels.setPixelColor(String(digits[1]).toInt(), MINUTE_COLOR);
        pixels.setPixelColor(String(digits[0]).toInt(), HOUR_COLOR);
      }
    } else if (ip.length() == 1) {
      pixels.setPixelColor(String(digits[0]).toInt(), HOUR_COLOR);
    }
    pixels.show();
    delay(100);
    pixels.fill(pixels.Color(0, 0, 0), 0, NUMPIXELS); // set all off
    pixels.show();
    delay(50);
  }

  for (int inx = 0; inx <= 20; inx++) {
    RollingFire(85);
    delay(random(40,200));
  }
  pixels.fill(pixels.Color(0, 40, 0), 0, NUMPIXELS); // Green For Loading
  pixels.show();
}

void applyBrightness() {
  uint8_t adjusted = uint8_t(round(255.0 * (brightness/100.0)));
  Serial.println("Adjusted Brightness: " + String(adjusted));
  HOUR_COLOR   = pixels.Color(adjusted, 0, 0);
  MINUTE_COLOR = pixels.Color(0, adjusted, 0);
  SECOND_COLOR = pixels.Color(0, 0, adjusted);
}

void getUpdateTime() {
  ledOnOff(true); // turn on the LED
  Serial.println("Updating Time...");
  //Update the Time
  timeClient.updateTime();
  lastEpoch = timeClient.getCurrentEpoch();
  Serial.println("Local time: " + timeClient.getAmPmFormattedTime());

  ledOnOff(false);  // turn off the LED
}

//ONLY USED in Special Build
void handleResetAdmin() {
  Serial.println("Reset System Configuration");
  if (SPIFFS.remove(CONFIG)) {
    Serial.println("Config settings removed...");
  }
  redirectHome();
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.restart();
}

void handleSystemReset() {
  Serial.println("Reset System Configuration");
  if (SPIFFS.remove(CONFIG)) {
    redirectHome();
    ESP.restart();
  }
}

void handleUpdateConfig() {
  USE_FLASH = server.hasArg("useFlash");
  minutesBetweenDataRefresh = server.arg("refresh").toInt();
  themeColor = server.arg("theme");
  UtcOffset = server.arg("utcoffset").toFloat();
  brightness = server.arg("bright").toInt();
  writeSettings();
  lastEpoch = 0;
  redirectHome();
}

void handleWifiReset() {
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  redirectHome();
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.eraseConfig();
  Clear(); 
  delay(2000);
  ESP.reset(); 
}

void handleConfigure() {
  ledOnOff(true);
  String html = "";

  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");

  html = getHeader();
  server.sendContent(html);
  
  html = "<form class='w3-container' action='/updateconfig' method='get'><h2>Configuration:</h2>";
  server.sendContent(html);

  String form = FPSTR(CLOCK_FORM);

  form.replace("%BRIGHT%", String(brightness));
  
  String utcOffsets = FPSTR(UTC_OFFSETS);
  utcOffsets.replace(">" + String(UtcOffset, 2) + "<", " selected>" + String(UtcOffset, 2) + "<");
  form.replace("%UTCOFFSET%", utcOffsets);

  String options = "<option>10</option><option>15</option><option>20</option><option>30</option><option>60</option>";
  options.replace(">"+String(minutesBetweenDataRefresh)+"<", " selected>"+String(minutesBetweenDataRefresh)+"<");
  form.replace("%OPTIONS%", options);

  server.sendContent(form);

  form = FPSTR(THEME_FORM);
  
  String themeOptions = FPSTR(COLOR_THEMES);
  themeOptions.replace(">"+String(themeColor)+"<", " selected>"+String(themeColor)+"<");
  form.replace("%THEME_OPTIONS%", themeOptions);

  server.sendContent(form);
  
  html = getFooter();
  server.sendContent(html);
  server.sendContent("");
  server.client().stop();
  ledOnOff(false);
}

void redirectHome() {
  // Send them back to the Root Directory
  server.sendHeader("Location", String("/"), true);
  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");
  server.client().stop();
}

String getHeader() {
  return getHeader(false);
}

String getHeader(boolean refresh) {
  String menu = FPSTR(WEB_ACTIONS);

  String html = "<!DOCTYPE HTML>";
  html += "<html><head><title>RGB Clock</title><link rel='icon' href='data:;base64,='>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  if (refresh) {
    html += "<meta http-equiv=\"refresh\" content=\"30\">";
  }
  html += "<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>";
  html += "<link rel='stylesheet' href='https://www.w3schools.com/lib/w3-theme-" + themeColor + ".css'>";
  html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>";
  html += "</head><body>";
  html += "<nav class='w3-sidebar w3-bar-block w3-card' style='margin-top:88px' id='mySidebar'>";
  html += "<div class='w3-container w3-theme-d2'>";
  html += "<span onclick='closeSidebar()' class='w3-button w3-display-topright w3-large'><i class='fa fa-times'></i></span>";
  html += "<div class='w3-cell w3-left w3-xxxlarge' style='width:60px'><i class='fa fa-cube'></i></div>";
  html += "<div class='w3-padding'>Menu</div></div>";
  html += menu;
  html += "</nav>";
  html += "<header class='w3-top w3-bar w3-theme'><button class='w3-bar-item w3-button w3-xxxlarge w3-hover-theme' onclick='openSidebar()'><i class='fa fa-bars'></i></button><h2 class='w3-bar-item'>RGB Clock</h2></header>";
  html += "<script>";
  html += "function openSidebar(){document.getElementById('mySidebar').style.display='block'}function closeSidebar(){document.getElementById('mySidebar').style.display='none'}closeSidebar();";
  html += "</script>";
  html += "<br><div class='w3-container w3-large' style='margin-top:88px'>";
  return html;
}

String getFooter() {
  int8_t rssi = getWifiQuality();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  Serial.println("%");
  String html = "<br><br><br>";
  html += "</div>";
  html += "<footer class='w3-container w3-bottom w3-theme w3-margin-top'>";
  if (lastReportStatus != "") {
    html += "<i class='fa fa-external-link'></i> Report Status: " + lastReportStatus + "<br>";
  }
  html += "<i class='fa fa-paper-plane-o'></i> Version: " + String(VERSION) + "<br>";
  html += "<i class='fa fa-rss'></i> Signal Strength: ";
  html += String(rssi) + "%";
  html += "</footer>";
  html += "</body></html>";
  return html;
}

void displayStatus() {
  ledOnOff(true);
  String html = "";

  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(String(getHeader(true)));

  String displayTime = timeClient.getAmPmHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds() + " " + timeClient.getAmPm();
  
  html += "<div class='w3-cell w3-container' style='width:100%'><p>";
 
  html += "</p></div></div>";

  html += "<div class='w3-cell-row' style='width:100%'><h2>Time: " + displayTime + " <a class='w3-bar-item w3-button' href='/configure'><i class='fa fa-cog'></i> Configure</a></h2></div>";

  server.sendContent(html); // spit out what we got
  html = "";

  server.sendContent(String(getFooter()));
  server.sendContent("");
  server.client().stop();
  ledOnOff(false);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println("Wifi Manager");
  Serial.println("Please connect to AP");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("To setup Wifi Configuration");
  pixels.fill(pixels.Color(0, 0, 100), 0, NUMPIXELS);
  pixels.show();
  flashLED(20, 50);
}

void ledOnOff(boolean value) {
  if (USE_FLASH) {
    if (value) {
      digitalWrite(externalLight, LOW); // LED ON
    } else {
      digitalWrite(externalLight, HIGH);  // LED OFF
    }
  }
}

void flashLED(int number, int delayTime) {
  for (int inx = 0; inx <= number; inx++) {
      delay(delayTime);
      digitalWrite(externalLight, LOW); // ON
      delay(delayTime);
      digitalWrite(externalLight, HIGH); // OFF
      delay(delayTime);
  }
}

String zeroPad(int value) {
  String rtnValue = String(value);
  if (value < 10) {
    rtnValue = "0" + rtnValue;
  }
  return rtnValue;
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}

void writeSettings() {
  // Save decoded message to SPIFFS file for playback on power up.
  File f = SPIFFS.open(CONFIG, "w");
  if (!f) {
    Serial.println("File open failed!");
  } else {
    Serial.println("Saving settings now...");
    f.println("UtcOffset=" + String(UtcOffset));
    f.println("refreshRate=" + String(minutesBetweenDataRefresh));
    f.println("brightness=" + String(brightness));
    f.println("themeColor=" + themeColor);
    f.println("www_username=" + String(www_username));
    f.println("www_password=" + String(www_password));;
    f.println("USE_FLASH=" + String(USE_FLASH));
  }
  f.close();
  readSettings();
  applyBrightness();
  timeClient.setUtcOffset(UtcOffset);
}

void readSettings() {
  if (SPIFFS.exists(CONFIG) == false) {
    Serial.println("Settings File does not yet exists.");
    writeSettings();
    return;
  }
  File fr = SPIFFS.open(CONFIG, "r");
  String line;
  while(fr.available()) {
    line = fr.readStringUntil('\n');

    if (line.indexOf("UtcOffset=") >= 0) {
      UtcOffset = line.substring(line.lastIndexOf("UtcOffset=") + 10).toFloat();
      Serial.println("UtcOffset=" + String(UtcOffset));
    }
    if (line.indexOf("brightness=") >= 0) {
      brightness = line.substring(line.lastIndexOf("brightness=") + 11).toInt();
      Serial.println("brightness=" + String(brightness));
    }
    if (line.indexOf("refreshRate=") >= 0) {
      minutesBetweenDataRefresh = line.substring(line.lastIndexOf("refreshRate=") + 12).toInt();
      Serial.println("minutesBetweenDataRefresh=" + String(minutesBetweenDataRefresh));
    }
    if (line.indexOf("themeColor=") >= 0) {
      themeColor = line.substring(line.lastIndexOf("themeColor=") + 11);
      themeColor.trim();
      Serial.println("themeColor=" + themeColor);
    }
    if (line.indexOf("www_username=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_username=") + 13);
      temp.trim();
      temp.toCharArray(www_username, sizeof(temp));
      Serial.println("www_username=" + String(www_username));
    }
    if (line.indexOf("www_password=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_password=") + 13);
      temp.trim();
      temp.toCharArray(www_password, sizeof(temp));
      Serial.println("www_password=" + String(www_password));
    }
    if(line.indexOf("USE_FLASH=") >= 0) {
      USE_FLASH = line.substring(line.lastIndexOf("USE_FLASH=") + 10).toInt();
      Serial.println("USE_FLASH=" + String(USE_FLASH));
    }
  }
  fr.close();
  timeClient.setUtcOffset(UtcOffset);
}

int getMinutesFromLastRefresh() {
  int minutes = (timeClient.getCurrentEpoch() - lastEpoch) / 60;
  return minutes;
}

///
/// Set all colors
///
void RollingFire(int offset) {
  Clear();
  
  for(int i=0; i < NUMPIXELS; i++) {
    AddColor(i, fire_color);
    int r = random(offset);
    uint32_t diff_color = pixels.Color (r, r/2, r/2);
    SubstractColor(i, diff_color);
  }
  pixels.show();
}

///
/// Every LED to black
///
void Clear() {
  for(uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, off_color);
  }
}

///
/// Set color of LED
///
void AddColor(uint8_t position, uint32_t color) {
  uint32_t blended_color = Blend(pixels.getPixelColor(position), color);
  pixels.setPixelColor(position, blended_color);
}

///
/// Set color of LED
///
void SubstractColor(uint8_t position, uint32_t color) {
  uint32_t blended_color = Substract(pixels.getPixelColor(position), color);
  pixels.setPixelColor(position, blended_color);
}

///
/// Color blending
///
uint32_t Blend(uint32_t color1, uint32_t color2) {
  uint8_t r1,g1,b1;
  uint8_t r2,g2,b2;
  uint8_t r3,g3,b3;
  
  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);
  
  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);
  
  return pixels.Color(constrain(r1+r2, 0, 255), constrain(g1+g2, 0, 255), constrain(b1+b2, 0, 255));
}

///
/// Color blending
///
uint32_t Substract(uint32_t color1, uint32_t color2) {
  uint8_t r1,g1,b1;
  uint8_t r2,g2,b2;
  uint8_t r3,g3,b3;
  int16_t r,g,b;
  
  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);
  
  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);
  
  r=(int16_t)r1-(int16_t)r2;
  g=(int16_t)g1-(int16_t)g2;
  b=(int16_t)b1-(int16_t)b2;
  if(r<0) r=0;
  if(g<0) g=0;
  if(b<0) b=0;
  
  return pixels.Color(r, g, b);
}
