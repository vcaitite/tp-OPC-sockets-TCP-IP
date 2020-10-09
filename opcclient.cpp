#include "opcclient.h"

// Global variables

// The OPC DA Spec requires that some constants be registered in order to use
// them. The one below refers to the OPC DA 1.0 IDataObject interface.
UINT OPC_DATA_TIME = RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));

const int items_quantity = 4;
const wchar_t* ITEM_IDS[items_quantity] = { L"Saw-toothed Waves.Real4", L"Random.UInt1", L"Random.UInt2", L"Random.UInt4" };
OPCHANDLE H_ITEMS_HANDLE[items_quantity] = {NULL, NULL, NULL, NULL};
//IOPCItemMgt* H_ITEMS_MGT[items_quantity] = { NULL, NULL, NULL, NULL };
wchar_t ITEM_ID_SAW_TOOTHED[] = L"Saw-toothed Waves.Real4";
wchar_t ITEM_ID_SAW_RANDOM_INT_1[] = L"Random.UInt1";
wchar_t ITEM_ID_SAW_RANDOM_INT_2[] = L"Random.UInt2";
wchar_t ITEM_ID_SAW_RANDOM_INT_4[] = L"Random.UInt4";


void __cdecl opcClient(void) {
	//do {
		//printf("\n\t[CLIENT] Testando!!!\n\n");
		//Sleep(2000);
		IOPCServer* pIOPCServer = NULL;   //pointer to IOPServer interface
		IOPCItemMgt* pIOPCItemMgt = NULL; //pointer to IOPCItemMgt interface

		OPCHANDLE hServerGroup; // server handle to the group

		int i;
		int j;
		int bRet;
		MSG msg;
		DWORD ticks1, ticks2;

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
		

		//Synchronous read of the device큦 item value.
		//VARIANT varValue; //to store the read value
		//VariantInit(&varValue);
		//printf("[OPCCLIENT] Reading synchronously during 10 seconds...\n");

		//for (i = 0; i < items_quantity; i++) {
		//	for (j = 0; j < 5; j++) {
		//		ReadItems(pIOPCItemMgt, H_ITEMS_HANDLE[i], varValue);
		//		// print the read value:
		//		printf("[OPCCLIENT] Read value: %6.2f\n", varValue.fltVal);
		//		// wait 1 second
		//		Sleep(1000);
		//	}
		//}

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
		// server큦 callback notification
		printf("[OPCCLIENT] Changing the group state to ACTIVE...\n");
		SetGroupActive(pIOPCItemMgt);

		// Enter again a message pump in order to process the server큦 callback
		// notifications, for the same reason explained before.

		ticks1 = GetTickCount();
		printf("[OPCCLIENT] Waiting for IOPCDataCallback notifications during 10 seconds...\n");
		do {
			bRet = GetMessage(&msg, NULL, 0, 0);
			if (!bRet) {
				printf("[OPCCLIENT] Failed to get windows message! Error code = %d\n", GetLastError());
				exit(0);
			}
			TranslateMessage(&msg); // This call is not really needed ...
			DispatchMessage(&msg);  // ... but this one is!
			ticks2 = GetTickCount();
		} while ((ticks2 - ticks1) < 5000);

		// Cancel the callback and release its reference
		printf("[OPCCLIENT] Cancelling the IOPCDataCallback notifications...\n");
		CancelDataCallback(pIConnectionPoint, dwCookie);
		//pIConnectionPoint->Release();
		pSOCDataCallback->Release();

		// Remove the OPC item:
		for (i = 0; i < items_quantity; i++) {
			printf("[OPCCLIENT] Removing the OPC item...\n");
			RemoveItem(pIOPCItemMgt, H_ITEMS_HANDLE[i]);
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
	/*} while (1);*/
}

void AddInitialItems(IOPCItemMgt* pIOPCItemMgt) {
	int i;
	char buf[100];
	for (i = 0; i < items_quantity; i++) {
		size_t m;
		wcstombs_s(&m, buf, 100, ITEM_IDS[i], _TRUNCATE);
		printf("[OPCCLIENT] Adding the item %s to the group...\n", buf);
		AddTheItem(pIOPCItemMgt, H_ITEMS_HANDLE[i], i);
	}
}


void method() {
	IOPCServer* pIOPCServer = NULL;   //pointer to IOPServer interface
	IOPCItemMgt* pIOPCItemMgt = NULL; //pointer to IOPCItemMgt interface

	OPCHANDLE hServerGroup; // server handle to the group
	OPCHANDLE hServerItem;  // server handle to the item

	int i;
	char buf[100];


	// Establish a callback asynchronous read by means of the old IAdviseSink()
	// (OPC DA 1.0) method. We first instantiate a new SOCAdviseSink object and
	// adjusts its reference count, and then call a wrapper function to
	// setup the callback.
	IDataObject* pIDataObject = NULL; //pointer to IDataObject interface
	DWORD tkAsyncConnection = 0;
	SOCAdviseSink* pSOCAdviseSink = new SOCAdviseSink();
	pSOCAdviseSink->AddRef();
	printf("Setting up the IAdviseSink callback connection...\n");
	SetAdviseSink(pIOPCItemMgt, pSOCAdviseSink, pIDataObject, &tkAsyncConnection);

	// Change the group to the ACTIVE state so that we can receive the
	// server큦 callback notification
	printf("Changing the group state to ACTIVE...\n");
	SetGroupActive(pIOPCItemMgt);

	// Enters a message pump in order to process the server큦 callback
	// notifications. This is needed because the CoInitialize() function
	// forces the COM threading model to STA (Single Threaded Apartment),
	// in which, according to the MSDN, "all method calls to a COM object
	// (...) are synchronized with the windows message queue for the
	// single-threaded apartment's thread." So, even being a console
	// application, the OPC client must process messages (which in this case
	// are only useless WM_USER [0x0400] messages) in order to process
	// incoming callbacks from a OPC server.
	//
	// A better alternative could be to use the CoInitializeEx() function,
	// which allows one to  specifiy the desired COM threading model;
	// in particular, calling
	//        CoInitializeEx(NULL, COINIT_MULTITHREADED)
	// sets the model to MTA (MultiThreaded Apartments) in which a message
	// loop is __not required__ since objects in this model are able to
	// receive method calls from other threads at any time. However, in the
	// MTA model the user is required to handle any aspects regarding
	// concurrency, since asynchronous, multiple calls to the object methods
	// can occur.
	//
	int bRet;
	MSG msg;
	DWORD ticks1, ticks2;
	ticks1 = GetTickCount();
	printf("Waiting for IAdviseSink callback notifications during 10 seconds...\n");
	do {
		bRet = GetMessage(&msg, NULL, 0, 0);
		if (!bRet) {
			printf("Failed to get windows message! Error code = %d\n", GetLastError());
			exit(0);
		}
		TranslateMessage(&msg); // This call is not really needed ...
		DispatchMessage(&msg);  // ... but this one is!
		ticks2 = GetTickCount();
	} while ((ticks2 - ticks1) < 10000);

	// Cancel the callback and release its reference
	printf("Cancelling the IAdviseSink callback...\n");
	CancelAdviseSink(pIDataObject, tkAsyncConnection);
	pSOCAdviseSink->Release();
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
	HRESULT hr = pIOPCServer->AddGroup(/*szName*/ L"Group2",
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
			/*hClient*/ 1,
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