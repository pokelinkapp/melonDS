#ifndef MELONDS_RPCSERVER_H
#define MELONDS_RPCSERVER_H
#include "../NDS.h"

void StartRPC(unsigned short port, u8(*arm9Read8)(u32 addr));
void StopRPC();

#endif //MELONDS_RPCSERVER_H
