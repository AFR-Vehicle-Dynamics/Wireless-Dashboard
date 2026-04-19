#pragma once
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

class dashboardServer
{
    private:
        static constexpr char* ssid = "Banana phone";
        static constexpr char* password = "1234abcd";
        AsyncWebServer server;
        AsyncWebSocket ws;
    public:
        dashboardServer() : server(8080), ws("/ws") {}
        static void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
        void setup();
        void sendData(String data);
        void cleanClients();
};