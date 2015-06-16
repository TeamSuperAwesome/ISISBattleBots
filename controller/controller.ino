#include <SPI.h>
#include <Ethernet.h>
#include <RF24.h>
#include <string.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {;}
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void sendReply(EthernetClient& client, const char* reply)
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println(reply);
}

void handleCommand(EthernetClient& client, const char* command) {
  Serial.println(command);
  //if(strcmp(command, "GET /forward HTTP/1.1") == 0) {}
  sendReply(client, "hello world");
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        const int buf_size = 256;
        char buf[buf_size];
        int line_length = 0;
        memset(buf, 0, buf_size);
        for(int i = 0; i < buf_size-1; ++i) {
          char c = client.read();
          buf[i] = c;
          line_length = (c == '\n') ? 0 : line_length + 1;

          if(line_length == 0)
            break;
        }
        handleCommand(client, &buf[0]);
        break;
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }
}
