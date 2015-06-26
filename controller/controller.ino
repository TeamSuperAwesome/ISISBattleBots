#include <SPI.h>
#include <Ethernet.h>
#include <RF24.h>
#include <string.h>
#include <ArduinoButton.h>
#include <UniversalInputManager.h>

// Comment out for quickness
#define SERIAL_DEBUG

ArduinoButton killswitchButton(0, 3);
UniversalInputManager inputs;

#ifdef SERIAL_DEBUG
int serial_putc(char c, FILE*) {
  Serial.write(c);
  return c;
}
#endif

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 150);
EthernetServer server(80);

RF24 radio(6, 7);

void setup() {
#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  fdevopen(&serial_putc, 0);
#endif

  inputs.addDevice(&killswitchButton);
  inputs.setCallback(buttonHandler);

  Ethernet.begin(mac, ip);
  server.begin();

#ifdef SERIAL_DEBUG
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
#endif

  radio.begin();

#ifdef SERIAL_DEBUG
  radio.printDetails();
#endif
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

uint64_t nameToPipe(const char* name) {
  if (strcmp("alice", name) == 0) return 0xA11CE;
  if (strcmp("bob", name) == 0) return 0xB0B;
  if (strcmp("eve", name) == 0) return 0xE11E;
  return 0;
}

void handleCommand(EthernetClient& client, const char* command) {
  if (strncmp(command, "OPTIONS /", 9) == 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Connection: close");
    client.println();
    return;
  }

  /* if (strncmp(command, "POST /", 6) != 0) { */
  /*   sendError(client, "Invalid method"); */
  /*   return; */
  /* } */

  if (!killswitchButton.isActive()) {
    sendError(client, "Disarmed");
    return;
  }

  char name[6];
  memset(name, 0, 6);
  strncpy(name, command + 6, 5);

  // Trim - no spaces or slashes allowed
  for (int i = 0; i < 6; ++i)
    if (name[i] == ' ' || name[i] == '/') name[i] = 0;

  const uint64_t pipe = nameToPipe(name);

  if (strlen(name) < 1 || pipe == 0) {
    sendError(client, "Invalid name");
    return;
  }

  // Now find the command portion
  const char* cmd = strchr(command + 6, '/') + 1;
  if (cmd == (const char*)1) {
    sendError(client, "Malformed request");
    return;
  }

  sendCommand(pipe, *cmd, atoi(cmd + 1));
  sendReply(client, "Command Sent");
}

void sendCommand(const uint64_t pipe, char cmd, int16_t value) {
  struct command_t c;
  c.command = cmd;
  c.value = value;
  char buf[128];
  memset(buf, 0, 128);
  snprintf(buf, 127, "command: %c%d", c.command, c.value);
  radio.openWritingPipe(pipe);
  radio.write(&c, sizeof(command_t));
}

void buttonHandler(inputtype_t type, IInputDevice * device)
{
  if (device == &killswitchButton) {
    IButton * button = (IButton *) device;
    bool armed = button->isActive();
#ifdef SERIAL_DEBUG
    Serial.print("Armed: ");
    Serial.println(armed);
#endif

    // Kill all the bots!
    if (!armed) {
      sendCommand(nameToPipe("alice"), 'S', 0);
      sendCommand(nameToPipe("bob"), 'S', 0);
      sendCommand(nameToPipe("eve"), 'S', 0);
    }
  }
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
        for (int i = 0; i < buf_size - 1; ++i) {
          char c = client.read();
          buf[i] = c;
          line_length = (c == '\n') ? 0 : line_length + 1;

          if (line_length == 0) break;
        }
        handleCommand(client, &buf[0]);
        break;
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }

  inputs.poll();
}
