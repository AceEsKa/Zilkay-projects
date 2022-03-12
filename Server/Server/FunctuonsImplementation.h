#pragma once
#include "Declarations.h"
#include "CustomDataStructs.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define xmalloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s))
#define xfree(p)   HeapFree(GetProcessHeap(),0,(p))

const char* g_Port = DEFAULT_PORT;
BOOL g_bEndServer = FALSE;			// set to TRUE on CTRL-C
BOOL g_bRestart = TRUE;				// set to TRUE to CTRL-BRK
HANDLE g_hIOCP = INVALID_HANDLE_VALUE;
SOCKET g_sdListen = INVALID_SOCKET;
HANDLE g_ThreadHandles[MAX_WORKER_THREAD];
WSAEVENT g_hCleanupEvent[1];
PPER_SOCKET_CONTEXT g_pCtxtListenSocket = NULL;

PPER_SOCKET_CONTEXT ConnectedClients = NULL; // linked list of context info structures
											// maintained to allow the the cleanup 
											// handler to cleanly close all sockets and 
											// free resources.
CRITICAL_SECTION g_CriticalSection;

void SendData(HANDLE& WorkThreadContext, LPWSAOVERLAPPED& lpOverlapped, PPER_SOCKET_CONTEXT& lpPerSocketContext, PPER_SOCKET_CONTEXT& lpAcceptSocketContext, PPER_IO_CONTEXT& lpIOContext, std::atomic_bool& Logout);

//
// Create a socket with all the socket options we need, namely disable buffering
// and set linger.
//
SOCKET CreateSocket(void)
{
	int nRet = 0;
	int nZero = 0;
	SOCKET sdSocket = INVALID_SOCKET;

	sdSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sdSocket == INVALID_SOCKET) 
	{
		std::cout << "WSASocket(sdSocket) failed: " << WSAGetLastError() << std::endl;
		return(sdSocket);
	}

	//
	//
	//
	nZero = 0;
	nRet =	setsockopt(sdSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nZero, sizeof(nZero));
	//nRet = setsockopt(sdSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nZero, sizeof(nZero));
	if (nRet == SOCKET_ERROR)
	{
		std::cout << "setsockopt(SNDBUF) failed:" << WSAGetLastError() << std::endl;
		return(sdSocket);
	}

	//
	// Don't disable receive buffering. This will cause poor network
	// performance since if no receive is posted and no receive buffers,
	// the TCP stack will set the window size to zero and the peer will
	// no longer be allowed to send data.
	//

	// 
	// Do not set a linger value...especially don't set it to an abortive
	// close. If you set abortive close and there happens to be a bit of
	// data remaining to be transfered (or data that has not been 
	// acknowledged by the peer), the connection will be forcefully reset
	// and will lead to a loss of data (i.e. the peer won't get the last
	// bit of data). This is BAD. If you are worried about malicious
	// clients connecting and then not sending or receiving, the server
	// should maintain a timer on each connection. If after some point,
	// the server deems a connection is "stale" it can then set linger
	// to be abortive and close the connection.
	//

	/*
	LINGER lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	nRet = setsockopt(sdSocket, SOL_SOCKET, SO_LINGER,
					  (char *)&lingerStruct, sizeof(lingerStruct));
	if( nRet == SOCKET_ERROR ) {
		myprintf("setsockopt(SO_LINGER) failed: %d\n", WSAGetLastError());
		return(sdSocket);
	}
	*/

	return(sdSocket);
}


//
//  Create a listening socket, bind, and set up its listening backlog.
//
BOOL CreateListenSocket(void) 
{

	int nRet = 0;
	LINGER lingerStruct;
	struct addrinfo hints = { 0 };
	struct addrinfo* addrlocal = NULL;

	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;

	//
	// Resolve the interface
	//
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	if (getaddrinfo(NULL, g_Port, &hints, &addrlocal) != 0)
	{
		std::cout << "getaddrinfo() failed with error" << WSAGetLastError() << std::endl;
		return(FALSE);
	}

	if (addrlocal == NULL)
	{
		std::cout << "getaddrinfo() failed to resolve/convert the interface" << std::endl;
		return(FALSE);
	}

	g_sdListen = CreateSocket();
	if (g_sdListen == INVALID_SOCKET)
	{
		freeaddrinfo(addrlocal);
		return(FALSE);
	}
	//SO_REUSEADDR
	
	nRet = bind(g_sdListen, addrlocal->ai_addr, (int)addrlocal->ai_addrlen);
	if (nRet == SOCKET_ERROR)
	{
		std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
		freeaddrinfo(addrlocal);
		return(FALSE);
	}

	/*if (CreateIoCompletionPort((HANDLE)g_sdListen, g_hIOCP, (ULONG_PTR)0, 0) == NULL)
	{
		std::cout << "CreateIoCompletionPort() failed: " << WSAGetLastError() << std::endl;
	}*/
	freeaddrinfo(addrlocal);

	return(TRUE);
}

