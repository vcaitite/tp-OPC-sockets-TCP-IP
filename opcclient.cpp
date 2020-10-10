#include "opcclient.h"

/* 
* C++ class to implement the OPC Client behavior for the UFMG discipline project:
* "Simulador do sistema de otimiza��o do carregamento de vag�es"
* 
* This file was modified based on the 'SimpleOPCClient_v3.cpp' file created and also modified 
* by the professor Luiz Themystokliz (based on the KEPWARE�s sample client code).
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
* (by: Jo�o P. A. Ferreira and Vitor G. R. Caitit�)
* 
*/

// The OPC DA Spec requires that some constants be registered in order to use
// them. The one below refers to the OPC DA 1.0 IDataObject interface.
UINT OPC_DATA_TIME = RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
OPCHANDLE H_ITEMS_READ_HANDLE[ITEMS_QUANTITY] = { NULL, NULL, NULL, NULL, NULL, NULL };
IOPCServer* pIOPCServer = NULL;   //pointer to IOPServer interface
IOPCItemMgt* pIOPCItemMgt = NULL; //pointer to IOPCItemMgt interface
OPCHANDLE hServerGroup; // server handle to the group


void __cdecl opcClient(void) {
		int i;
		char buf[100];
		int bRet;
		MSG msg;

		// Have to be done before using microsoft COM library:
		printf("[OPCCLIENT] Initializing the COM environment...\n");
		CoInitialize(NULL);

		// Let's instantiante the IOPCServer interface and get a pointer of it:
		printf("[OPCCLIENT] Instantiating the MATRIKON OPC Server for Simulation...\n");
		pIOPCServer = InstantiateServer(OPC_SERVER_NAME);

		// Add the OPC group the OPC server and get an handle to the IOPCItemMgt
		//interface:
		printf("[OPCCLIENT] Adding a group in the INACTIVE state for the moment...\n");
		AddTheGroup(pIOPCServer, pIOPCItemMgt, hServerGroup);

		// Add the OPC items. First we have to convert from wchar_t* to char*
		// in order to print the item name in the console.
		AddInitialItems(pIOPCItemMgt);

		// Establish a callback asynchronous read by means of the IOPCDaraCallback
		// (OPC DA 2.0) method. We first instantiate a new SOCDataCallback object and
		// adjusts its reference count, and then call a wrapper function to
		// setup the callback.
		IConnectionPoint* pIConnectionPoint = NULL; //pointer to IConnectionPoint Interface
		DWORD dwCookie = 0;
		SOCDataCallback* pSOCDataCallback = new SOCDataCallback();
		pSOCDataCallback->AddRef();

		printf("[OPCCLIENT] Setting up the IConnectionPoint callback connection...\n");
		SetDataCallback(pIOPCItemMgt, pSOCDataCallback, pIConnectionPoint, &dwCookie);

		// Change the group to the ACTIVE state so that we can receive the
		// server�s callback notification
		printf("[OPCCLIENT] Changing the group state to ACTIVE...\n");
		SetGroupActive(pIOPCItemMgt);

		// Enter in the process reading items loop	
		do {
			printf("[OPCCLIENT] Reading items values asynchrounously\n");
			bRet = GetMessage(&msg, NULL, 0, 0);
			if (!bRet) {
				printf("[OPCCLIENT] Failed to get windows message! Error code = %d\n", GetLastError());
				exit(0);
			}
			TranslateMessage(&msg); // This call is not really needed ...
			DispatchMessage(&msg);

			// Check if should write the variable on the OPC server
			if (SHOULD_WRITE) {
				//Synchronous read of the device�s item value.
				VARIANT varValue; //to store the read value

				// Define the int1 value to write
				varValue.intVal = loadingParameters.openTime;
				varValue.vt = VT_I1;
				WriteItem(pIOPCItemMgt, 1, H_ITEMS_READ_HANDLE[4], varValue);
				printf("[OPCCLIENT] WRITE ITEM %i: Value = %i", 4, varValue.intVal);

				// Define the real4 value to write
				varValue.fltVal = loadingParameters.oreQuantity;
				varValue.vt = VT_R4;
				WriteItem(pIOPCItemMgt, 1, H_ITEMS_READ_HANDLE[5], varValue);
				printf("[OPCCLIENT] WRITE ITEM %i: Value = %i", 5, varValue.fltVal);

				SHOULD_WRITE = false;
			}
		} while (1);

		// Cancel the callback and release its reference
		printf("[OPCCLIENT] Cancelling the IOPCDataCallback notifications...\n");
		CancelDataCallback(pIConnectionPoint, dwCookie);
		//pIConnectionPoint->Release();
		pSOCDataCallback->Release();

		// Remove the OPC item:
		for (i = 0; i < ITEMS_QUANTITY; i++) {
			printf("[OPCCLIENT] Removing the OPC item...\n");
			RemoveItem(pIOPCItemMgt, H_ITEMS_READ_HANDLE[i]);
		}

		// Remove the OPC group:
		printf("[OPCCLIENT] Removing the OPC group object...\n");
		pIOPCItemMgt->Release();
		RemoveGroup(pIOPCServer, hServerGroup);

		// release the interface references:
		printf("[OPCCLIENT] Removing the OPC server object...\n");
		pIOPCServer->Release();

		//close the COM library:
		printf("[OPCCLIENT] Releasing the COM environment...\n");
		CoUninitialize();
}

