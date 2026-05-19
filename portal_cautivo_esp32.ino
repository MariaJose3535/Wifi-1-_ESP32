#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "lwip/apps/lwip_napt.h"

// ==========================
// WIFI CON INTERNET
// ==========================

const char* sta_ssid = "estudiantes";
const char* sta_password = "987654312.";

// ==========================
// RED DEL ESP32
// ==========================

const char* ap_ssid = "Portal_ESP32";
const char* ap_password = "12345678";

// ==========================

IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);
DNSServer dnsServer;

// ==========================
// PAGINA WEB
// ==========================

void paginaPrincipal() {

  String html = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Portal ESP32</title>

<style>

body{
    background:#0f172a;
    color:white;
    font-family:Arial;
    text-align:center;
    margin-top:40px;
}

.card{
    background:#1e293b;
    width:320px;
    margin:auto;
    padding:20px;
    border-radius:15px;
}

input{
    width:90%;
    padding:10px;
    margin:10px;
    border:none;
    border-radius:10px;
}

button{
    width:95%;
    padding:12px;
    background:#2563eb;
    color:white;
    border:none;
    border-radius:10px;
}

</style>

</head>

<body>

<div class='card'>

<h1>Bienvenido</h1>

<h2>Portal Cautivo ESP32</h2>

<p>Sistema con Internet Compartido</p>

<form action='/guardar' method='POST'>

<input type='text' name='nombre' placeholder='Nombre' required>

<input type='text' name='control' placeholder='Numero de Control' required>

<button type='submit'>Registrar</button>

</form>

<hr>

<p>Internet Compartido Activo</p>

</div>

</body>
</html>

)rawliteral";

  server.send(200, "text/html", html);
}

// ==========================
// RESPUESTA FORMULARIO
// ==========================

void guardarDatos() {

  String nombre = server.arg("nombre");
  String control = server.arg("control");

  String respuesta = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body{
    background:#020617;
    color:white;
    font-family:Arial;
    text-align:center;
    margin-top:50px;
}

.card{
    background:#1e293b;
    width:320px;
    margin:auto;
    padding:20px;
    border-radius:15px;
}

</style>

</head>

<body>

<div class='card'>

<h1>Registro Exitoso</h1>

)rawliteral";

  respuesta += "<p>Bienvenido: " + nombre + "</p>";
  respuesta += "<p>Numero de Control: " + control + "</p>";

  respuesta += R"rawliteral(

<p>ESP32 Compartiendo Internet</p>

</div>

</body>
</html>

)rawliteral";

  server.send(200, "text/html", respuesta);
}

// ==========================
// SETUP
// ==========================

void setup() {

  Serial.begin(115200);

  // AP + STA
  WiFi.mode(WIFI_AP_STA);

  // CONECTAR A INTERNET
  WiFi.begin(sta_ssid, sta_password);

  Serial.println("Conectando a internet...");

  while(WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Internet conectada");

  Serial.print("IP Internet: ");
  Serial.println(WiFi.localIP());

  // CREAR AP
  WiFi.softAP(ap_ssid, ap_password);

  WiFi.softAPConfig(local_ip, gateway, subnet);

  Serial.println("");
  Serial.println("AP iniciado");

  Serial.print("IP AP: ");
  Serial.println(WiFi.softAPIP());

  // ACTIVAR NAT
  ip_napt_init(1000, 10);

  ip_napt_enable_no(SOFTAP_IF, 1);

  // DNS
  dnsServer.start(53, "*", local_ip);

  // WEB
  server.on("/", paginaPrincipal);

  server.on("/guardar", HTTP_POST, guardarDatos);

  server.onNotFound([]() {

      server.sendHeader("Location", "/", true);

      server.send(302, "text/plain", "");
  });

  server.begin();

  Serial.println("Portal iniciado");
}

// ==========================
// LOOP
// ==========================

void loop() {

  dnsServer.processNextRequest();

  server.handleClient();
}