//
// Worker thread that handles all I/O requests on any socket handle added to the IOCP.
//
DWORD WINAPI WorkerThread(LPVOID WorkThreadContext) 
{

	HANDLE hIOCP = (HANDLE)WorkThreadContext;
	BOOL bSuccess = FALSE;
	BOOL AllSend = FALSE;
	int nRet = 0;
	LPWSAOVERLAPPED lpOverlapped = NULL;
	PPER_SOCKET_CONTEXT lpPerSocketContext = NULL;
	PPER_SOCKET_CONTEXT lpAcceptSocketContext = NULL;
	PPER_IO_CONTEXT lpIOContext = NULL;
	WSABUF buffRecv;
	WSABUF buffSend;
	DWORD dwRecvNumBytes = 0;
	DWORD dwSendNumBytes = 0;
	DWORD dwFlags = 0;
	DWORD dwIoSize = 0;
	DWORD Flags2 = MSG_PEEK;
	SOCKET tmp = INVALID_SOCKET;
	HRESULT hRet;
	//double seconds_since_start;
	std::atomic_bool ConnectionClosed = false;

	while (TRUE)
	{
		std::cout << "tu" << std::endl;
		//allocate
		lpIOContext = (PPER_IO_CONTEXT)lpOverlapped; // ?

		/*nRet = WSARecv(tmp,
			&buffRecv, 1,
			&dwRecvNumBytes,
			&Flags2,
			&lpAcceptSocketContext->pIOContext->Overlapped, NULL);*/
		//
		// continually loop to service io completion packets
		//
		bSuccess = GetQueuedCompletionStatus(
											hIOCP,
											&dwIoSize,
											(PDWORD_PTR)&lpPerSocketContext,
											(LPOVERLAPPED*)&lpOverlapped,
											INFINITE
											);
		if (!bSuccess)
		{
			std::cout << "GetQueuedCompletionStatus() failed::" << GetLastError();
		}

		//
		// 
		// 
		// 
		// 
		// 
		//failing
		// 
		// 
		// 
		// 
		// 
		// 
		// 
		// 


		std::cout << "User succesfuly connected" << std::endl;
		if (lpPerSocketContext->pIOContext->SocketUser == INVALID_SOCKET)
		{
			std::cout << "wtf" << std::endl;
		}
		lpAcceptSocketContext = UpdateCompletionPort(
			lpPerSocketContext->pIOContext->SocketUser,
			ClientIoAccept, TRUE);
		if (lpAcceptSocketContext == NULL)
		{

			//
			//just warn user here.
			//
			std::cout << "failed to update accept socket to IOCP" << std::endl;
			WSASetEvent(g_hCleanupEvent[0]);
			return(0);
		}

		if (dwIoSize)
		{
			//lpAcceptSocketContext->pIOContext->IOOperation = ClientIoWrite;
			lpAcceptSocketContext->pIOContext->nTotalBytes = dwIoSize;
			lpAcceptSocketContext->pIOContext->nSentBytes = 0;
			lpAcceptSocketContext->pIOContext->wsabuf.len = dwIoSize;
			hRet = StringCbCopyN(lpAcceptSocketContext->pIOContext->Buffer,
				MAX_BUFF_SIZE,
				lpPerSocketContext->pIOContext->Buffer,
				sizeof(lpPerSocketContext->pIOContext->Buffer)
			);
			lpAcceptSocketContext->pIOContext->wsabuf.buf = lpAcceptSocketContext->pIOContext->Buffer;

			//add message to the Global buffer
			//
			NewMessage = true;
			/*GlobalBuf.len = dwIoSize;
			GlobalBuf.buf = lpAcceptSocketContext->pIOContext->Buffer;
			std::cout << GlobalBuf.buf << std::endl;*/
			//To Implement: Send last 200 messages
			nRet = WSASendTo(
				lpAcceptSocketContext->pIOContext->SocketUser,
				&lpAcceptSocketContext->pIOContext->wsabuf, 1,
				&dwSendNumBytes,
				0,
				lpAcceptSocketContext->pIOContext->lpFrom,
				int(lpAcceptSocketContext->pIOContext->lpFromlen),
				&(lpAcceptSocketContext->pIOContext->Overlapped), NULL);
			if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
			{
				std::cout << "WSASendTo() failed: " << WSAGetLastError() << std::endl;
				CloseClient(lpAcceptSocketContext, FALSE);
				ConnectionClosed = true;
			}
		}//if(lpIOContext->IOOperation==ClientIoAccept)
		else
		{

			//
			// AcceptEx completes but doesn't read any data so we need to post
			// an outstanding overlapped read.
			//
			//lpAcceptSocketContext->pIOContext->IOOperation = ClientIoRead;
			dwRecvNumBytes = 0;
			dwFlags = 0;
			buffRecv.buf = lpAcceptSocketContext->pIOContext->Buffer,
				buffRecv.len = MAX_BUFF_SIZE;
			nRet = WSARecvFrom(
				lpAcceptSocketContext->Socket,
				&buffRecv, 1,
				&dwRecvNumBytes,
				&dwFlags,
				lpAcceptSocketContext->pIOContext->lpFrom,
				lpAcceptSocketContext->pIOContext->lpFromlen,
				&lpAcceptSocketContext->pIOContext->Overlapped, NULL);
			GlobalBuf = buffRecv;
			GlobalBuf.len = dwRecvNumBytes;
			if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
			{
				std::cout << "WSARecvFrom() failed:" << WSAGetLastError() << std::endl;
				CloseClient(lpAcceptSocketContext, FALSE);
				ConnectionClosed = true;
			}
			std::cout << "recved: " << GlobalBuf.buf << std::endl;
			//
			//Time to post another outstanding AcceptEx
			//
		}
		if (!CreateAcceptSocket(FALSE))
		{
			std::cout << "Please shut down and reboot the server." << std::endl;
			WSASetEvent(g_hCleanupEvent[0]);
			return(0);
		}
		/*switch (lpIOContext->IOOperation)//== ClientIoAccept)
		{
		case ClientIoAccept:

			break;

			case ClientIoRead:
				//
				// a read operation has completed, post a write operation to echo the
				// data back to the client using the same data buffer.
				//
				lpIOContext->IOOperation = ClientIoWrite;
				lpIOContext->nTotalBytes = dwIoSize;
				lpIOContext->nSentBytes = 0;
				lpIOContext->wsabuf.len = dwIoSize;
				dwFlags = 0;
				std::cout << lpPerSocketContext->Socket << std::endl;
				nRet = WSASendTo(
					lpAcceptSocketContext->pIOContext->SocketUser,
					&lpAcceptSocketContext->pIOContext->wsabuf, 1,
					&dwSendNumBytes,
					0,
					lpAcceptSocketContext->pIOContext->lpFrom,
					int(lpAcceptSocketContext->pIOContext->lpFromlen),
					&(lpAcceptSocketContext->pIOContext->Overlapped), NULL);
				if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
					std::cout << "WSASendTo() failed: " << WSAGetLastError() << std::endl;
					CloseClient(lpPerSocketContext, FALSE);
				}
				break;

			case ClientIoWrite:

				//
				// a write operation has completed, determine if all the data intended to be
				// sent actually was sent.
				//
				lpIOContext->IOOperation = ClientIoWrite;
				lpIOContext->nSentBytes += dwIoSize;
				dwFlags = 0;
				if (lpIOContext->nSentBytes < lpIOContext->nTotalBytes) {

					//
					// the previous write operation didn't send all the data,
					// post another send to complete the operation
					//
					buffSend.buf = lpIOContext->Buffer + lpIOContext->nSentBytes;
					buffSend.len = lpIOContext->nTotalBytes - lpIOContext->nSentBytes;
					nRet = WSASendTo(
						lpAcceptSocketContext->pIOContext->SocketUser,
						&lpAcceptSocketContext->pIOContext->wsabuf, 1,
						&dwSendNumBytes,
						0,
						lpAcceptSocketContext->pIOContext->lpFrom,
						int(lpAcceptSocketContext->pIOContext->lpFromlen),
						&(lpAcceptSocketContext->pIOContext->Overlapped), NULL);
					if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
						std::cout << "WSASendTo() failed: " << WSAGetLastError() << std::endl;
						CloseClient(lpPerSocketContext, FALSE);
					}
				}
				else {

					//
					// previous write operation completed for this socket, post another recv
					//
					lpIOContext->IOOperation = ClientIoRead;
					dwRecvNumBytes = 0;
					dwFlags = 0;
					buffRecv.buf = lpIOContext->Buffer,
						buffRecv.len = MAX_BUFF_SIZE;
					nRet = WSARecvFrom(
						lpAcceptSocketContext->Socket,
						&buffRecv, 1,
						&dwRecvNumBytes,
						&dwFlags,
						lpAcceptSocketContext->pIOContext->lpFrom,
						lpAcceptSocketContext->pIOContext->lpFromlen,
						&lpAcceptSocketContext->pIOContext->Overlapped, NULL);
					if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
						std::cout << "WSARecvFrom() failed: " << WSAGetLastError() << std::endl;
						CloseClient(lpPerSocketContext, FALSE);
					}
				}
				break;
		}


		//send
/*

		if (lpIOContext->IOOperation == ClientIoWrite && !ConnectionClosed)
		{
			std::cout << "tu" << std::endl;
			//
			// a write operation has completed, determine if all the data intended to be
			// sent actually was sent.
			//
			lpIOContext->IOOperation = ClientIoWrite;
			lpIOContext->nSentBytes += dwIoSize;
			dwFlags = 0;
			if (lpIOContext->nSentBytes < lpIOContext->nTotalBytes)
			{

				//
				// the previous write operation didn't send all the data,
				// post another send to complete the operation
				//
				buffSend.buf = lpIOContext->Buffer + lpIOContext->nSentBytes;
				buffSend.len = lpIOContext->nTotalBytes - lpIOContext->nSentBytes;
				nRet = WSASendTo(
					lpPerSocketContext->Socket,
					&buffSend, 1, &dwSendNumBytes,
					dwFlags,
					&(lpIOContext->Overlapped), NULL);
				if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
				{
					std::cout << "WSASendTo() failed: " << WSAGetLastError() << std::endl;
					CloseClient(lpPerSocketContext, FALSE);
				}
			}
			else
			{
				//
				// previous write operation completed for this socket, post another recv
				//
				std::thread SendThread(SendData, std::ref(WorkThreadContext),std::ref(lpOverlapped), std::ref(lpPerSocketContext), std::ref(lpAcceptSocketContext), std::ref(lpIOContext), std::ref(ConnectionClosed));
				std::cout << lpPerSocketContext->Socket << std::endl;
				while (true && !ConnectionClosed)
				{
					lpIOContext->IOOperation = ClientIoRead;
					dwRecvNumBytes = 0;
					dwFlags = 0;
					buffRecv.buf = lpIOContext->Buffer,
						buffRecv.len = MAX_BUFF_SIZE;
					nRet = WSARecvFrom(
						lpPerSocketContext->Socket,
						&buffRecv, 1, &dwRecvNumBytes,
						&dwFlags,
						&lpIOContext->Overlapped, NULL);
					if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
					{
						std::cout << "WSARecvFrom() failed:" << WSAGetLastError() << std::endl;
						CloseClient(lpPerSocketContext, FALSE);
						std::atomic_bool Logout = false;
						break;
					}
					NewMessage = true;
					GlobalBuf = buffRecv;
					GlobalBuf.len = dwRecvNumBytes;
				}
				SendThread.join();
			}
		}
		if (!ConnectionClosed)
		{
			CloseClient(lpPerSocketContext, FALSE);
		}
		ConnectionClosed = false;*/
	} //while
	return(0);
}


