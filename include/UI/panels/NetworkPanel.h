#pragma once

#include "network/NetworkService.h"

#include <cstring>
#include <string>

class NetworkPanel {
public:
    NetworkPanel(NetworkService& net) : network(net){}
    void Render();

private:
    void RenderHostSection();
    void RenderConnectSection();

    NetworkService& network;

    int hostPort = VIZ_DATA_PORT;
    std::string hostErrorMsg;

    char connectIpBuffer[64] = "";
    int connectPort = VIZ_DATA_PORT;
    std::string connectErrorMsg;
};