void WriteItem(IUnknown* pGroupIUnknown, DWORD dwCount, OPCHANDLE hServerItem, VARIANT &varValue)
{
	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	pGroupIUnknown->QueryInterface(__uuidof(pIOPCSyncIO), (void**)&pIOPCSyncIO);

	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Write(dwCount, &hServerItem, &varValue, &pErrors);
	_ASSERT(!hr);

	CoTaskMemFree(pErrors);
	pErrors = NULL;

	// release the reference to the IOPCSyncIO interface:
	pIOPCSyncIO->Release();
}

void AddInitialItems(IOPCItemMgt* pIOPCItemMgt) {
	int i;
	char buf[100];
	for (i = 0; i < ITEMS_QUANTITY; i++) {
		size_t m;
		wcstombs_s(&m, buf, 100, ITEM_IDS[i], _TRUNCATE);
		printf("[OPCCLIENT] ITEM %i - Adding the item %s\n", i, buf);
		AddTheItem(pIOPCItemMgt, H_ITEMS_READ_HANDLE[i], i);
	}
}


////////////////////////////////////////////////////////////////////
// Instantiate the IOPCServer interface of the OPCServer
// having the name ServerName. Return a pointer to this interface
//
IOPCServer* InstantiateServer(wchar_t ServerName[])
{
	CLSID CLSID_OPCServer;
	HRESULT hr;

	// get the CLSID from the OPC Server Name:
	hr = CLSIDFromString(ServerName, &CLSID_OPCServer);
	_ASSERT(!FAILED(hr));


	//queue of the class instances to create
	LONG cmq = 1; // nbr of class instance to create.
	MULTI_QI queue[1] =
	{ {&IID_IOPCServer,
	NULL,
	0} };

	//Server info:
	//COSERVERINFO CoServerInfo =
	//{
	//	/*dwReserved1*/ 0,
	//	/*pwszName*/ REMOTE_SERVER_NAME,
	//	/*COAUTHINFO*/  NULL,
	//	/*dwReserved2*/ 0
	//}; 

	// create an instance of the IOPCServer
	hr = CoCreateInstanceEx(CLSID_OPCServer, NULL, CLSCTX_SERVER,
		/*&CoServerInfo*/NULL, cmq, queue);
	_ASSERT(!hr);

	// return a pointer to the IOPCServer interface:
	return(IOPCServer*)queue[0].pItf;
}