//
//  Allocate a context structures for the socket and add the socket to the IOCP.  
//  Additionally, add the context structure to the global list of context structures.
//
PPER_SOCKET_CONTEXT UpdateCompletionPort(SOCKET sd, IO_OPERATION ClientIo, BOOL bAddToList) 
{

	PPER_SOCKET_CONTEXT lpPerSocketContext;

	lpPerSocketContext = CtxtAllocate(sd, ClientIo);
	if (lpPerSocketContext == NULL)
	{
		return(NULL);
	}

	g_hIOCP = CreateIoCompletionPort((HANDLE)sd, g_hIOCP, (DWORD_PTR)lpPerSocketContext, 0);
	if (g_hIOCP == NULL)
	{
		std::cout << "CreateIoCompletionPort() failed: " << GetLastError() << std::endl;
		if (lpPerSocketContext->pIOContext)
		{
			xfree(lpPerSocketContext->pIOContext);
		}
		xfree(lpPerSocketContext);
		return(NULL);
	}

	//
	//The listening socket context (bAddToList is FALSE) is not added to the list.
	//All other socket contexts are added to the list.
	//
	if (bAddToList) 
	{
		CtxtListAddTo(lpPerSocketContext); 
	}

	return(lpPerSocketContext);
}


//
//  Close down a connection with a client.  This involves closing the socket (when 
//  initiated as a result of a CTRL-C the socket closure is not graceful).  Additionally, 
//  any context data associated with that socket is free'd.
//
VOID CloseClient(PPER_SOCKET_CONTEXT lpPerSocketContext, BOOL bGraceful) 
{

	if (lpPerSocketContext) 
	{
		if (!bGraceful) 
		{

			//
			// force the subsequent closesocket to be abortative.
			//
			LINGER  lingerStruct;

			lingerStruct.l_onoff = 1;
			lingerStruct.l_linger = 0;
			setsockopt(lpPerSocketContext->Socket, SOL_SOCKET, SO_LINGER,
				(char*)&lingerStruct, sizeof(lingerStruct));
		}
		if (lpPerSocketContext->pIOContext->SocketUser != INVALID_SOCKET) 
		{
			closesocket(lpPerSocketContext->pIOContext->SocketUser);
			lpPerSocketContext->pIOContext->SocketUser = INVALID_SOCKET;
		};

		closesocket(lpPerSocketContext->Socket);
		lpPerSocketContext->Socket = INVALID_SOCKET;
		CtxtListDeleteFrom(lpPerSocketContext);
		lpPerSocketContext = NULL;
	}
	else 
	{
		std::cout << "CloseClient: lpPerSocketContext is NULL" << std::endl;
	}


	return;
}

