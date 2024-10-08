#include <Windows.h>
#include <winerror.h>
#include "wincrypt.h"
#pragma comment(lib, "Crypt32.lib")

#include <WebServices.h>
#pragma comment(lib, "WebServices.lib")

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

HANDLE closeServer = NULL;


HRESULT CALLBACK Add(
    __in const WS_OPERATION_CONTEXT* context,
    __in int a,
    __in int b,
    __out int* result,
    __in_opt const WS_ASYNC_CONTEXT* asyncContext,
    __in_opt WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(asyncContext);
    UNREFERENCED_PARAMETER(error);

    *result = a + b;
    printf("%d + %d = %d\n", a, b, *result);
    fflush(stdout);
    return NOERROR;
}

HRESULT CALLBACK Subtract(
    __in const WS_OPERATION_CONTEXT* context,
    __in int a,
    __in int b,
    __out int* result,
    __in_opt const WS_ASYNC_CONTEXT* asyncContext,
    __in_opt WS_ERROR* error)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(asyncContext);
    UNREFERENCED_PARAMETER(error);

    *result = a - b;
    printf("%d - %d = %d\n", a, b, *result);
    fflush(stdout);
    return NOERROR;
}

HRESULT CALLBACK CloseChannelCallback(
    __in const WS_OPERATION_CONTEXT* context,
    __in_opt const WS_ASYNC_CONTEXT* asyncContext)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(asyncContext);

    SetEvent(closeServer);
    return NOERROR;
}

static const DefaultBinding_ICalculatorFunctionTable calculatorFunctions = { Add, Subtract };

// Method contract for the service
static const WS_SERVICE_CONTRACT calculatorContract =
{
    &CalculatorSSL_wsdl.contracts.DefaultBinding_ICalculator, // comes from the generated header.
    NULL, // for not specifying the default contract
    &calculatorFunctions // specified by the user
};


// Main entry point
int __cdecl wmain(int argc, __in_ecount(argc) wchar_t** argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    HRESULT hr = NOERROR;
    WS_SERVICE_HOST* host = NULL;
    WS_SERVICE_ENDPOINT serviceEndpoint = {};
    const WS_SERVICE_ENDPOINT* serviceEndpoints[1];
    serviceEndpoints[0] = &serviceEndpoint;

    WS_ERROR* error = NULL;

    // declare and initialize an SSL transport security binding
    WS_SSL_TRANSPORT_SECURITY_BINDING sslBinding = {}; // zero out the struct
    sslBinding.binding.bindingType = WS_SSL_TRANSPORT_SECURITY_BINDING_TYPE; // set the binding type
    // NOTE: At the server, the SSL certificate for the listen URI must be
    // registered with http.sys using a tool such as httpcfg.exe.

    // declare and initialize the array of all security bindings
    WS_SECURITY_BINDING* securityBindings[1] = { &sslBinding.binding };

    // declare and initialize the security description
    WS_SECURITY_DESCRIPTION securityDescription = {}; // zero out the struct
    securityDescription.securityBindings = securityBindings;
    securityDescription.securityBindingCount = WsCountOf(securityBindings);
    WS_SERVICE_ENDPOINT_PROPERTY serviceEndpointProperties[1];
    WS_SERVICE_PROPERTY_CLOSE_CALLBACK closeCallbackProperty = { CloseChannelCallback };
    serviceEndpointProperties[0].id = WS_SERVICE_ENDPOINT_PROPERTY_CLOSE_CHANNEL_CALLBACK;
    serviceEndpointProperties[0].value = &closeCallbackProperty;
    serviceEndpointProperties[0].valueSize = sizeof(closeCallbackProperty);

    // Initialize service endpoint
    //WS_STRING url = WS_STRING_VALUE((WCHAR*)L"https://localhost:8443/calculator");
    //serviceEndpoint.address.url = url;
    //serviceEndpoint.address.url.chars = (WCHAR*)L"https://10.91.0.79:8443/calculator"; // address given as uri
    serviceEndpoint.address.url.chars = (WCHAR*)L"https://localhost:8443/calculator"; // address given as uri
    serviceEndpoint.address.url.length = (ULONG)wcslen(serviceEndpoint.address.url.chars);
    serviceEndpoint.channelBinding = WS_HTTP_CHANNEL_BINDING; // channel binding for the endpoint
    serviceEndpoint.channelType = WS_CHANNEL_TYPE_REPLY; // the channel type
    serviceEndpoint.securityDescription = &securityDescription; // security description
    serviceEndpoint.contract = &calculatorContract;  // the contract
    serviceEndpoint.properties = serviceEndpointProperties;
    serviceEndpoint.propertyCount = WsCountOf(serviceEndpointProperties);

    // Create an error object for storing rich error information
    hr = WsCreateError(NULL, 0, &error);
    if (FAILED(hr))
    {
        goto Exit;
    }
    // Create Event object for closing the server
    closeServer = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (closeServer == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }
    // Creating a service host
    hr = WsCreateServiceHost(serviceEndpoints, 1, NULL, 0, &host, error);
    if (FAILED(hr))
    {
        goto Exit;
    }
    // WsOpenServiceHost to start the listeners in the service host 
    hr = WsOpenServiceHost(host, NULL, error);
    if (FAILED(hr))
    {
        goto Exit;
    }
    WaitForSingleObject(closeServer, INFINITE);
    // Close the service host
    hr = WsCloseServiceHost(host, NULL, error);
    if (FAILED(hr))
    {
        goto Exit;
    }

Exit:
    if (FAILED(hr))
    {
        // Print out the error
        PrintError(hr, error);
    }
    fflush(stdout);
    if (host != NULL)
    {
        WsFreeServiceHost(host);
    }
    if (error != NULL)
    {
        WsFreeError(error);
    }
    if (closeServer != NULL)
    {
        CloseHandle(closeServer);
    }
    fflush(stdout);
    return SUCCEEDED(hr) ? 0 : -1;
}