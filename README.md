# Passthrough APP (Asynchronous Pluggable Protocol)

## Motivation and History

Internet Explorer takes a complex and challenging [approach to extensibility](http://msdn.microsoft.com/en-us/library/hh772401.aspx) based on [Window's Component Object Model (COM)](https://www.microsoft.com/com/default.mspx). If you are an experienced and skilled C++ developer, you can create add-ons for IE that offer similar functionality to that of equivalents for other popular browsers like Chrome and Firefox.

However, there are striking gaps in the capabilities of the official APIs that make certain types of add-ons impossible. One significant example is the processing of network requests. The web browser control used by IE handles a few events that are triggered before navigating to a page, when navigation completes, when document loading completes, etc. However, these events are only triggered for page and frame loads (not for subordinate requests like images and stylesheets). Their behavior when a page is refreshed is counterintuitive. And they provide little or no features for viewing and modifying web requests and traffic (e.g. adding new headers to a request or redirecting it to a different URL).

Igor Tandetnik's Passthrough APP is an attempt to bridge these gaps in an ingenious way: by replacing IE's implementation of the HTTP and HTTPS protocols with a new one that does nothing more than delegate to the default implementation. In this way, we are able to spy on and influence the interactions that occur. If a request is made for a URL, we can add headers to the request,for example, before passing it on to the built-in protocol handler.

As the Passthrough APP is no longer actively maintained, this Github project is an attempt to provide a canonical location where the source code can be downloaded, bugfixes and improvements can be proposed, issues can be filed, etc. We have updated the "Passthrough APP beta" sources written by Igor Tandetnik to work with the latest versions of Internet Explorer (versions 8 and 9 at the time of this writing).

## User Guide

### Customizing the protocol sink

The Passthrough APP is designed for us in a [Browser Helper Object](http://msdn.microsoft.com/en-us/library/bb250436). If you are using it, you presumably want to customize some of the behavior of the protocol (used to start requests) and/or sink (used to receive notifications about requests).

In the simplest case you can customize the sink so that you can monitor the progress of requests and modify them (e.g. by changing headers). To do so, create a source file to implement your new sink class:

```c++
class CMyProtocolSink :
  public PassthroughAPP::CInternetProtocolSinkWithSP<CMyProtocolSink>,
  public IHttpNegotiate
{
  typedef PassthroughAPP::CInternetProtocolSinkWithSP<CMyProtocolSink> BaseClass;

public:
  BEGIN_COM_MAP(CMyProtocolSink)
    COM_INTERFACE_ENTRY(IHttpNegotiate)
    COM_INTERFACE_ENTRY_CHAIN(BaseClass)
  END_COM_MAP()

  BEGIN_SERVICE_MAP(CMyProtocolSink)
    SERVICE_ENTRY(IID_IHttpNegotiate)
  END_SERVICE_MAP()

  // IHttpNegotiate
  STDMETHODIMP BeginningTransaction(
    /* [in] */ LPCWSTR szURL,
    /* [in] */ LPCWSTR szHeaders,
    /* [in] */ DWORD dwReserved,
    /* [out] */ LPWSTR *pszAdditionalHeaders);

  STDMETHODIMP OnResponse(
    /* [in] */ DWORD dwResponseCode,
    /* [in] */ LPCWSTR szResponseHeaders,
    /* [in] */ LPCWSTR szRequestHeaders,
    /* [out] */ LPWSTR *pszAdditionalRequestHeaders);

  STDMETHODIMP ReportProgress(
    /* [in] */ ULONG ulStatusCode,
    /* [in] */ LPCWSTR szStatusText);
};
```

In this case we are implementing a sink that overrides the functionality of the `IHttpNegotiate` interface. You can override any of the interfaces implemented by the Passthrough APP sink class (`IInternetProtocolSinkImpl`) in a similar manner.

### Creating the APP

In addition to a sink, you also need to create a class that implements the APP itself. This class takes a "start policy" class as a template parameter. The Passthrough APP toolkit provides two built-in start policy classes: `NoSinkStartPolicy`, which simply starts the request using the default sink, and `CustomSinkStartPolicy`, which uses your custom sink (see previous section).

The latter is used as follows:

```c++
class CMyAPP;
typedef PassthroughAPP::CustomSinkStartPolicy<CMyAPP, CMyProtocolSink> MyStartPolicy;

class CMyAPP :
  public PassthroughAPP::CInternetProtocol<MyStartPolicy>
{
};
```

### Registering the class factory

In order for your Passthrough APP to be used, you need to register the class factory used by Internet Explorer to instantiate handlers for the HTTP and HTTPS protocols. First, define your factory using the templated `CMetaFactory` class included in the toolkit:

```c++
#include "ProtocolCF.h"
#include "MyProtocolSink.h"

typedef PassthroughAPP::CMetaFactory<PassthroughAPP::CComClassFactoryProtocol,
  CTrustedAdsProtocolAPP> MetaFactory;
```

Add two new members to your BHO's class:

```c++
CComPtr<IClassFactory> m_CFHTTP;
CComPtr<IClassFactory> m_CFHTTPS;
```

Then add this code to your BHO's `SetSite` method to register the class factory:

```c++
CComPtr<IInternetSession> pInternetSession;
CoInternetGetSession(0, &pInternetSession, 0);

MetaFactory::CreateInstance(CLSID_HttpProtocol, &m_CFHTTP);
pInternetSession->RegisterNameSpace(m_CFHTTP, CLSID_NULL, L"http", 0, 0, 0);

MetaFactory::CreateInstance(CLSID_HttpSProtocol, &m_CFHTTPS);
pInternetSession->RegisterNameSpace(m_CFHTTPS, CLSID_NULL, L"https", 0, 0, 0);
```

You can unregister the factories using the `UnregisterNameSpace` methods of `IInternetSession` if you no longer want your Passthrough APP to be used.
