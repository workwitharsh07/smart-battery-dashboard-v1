/*
 * =============================================================================
 * Project  : Smart Battery Dashboard V1
 * Version  : 1.0.0
 * Author   : LifeTronix
 * License  : MIT License
 * =============================================================================
 *
 * A smart ESP32-based battery monitoring system that measures real-time
 * battery voltage and current, calculates power and battery percentage,
 * and displays the data on a live web-based dashboard.
 *
 * Hardware Overview:
 * • ESP32 Development Board
 * • 18650 Li-ion Battery
 * • TP4056 Charging Module
 * • 2kΩ + 1kΩ Voltage Divider
 *
 * GPIO Connections:
 * • GPIO 35 → Battery voltage monitoring
 * • GPIO 34 → Current sensing input
 *
 * Features:
 * • Real-time voltage monitoring
 * • Current flow detection
 * • Charging and discharging status
 * • Power calculation
 * • Battery percentage estimation
 * • Live web-based dashboard
 * • Wi-Fi connectivity
 *
 * =============================================================================
 */

#include <WiFi.h>
#include <WebServer.h>

#define BATTERY_PIN 34
#define CHARGER_PIN 35

const char* WIFI_SSID="wifi-name";
const char* WIFI_PASSWORD="your-passwor";

WebServer server(80);

const float BATTERY_DIVIDER_RATIO=2.0;
const int SAMPLE_COUNT=15;

const float BATTERY_REMOVED_THRESHOLD=1.0;

const float CHARGER_ON_THRESHOLD=1.0;
const float CHARGER_OFF_THRESHOLD=0.7;

const float VOLTAGE_SMOOTHING=0.35;
const float PERCENT_SMOOTHING=0.08;

const unsigned long SENSOR_INTERVAL=400;
const unsigned long PERCENT_INTERVAL=3000;
const unsigned long SERIAL_INTERVAL=2000;

float rawBatteryVoltage=0.0;
float displayVoltage=0.0;
float percentageVoltage=0.0;
float chargerVoltage=0.0;

int batteryPercentage=0;

bool batteryPresent=false;
bool chargerConnected=false;

unsigned long lastSensorUpdate=0;
unsigned long lastPercentUpdate=0;
unsigned long lastSerialUpdate=0;

const char MAIN_page[] PROGMEM=R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>Live Battery Dashboard</title>

<style>
*{
  margin:0;
  padding:0;
  box-sizing:border-box;
}

