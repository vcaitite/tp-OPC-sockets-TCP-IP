#ifndef OPC_CLIENT_H
#define OPC_CLIENT_H

/*
* C++ script to implement the OPC Client behavior for the UFMG discipline project:
* "Simulador do sistema de otimização do carregamento de vagões"
*
* This file was modified based on the 'SimpleOPCClient_v3.cpp' file created and also modified
* by the professor Luiz Themystokliz (based on the KEPWARE´s sample client code).
*
* The main modifications on this file are:
* - Created the method 'opcClient(void)' that is responsible for initializing the COM environment
* instantiating the Matrikon Simulation Server, adding the group on the OPC server, adding the items
* for each control variable used, starting the process that will read these items values asynchronously,
* also writing some variables on the OPC server when necessary.
* - Created a 'WriteItem(..)' method responsible for writing a given value for a defined item on the
* OPC server.
* - Created a'AddInitialItems(..)' method responsible for adding the necessary items on server for
* a given OPC group.
* - Modified the 'AddTheItem(..)' so it could be able to add items dynamically on server for a given
* OPC group.
*
* (by: João P. A. Ferreira and Vitor G. R. Caitité)
*
*/

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include "msg_treatment.h"
#include <thread>
#include <atlbase.h>    // required for using the "_T" macro
#include <iostream>
#include <ObjIdl.h>

#include "global_variables.h"
#include "opcda.h"
#include "opcerror.h"
#include "SOCAdviseSink.h"
#include "SOCDataCallback.h"
#include "SOCWrapperFunctions.h"

using namespace std;

#define OPC_SERVER_NAME L"Matrikon.OPC.Simulation.1"
#define VT VT_R4


//#define REMOTE_SERVER_NAME L"your_path"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

IOPCServer* InstantiateServer(wchar_t ServerName[]);
void AddTheGroup(IOPCServer* pIOPCServer, IOPCItemMgt*& pIOPCItemMgt,
	OPCHANDLE& hServerGroup);
void AddInitialItems(IOPCItemMgt* pIOPCItemMgt);
void AddTheItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE& hServerItem, int index);
void ReadItems(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem, VARIANT& varValue);
void RemoveItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE hServerItem);
void RemoveGroup(IOPCServer* pIOPCServer, OPCHANDLE hServerGroup);
void WriteItem(IUnknown* pGroupIUnknown, DWORD dwCount, OPCHANDLE hServerItem, VARIANT &varValue);
void __cdecl opcClient(void);
#endif