//
// Allocate a socket context for the new connection.  
//
PPER_SOCKET_CONTEXT CtxtAllocate(SOCKET sd, IO_OPERATION ClientIO)
{

	PPER_SOCKET_CONTEXT lpPerSocketContext;

	lpPerSocketContext = (PPER_SOCKET_CONTEXT)xmalloc(sizeof(PER_SOCKET_CONTEXT));
	if (lpPerSocketContext) 
	{
		lpPerSocketContext->pIOContext = (PPER_IO_CONTEXT)xmalloc(sizeof(PER_IO_CONTEXT));
		if (lpPerSocketContext->pIOContext) {
			lpPerSocketContext->Socket = sd;
			lpPerSocketContext->pCtxtBack = NULL;
			lpPerSocketContext->pCtxtForward = NULL;

			lpPerSocketContext->pIOContext->Overlapped.Internal = 0;
			lpPerSocketContext->pIOContext->Overlapped.InternalHigh = 0;
			lpPerSocketContext->pIOContext->Overlapped.Offset = 0;
			lpPerSocketContext->pIOContext->Overlapped.OffsetHigh = 0;
			lpPerSocketContext->pIOContext->Overlapped.hEvent = NULL;
			//lpPerSocketContext->pIOContext->IOOperation = ClientIO;
			lpPerSocketContext->pIOContext->pIOContextForward = NULL;
			lpPerSocketContext->pIOContext->nTotalBytes = 0;
			lpPerSocketContext->pIOContext->nSentBytes = 0;
			lpPerSocketContext->pIOContext->wsabuf.buf = lpPerSocketContext->pIOContext->Buffer;
			lpPerSocketContext->pIOContext->wsabuf.len = sizeof(lpPerSocketContext->pIOContext->Buffer);
			lpPerSocketContext->pIOContext->SocketUser = INVALID_SOCKET;
			lpPerSocketContext->pIOContext->lpFrom = NULL;
			lpPerSocketContext->pIOContext->lpFromlen = 0;


			ZeroMemory(lpPerSocketContext->pIOContext->wsabuf.buf, lpPerSocketContext->pIOContext->wsabuf.len);
		}
		else 
		{
			xfree(lpPerSocketContext);
			std::cout << "HeapAlloc() PER_IO_CONTEXT failed:" << GetLastError() << std::endl;
			return NULL;
		}

	}
	else 
	{
		std::cout << "HeapAlloc() PER_SOCKET_CONTEXT failed:" << GetLastError() << std::endl;
		return NULL;
	}

	return(lpPerSocketContext);
}

