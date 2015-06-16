#include <SPI.h>
#include <Ethernet.h>
#include <RF24.h>
#include <string.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

RF24 radio(7,8);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {;}
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  radio.begin();
}

void sendReply(EthernetClient& client, const char* reply) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println(reply);
}

void sendError(EthernetClient& client, const char* reply) {
  client.println("HTTP/1.1 400 Bad Request");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println(reply);
}

struct command_t {
  char command;
  int16_t value;
};

void handleCommand(EthernetClient& client, const char* command) {
  Serial.println(command);

  if(strncmp(command, "POST /", 6) != 0) {
    sendError(client, "Invalid method");
    return;
  }

  char name[6];
  memset(name, 0, 6);
  strncpy(name, command + 6, 5);

  //Trim - no spaces or slashes allowed
  for(int i = 0; i < 6; ++i) if(name[i] == ' ' || name[i] == '/') name[i] = 0;

  if(strlen(name) < 1) {
    sendError(client, "Invalid name");
    return;
  }

  //Now find the command portion
  const char* cmd = strchr(command+6, '/') + 1;
  if(cmd == (const char*)1) {
    sendError(client, "Malformed request");
    return;
  }


  /* struct {char command; int value;} command_s; */
  struct command_t c;
  c.command = *cmd;
  c.value = atoi(cmd+1);
  radio.openWritingPipe((uint8_t*)name);
  radio.write(&c, sizeof(command_t));
  char buf[128];
  memset(buf, 0, 128);
  snprintf(buf, 127, "command: %c%d", c.command, c.value);
  Serial.print("bot: ");
  Serial.println(name);
  Serial.println(buf);

  sendReply(client, "Command Sent");
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
