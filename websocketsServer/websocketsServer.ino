/*
  ESP8266 WebSocket Server

  YouTube:  Rodney McKay
  https://www.youtube.com/watch?v=6utten8jP9E
  
  Github:   Portia-Lin
  https://github.com/Portia-Lin/websocketsServer

  ArduinoWebSockets library:
  https://github.com/Links2004/arduinoWebSockets
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h> // Подключить библиотеку для работы с вэбсокет сервером
const char* ssid = "SSID";  // Имя сети WiFi
const char* password = "PASSWORD"; // Пароль от сети WiFi

// Переменная с содержанием вэб страницы:
String web = "<!DOCTYPE html><html><head><title>Websocket</title><meta name='viewport' content='width=device-width, initial-scale=1.0' /><meta charset='UTF-8'><style>body {background-color: #F7F9FD;text-align: center;}</style></head><body><h1>Received message: <span id='message'>-</span></h1><button type='button' id='btnA'><h1>ON</h1></button><button type='button' id='btnB'><h1>OFF</h1></button></body><script>var Socket;document.getElementById('btnA').addEventListener('click', buttonApressed);document.getElementById('btnB').addEventListener('click', buttonBpressed);function init() {Socket = new WebSocket('ws://' + window.location.hostname + ':81/');Socket.onmessage = function(event) { processReceivedCommand(event); };}function processReceivedCommand(event) {document.getElementById('message').innerHTML = event.data;}function buttonApressed() {Socket.send('1');}function buttonBpressed() {Socket.send('0');}window.onload = function(event) {init();}</script></html>";

ESP8266WebServer server(80);  // Вэб сервер по порту "80"
WebSocketsServer webSocket = WebSocketsServer(81);  // Вэбсокет сервер по порту "81"

void setup() {
  // put your setup code here, to run once:
  pinMode(16, OUTPUT);
  Serial.begin(115200);
  Serial.println("");
  
  WiFi.begin(ssid, password); // Подключиться к WiFi сети с именем в переменной "ssid" и паролем в переменной "password"
  
  // Пока нет подключения, тормозить програму и выводить точки в монитор порта...
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Вывести IP адрес в монитор порта
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Отправить вэб страницу в переменной "web" по адресу "/"
  server.on("/", []() {
    server.send(200, "text\html", web);
  });
  
  server.begin(); // Запустить вэб сервер
  webSocket.begin();  // Запустить вэбсокет сервер
  webSocket.onEvent(webSocketEvent);  // Запустить функцию "webSocketEvent", если что-то произошло (подключение, отключение, входящее сообщение)
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();  // Зациклить вэб сервер
  webSocket.loop(); // Зациклить вэбсокет сервер
  
}

// Если что-то произошло, запускается:
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: // Когда клиент отключился
      Serial.print("WS Type ");
      Serial.print(type);
      Serial.println(": DISCONNECTED");
      break;
    case WStype_CONNECTED:  // Когда клиент подключился
      Serial.print("WS Type ");
      Serial.print(type);
      Serial.println(": CONNECTED");
      if (digitalRead(16) == HIGH) {  // Если на порту высокий логический уровень,
        webSocket.broadcastTXT("ON"); // Отправить клиенту "ON" через вэбсокет соединение
      }
      else {                            // Если на порту низкий логический уровень,
        webSocket.broadcastTXT("OFF");  // Отправить клиенту "ON" через вэбсокет соединение
      }
      break;
    case WStype_TEXT: // Когда пришло входящее сообщение
      Serial.println();
      Serial.println(payload[0]); // Вывести порядок первого элемента который пришел
      Serial.println(char(payload[0])); // Вывести первый символ, который пришел
      if (payload[0] == '1') {  // Если первый символ == 1,
        sendA();                // Выполнить функцию "sendA()"
      }
      if (payload[0] == '0') {  // Если первый символ == 0,
        sendB();                // Выполнить функцию "sendB()"
      }
      break;
  }
}

void sendA() {
  webSocket.broadcastTXT("ON"); // Отправить клиенту "ON" через вэбсокет соединение
  digitalWrite(16, HIGH);       // Установить высокий логический уровень на порту
}

void sendB() {
  webSocket.broadcastTXT("OFF");  // Отправить клиенту "OFF" через вэбсокет соединение
  digitalWrite(16, LOW);          // Установить низкий логический уровень на порту
}
