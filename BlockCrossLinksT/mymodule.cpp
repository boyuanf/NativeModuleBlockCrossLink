#include "precomp.h"
#include <string>

//  Implementation of the OnAcquireRequestState method
REQUEST_NOTIFICATION_STATUS
CMyHttpModule::OnBeginRequest(
    IN IHttpContext *                       pHttpContext,
    IN OUT IHttpEventProvider *             pProvider
)
{
#ifndef REDIRECT
    HRESULT                         hr = S_OK;
		// TODO: implement the AcquireRequestState module functionality
	IHttpResponse * pHttpResponse = pHttpContext->GetResponse();
	//hr = pHttpResponse->Redirect("http://www.google.com/");
	//if (hr != S_OK)
	//{
	//	// Can’t determine whether this is an image folder request,
	//	// so give it back to IIS to finish it off.
	//	return RQ_NOTIFICATION_CONTINUE;
	//}
	PCSTR pszBuffer;
	std::string s = "Valid link";  //Write constant string for
	pszBuffer = s.c_str();
	HTTP_DATA_CHUNK dataChunk;
	dataChunk.DataChunkType = HttpDataChunkFromMemory;
	DWORD cbSent;
	dataChunk.FromMemory.pBuffer = (PVOID)pszBuffer;
	dataChunk.FromMemory.BufferLength = (USHORT)strlen(pszBuffer);
	hr = pHttpResponse->WriteEntityChunks(&dataChunk, 1, FALSE, TRUE, &cbSent);

	if (FAILED(hr))
	{
		return RQ_NOTIFICATION_FINISH_REQUEST;
	}
	else
	{
		return RQ_NOTIFICATION_CONTINUE;
	}
#endif

#ifdef CHECK_IMAGES

	UNREFERENCED_PARAMETER(pProvider);
	// The images folder to be protected
	// Change this value to reflect the images
	// path for your own web site
	PCSTR pszProtectedPath = "/images/";
	// controls whether to permit loading of images from
	// bookmarks or type the url into the browser location
	BOOL permitBookmarks = false;
	// Create an HRESULT to receive return values from methods.
	HRESULT hr;
	// Buffer size for returned variable values.
	DWORD cbValue = 512;
	// Allocating buffers for relevant
	// CGI environment variable values
	PCSTR pszServerName =
		(PCSTR)pHttpContext->AllocateRequestMemory(cbValue);
	PCSTR pszReferer =
		(PCSTR)pHttpContext->AllocateRequestMemory(cbValue);
	PCSTR pszPathInfo =
		(PCSTR)pHttpContext->AllocateRequestMemory(cbValue);
	if (pszPathInfo == NULL ||
		pszServerName == NULL ||
		pszReferer == NULL
		)
	{
		// looks like a memory allocation problem
		// bail out and let IIS take care of it.
		return RQ_NOTIFICATION_CONTINUE;
	}
	// Retrieve a pointer to the response.
	IHttpResponse * pHttpResponse = pHttpContext->GetResponse();
	// start by inspecting the path
	hr = pHttpContext->GetServerVariable("PATH_INFO",
		&pszPathInfo,
		&cbValue);
	if (hr != S_OK)
	{
		// Can’t determine whether this is an image folder request,
		// so give it back to IIS to finish it off.
		return RQ_NOTIFICATION_CONTINUE;
	}
	// is it the folder of interest?
	if (strstr(pszPathInfo, pszProtectedPath) == NULL)
	{
		// not a path of interest - let it go through unchallenged
		return RQ_NOTIFICATION_CONTINUE;
	}
	// Look for the “SERVER_NAME” variable.
	hr = pHttpContext->GetServerVariable("SERVER_NAME",
		&pszServerName,
		&cbValue);
	if (hr != S_OK)
	{
		// No point continuing if we have no SERVER_NAME
		// give it back to IIS to finish it off.
		return RQ_NOTIFICATION_CONTINUE;
	}
	// now retrieve the HTTP_REFERER value
	hr = pHttpContext->GetServerVariable("HTTP_REFERER", &pszReferer, &cbValue);
	// check for a valid result
	PCSTR pszBuffer;
	if (hr == S_OK)
	{
		// if the referer is the same web site, then pszServerName
		// will appear in pszReferer
		if (strstr(pszReferer, pszServerName) != 0)
		{
			//// it is there, so this is a valid link
			//std::string s = "Valid link";  //Write constant string for
			//pszBuffer = s.c_str();
			//HTTP_DATA_CHUNK dataChunk;
			//dataChunk.DataChunkType = HttpDataChunkFromMemory;
			//DWORD cbSent;
			//dataChunk.FromMemory.pBuffer = (PVOID)pszBuffer;
			//dataChunk.FromMemory.BufferLength = (USHORT)strlen(pszBuffer);
			//hr = pHttpResponse->WriteEntityChunks(&dataChunk, 1, FALSE, TRUE, &cbSent);
			return RQ_NOTIFICATION_CONTINUE;
		}
		else
		{
			//// the referer does not match server_name
			//std::string s = "Invalid link";  //Write constant string for
			//pszBuffer = s.c_str();
			//HTTP_DATA_CHUNK dataChunk;
			//dataChunk.DataChunkType = HttpDataChunkFromMemory;
			//DWORD cbSent;
			//dataChunk.FromMemory.pBuffer = (PVOID)pszBuffer;
			//dataChunk.FromMemory.BufferLength = (USHORT)strlen(pszBuffer);
			//hr = pHttpResponse->WriteEntityChunks(&dataChunk, 1, FALSE, TRUE, &cbSent);
			//return RQ_NOTIFICATION_CONTINUE;
			return RQ_NOTIFICATION_FINISH_REQUEST;
		}
	}
	if (hr = ERROR_INVALID_INDEX)
	{
		// the referer value is missing from the header
		if (permitBookmarks)
			return RQ_NOTIFICATION_CONTINUE;
		else
			return RQ_NOTIFICATION_FINISH_REQUEST;
	}
	// we only arrive here if there was an error
	// allow IIS to deal with the rest.
	// Return processing to the pipeline.
	return RQ_NOTIFICATION_CONTINUE;
#endif
}

// TODO: implement other desired event handler methods below