#include <Windows.h>
#include <winerror.h>
#include <WebServices.h>
#pragma comment(lib, "WebServices.lib")

#include <cassert>

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

class WsServiceProxy
{
    WS_SERVICE_PROXY* m_h;
public:
    WsServiceProxy() : m_h(0)
    {}
    ~WsServiceProxy()
    {
        if (0 != m_h)
        {
            Close(0, 0); // error
            WsFreeServiceProxy(m_h);
        }
    }
    HRESULT Open(const WS_ENDPOINT_ADDRESS* address,
        const WS_ASYNC_CONTEXT* asyncContext,
        WS_ERROR* error)
    {
        return WsOpenServiceProxy(m_h, address, asyncContext, error);
    }
    HRESULT Close(const WS_ASYNC_CONTEXT* asyncContext,
        WS_ERROR* error)
    {
        return WsCloseServiceProxy(m_h, asyncContext, error);
    }
    WS_SERVICE_PROXY** operator&()
    {
        return &m_h;
    }
    operator WS_SERVICE_PROXY* () const
    {
        return m_h;
    }
};

void wmain()
{
    WCHAR url[] = { L"http://localhost:81/calculator" };
    //WCHAR url[] = { L"http://10.91.0.149:81/calculator" };

    WsError error;
    HR(error.Create(0, 0));

    WsHeap heap;
    HR(heap.Create(250, // max size
        0, // trim size
        0, // properties
        0, // property count
        error));

    WsServiceProxy serviceProxy;
    HR(CalculatorBinding_CreateServiceProxy(0, // template value
        0, // properties
        0, // property count
        &serviceProxy,
        error));

    WS_ENDPOINT_ADDRESS address =
    {
        {
            static_cast<ULONG>(wcslen(url)),
            const_cast<PWSTR>(url)
        }
    };

    HR(serviceProxy.Open(&address,
        0, // async context
        error));

    const double first = 1.23;
    const double second = 2.34;
    double result = 0.0;

    HR(CalculatorBinding_Add(serviceProxy,
        first,
        second,
        &result,
        heap,
        0, // properties
        0, // property count
        0, // async context
        error));

    cout << "result=" << result << endl;

    HR(serviceProxy.Close(0, // async context
        error));
}