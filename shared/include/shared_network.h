#pragma once

#include <iostream>
#include <enet/enet.h>

using std::string;

namespace shared::network {
    string from_packet_to_string(const ENetPacket *packet);
    int get_packet_type(ENetPacket t_packet);


}