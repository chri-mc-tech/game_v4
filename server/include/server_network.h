#pragma once
#include <enet/enet.h>
#include <string>

#include "server_player.h"

int enet_loop();
void enet_event_connected(const ENetEvent &enet_event);
void enet_event_receive(const ENetEvent &enet_event);
void enet_event_disconnected(const ENetEvent &enet_event);
int create_enet_host();
void send_players_location();
void send_player_list(ENetPeer* peer);
void send_a_player_has_connected(Player* connected_player);
void send_a_player_has_disconnected(string uuid);
