#include <Windows.h>
#include <winerror.h>
#include "wincrypt.h"
#pragma comment(lib, "Crypt32.lib")

#include <WebServices.h>
#pragma comment(lib, "WebServices.lib")

#include <cassert>

#include <stdio.h>

#include <iostream>
using namespace std;

#include <tchar.h>
#include <strsafe.h>

#include "..\CalculatorSSLWSDL\CalculatorSSL.wsdl.h"

// Print out rich error info
void PrintError(HRESULT errorCode, WS_ERROR* error)
{
    wprintf(L"Failure: errorCode=0x%lx\n", errorCode);

    if (errorCode == E_INVALIDARG || errorCode == WS_E_INVALID_OPERATION)
    {
        // Correct use of the APIs should never generate these errors
        wprintf(L"The error was due to an invalid use of an API.  This is likely due to a bug in the program.\n");
        DebugBreak();
    }

    HRESULT hr = NOERROR;
    if (error != NULL)
    {
        ULONG errorCount;
        hr = WsGetErrorProperty(error, WS_ERROR_PROPERTY_STRING_COUNT, &errorCount, sizeof(errorCount));
        if (FAILED(hr))
        {
            goto Exit;
        }
        for (ULONG i = 0; i < errorCount; i++)
        {
            WS_STRING string;
            hr = WsGetErrorString(error, i, &string);
            if (FAILED(hr))
            {
                goto Exit;
            }
            wprintf(L"%.*s\n", string.length, string.chars);
        }
    }
Exit:
    if (FAILED(hr))
    {
        wprintf(L"Could not get error string (errorCode=0x%lx)\n", hr);
    }
}

// Main entry point
int __cdecl wmain(int argc, __in_ecount(argc) wchar_t** argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    HRESULT hr = NOERROR;
    WS_ERROR* error = NULL;
    WS_HEAP* heap = NULL;
    WS_SERVICE_PROXY* proxy = NULL;

    // declare and initialize an SSL transport security binding
    WS_SSL_TRANSPORT_SECURITY_BINDING sslBinding = {}; // zero out the struct
    sslBinding.binding.bindingType = WS_SSL_TRANSPORT_SECURITY_BINDING_TYPE; // set the binding type

    // declare and initialize the array of all security bindings
    WS_SECURITY_BINDING* securityBindings[1] = { &sslBinding.binding };

    // declare and initialize the security description
    WS_SECURITY_DESCRIPTION securityDescription = {}; // zero out the struct
    securityDescription.securityBindings = securityBindings;
    securityDescription.securityBindingCount = WsCountOf(securityBindings);

    int result = 0;
    WS_ENDPOINT_ADDRESS address = {};
    //WS_STRING url = WS_STRING_VALUE((WCHAR*)L"https://localhost:8443/calculator");
    //WS_STRING url = WS_STRING_VALUE((WCHAR*)L"https://10.91.0.79:8443/calculator");
    WS_STRING url;
    //url.chars = (WCHAR*)L"https://10.91.0.79:8443/calculator"; // address given as uri
    url.chars = (WCHAR*)L"https://localhost:8443/calculator"; // address given as uri
    url.length = (ULONG)wcslen(url.chars);
    address.url = url;

    // Create an error object for storing rich error information
    hr = WsCreateError(NULL, 0, &error);
    if (FAILED(hr))
    {
        goto Exit;
    }

    // Create a heap to store deserialized data
    hr = WsCreateHeap(/*maxSize*/ 2048, /*trimSize*/ 512, NULL, 0, &heap, error);
    if (FAILED(hr))
    {
        goto Exit;
    }

    // Create the proxy
    hr = WsCreateServiceProxy(WS_CHANNEL_TYPE_REQUEST, WS_HTTP_CHANNEL_BINDING, &securityDescription,
        NULL, 0, NULL, 0, &proxy, error);
    if (FAILED(hr))
    {
        goto Exit;
    }

    hr = WsOpenServiceProxy(proxy, &address, NULL, error);
    if (FAILED(hr))
    {
        goto Exit;
    }

    hr = DefaultBinding_ICalculator_Add(proxy, 1, 2, &result, heap, NULL, 0, NULL, error);
    if (FAILED(hr))
    {
        goto Exit;
    }

    wprintf(L"%d + %d = %d\n", 1, 2, result);

Exit:
    if (FAILED(hr))
    {
        // Print out the error
        PrintError(hr, error);
    }
    fflush(stdout);
    if (proxy != NULL)
    {
        WsCloseServiceProxy(proxy, NULL, NULL);
        WsFreeServiceProxy(proxy);
    }

    if (heap != NULL)
    {
        WsFreeHeap(heap);
    }
    if (error != NULL)
    {
        WsFreeError(error);
    }
    fflush(stdout);
    return SUCCEEDED(hr) ? 0 : -1;
}