//
//  Add a client connection context structure to the global list of context structures.
//
VOID CtxtListAddTo(PPER_SOCKET_CONTEXT lpPerSocketContext) 
{

	PPER_SOCKET_CONTEXT pTemp;
	//first node 
	if (!ActiveUsers)
	{

		//
		// add the first node to the linked list
		//
		//std::cout << "Are u kidding me ?!" << std::endl;
		std::cout << "First User" << std::endl;
		lpPerSocketContext->pCtxtBack = NULL;
		lpPerSocketContext->pCtxtForward = NULL;
		ConnectedClients = lpPerSocketContext;
		ActiveUsers = true;
	}
	else 
	{

		//
		// add node to head of list
		//
		pTemp = ConnectedClients;

		ConnectedClients = lpPerSocketContext;
		lpPerSocketContext->pCtxtBack = pTemp;
		lpPerSocketContext->pCtxtForward = NULL;

		pTemp->pCtxtForward = lpPerSocketContext;
	}


	return;
}

//
//  Remove a client context structure from the global list of context structures.
//

VOID CtxtListDeleteFrom(PPER_SOCKET_CONTEXT lpPerSocketContext) 
{

	PPER_SOCKET_CONTEXT pBack;
	PPER_SOCKET_CONTEXT pForward;
	PPER_IO_CONTEXT     pNextIO = NULL;
	PPER_IO_CONTEXT     pTempIO = NULL;

	if (lpPerSocketContext) 
	{
		pBack = lpPerSocketContext->pCtxtBack;
		pForward = lpPerSocketContext->pCtxtForward;

		if (pBack == NULL && pForward == NULL) 
		{

			//
			// This is the only node in the list to delete
			//
			ConnectedClients = NULL;
			ActiveUsers = false;
		}
		else if (pBack == NULL && pForward != NULL) 
		{

			//
			// This is the start node in the list to delete
			//
			pForward->pCtxtBack = NULL;
			ConnectedClients = pForward;
		}
		else if (pBack != NULL && pForward == NULL)
		{

			//
			// This is the end node in the list to delete
			//
			pBack->pCtxtForward = NULL;
		}
		else if (pBack && pForward) {

			//
			// Neither start node nor end node in the list
			//
			pBack->pCtxtForward = pForward;
			pForward->pCtxtBack = pBack;
		}

		//
		// Free all i/o context structures per socket
		//
		pTempIO = (PPER_IO_CONTEXT)(lpPerSocketContext->pIOContext);
		do 
		{
			pNextIO = (PPER_IO_CONTEXT)(pTempIO->pIOContextForward);
			if (pTempIO) 
			{

				//
				//The overlapped structure is safe to free when only the posted i/o has
				//completed. Here we only need to test those posted but not yet received 
				//by PQCS in the shutdown process.
				//
				xfree(pTempIO);
				pTempIO = NULL;
			}
			pTempIO = pNextIO;
		} while (pNextIO);

		xfree(lpPerSocketContext);
		lpPerSocketContext = NULL;
	}
	else
	{
		std::cout << "CtxtListDeleteFrom: lpPerSocketContext is NULL" << std::endl;
	}


	return;
}

