#include "RpcServer.h"
#include "../X-RPC/include/X-RPC/x_RPC.h"
#include <thread>

u8(*arm9Read)(u32 addr);
std::thread rpcThread;

void SupportedGens(msgpack_object* _, msgpack_packer* packer) {
	msgpack_pack_array(packer, 2);

	msgpack_pack_char(packer, 4);
	msgpack_pack_char(packer, 5);
}

void ReadMemory(msgpack_object* args, msgpack_packer* packer) {
	if (args->type != MSGPACK_OBJECT_ARRAY) {
		return;
	}

	if (args->via.array.size < 2) {
		return;
	}

	if (args->via.array.ptr[0].type != MSGPACK_OBJECT_POSITIVE_INTEGER
		|| args->via.array.ptr[1].type != MSGPACK_OBJECT_POSITIVE_INTEGER) {
		return;
	}

	auto address = args->via.array.ptr[0].via.u64;
	auto size = args->via.array.ptr[1].via.u64;

	msgpack_pack_array(packer, size);

	for (auto i = address; i < address + size; i++) {
		msgpack_pack_fix_uint8(packer, (char)arm9Read(i));
	}
}

void StartRPC(unsigned short port, u8(*arm9Read8)(u32 addr)) {
	arm9Read8 = arm9Read8;
	xRPC_Server_Stop();
	xRPC_Server_ClearCallbacks();

	xRPC_Server_RegisterCallBack("supportedGens", &SupportedGens);
	xRPC_Server_RegisterCallBack("readMemory", &ReadMemory);

	rpcThread = std::thread([port]() {
		xRPC_Server_Start(port, "127.0.0.1", 10);
		xRPC_Server_ClearCallbacks();
	});
}

void StopRPC() {
	xRPC_Server_Stop();
	if (rpcThread.joinable()) {
		rpcThread.join();
	}
	xRPC_Server_ClearCallbacks();
}
