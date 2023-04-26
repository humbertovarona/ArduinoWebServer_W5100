#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>

#define EthernetOn 2

LiquidCrystal Screen16x2(7, 8, 9, 3, 5 , 6);

byte mac[] = {0xDE, 0xAD, 0xBE, 0x00, 0xFE, 0xED};
IPAddress ip(150,161,75,217);
IPAddress gateway(150,161,255,2);
IPAddress subnet(255, 255, 255, 0);
 

EthernetServer WebServer(80);
EthernetClient WebClient;

void setup() {
    Screen16x2.begin(16, 2);
    
    Ethernet.begin(mac, ip, gateway, subnet);
    WebServer.begin();

    pinMode(EthernetOn, OUTPUT);
 
}

void loop() {

    printLCD(0, 0, "25.5 C");
    printLCD(8, 0, "60.7%");

    WebClient = WebServer.available();
    if (WebClient) {
      digitalWrite(EthernetOn, HIGH);
      boolean currentLineIsBlank = true;
      while (WebClient.connected()) {
        if (WebClient.available()) {
          char c = WebClient.read();
          if (c == '\n' && currentLineIsBlank) {
            beginHTML();
              printEthernet("Temperature", "25.5 C");
              printEthernet("Humidity", "60.7%");
              endHTML();
              break;
          }
          if (c == '\n') {
            currentLineIsBlank = true;
          }
          else if (c != '\r') {
            currentLineIsBlank = false;
          }
        }
      }
      delay(1);
      WebClient.stop();
    }
    else {
      digitalWrite(EthernetOn, LOW);
    }
}
  
void printEthernet(String Title, String Value) {
    WebClient.print(Title+": ");
    WebClient.println(Value);
    WebClient.println("<br />");
}

void beginHTML(void) {
  WebClient.println("HTTP/1.1 200 OK");
  WebClient.println("Content-Type: text/html");
  WebClient.println();
  WebClient.println("<!DOCTYPE HTML>");
  WebClient.println("<html>");
  WebClient.println("<head>");
  //WebClient.print("<meta http-equiv=\"refresh\" content=\"3\">");
  WebClient.println("<title>Sensor data</title>");
  WebClient.println("</head>");
  WebClient.println("<body>");  
}

void endHTML(void) {
  WebClient.println("</body>");
  WebClient.println("</html>");
}

void printLCD(int col, int row, String Value) {
    Screen16x2.setCursor(col, row);
    Screen16x2.write(Value.c_str());
}
