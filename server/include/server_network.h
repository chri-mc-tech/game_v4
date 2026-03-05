#pragma once
#include <enet/enet.h>
#include <string>

int enet_loop();
void enet_event_connected();
void enet_event_receive();
void enet_event_disconnected();
int create_enet_host();