/////////////////////////////////////////////////////////////////////
// Add group "Group1" to the Server whose IOPCServer interface
// is pointed by pIOPCServer. 
// Returns a pointer to the IOPCItemMgt interface of the added group
// and a server opc handle to the added group.
//
void AddTheGroup(IOPCServer* pIOPCServer, IOPCItemMgt*& pIOPCItemMgt,
	OPCHANDLE& hServerGroup)
{
	DWORD dwUpdateRate = 0;
	OPCHANDLE hClientGroup = 0;

	// Add an OPC group and get a pointer to the IUnknown I/F:
	HRESULT hr = pIOPCServer->AddGroup(/*szName*/ L"GroupTeste",
		/*bActive*/ FALSE,
		/*dwRequestedUpdateRate*/ 1000,
		/*hClientGroup*/ hClientGroup,
		/*pTimeBias*/ 0,
		/*pPercentDeadband*/ 0,
		/*dwLCID*/0,
		/*phServerGroup*/&hServerGroup,
		&dwUpdateRate,
		/*riid*/ IID_IOPCItemMgt,
		/*ppUnk*/ (IUnknown**)&pIOPCItemMgt);
	_ASSERT(!FAILED(hr));
}



//////////////////////////////////////////////////////////////////
// Add the Item ITEM_ID_SAW_TOOTHED to the group whose IOPCItemMgt interface
// is pointed by pIOPCItemMgt pointer. Return a server opc handle
// to the item.

void AddTheItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE& hServerItem, int index)
{
	HRESULT hr;

	// Array of items to add:
	OPCITEMDEF ItemArray[1] =
	{ {
			/*szAccessPath*/ L"",
			/*szItemID*/ LPWSTR(ITEM_IDS[index]),
			/*bActive*/ TRUE,
			/*hClient*/ index,
			/*dwBlobSize*/ 0,
			/*pBlob*/ NULL,
			/*vtRequestedDataType*/ VT,
			/*wReserved*/0
			} };

	//Add Result:
	OPCITEMRESULT* pAddResult = NULL;
	HRESULT* pErrors = NULL;
	// Add an Item to the previous Group:
	hr = pIOPCItemMgt->AddItems(1, ItemArray, &pAddResult, &pErrors);
	if (hr != S_OK) {
		printf("[OPCCLIENT] Failed call to AddItems function. Error code = %x\n", hr);
		exit(0);
	}

	// Server handle for the added item:
	hServerItem = pAddResult[0].hServer;

	// release memory allocated by the server:
	CoTaskMemFree(pAddResult->pBlob);

	CoTaskMemFree(pAddResult);
	pAddResult = NULL;

	CoTaskMemFree(pErrors);
	pErrors = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Read from device the value of the item having the "hServerItem" server 
// handle and belonging to the group whose one interface is pointed by
// pGroupIUnknown. The value is put in varValue. 
//
void ReadItems(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem, VARIANT& varValue)
{
	// value of the item:
	OPCITEMSTATE* pValue = NULL;

	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	pGroupIUnknown->QueryInterface(__uuidof(pIOPCSyncIO), (void**)&pIOPCSyncIO);

	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Read(OPC_DS_DEVICE, 1, &hServerItem, &pValue, &pErrors);
	_ASSERT(!hr);
	_ASSERT(pValue != NULL);

	varValue = pValue[0].vDataValue;

	//Release memeory allocated by the OPC server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	CoTaskMemFree(pValue);
	pValue = NULL;

	// release the reference to the IOPCSyncIO interface:
	pIOPCSyncIO->Release();
}

///////////////////////////////////////////////////////////////////////////
// Remove the item whose server handle is hServerItem from the group
// whose IOPCItemMgt interface is pointed by pIOPCItemMgt
//
void RemoveItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE hServerItem)
{
	// server handle of items to remove:
	OPCHANDLE hServerArray[1];
	hServerArray[0] = hServerItem;

	//Remove the item:
	HRESULT* pErrors; // to store error code(s)
	HRESULT hr = pIOPCItemMgt->RemoveItems(1, hServerArray, &pErrors);
	_ASSERT(!hr);

	//release memory allocated by the server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;
}

////////////////////////////////////////////////////////////////////////
// Remove the Group whose server handle is hServerGroup from the server
// whose IOPCServer interface is pointed by pIOPCServer
//
void RemoveGroup(IOPCServer* pIOPCServer, OPCHANDLE hServerGroup)
{
	// Remove the group:
	HRESULT hr = pIOPCServer->RemoveGroup(hServerGroup, FALSE);
	if (hr != S_OK) {
		if (hr == OPC_S_INUSE)
			printf("Failed to remove OPC group: object still has references to it.\n");
		else printf("Failed to remove OPC group. Error code = %x\n", hr);
		exit(0);
	}
}