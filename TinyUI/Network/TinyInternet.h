#pragma once
#include "TinyNetwork.h"
#include <WinInet.h>
#pragma comment(lib, "wininet.lib")

namespace TinyUI
{
	namespace Network
	{
		class TinyInternet;
		class TinyInternetSession;
		class TinyHTTPConnection;
		class TinyHTTPStream;

		class TinyInternet
		{
			DISALLOW_COPY_AND_ASSIGN(TinyInternet)
		public:
			TinyInternet();
			virtual ~TinyInternet();
			operator HINTERNET() const;
			HINTERNET Handle() const;
			BOOL operator == (const TinyInternet& obj) const;
			BOOL operator != (const TinyInternet& obj) const;
			BOOL Attach(HINTERNET hNET);
			HINTERNET Detach();
		public:
			virtual void Close();
		public:
			static TinyInternet* Lookup(HINTERNET hNET);
		public:
			BOOL QueryOption(DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufLen) const;
			BOOL QueryOption(DWORD dwOption, DWORD& dwValue) const;
			BOOL SetOption(DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
			BOOL SetOption(DWORD dwOption, DWORD dwValue);
		protected:
			HINTERNET	m_hNET;
			static  TinyPointerMap	m_sessionMap;
		};
		//////////////////////////////////////////////////////////////////////////
		class TinyInternetSession : public TinyInternet
		{
			DISALLOW_COPY_AND_ASSIGN(TinyInternetSession)
		public:
			explicit TinyInternetSession(LPCTSTR pstrAgent = NULL,
				DWORD_PTR dwContext = 1,
				DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS,
				LPCTSTR pstrProxyName = NULL,
				LPCTSTR pstrProxyBypass = NULL,
				DWORD dwFlags = 0);
			virtual ~TinyInternetSession();
			DWORD_PTR GetContext() const;
			TinyHTTPConnection* GetHttpConnection(LPCTSTR pstrServer,
				INTERNET_PORT nPort = INTERNET_INVALID_PORT_NUMBER,
				LPCTSTR pstrUserName = NULL, LPCTSTR pstrPassword = NULL);
			BOOL EnableStatusCallback(BOOL bEnable);
		public:
			static BOOL SetCookie(LPCTSTR pstrUrl, LPCTSTR pstrCookieName, LPCTSTR pstrCookieData);
			static BOOL GetCookie(LPCTSTR pstrUrl, LPCTSTR pstrCookieName, LPTSTR pstrCookieData, DWORD dwBufLen);
			static DWORD GetCookieLength(LPCTSTR pstrUrl, LPCTSTR pstrCookieName);
			static BOOL GetCookie(LPCTSTR pstrUrl, LPCTSTR pstrCookieName, TinyString& strCookieData);
			virtual void OnStatusCallback(DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
		private:
			static void WINAPI InertnetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
		protected:
			BOOL						m_bEnable;
			DWORD_PTR					m_dwContext;
			INTERNET_STATUS_CALLBACK	m_pCallback;
		};
		//////////////////////////////////////////////////////////////////////////
		class TinyInternetConnection : public TinyInternet
		{
			DISALLOW_COPY_AND_ASSIGN(TinyInternetConnection)
		public:
			TinyInternetConnection(TinyInternetSession* pSession, LPCTSTR pstrServer, INTERNET_PORT nPort = INTERNET_INVALID_PORT_NUMBER, DWORD_PTR dwContext = 1);
			virtual ~TinyInternetConnection();
			DWORD_PTR GetContext() const;
			TinyInternetSession* GetSession() const;
			TinyString GetServer() const;
		private:
			TinyInternetSession*	m_pSession;
			TinyString				m_server;
			INTERNET_PORT			m_port;
			DWORD_PTR				m_dwContext;
		};
		//////////////////////////////////////////////////////////////////////////
		class TinyInternetStream : public TinyInternet
		{
			friend class TinyInternetSession;
			friend class TinyHTTPConnection;
			DISALLOW_COPY_AND_ASSIGN(TinyInternetStream)
		public:
			TinyInternetStream(HINTERNET hRequest, TinyInternetConnection* pConnection, LPCTSTR pstrObject, LPCTSTR pstrServer, LPCTSTR pstrVerb, DWORD_PTR dwContext);
			BOOL SetWriteBufferSize(UINT nWriteSize);
			BOOL SetReadBufferSize(UINT nReadSize);
			LONGLONG Seek(LONGLONG lOff, DWORD dwMoveMethod);
			DWORD Read(void* pData, DWORD cbData);
			DWORD Write(const void* pData, DWORD cbData);
			BOOL Flush();
			LONGLONG GetSize() const;
		public:
			TinyString	GetObject() const;
			TinyString	GetServer() const;
			TinyString	GetVerb() const;
			BOOL ErrorDialog(HWND hWND, DWORD dwError, DWORD dwFlags /*FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS*/, LPVOID* lppvData);
			virtual TinyString GetFileURL() const;
		public:
			virtual ~TinyInternetStream();
		protected:
			UINT					m_nWriteBufferSize;
			UINT					m_nWriteBufferPos;
			LPBYTE					m_pbWriteBuffer;
			UINT					m_nReadBufferSize;
			UINT					m_nReadBufferPos;
			LPBYTE					m_pbReadBuffer;
			UINT					m_nReadBufferBytes;
			DWORD_PTR				m_dwContext;
			TinyString				m_strObject;
			TinyString				m_strVerb;
			TinyString				m_strServer;
			TinyInternetConnection* m_pConnection;
		};
		//////////////////////////////////////////////////////////////////////////
		class TinyHTTPConnection : public TinyInternetConnection
		{
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPConnection)
		public:
			enum
			{
				_HTTP_VERB_MIN = 0,
				HTTP_VERB_POST = 0,
				HTTP_VERB_GET = 1,
				HTTP_VERB_HEAD = 2,
				HTTP_VERB_PUT = 3,
				HTTP_VERB_LINK = 4,
				HTTP_VERB_DELETE = 5,
				HTTP_VERB_UNLINK = 6,
				_HTTP_VERB_MAX = 6,
			};
		public:
			TinyHTTPConnection(TinyInternetSession* pSession, HINTERNET hConnected,
				LPCTSTR pstrServer, DWORD_PTR dwContext);
			TinyHTTPConnection(TinyInternetSession* pSession, LPCTSTR pstrServer,
				INTERNET_PORT nPort = INTERNET_INVALID_PORT_NUMBER,
				LPCTSTR pstrUserName = NULL, LPCTSTR pstrPassword = NULL,
				DWORD_PTR dwContext = 1);
			TinyHTTPStream* OpenRequest(LPCTSTR pstrVerb,
				LPCTSTR pstrObjectName, LPCTSTR pstrReferer, DWORD_PTR dwContext,
				LPCTSTR* ppstrAcceptTypes, LPCTSTR pstrVersion, DWORD dwFlags);
			TinyHTTPStream* OpenRequest(INT nVerb, LPCTSTR pstrObjectName,
				LPCTSTR pstrReferer = NULL, DWORD_PTR dwContext = 1,
				LPCTSTR* ppstrAcceptTypes = NULL, LPCTSTR pstrVersion = NULL,
				DWORD dwFlags = INTERNET_FLAG_EXISTING_CONNECT);
			virtual ~TinyHTTPConnection();
		protected:
			static const LPCTSTR szHtmlVerbs[];
		};
		//////////////////////////////////////////////////////////////////////////
		class TinyHTTPStream : public TinyInternetStream
		{
		public:
			TinyHTTPStream(HINTERNET hRequest, TinyHTTPConnection* pConnection, LPCTSTR pstrObject, LPCTSTR pstrServer, LPCTSTR pstrVerb, DWORD_PTR dwContext);
			virtual ~TinyHTTPStream();
		public:
			BOOL AddRequestHeaders(LPCTSTR pstrHeaders, DWORD dwFlags = HTTP_ADDREQ_FLAG_ADD_IF_NEW, INT dwHeadersLen = -1);
			BOOL AddRequestHeaders(TinyString& str, DWORD dwFlags = HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			BOOL SendRequest(LPCTSTR pstrHeaders = NULL, DWORD dwHeadersLen = 0, LPVOID lpOptional = NULL, DWORD dwOptionalLen = 0);
			BOOL SendRequest(TinyString& strHeaders, LPVOID lpOptional = NULL, DWORD dwOptionalLen = 0);
			BOOL SendRequestEx(DWORD dwTotalLen, DWORD dwFlags = HSR_INITIATE, DWORD_PTR dwContext = 1);
			BOOL SendRequestEx(LPINTERNET_BUFFERS lpBuffIn, LPINTERNET_BUFFERS lpBuffOut, DWORD dwFlags = HSR_INITIATE, DWORD_PTR dwContext = 1);
			BOOL EndRequest(DWORD dwFlags = 0, LPINTERNET_BUFFERS lpBuffIn = NULL, DWORD_PTR dwContext = 1);
			BOOL QueryInfo(DWORD dwInfoLevel, LPVOID lpvBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex = NULL) const;
			BOOL QueryInfo(DWORD dwInfoLevel, TinyString& str, LPDWORD dwIndex = NULL) const;
			BOOL QueryInfo(DWORD dwInfoLevel, SYSTEMTIME* pSysTime, LPDWORD dwIndex = NULL) const;
			BOOL QueryInfo(DWORD dwInfoLevel, DWORD& dwResult, LPDWORD dwIndex = NULL) const;
			BOOL QueryInfoStatusCode(DWORD& dwStatusCode) const;
		};
		//////////////////////////////////////////////////////////////////////////
		BOOL WINAPI ParseURL(LPCTSTR pstrURL, INTERNET_SCHEME& scheme, TinyString& strServer, TinyString& strObject, INTERNET_PORT& nPort);
		BOOL WINAPI ParseURLEx(LPCTSTR pstrURL, INTERNET_SCHEME& scheme, TinyString& strServer, TinyString& strObject, INTERNET_PORT& nPort, TinyString& strUsername, TinyString& strPassword, DWORD dwFlags);
		//////////////////////////////////////////////////////////////////////////
		/*class TinyAsyncHTTP;
		typedef struct _InetContext
		{
			TinyAsyncHTTP *obj;
			DWORD dwContext;
		} InetContext;

		typedef enum _RequestType {
			GET,
			POST
		} RequestType;

		class TinyAsyncHTTP
		{
		public:
			TinyAsyncHTTP();
			~TinyAsyncHTTP();

			enum {
				CONTEXT_CONNECT,
				CONTEXT_REQUESTHANDLE
			};

			BOOL Connect(
				string szAddr,
				unsigned short port = INTERNET_DEFAULT_HTTP_PORT,
				string szAgent = "AsyncInet",
				DWORD dwTimeout = 20 * 1000);

			BOOL SendRequest(
				string szURL,
				RequestType requestType,
				string szRequestData,
				string szReferrer,
				DWORD dwTimeout = 20 * 1000);

			unsigned long ReadData(
				PBYTE lpBuffer,
				DWORD dwSize,
				DWORD dwTimeout = 20 * 1000);

			void CloseConnection();

			static void WINAPI CallbackFunction(
				HINTERNET hInternet,
				DWORD dwContext,
				DWORD dwInetStatus,
				LPVOID lpStatusInfo,
				DWORD dwStatusInfoLength);

		protected:
			InetContext context;
			HANDLE hConnectEvent, hRequestOpenEvent, hRequestCompleteEvent;
			HINTERNET hInetInstance, hInetConnect, hInetRequest;

		};*/
	}
}

