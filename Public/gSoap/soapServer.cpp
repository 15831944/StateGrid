/* soapServer.cpp
   Generated by gSOAP 2.8.32 for add.h

gSOAP XML Web services tools
Copyright (C) 2000-2016, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "soapH.h"

SOAP_SOURCE_STAMP("@(#) soapServer.cpp ver 2.8.32 2016-07-07 06:17:58 GMT")
extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
#ifndef WITH_FASTCGI
	unsigned int k = soap->max_keep_alive;
#endif
	do
	{
#ifndef WITH_FASTCGI
		if (soap->max_keep_alive > 0 && !--k)
			soap->keep_alive = 0;
#endif
		if (soap_begin_serve(soap))
		{	if (soap->error >= SOAP_STOP)
				continue;
			return soap->error;
		}
		if (soap_serve_request(soap) || (soap->fserveloop && soap->fserveloop(soap)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(soap);
#else
			return soap_send_fault(soap);
#endif
		}

#ifdef WITH_FASTCGI
		soap_destroy(soap);
		soap_end(soap);
	} while (1);
#else
	} while (soap->keep_alive);
#endif
	return SOAP_OK;
}

#ifndef WITH_NOSERVEREQUEST
extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:Syndwsadata"))
		return soap_serve_ns__Syndwsadata(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:syndwsaTicketInfo"))
		return soap_serve_ns__syndwsaTicketInfo(soap);
	return soap->error = SOAP_NO_METHOD;
}
#endif

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__Syndwsadata(struct soap *soap)
{	struct ns__Syndwsadata soap_tmp_ns__Syndwsadata;
	struct ns__SyndwsadataResponse soap_tmp_ns__SyndwsadataResponse;
	struct SyndwsadataResponse soap_tmp_SyndwsadataResponse;
	soap_default_ns__SyndwsadataResponse(soap, &soap_tmp_ns__SyndwsadataResponse);
	soap_default_SyndwsadataResponse(soap, &soap_tmp_SyndwsadataResponse);
	soap_tmp_ns__SyndwsadataResponse.serviceResponse = &soap_tmp_SyndwsadataResponse;
	soap_default_ns__Syndwsadata(soap, &soap_tmp_ns__Syndwsadata);
	if (!soap_get_ns__Syndwsadata(soap, &soap_tmp_ns__Syndwsadata, "ns:Syndwsadata", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__Syndwsadata(soap, soap_tmp_ns__Syndwsadata.request, soap_tmp_ns__SyndwsadataResponse.serviceResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL;
	soap_serializeheader(soap);
	soap_serialize_ns__SyndwsadataResponse(soap, &soap_tmp_ns__SyndwsadataResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__SyndwsadataResponse(soap, &soap_tmp_ns__SyndwsadataResponse, "ns:SyndwsadataResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__SyndwsadataResponse(soap, &soap_tmp_ns__SyndwsadataResponse, "ns:SyndwsadataResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__syndwsaTicketInfo(struct soap *soap)
{	struct ns__syndwsaTicketInfo soap_tmp_ns__syndwsaTicketInfo;
	struct ns__syndwsaTicketInfoResponse soap_tmp_ns__syndwsaTicketInfoResponse;
	struct SyndwsaTicketInfoResponse soap_tmp_SyndwsaTicketInfoResponse;
	soap_default_ns__syndwsaTicketInfoResponse(soap, &soap_tmp_ns__syndwsaTicketInfoResponse);
	soap_default_SyndwsaTicketInfoResponse(soap, &soap_tmp_SyndwsaTicketInfoResponse);
	soap_tmp_ns__syndwsaTicketInfoResponse.serviceResponse = &soap_tmp_SyndwsaTicketInfoResponse;
	soap_default_ns__syndwsaTicketInfo(soap, &soap_tmp_ns__syndwsaTicketInfo);
	if (!soap_get_ns__syndwsaTicketInfo(soap, &soap_tmp_ns__syndwsaTicketInfo, "ns:syndwsaTicketInfo", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__syndwsaTicketInfo(soap, soap_tmp_ns__syndwsaTicketInfo.request, soap_tmp_ns__syndwsaTicketInfoResponse.serviceResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL;
	soap_serializeheader(soap);
	soap_serialize_ns__syndwsaTicketInfoResponse(soap, &soap_tmp_ns__syndwsaTicketInfoResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__syndwsaTicketInfoResponse(soap, &soap_tmp_ns__syndwsaTicketInfoResponse, "ns:syndwsaTicketInfoResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__syndwsaTicketInfoResponse(soap, &soap_tmp_ns__syndwsaTicketInfoResponse, "ns:syndwsaTicketInfoResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapServer.cpp */