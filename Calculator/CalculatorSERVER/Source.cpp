#include <Windows.h>
#include <winerror.h>

#include <WebServices.h>
#pragma comment(lib, "WebServices.lib")

#include <stdio.h>

#include <iostream>
using namespace std;

#include <tchar.h>
#include <strsafe.h>

#include "..\ClaculatorWSDL\Calculator.wsdl.h"

//#define HR(expr) { HRESULT hr = (expr); if (FAILED(hr)) { return hr; } }
#define HR(expr) { HRESULT hr = (expr); if (FAILED(hr)) { return; } }

class WsError
{
    WS_ERROR* m_h;
public:
    WsError() : m_h(0)
    {}
    ~WsError()
    {
        if (0 != m_h)
            WsFreeError(m_h);
    }
    HRESULT Create(const WS_ERROR_PROPERTY* properties,
        ULONG propertyCount)
    {
        return WsCreateError(properties, propertyCount, &m_h);
    }
    HRESULT GetProperty(WS_ERROR_PROPERTY_ID id, void* buffer,
        ULONG bufferSize)
    {
        return WsGetErrorProperty(m_h, id, buffer, bufferSize);
    }
    template <typename T>
    HRESULT GetProperty(WS_ERROR_PROPERTY_ID id, T* buffer)
    {
        return GetProperty(id, buffer, sizeof(T));
    }
    HRESULT GetString(ULONG index, WS_STRING* string)
    {
        return WsGetErrorString(m_h, index, string);
    }
    operator WS_ERROR* () const
    {
        return m_h;
    }
};

class WsHeap
{
    WS_HEAP* m_h;
public:
    WsHeap() : m_h(0)
    {}
    ~WsHeap()
    {
        if (0 != m_h) WsFreeHeap(m_h);
    }
    HRESULT Create(SIZE_T maxSize, SIZE_T trimSize,
        const WS_HEAP_PROPERTY* properties,
        ULONG propertyCount,
        WS_ERROR* error)
    {
        return WsCreateHeap(maxSize, trimSize, properties, propertyCount,
            &m_h, error);
    }
    operator WS_HEAP* () const
    {
        return m_h;
    }
};

class WsServiceHost
{
    WS_SERVICE_HOST* m_h;
public:
    WsServiceHost() : m_h(0)
    {}
    ~WsServiceHost()
    {
        if (0 != m_h)
        {
            Close(0, 0);
            WsFreeServiceHost(m_h);
        }
    }
    HRESULT Create(const WS_SERVICE_ENDPOINT** endpoints,
        const USHORT endpointCount,
        const WS_SERVICE_PROPERTY* properties,
        ULONG propertyCount, WS_ERROR* error)
    {
        return WsCreateServiceHost(endpoints, endpointCount, properties,
            propertyCount, &m_h, error);
    }
    HRESULT Open(const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
    {
        return WsOpenServiceHost(m_h, asyncContext, error);
    }
    HRESULT Close(const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error)
    {
        return WsCloseServiceHost(m_h, asyncContext, error);
    }
    operator WS_SERVICE_HOST* () const
    {
        return m_h;
    }
};

HRESULT CALLBACK AddCallback(__in const WS_OPERATION_CONTEXT*,
    __in double first,
    __in double second,
    __out double* result,
    __in_opt const WS_ASYNC_CONTEXT* /*asyncContext*/,
    __in_opt WS_ERROR* /*error*/)
{
    *result = first * second;
    return S_OK;
}

void wmain()
{
    WCHAR url[] = { L"http://localhost:81/calculator" };
    //WCHAR url[] = { L"http://10.91.0.149:81/calculator" };

    const WS_STRING address =
    {
        static_cast<ULONG>(wcslen(url)),
        const_cast<PWSTR>(url)
    };

    CalculatorBindingFunctionTable functions =
    {
        AddCallback
    };

    WS_SERVICE_ENDPOINT* endpoint = 0;

    WsError error;
    HR(error.Create(0, 0));

    WsHeap heap;
    HR(heap.Create(250, // max size
        0, // trim size
        0, // properties
        0, // property count
        error));

    HR(CalculatorBinding_CreateServiceEndpoint(0, // template value
        &address,
        &functions,
        0, // authz callback
        0, // properties
        0, // property count
        heap,
        &endpoint,
        error));

    const WS_SERVICE_ENDPOINT* endpoints[] = { endpoint };

    WsServiceHost serviceHost;

    HR(serviceHost.Create(endpoints,
        _countof(endpoints),
        0, // properties
        0, // property count
        error));

    HR(serviceHost.Open(0, // async context
        error));

    MessageBox(NULL, L"1", L"2", MB_OK);

    HR(serviceHost.Close(0, // async context
        error));
}
