#if defined(ESP8266)
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "WiFiUdp.h"
#elif defined(ESP32)
#include "WiFi.h"
#include "HTTPClient.h"
#endif

#include <WebServer.h>
#include <EasyDDNS.h>

const int ledPin = 2;
const char *ssid = "HUAWEI-1CR9M9";                         // WiFi名称
const char *password = "88888888";                          // WiFi密码
const char *macAddress = "2C:F0:5D:41:49:A6";               // 电脑的物理地址
const char *domain = "onedmb.duckdns.org";                  // 域名:mingbo.duckdns.org
const char *token = "fa2a8b51-96da-4776-81af-7a0c3a6fdc76"; // token:fa2a8b51-96da-4776-81af-7a0c3a6fdc76
const char *PublicIP = "";                                  // 默认空
const char *dnsServer = "duckdns";                          // dns服务商

WiFiServer server(80);
WebServer webServer(800);

void handleRoot()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  String html = "<html><head>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<title>ESP 控制面板</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; }";
  html += "h1 { color: #333; }";
  html += "button { width: 80px; height: 40px; border-radius: 10px; background-color: #4CAF50; color: white; margin: 20px 0px; cursor: pointer;}";
  html += "fieldset {width:300px; height:150px; margin: 0 auto;}";
  html += ".box-p {margin:0;padding:0;text-align:left;display:block;}";
  html += ".box-p2 {margin:0;padding:0;text-align:left;display:inline-block;width:50px;height:30px;}";
  html += "input {width:250px; height:30px;outline:0;}";
  html += ".but {width:50px; height:30px;border-radius: 8px; background-color: #4CAF50; color: white; margin: 5px 0px; cursor: pointer;}";
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
  html += "<fieldset>";                // 新添加的fieldset元素
  html += "<legend>WiFi配置</legend>"; // 新添加的legend元素
  html += "<div id=\"wifiConfig\">";
  html += "<p class=\"box-p2\">名称:</p>";
  html += "<input id=\"wifiId\" value='" + String(ssid) + "'>";
  html += "<br/>";
  html += "<p class=\"box-p2\">密码:</p>";
  html += "<input id=\"wifiPwd\" type=\"password\" value='" + String(password) + "'>";
  html += "<br/>";
  html += " <button class=\"but\" onclick=\"updateWifi()\" >配网</button>";
  html += "</div>";
  html += "</fieldset>";
  html += "<fieldset>";                // 新添加的fieldset元素
  html += "<legend>域名配置</legend>"; // 新添加的legend元素
  html += "<div id=\"domainConfig\">";
  html += "<p class=\"box-p2\">服务:</p>";
  html += "<input id=\"domainDns\" value='" + String(dnsServer) + "'/>";
  html += "<p class=\"box-p2\">域名:</p>";
  html += "<input id=\"domainName\" value='" + String(domain) + "'/>";
  html += "<br/>";
  html += "<p class=\"box-p2\">token:</p>";
  html += "<input id=\"tokenKey\" type=\"password\" value='" + String(token) + "'/>";
  html += "<br/>";
  html += " <button class=\"but\" onclick=\"domainUpdate()\" >配置</button>";
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
  html += "          alert('开机成功');"; // 成功时的消息
  html += "        } else {";
  html += "          alert('开机失败');"; // 失败时的消息
  html += "        }";
  html += "      }";
  html += "    };";
  html += "    xhr.open('GET', '/power-on', true);";
  html += "    xhr.send();";
  html += "  } else {";
  html += "    alert('取消开机');"; // 用户取消时的消息
  html += "  }";
  html += "}";
  html += "  var messageBox = document.getElementById('messageBox');";
  html += "  messageBox.innerHTML ='IP:" + String(PublicIP) + "';";
  html += "  var wifiInfo = document.getElementById('wifiInfo');";
  html += "  wifiInfo.innerHTML = 'WiFi:" + String(ssid) + "';";
  html += "  var domainInfo = document.getElementById('domainInfo');";
  html += "  domainInfo.innerHTML = '域名:" + String(domain) + "';";
  html += "function updateWifi() {";
  html += "  var wifiId = document.querySelector('#wifiId').value;";
  html += "  var wifiPwd = document.querySelector('#wifiPwd').value;";
  html += "  var wifiConfig = {ssid:wifiId,password:wifiPwd};";
  html += "  const wifiString = Object.keys(wifiConfig).map(key => key + '=' + encodeURIComponent(wifiConfig[key])).join('&');";
  html += "  console.log(wifiString);";
  html += "  fetch(`/wifiConfig?${wifiString}`)";
  html += "    .then(response => {";
  html += "      console.log('来自ESP的响应：', response);";
  html += "    })";
  html += "    .catch(error => {";
  html += "      console.error('发送JSON时出错：', error);";
  html += "    });";
  html += "};";
  html += "function domainUpdate() {";
  html += "  var domain = document.querySelector('#domainName').value;";
  html += "  var token = document.querySelector('#tokenKey').value;";
  html += "  var domainConfig = {domain:domain,token:token};";
  html += "  console.log(domainConfig);";
  html += "  const domainString = Object.keys(domainConfig).map(key => key + '=' + encodeURIComponent(domainConfig[key])).join('&');";
  html += "  fetch(`/domainConfig?${domainString}`)";
  html += "    .then(response => {";
  html += "      console.log('来自ESP的响应：', response);";
  html += "    })";
  html += "    .catch(error => {";
  html += "      console.error('发送JSON时出错：', error);";
  html += "    });";
  html += "};";
  html += "</script>";
  html += "</body></html>";
  webServer.send(200, "text/html;charset=UTF-8", html);
}

