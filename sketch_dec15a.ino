#if defined(ESP8266)
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#elif defined(ESP32)
#include "WiFi.h"
#include "HTTPClient.h"
#endif

#include <WebServer.h>
#include <EasyDDNS.h>

const int ledPin = 2;
const char* ssid = "HUAWEI-1CR9M9";            //WiFi名称
const char* password = "88888888";             //WiFi密码
const char* macAddress = "BC-09-1B-10-25-A2";  //电脑的物理地址
const char* domain = "mingbo.duckdns.org";     //域名
const char* token = "000-000-000";                        //token
const char* PublicIP = "";                     //默认空
const char* dnsServer = "duckdns";             //dns服务商

WiFiServer server(80);
WebServer webServer(800);

void setup() {
  Serial.begin(115200);  //波特率

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("WiFi连接成功");
  }
  // 启动服务
  server.begin();

  // 启动web服务
  webServer.begin();
  webServer.on("/", HTTP_GET, handleRoot);
  webServer.on("/power-on", HTTP_GET, handlePowerOn);

  EasyDDNS.service(dnsServer);

  EasyDDNS.client(domain, token);

  EasyDDNS.onUpdate([&](const char* oldIP, const char* newIP) {
    Serial.print("EasyDDNS - IP Change Detected: ");
    Serial.println(newIP);
    PublicIP = newIP;
  });
}

void handleRoot() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  String html = "<html><head>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<title>ESP 控制面板</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; }";
  html += "h1 { color: #333; }";
  html += "button { width: 80px; height: 40px; border-radius: 10px; background-color: #4CAF50; color: white; margin: 20px 0px; cursor: pointer;}";
  html += "fieldset {width:300px; height:110px; margin: 0 auto;}";
  html += ".box-p {margin:0;padding:0;text-align:left;display:block;}";
  html += ".box-p2 {margin:0;padding:0;text-align:left;display:inline-block;width:50px;height:30px;}";
  html += "input {width:250px; height:30px;outline:0;}";
  html += "</style></head><body>";
  html += "<h1>ESP 控制面板</h1>";
  html += "<button onclick=\"openPC()\">开机</button>";
  html += "<fieldset>";
  html += "<legend>信息盒子</legend>";
  html += "<div>";
  html += "<p id=\"messageBox\" class=\"box-p\"></p>";
  html += "<p id=\"wifiInfo\"   class=\"box-p\"></p>";
  html += "<p id=\"domainInfo\" class=\"box-p\"></p>";
  html += "</div>";
  html += "</fieldset>";
  html += "<fieldset>";                 // 新添加的fieldset元素
  html += "<legend>WiFi配置</legend>";  // 新添加的legend元素
  html += "<div id=\"wifiConfig\">";
  html += "<p class=\"box-p2\">名称:</p>";
  html += "<input class=\"wifiId\" value='" + String(ssid) + "'>";
  html += "<br/>";
  html += "<p class=\"box-p2\">密码:</p>";
  html += "<input type=\"password\" class=\"wifiPwd\" value='" + String(password) + "'>";
  html += "</div>";
  html += "</fieldset>";
  html += "<fieldset>";                 // 新添加的fieldset元素
  html += "<legend>域名配置</legend>";  // 新添加的legend元素
  html += "<div id=\"domainConfig\">";
  html += "<p class=\"box-p2\">服务:</p>";
  html += "<input value='" + String(dnsServer) + "'/>";
  html += "<p class=\"box-p2\">域名:</p>";
  html += "<input value='" + String(domain) + "'/>";
  html += "<br/>";
  html += "<p class=\"box-p2\">token:</p>";
  html += "<input type=\"password\" value='" + String(token) + "'/>";
  html += "</div>";
  html += "</fieldset>";
  html += "<script>";
  html += "function openPC() {";
  html += "  var confirmed = confirm('确定要开机吗？');";
  html += "  if (confirmed) {";
  html += "    var xhr = new XMLHttpRequest();";
  html += "    xhr.onreadystatechange = function() {";
  html += "      if (xhr.readyState == 4) {";
  html += "        if (xhr.status == 200) {";
  html += "          alert('开机成功');";  // 成功时的消息
  html += "        } else {";
  html += "          alert('开机失败');";  // 失败时的消息
  html += "        }";
  html += "      }";
  html += "    };";
  html += "    xhr.open('GET', '/power-on', true);";
  html += "    xhr.send();";
  html += "  } else {";
  html += "    alert('取消开机');";  // 用户取消时的消息
  html += "  }";
  html += "}";
  html += "  var messageBox = document.getElementById('messageBox');";
  html += "  messageBox.innerHTML ='IP:" + String(PublicIP) + "';";
  html += "  var wifiInfo = document.getElementById('wifiInfo');";
  html += "  wifiInfo.innerHTML = 'WiFi:" + String(ssid) + "';";
  html += "  var domainInfo = document.getElementById('domainInfo');";
  html += "  domainInfo.innerHTML = '域名:" + String(domain) + "';";
  html += "</script>";
  html += "</body></html>";

  webServer.send(200, "text/html;charset=UTF-8", html);
}

void handlePowerOn() {
  // 发送 Wake-on-LAN 包
  sendWakeOnLan();
  webServer.send(200, "text/;charset=UTF-8", "已发送开机请求");
}

void sendWakeOnLan() {
  // 将 MAC 地址解析为字节数组
  uint8_t mac[6];
  // 将 MAC 地址解析为字节数组
  if (
    sscanf(macAddress, "%2hhx:%*c%2hhx:%*c%2hhx:%*c%2hhx:%*c%2hhx:%*c%2hhx",
           &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])
      != 6
    && sscanf(macAddress, "%2hhx-%*c%2hhx-%*c%2hhx-%*c%2hhx-%*c%2hhx-%*c%2hhx",
              &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])
         != 6) {
    Serial.println("无法解析 MAC 地址");
    return;
  }
  // 创建 UDP 客户端
  WiFiUDP udp;
  udp.beginPacket(IPAddress(255, 255, 255, 255), 9);  // 使用广播地址
  udp.write(0xFF);
  for (int i = 0; i < 16; i++) {
    udp.write(mac, 6);  // 重复 16 次 MAC 地址
  }
  udp.endPacket();
  udp.stop();
}
void loop() {
  EasyDDNS.update(10000);
  webServer.handleClient();
}