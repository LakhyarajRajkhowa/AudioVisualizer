#include <imgui/imgui.h>

#include "UI/panels/NetworkPanel.h"

void NetworkPanel::Render() {
    if (!ImGui::CollapsingHeader("Network", ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    RenderHostSection();
    ImGui::Separator();
    RenderConnectSection();
}

void NetworkPanel::RenderHostSection() {
    ImGui::Text("Host");
    ImGui::PushID("host_section");

    bool hosting = network.IsHosting();

    if (hosting) ImGui::BeginDisabled();
    ImGui::InputInt("Port##host", &hostPort);
    if (hostPort < 0) hostPort = 0;
    if (hostPort > 65535) hostPort = 65535;
    if (hosting) ImGui::EndDisabled();

    if (!hosting) {
        if (ImGui::Button("Start Hosting")) {
            hostErrorMsg.clear();
            if (!network.StartHost(static_cast<uint16_t>(hostPort))) {
                hostErrorMsg = "Failed to start hosting on port " + std::to_string(hostPort) +
                               " (already in use?)";
            }
        }
    } else {
        if (ImGui::Button("Stop Hosting")) {
            network.StopHost();
        }
    }

    if (!hostErrorMsg.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", hostErrorMsg.c_str());
    }

    if (hosting) {
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Hosting on port %d", hostPort);
        ImGui::Text("Receivers connected: %zu", network.ConnectedReceiverCount());
    }

    ImGui::PopID();
}

void NetworkPanel::RenderConnectSection() {
    ImGui::Text("Connect");
    ImGui::PushID("connect_section");

    bool receiving = network.IsReceiving();

    if (receiving) ImGui::BeginDisabled();
    ImGui::InputText("Host IP##connect", connectIpBuffer, sizeof(connectIpBuffer));
    ImGui::InputInt("Port##connect", &connectPort);
    if (connectPort < 0) connectPort = 0;
    if (connectPort > 65535) connectPort = 65535;
    if (receiving) ImGui::EndDisabled();

    if (!receiving) {
        if (ImGui::Button("Connect")) {
            connectErrorMsg.clear();
            if (std::strlen(connectIpBuffer) == 0) {
                connectErrorMsg = "Enter a host IP address.";
            } else if (!network.StartReceiver(connectIpBuffer, static_cast<uint16_t>(connectPort))) {
                connectErrorMsg = "Failed to connect to " + std::string(connectIpBuffer) +
                                   ":" + std::to_string(connectPort);
            }
        }
    } else {
        if (ImGui::Button("Disconnect")) {
            network.StopReceiver();
        }
    }

    if (!connectErrorMsg.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", connectErrorMsg.c_str());
    }

    if (receiving) {
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Connected to %s",
                            network.ReceiverHostIp().c_str());
        ImGui::Text("Pending packets: %zu", network.PendingReceiverPackets());
        ImGui::Text("Dropped (late): %u", network.DroppedLateCount());
    }

    ImGui::PopID();
}