void sendWakeOnLan()
{
  // 将 MAC 地址解析为字节数组
  uint8_t mac[6];
  // 将 MAC 地址解析为字节数组
  if (
      sscanf(macAddress, "%2hhx:%*c%2hhx:%*c%2hhx:%*c%2hhx:%*c%2hhx:%*c%2hhx",
             &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6 &&
      sscanf(macAddress, "%2hhx-%*c%2hhx-%*c%2hhx-%*c%2hhx-%*c%2hhx-%*c%2hhx",
             &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6)
  {
    Serial.println("无法解析 MAC 地址");
    return;
  }
  // 创建 UDP 客户端
  WiFiUDP udp;
  udp.beginPacket(IPAddress(255, 255, 255, 255), 9); // 使用广播地址
  udp.write(0xFF);
  for (int i = 0; i < 16; i++)
  {
    udp.write(mac, 6); // 重复 16 次 MAC 地址
  }
  udp.endPacket();
  udp.stop();
}

void handlePowerOn()
{
  // 发送 Wake-on-LAN 包
  sendWakeOnLan();
  webServer.send(200, "text/;charset=UTF-8", "已发送开机请求");
}
void wifiConfig()
{
  String ssid = webServer.arg("ssid");
  String password = webServer.arg("password");
  // 处理收到的数据
  Serial.print("ssid:");
  Serial.println(ssid);
  Serial.print("password:");
  Serial.println(password);
  // 返回响应
  webServer.send(200, "application/json", "{\"status\":\"ok\"}");
}

void domainConfig()
{
  String newDomain = webServer.arg("domain");
  String token = webServer.arg("token");
  // 处理收到的数据
  Serial.print("domain:");
  Serial.println(domain);
  Serial.print("token:");
  Serial.println(token);
  String(domain)=newDomain;

  // 返回响应
  webServer.send(200, "application/json", "{\"status\":\"ok\"}");
}
void setup()
{

  Serial.begin(115200); // 波特率
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("连接成功！");
  // 启动服务
  server.begin();

  // 启动web服务
  webServer.begin();
  webServer.on("/", HTTP_GET, handleRoot);
  webServer.on("/power-on", HTTP_GET, handlePowerOn);
  webServer.on("/wifiConfig", HTTP_GET, wifiConfig);
  webServer.on("/domainConfig", HTTP_GET, domainConfig);
  EasyDDNS.service(dnsServer);

  EasyDDNS.client(domain, token);

  EasyDDNS.onUpdate([&](const char *oldIP, const char *newIP)
                    {
    Serial.print("PublicIP: ");
    PublicIP = newIP;
    Serial.println(PublicIP); });
}

void loop()
{
  EasyDDNS.update(1000);
  webServer.handleClient();
}