BOOL CreateAcceptSocket(BOOL fUpdateIOCP)
{

	int nRet = 0;
	DWORD dwRecvNumBytes = 0;
	DWORD bytes = 0;

	//
	// GUID to Microsoft specific extensions
	//
	GUID acceptex_guid = WSAID_ACCEPTEX;

	//
	//The context for listening socket uses the SockAccept member to store the
	//socket for client connection. 
	//
	if (fUpdateIOCP) 
	{
		g_pCtxtListenSocket = UpdateCompletionPort(g_sdListen, ClientIoAccept, FALSE);
		if (g_pCtxtListenSocket == NULL) 
		{
			std::cout << "failed to update listen socket to IOCP" << std::endl;
			return(FALSE);
		}

		// Load the AcceptEx extension function from the provider for this socket
		nRet = WSAIoctl(
			g_sdListen,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&acceptex_guid,
			sizeof(acceptex_guid),
			&g_pCtxtListenSocket->fnAcceptEx,
			sizeof(g_pCtxtListenSocket->fnAcceptEx),
			&bytes,
			NULL,
			NULL
		);
		if (nRet == SOCKET_ERROR)
		{
			std::cout << "failed to load AcceptEx:" << WSAGetLastError() << std::endl;
			return (FALSE);
		}
	}

	g_pCtxtListenSocket->pIOContext->SocketUser = CreateSocket();
	if (g_pCtxtListenSocket->pIOContext->SocketUser == INVALID_SOCKET) 
	{
		std::cout << "failed to create new accept socket" << std::endl;
		return(FALSE);
	}

	//
	// pay close attention to these parameters and buffer lengths
	//
	nRet = g_pCtxtListenSocket->fnAcceptEx(g_sdListen, g_pCtxtListenSocket->pIOContext->SocketUser,
		(LPVOID)(g_pCtxtListenSocket->pIOContext->Buffer),
		MAX_BUFF_SIZE - (2 * (sizeof(SOCKADDR_STORAGE) + 16)),
		sizeof(SOCKADDR_STORAGE) + 16, sizeof(SOCKADDR_STORAGE) + 16,
		&dwRecvNumBytes,
		(LPOVERLAPPED) & (g_pCtxtListenSocket->pIOContext->Overlapped));
	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		std::cout << "AcceptEx() failed: " << WSAGetLastError() << std::endl;
		return(FALSE);
	}

	return(TRUE);
}

//
//  Free all context structure in the global list of context structures.
//
VOID CtxtListFree() 
{
	PPER_SOCKET_CONTEXT pTemp1, pTemp2;


	pTemp1 = ConnectedClients;
	while (pTemp1) 
	{
		pTemp2 = pTemp1->pCtxtBack;
		CloseClient(pTemp1, FALSE);
		pTemp1 = pTemp2;
	}


	return;
}