body{
  min-height:100vh;
  font-family:Arial,Helvetica,sans-serif;
  background:radial-gradient(circle at top,#10233d,#02070f 60%);
  color:#fff;
  display:flex;
  justify-content:center;
}

.container{
  width:100%;
  max-width:500px;
  padding:32px 18px 35px;
}

.header{
  text-align:center;
  margin-bottom:28px;
}

.title{
  font-size:34px;
  font-weight:800;
  letter-spacing:1px;
  line-height:1.05;
}

.title span{
  color:#2498ff;
}

.main-card{
  background:linear-gradient(145deg,#0d223d,#06111f);
  border:1px solid #1469b5;
  border-radius:28px;
  padding:30px 20px 25px;
  box-shadow:0 0 40px rgba(36,152,255,.10);
}

.battery-label{
  text-align:center;
  color:#48aaff;
  font-size:18px;
  font-weight:700;
  letter-spacing:1.5px;
  margin-bottom:25px;
}

.battery-wrap{
  display:flex;
  justify-content:center;
  margin-bottom:25px;
}

.battery{
  width:190px;
  height:330px;
  border:7px solid #2498ff;
  border-radius:28px;
  position:relative;
  overflow:visible;
  background:linear-gradient(180deg,#102b45,#030b13);
  box-shadow:0 0 28px rgba(36,152,255,.22);
  transition:.4s;
}

.battery::before{
  content:"";
  position:absolute;
  width:70px;
  height:22px;
  background:#2498ff;
  top:-27px;
  left:50%;
  transform:translateX(-50%);
  border-radius:7px 7px 0 0;
  transition:.4s;
}

.battery-fill{
  position:absolute;
  left:9px;
  right:9px;
  bottom:9px;
  height:0%;
  overflow:hidden;
  border-radius:18px 18px 13px 13px;
  background:linear-gradient(to top,#06366f,#087ed9,#39cfff);
  transition:height .8s cubic-bezier(.22,.8,.25,1);
}

.wave{
  position:absolute;
  width:220%;
  height:80px;
  left:-60%;
  top:-35px;
  border-radius:45%;
  background:rgba(255,255,255,.30);
  animation:waveMove 4s linear infinite;
}

.wave.two{
  top:-20px;
  background:rgba(90,220,255,.35);
  animation:waveMoveReverse 5.5s linear infinite;
}

@keyframes waveMove{
  from{
    transform:translateX(-10%) rotate(2deg);
  }
  to{
    transform:translateX(10%) rotate(2deg);
  }
}

@keyframes waveMoveReverse{
  from{
    transform:translateX(10%) rotate(-2deg);
  }
  to{
    transform:translateX(-10%) rotate(-2deg);
  }
}

.percent{
  position:absolute;
  top:50%;
  left:50%;
  transform:translate(-50%,-50%);
  z-index:10;
  font-size:54px;
  font-weight:800;
  text-shadow:0 3px 15px rgba(0,0,0,.85);
}

.charging-icon{
  position:absolute;
  top:18px;
  left:50%;
  transform:translateX(-50%);
  z-index:20;
  font-size:38px;
  opacity:0;
  transition:.3s;
}

.charging-icon.active{
  opacity:1;
  animation:chargePulse 1s ease-in-out infinite;
}

@keyframes chargePulse{
  0%,100%{
    transform:translateX(-50%) scale(.85);
    opacity:.45;
  }

  50%{
    transform:translateX(-50%) scale(1.2);
    opacity:1;
  }
}

.battery.charging{
  border-color:#ffb347;
  box-shadow:0 0 38px rgba(255,179,71,.38);
}

.battery.charging::before{
  background:#ffb347;
}

.battery-fill.charging{
  background:linear-gradient(to top,#8a3500,#e87500,#ffb12e,#ffe28c);
}

.battery-fill.charging .wave{
  animation-duration:2s;
  background:rgba(255,255,255,.45);
}

.battery-fill.charging .wave.two{
  animation-duration:2.8s;
  background:rgba(255,220,120,.45);
}

.battery.low{
  border-color:#ff5c5c;
  box-shadow:0 0 30px rgba(255,92,92,.25);
}

.battery.low::before{
  background:#ff5c5c;
}

.battery-fill.low{
  background:linear-gradient(to top,#681010,#c83232,#ff7373);
}

.battery.empty{
  border-color:#536170;
  box-shadow:none;
}

.battery.empty::before{
  background:#536170;
}

.divider{
  height:1px;
  background:#16436c;
  margin:10px 0 25px;
}

.info-row{
  display:grid;
  grid-template-columns:1fr 1fr;
  gap:10px;
}

.info-box{
  text-align:center;
}

.info-label{
  color:#b5bdc8;
  font-size:14px;
  letter-spacing:1px;
  margin-bottom:10px;
}

.info-value{
  font-size:27px;
  font-weight:700;
}

.voltage{
  color:#2498ff;
}

.status{
  color:#55d68a;
}

.status.charging{
  color:#ffb347;
}

.status.low{
  color:#ff5c5c;
}

.status.no-battery{
  color:#7e8a96;
  font-size:20px;
}

.connection{
  margin-top:22px;
  padding:22px;
  border-radius:22px;
  background:linear-gradient(145deg,#0c1c31,#071321);
  border:1px solid #164d7d;
}

.connection-left{
  display:flex;
  align-items:center;
  gap:15px;
}

.dot{
  width:18px;
  height:18px;
  border-radius:50%;
  background:#2498ff;
  box-shadow:0 0 15px rgba(36,152,255,.8);
}

.connection-label{
  color:#b8c0ca;
  font-size:13px;
  letter-spacing:1px;
  margin-bottom:5px;
}

.connection-status{
  color:#2498ff;
  font-size:22px;
  font-weight:700;
}

.footer{
  margin-top:28px;
  text-align:center;
  color:#2498ff;
  font-size:20px;
  letter-spacing:1px;
}

@media(max-width:380px){
  .title{
    font-size:29px;
  }

  .battery{
    width:165px;
    height:285px;
  }

  .percent{
    font-size:48px;
  }
}
</style>
</head>

<body>

<div class="container">

  <div class="header">
    <div class="title">
      LIVE <span>BATTERY</span><br>
      DASHBOARD
    </div>
  </div>

  <div class="main-card">

    <div class="battery-label">BATTERY LEVEL</div>

    <div class="battery-wrap">
      <div class="battery" id="battery">

        <div class="battery-fill" id="batteryFill">
          <div class="wave"></div>
          <div class="wave two"></div>
        </div>

        <div class="charging-icon" id="chargingIcon">⚡</div>

        <div class="percent" id="percentage">0%</div>

      </div>
    </div>

    <div class="divider"></div>

    <div class="info-row">

      <div class="info-box">
        <div class="info-label">VOLTAGE</div>
        <div class="info-value voltage" id="voltage">
          0.00 V
        </div>
      </div>

      <div class="info-box">
        <div class="info-label">STATUS</div>
        <div class="info-value status" id="status">
          --
        </div>
      </div>

    </div>

  </div>

  <div class="connection">
    <div class="connection-left">

      <div class="dot"></div>

      <div>
        <div class="connection-label">
          CONNECTION
        </div>

        <div class="connection-status">
          Connected
        </div>
      </div>

    </div>
  </div>

  <div class="footer">
    lifetronix.in
  </div>

</div>

<script>

let requestRunning=false;

async function updateData(){

  if(requestRunning)return;

  requestRunning=true;

  try{

    const response=await fetch(
      "/data?time="+Date.now(),
      {
        method:"GET",
        cache:"no-store"
      }
    );

    if(!response.ok){
      throw new Error("ESP32 error");
    }

    const data=await response.json();

    const battery=document.getElementById("battery");
    const fill=document.getElementById("batteryFill");
    const icon=document.getElementById("chargingIcon");
    const status=document.getElementById("status");

    document.getElementById("voltage").textContent=
      Number(data.voltage).toFixed(2)+" V";

    document.getElementById("percentage").textContent=
      data.percentage+"%";

    fill.style.height=data.percentage+"%";

    battery.className="battery";
    fill.className="battery-fill";
    status.className="info-value status";

    icon.classList.remove("active");

    if(data.status==="Charging"){

      battery.classList.add("charging");
      fill.classList.add("charging");
      status.classList.add("charging");

      icon.classList.add("active");

    }

    else if(data.status==="Low"){

      battery.classList.add("low");
      fill.classList.add("low");
      status.classList.add("low");

    }

    else if(data.status==="No Battery"){

      battery.classList.add("empty");
      status.classList.add("no-battery");

    }

    status.textContent=data.status;

  }

  catch(error){

    console.log("Dashboard update failed");

  }

  finally{

    requestRunning=false;

  }
}

updateData();

setInterval(
  updateData,
  500
);

</script>

</body>
</html>
)rawliteral";

float readADCVoltage(int pin){

  uint32_t total=0;

  for(int i=0;i<SAMPLE_COUNT;i++){

    total+=analogReadMilliVolts(pin);

    delay(2);

  }

  return(total/(float)SAMPLE_COUNT)/1000.0;
}

int calculatePercentage(float voltage){

  if(voltage>=4.20)return 100;

  if(voltage>=4.15)
    return map((int)(voltage*1000),4150,4200,95,100);

  if(voltage>=4.10)
    return map((int)(voltage*1000),4100,4150,88,95);

  if(voltage>=4.00)
    return map((int)(voltage*1000),4000,4100,75,88);

  if(voltage>=3.90)
    return map((int)(voltage*1000),3900,4000,60,75);

  if(voltage>=3.80)
    return map((int)(voltage*1000),3800,3900,45,60);

  if(voltage>=3.70)
    return map((int)(voltage*1000),3700,3800,30,45);

  if(voltage>=3.60)
    return map((int)(voltage*1000),3600,3700,15,30);

  if(voltage>=3.50)
    return map((int)(voltage*1000),3500,3600,8,15);

  if(voltage>=3.40)
    return map((int)(voltage*1000),3400,3500,3,8);

  if(voltage>=3.30)
    return map((int)(voltage*1000),3300,3400,1,3);

  return 0;
}

void updateCharger(){

  chargerVoltage=
    readADCVoltage(CHARGER_PIN);

  if(chargerConnected){

    if(chargerVoltage<
       CHARGER_OFF_THRESHOLD){

      chargerConnected=false;

    }

  }

  else{

    if(chargerVoltage>
       CHARGER_ON_THRESHOLD){

      chargerConnected=true;

    }

  }
}

void updateBattery(){

  float adcVoltage=
    readADCVoltage(BATTERY_PIN);

  rawBatteryVoltage=
    adcVoltage*
    BATTERY_DIVIDER_RATIO;

  // Battery removed
  if(rawBatteryVoltage<
     BATTERY_REMOVED_THRESHOLD){

    batteryPresent=false;

    rawBatteryVoltage=0.0;
    displayVoltage=0.0;
    percentageVoltage=0.0;
    batteryPercentage=0;

    return;
  }

  // Battery connected
  if(!batteryPresent){

    batteryPresent=true;

    displayVoltage=
      rawBatteryVoltage;

    percentageVoltage=
      rawBatteryVoltage;

    batteryPercentage=
      calculatePercentage(
        rawBatteryVoltage
      );

    return;
  }

  // Smooth displayed voltage
  displayVoltage+=
    VOLTAGE_SMOOTHING*
    (
      rawBatteryVoltage-
      displayVoltage
    );
}

void updatePercentage(){

  if(!batteryPresent){

    batteryPercentage=0;

    return;
  }

  if(
    millis()-
    lastPercentUpdate
    <
    PERCENT_INTERVAL
  ){
    return;
  }

  lastPercentUpdate=millis();

  percentageVoltage+=
    PERCENT_SMOOTHING*
    (
      rawBatteryVoltage-
      percentageVoltage
    );

  int target=
    calculatePercentage(
      percentageVoltage
    );

  if(chargerConnected){

    // While charging percentage
    // only moves upward
    if(target>
       batteryPercentage){

      batteryPercentage++;

    }

  }

  else{

    // Slowly follow real battery level
    if(target>
       batteryPercentage){

      batteryPercentage++;

    }

    else if(target<
            batteryPercentage){

      batteryPercentage--;

    }

  }

  batteryPercentage=
    constrain(
      batteryPercentage,
      0,
      100
    );
}

String getBatteryStatus(){

  if(!batteryPresent){
    return"No Battery";
  }

  if(chargerConnected){
    return"Charging";
  }

  if(batteryPercentage<=10){
    return"Low";
  }

  if(batteryPercentage>=95){
    return"Full";
  }

  return"Normal";
}

void updateSystem(){

  updateBattery();

  updateCharger();

  updatePercentage();
}

void handleData(){

  String json="{";

  json+="\"voltage\":"+
        String(
          displayVoltage,
          3
        );

  json+=",\"percentage\":"+
        String(
          batteryPercentage
        );

  json+=",\"status\":\""+
        getBatteryStatus()+
        "\"";

  json+="}";

  server.sendHeader(
    "Cache-Control",
    "no-store, no-cache, must-revalidate, max-age=0"
  );

  server.sendHeader(
    "Pragma",
    "no-cache"
  );

  server.sendHeader(
    "Expires",
    "0"
  );

  server.send(
    200,
    "application/json",
    json
  );
}

void setup(){

  Serial.begin(115200);

  analogReadResolution(12);

  analogSetPinAttenuation(
    BATTERY_PIN,
    ADC_11db
  );

  analogSetPinAttenuation(
    CHARGER_PIN,
    ADC_11db
  );

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  Serial.println();
  Serial.print(
    "Connecting to WiFi"
  );

  while(
    WiFi.status()!=WL_CONNECTED
  ){

    delay(500);

    Serial.print(".");

  }

  Serial.println();

  Serial.println(
    "WiFi Connected"
  );

  Serial.print(
    "Open: http://"
  );

  Serial.println(
    WiFi.localIP()
  );

  updateSystem();

  server.on(
    "/",
    HTTP_GET,
    [](){

      server.sendHeader(
        "Cache-Control",
        "no-store"
      );

      server.send_P(
        200,
        "text/html",
        MAIN_page
      );

    }
  );

  server.on(
    "/data",
    HTTP_GET,
    handleData
  );

  server.begin();

  Serial.println(
    "Web Server Started"
  );
}

void loop(){

  server.handleClient();

  if(
    millis()-
    lastSensorUpdate
    >=
    SENSOR_INTERVAL
  ){

    lastSensorUpdate=
      millis();

    updateSystem();

  }

  if(
    millis()-
    lastSerialUpdate
    >=
    SERIAL_INTERVAL
  ){

    lastSerialUpdate=
      millis();

    Serial.print(
      "Battery: "
    );

    Serial.print(
      rawBatteryVoltage,
      3
    );

    Serial.print(
      "V | Display: "
    );

    Serial.print(
      displayVoltage,
      3
    );

    Serial.print(
      "V | Percentage: "
    );

    Serial.print(
      batteryPercentage
    );

    Serial.print(
      "% | Charger: "
    );

    Serial.print(
      chargerConnected?
      "YES":"NO"
    );

    Serial.print(
      " | Status: "
    );

    Serial.println(
      getBatteryStatus()
    );

  }

}