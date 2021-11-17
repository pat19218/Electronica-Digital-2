     /*************************************************************************************************
  ESP32 Web Server
  Ejemplo de creación de Web server con acceso a SPIFFS
  Basándose en los ejemplos de:
  https://github.com/khoih-prog/WiFiWebServer
  https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/
  https://github.com/luisllamasbinaburo/ESP32-Examples
  https://www.luisllamas.es/esp8266-esp32/
**************************************************************************************************/
//************************************************************************************************
// Librerías
//************************************************************************************************
#include <WiFi.h> 
#include <SPIFFS.h>
#include <WebServer.h>
char palabra;
//************************************************************************************************
// Variables globales
//************************************************************************************************
// SSID & Password
const char* ssid = "tacu´s wifi";  // Enter SSID here
const char* password = "12345678";  //Enter Password here
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)


//************************************************************************************************
// Configuración
//************************************************************************************************
void setup() {
  Serial2.begin(115200);
  Serial.begin(115200);
  
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("password: ");
  Serial.println(password);

  server.on("/", handle_OnConnect); // página de inicio
  server.on("/0espacio", handle_0on); // handler al existir 0 espacio
  server.on("/1espacio", handle_1on); // handler al existir 1 espacio
  server.on("/2espacio", handle_2on); // handler al existir 2 espacio
  server.on("/3espacio", handle_3on); // handler al existir 3 espacio
  server.on("/4espacio", handle_4on); // handler al existir 4 espacio
  server.on("/5espacio", handle_5on); // handler al existir 5 espacio
  server.on("/6espacio", handle_6on); // handler al existir 6 espacio
  server.on("/7espacio", handle_7on); // handler al existir 7 espacio
  server.on("/8espacio", handle_8on); // handler al existir 8 espacio
  server.onNotFound([]() {                  // si el cliente solicita una uri desconocida
    if (!HandleFileRead(server.uri()))      // enviar archivo desde SPIFF, si existe
      handleNotFound();             // sino responder con un error 404 (no existe)
  });
  //server.onNotFound(handle_NotFound); //si no se encuentra el uri, responder con error 404 (no existe)

  server.begin(); // iniciar servidor
  Serial.println("HTTP server started");
  delay(100);
}
//************************************************************************************************
// loop principal
//************************************************************************************************
void loop() {
  if (Serial2.available()) {
    palabra = Serial2.read();
  }
  server.handleClient(); // escuchar solicitudes de clientes
  delay(500);
  delay(500);
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
  
}
//************************************************************************************************
// Handler de página de inicio
//************************************************************************************************
void handle_OnConnect() {
  handle_8on(); // inicia LED apagado, por defecto
}
//************************************************************************************************
// Handler de handle_0on
//************************************************************************************************
void handle_0on() {
  
  Serial.println("Espacio disponibles: 0");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}
//************************************************************************************************
// Handler de handle_1on
//************************************************************************************************
void handle_1on() {
  
  Serial.println("Espacio disponibles: 1");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}
//************************************************************************************************
// Handler de handle_2on
//************************************************************************************************
void handle_2on() {
  
  Serial.println("Espacio disponibles: 2");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}
//************************************************************************************************
// Handler de handle_3on
//************************************************************************************************
void handle_3on() {
  
  Serial.println("Espacio disponibles: 3");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}
//************************************************************************************************
// Handler de handle_4on
//************************************************************************************************
void handle_4on() {
  
  Serial.println("Espacio disponibles: 4");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}
//************************************************************************************************
// Handler de handle_5on
//************************************************************************************************
void handle_5on() {
  
  Serial.println("Espacio disponibles: 5");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}
//************************************************************************************************
// Handler de handle_6on
//************************************************************************************************
void handle_6on() {
  
  Serial.println("Espacio disponibles: 6");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}
//************************************************************************************************
// Handler de handle_7on
//************************************************************************************************
void handle_7on() {
  
  Serial.println("Espacio disponibles: 7");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}
//************************************************************************************************
// Handler de handle_8on
//************************************************************************************************
void handle_8on() {
  
  Serial.println("Espacio disponibles: 8");
  server.send(200, "text/html", SendHTML(palabra)); //responde con un OK (200) y envía HTML
}

//************************************************************************************************
// Procesador de HTML
//************************************************************************************************
String SendHTML(char cantidad) {
 String ptr = "<!DOCTYPE html> <html>\n";  //indicates that we’re sending HTML code.
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title> Parqueo-matic </title>\n";
  ptr += "<meta http-equiv=\"refresh\" content=\"3\" />";
  ptr += "</head>";
  ptr += "<body bgcolor = \"#211040\" text=\"#54B0F9\">";
  ptr += "<img src=\"uvg.png\" align=\"right\">";
  ptr += "<h1 align=\"center\"> Parqueo-matic </h1>";
  ptr += "<h4 align=\"center\"> contactanos al: 1-800-tech </h4>";
  ptr += "<br>  <br/>";
  ptr += "<br>  <br/>";
  ptr += "<p>  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; La cantidad de espacios disponibles es: </p>";
  ptr += "<br>  <br/>";
  
  if (cantidad == '0') {
    ptr += "<h1 align=\"center\"> 0 </h1>";
  }else if(cantidad == '1') {
    ptr += "<h1 align=\"center\"> 1 </h1>";
  }else if(cantidad == '2') {
    ptr += "<h1 align=\"center\"> 2 </h1>";
  }else if(cantidad == '3') {
    ptr += "<h1 align=\"center\"> 3 </h1>";
  }else if(cantidad == '4') {
    ptr += "<h1 align=\"center\"> 4 </h1>";
  }else if(cantidad == '5') {
    ptr += "<h1 align=\"center\"> 5 </h1>";
  }else if(cantidad == '6') {
    ptr += "<h1 align=\"center\"> 6 </h1>";
  }else if(cantidad == '7') {
    ptr += "<h1 align=\"center\"> 7 </h1>";
  }else if(cantidad == '8') {
    ptr += "<h1 align=\"center\"> 8 </h1>";
  }

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
//************************************************************************************************
// Handler de not found
//************************************************************************************************
void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}
//************************************************************************************************
// Obtener el tipo de contenido del archivo
//************************************************************************************************
String GetContentType(String filename)
{
  if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
//************************************************************************************************
// Enviar al servidor el archivo desde SPIFFS
//************************************************************************************************
void ServeFile(String path)
{
  File file = SPIFFS.open(path, "r");
  size_t sent = server.streamFile(file, GetContentType(path));
  file.close();
}
//************************************************************************************************
// Enviar al servidor el archivo desde SPIFFS
//************************************************************************************************
void ServeFile(String path, String contentType)
{
  File file = SPIFFS.open(path, "r");
  size_t sent = server.streamFile(file, contentType);
  file.close();
}
//************************************************************************************************
// Handler de not found
//************************************************************************************************
bool HandleFileRead(String path)
{
  if (path.endsWith("/")) path += "index.html";
  Serial.println("handleFileRead: " + path);

  if (SPIFFS.exists(path))
  {
    ServeFile(path);
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;
}