/*void SendData(HANDLE& WorkThreadContext, LPWSAOVERLAPPED& lpOverlapped, PPER_SOCKET_CONTEXT& lpPerSocketContext, PPER_SOCKET_CONTEXT& lpAcceptSocketContext, PPER_IO_CONTEXT& lpIOContext, std::atomic_bool& Logout)
{
	HANDLE hIOCP = WorkThreadContext;
	BOOL bSuccess = FALSE;
	BOOL AllSend = FALSE;
	int nRet = 0;
	WSABUF buffSend;
	DWORD dwSendNumBytes = 0;
	DWORD dwFlags = 0;
	DWORD dwIoSize = 0;
	//HRESULT hRet;

	std::list<std::string> MessageQueue;
	std::cout << lpPerSocketContext->Socket << std::endl;
	while (TRUE)
	{
		//
		// continually loop to service io completion packets
		//
		bSuccess = GetQueuedCompletionStatus(
			hIOCP,
			&dwIoSize,
			(PDWORD_PTR)&lpPerSocketContext,
			(LPOVERLAPPED*)&lpOverlapped,
			INFINITE
		);
		if (!bSuccess)
		{
			std::cout << "GetQueuedCompletionStatus() failed::" << GetLastError();
		}

		lpIOContext = (PPER_IO_CONTEXT)lpOverlapped;


		if (lpIOContext->IOOperation != ClientIoAccept)
		{
			if (!bSuccess || (bSuccess && (0 == dwIoSize)))
			{

				//
				// client connection dropped, continue to service remaining (and possibly 
				// new) client connections
				//
				CloseClient(lpPerSocketContext, FALSE);
				MessageQueue.clear();
				MessageQueue.clear();
				return;
			}
			else
			{
				//
				//if there was a message sent and this is not the first time this user log in 
				// then add the message to the message quee 
				//if users net connection is bad message quee will build up and hopefully eventually all messages will
				//be delivered to the user
				//
				if (NewMessage)
				{
					std::string tmp = GlobalBuf.buf;
					MessageQueue.push_back(tmp);
				}
			}
		}
		if (NewMessage /*&& MessageQueue.empty()*///)
		/*{
			NewMessage = false;
			//lpIOContext->IOOperation = ClientIoWrite;
			lpIOContext->nTotalBytes = GlobalBuf.len;
			lpIOContext->nSentBytes = 0;
			lpIOContext->wsabuf.len = GlobalBuf.len;
			buffSend.buf = GlobalBuf.buf;
			buffSend.len = GlobalBuf.len;
			std::cout << GlobalBuf.buf << std::endl;
			dwFlags = 0;
			nRet = WSASendTo(
				lpAcceptSocketContext->pIOContext->SocketUser,
				&lpAcceptSocketContext->pIOContext->wsabuf, 1,
				&dwSendNumBytes,
				0,
				lpAcceptSocketContext->pIOContext->lpFrom,
				int(lpAcceptSocketContext->pIOContext->lpFromlen),
				&(lpAcceptSocketContext->pIOContext->Overlapped), NULL);
			if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
			{
				std::cout << "WSASendTo() failed:" << WSAGetLastError() << std::endl;
				CloseClient(lpPerSocketContext, FALSE);
				MessageQueue.clear();
				Logout=true;
				return;
			}
			//MessageQueue.pop_front();
			lpIOContext->nSentBytes += dwIoSize;
			while (lpIOContext->nSentBytes < lpIOContext->nTotalBytes)
			{

				//lpIOContext->IOOperation = ClientIoWrite;
				dwFlags = 0;
				//
				// the previous write operation didn't send all the data,
				// post another send to complete the operation
				//
				buffSend.buf = lpIOContext->Buffer + lpIOContext->nSentBytes;
				buffSend.len = lpIOContext->nTotalBytes - lpIOContext->nSentBytes;
				nRet = WSASendTo(
					lpAcceptSocketContext->pIOContext->SocketUser,
					&lpAcceptSocketContext->pIOContext->wsabuf, 1,
					&dwSendNumBytes,
					0,
					lpAcceptSocketContext->pIOContext->lpFrom,
					int(lpAcceptSocketContext->pIOContext->lpFromlen),
					&(lpAcceptSocketContext->pIOContext->Overlapped), NULL);
				if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
				{
					std::cout << "WSASendTo() failed: " << WSAGetLastError() << std::endl;
					CloseClient(lpPerSocketContext, FALSE);
					MessageQueue.clear();
					Logout = true;
					std::cout << "terminating conection with user" << std::endl;
					return;
				}
			}//while
		}*/
		/*else if(!MessageQueue.empty())
		{
			
			lpIOContext->IOOperation = ClientIoWrite;
			lpIOContext->nTotalBytes = strlen(MessageQueue.front().c_str());
			lpIOContext->nSentBytes = 0;
			lpIOContext->wsabuf.len = strlen(MessageQueue.front().c_str());
			buffSend.buf = essageQueue.front().c_str();
			buffSend.len = strlen(MessageQueue.front().c_str());
			dwFlags = 0;
			nRet = WSASendTo(
				lpPerSocketContext->Socket,
				&GlobalBuf, 1, &dwSendNumBytes,
				dwFlags,
				&(lpIOContext->Overlapped), NULL);
			if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
			{
				std::cout << "WSASendTo() failed:" << WSAGetLastError() << std::endl;
				CloseClient(lpPerSocketContext, FALSE);
				MessageQueue.clear();
			}
			lpIOContext->nSentBytes += dwIoSize;
			while (lpIOContext->nSentBytes < lpIOContext->nTotalBytes)
			{

				//lpIOContext->IOOperation = ClientIoWrite;
				dwFlags = 0;
				//
				// the previous write operation didn't send all the data,
				// post another send to complete the operation
				//
				buffSend.buf = lpIOContext->Buffer + lpIOContext->nSentBytes;
				buffSend.len = lpIOContext->nTotalBytes - lpIOContext->nSentBytes;
				nRet = WSASendTo(
					lpPerSocketContext->Socket,
					&buffSend, 1, &dwSendNumBytes,
					dwFlags,
					&(lpIOContext->Overlapped), NULL);
				if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
				{
					std::cout << "WSASendTo() failed: " << WSAGetLastError() << std::endl;
					CloseClient(lpPerSocketContext, FALSE);
				}
				else if (g_bVerbose) 
				{
					std::cout << "WorkerThread " << GetCurrentThreadId() << ": " << "Socket(" << lpPerSocketContext->Socket << "Send partially completed( " << dwIoSize << " bytes), Recv posted" << std::endl;
				}
			}
			MessageQueue.pop_front();
		}*/
	/*}//while
}

bool RecvOperation(void)
{
	WSAOVERLAPPED               Overlapped;
	SecureZeroMemory((PVOID)&Overlapped, sizeof(WSAOVERLAPPED));
	Overlapped.hEvent = WSACreateEvent();
	if (Overlapped.hEvent == NULL) {
		wprintf(L"WSACreateEvent failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	WSABUF                         lpBuffers;
	char RecvBuf[1024];
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	DWORD                            lpNumberOfBytesRecvd=0;
	DWORD                            lpFlags=0;
	int BufLen = 1024;
	lpBuffers.len = BufLen;
	lpBuffers.buf = RecvBuf;


	int iResult=WSARecvFrom(g_sdListen, &lpBuffers, 1, &lpNumberOfBytesRecvd, &lpFlags, (SOCKADDR*)&SenderAddr, &SenderAddrSize,&Overlapped, NULL);
	if (iResult !=0 )
	{
		std::cout << "WSArecvFrom failed with error :" << WSAGetLastError() << std::endl;
		return false;
		
	}
	std::cout << RecvBuf << std::endl;
	return true;
}

bool Contains(SOCKET user)
{
	PPER_SOCKET_CONTEXT tmp=ConnectedClients;
	while (tmp)
	{
		if (tmp->pIOContext->SocketUser==user)
		{
			return true;
		}
		tmp->pCtxtForward;
	}
	return false;
}*/

