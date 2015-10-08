#include "../stdafx.h"
#include "TinyNetwork.h"
#include "vld.h"
#include <process.h>

namespace TinyUI
{
	PVOID GetExtensionPtr(SOCKET socket, GUID guid)
	{
		LPVOID ps = NULL;
		DWORD dwBytes = 0;
		if (WSAIoctl(
			socket,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guid,
			sizeof(guid),
			&ps,
			sizeof(ps),
			&dwBytes,
			NULL,
			NULL) == SOCKET_ERROR)
		{
			return NULL;
		}
		return ps;
	}
}