bool recv()
{
	WSAOVERLAPPED               Overlapped;
	SecureZeroMemory((PVOID)&Overlapped, sizeof(WSAOVERLAPPED));
	Overlapped.hEvent = WSACreateEvent();
	WSABUF                         lpBuffers;
	char RecvBuf[1024];
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	DWORD                            lpNumberOfBytesRecvd = 0;
	DWORD                            lpFlags = 0;
	int BufLen = 1024;
	lpBuffers.len = BufLen;
	lpBuffers.buf = RecvBuf;


	int iResult = WSARecvFrom(g_sdListen, &lpBuffers, 1, &lpNumberOfBytesRecvd, &lpFlags, (SOCKADDR*)&SenderAddr, &SenderAddrSize, &Overlapped, NULL);
	if (iResult != 0)
	{
		std::cout << "WSArecvFrom failed with error :" << WSAGetLastError() << std::endl;
		//return false;

	}
	std::cout << RecvBuf << std::endl;

	nRet = g_pCtxtListenSocket->fnAcceptEx(g_sdListen, g_pCtxtListenSocket->pIOContext->SocketAccept,
		(LPVOID)(g_pCtxtListenSocket->pIOContext->Buffer),
		MAX_BUFF_SIZE - (2 * (sizeof(SOCKADDR_STORAGE) + 16)),
		sizeof(SOCKADDR_STORAGE) + 16, sizeof(SOCKADDR_STORAGE) + 16,
		&dwRecvNumBytes,
		(LPOVERLAPPED) & (g_pCtxtListenSocket->pIOContext->Overlapped));
}