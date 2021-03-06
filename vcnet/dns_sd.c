/*
 * Dynamic wrapper for Bonjour SDK for Windows.
 *
 * Copyright 2018 by Apple Inc.
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
 */

#include "dns_sd.h"
#include <windows.h>
#include <cups/thread-private.h>


/*
 * Pointers for functions...
 */

static int		dnssd_initialized = 0;
static _cups_mutex_t	dnssd_mutex = _CUPS_MUTEX_INITIALIZER;
static DNSServiceErrorType DNSSD_API (*dnssd_add_record)(DNSServiceRef sdRef, DNSRecordRef *RecordRef, DNSServiceFlags flags, uint16_t rrtype, uint16_t rdlen, const void *rdata, uint32_t ttl);
static DNSServiceErrorType DNSSD_API (*dnssd_browse)(DNSServiceRef *sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, const char *regtype, const char *domain, DNSServiceBrowseReply callBack, void *context);
static DNSServiceErrorType DNSSD_API (*dnssd_construct_full_name)(char * const fullName, const char * const service, const char * const regtype, const char * const domain);
static DNSServiceErrorType DNSSD_API (*dnssd_create_connection)(DNSServiceRef *sdRef);
static DNSServiceErrorType DNSSD_API (*dnssd_process_result)(DNSServiceRef sdRef);
static DNSServiceErrorType DNSSD_API (*dnssd_query_record)(DNSServiceRef *sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, const char *fullname, uint16_t rrtype, uint16_t rrclass, DNSServiceQueryRecordReply callBack, void *context);
static void DNSSD_API (*dnssd_deallocate)(DNSServiceRef sdRef);
static int DNSSD_API (*dnssd_sock_fd)(DNSServiceRef sdRef);
static DNSServiceErrorType DNSSD_API (*dnssd_register)(DNSServiceRef *sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, const char *name, const char *regtype, const char *domain, const char *host, uint16_t port, uint16_t txtLen, const void *txtRecord, DNSServiceRegisterReply callBack, void *context);
static DNSServiceErrorType DNSSD_API (*dnssd_remove_record)(DNSServiceRef sdRef, DNSRecordRef RecordRef, DNSServiceFlags flags);
static DNSServiceErrorType DNSSD_API (*dnssd_resolve)(DNSServiceRef *sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, const char *name, const char *regtype, const char *domain, DNSServiceResolveReply callBack, void *context);
static DNSServiceErrorType DNSSD_API (*dnssd_update_record)(DNSServiceRef sdRef, DNSRecordRef RecordRef, DNSServiceFlags flags, uint16_t rdlen, const void *rdata, uint32_t ttl);


void DNSSD_API TXTRecordCreate
    (
    TXTRecordRef     *txtRecord,
    uint16_t         bufferLen,
    void             *buffer
    );
void DNSSD_API TXTRecordDeallocate
    (
    TXTRecordRef     *txtRecord
    );
const void * DNSSD_API TXTRecordGetBytesPtr
    (
    const TXTRecordRef *txtRecord
    );
uint16_t DNSSD_API TXTRecordGetLength
    (
    const TXTRecordRef *txtRecord
    );
DNSServiceErrorType DNSSD_API TXTRecordSetValue
    (
    TXTRecordRef     *txtRecord,
    const char       *key,
    uint8_t          valueSize,        /* may be zero */
    const void       *value            /* may be NULL */
    );


/*
 * Function to initialize pointers...
 */

static void
dnssd_init(void)
{
  _cupsMutexLock(&dnssd_mutex);
  if (!dnssd_initialized)
  {
    HINSTANCE	dll_handle = LoadLibrary("dnssd.dll");

    if (dll_handle)
    {
      dnssd_add_record          = GetProcAddress("DNSServiceAddRecord");
      dnssd_browse              = GetProcAddress("DNSServiceBrowse");
      dnssd_construct_full_name = GetProcAddress("DNSServiceConstructFullName");
      dnssd_create_connection   = GetProcAddress("DNSServiceCreateConnection");
      dnssd_deallocate          = GetProcAddress("DNSServiceRefDeallocate");
      dnssd_process_result      = GetProcAddress("DNSServiceProcessResult");
      dnssd_query_record        = GetProcAddress("DNSServiceQueryRecord");
      dnssd_register            = GetProcAddress("DNSServiceRegister");
      dnssd_remove_record       = GetProcAddress("DNSServiceRemoveRecord");
      dnssd_resolve             = GetProcAddress("DNSServiceResolve");
      dnssd_sock_fd             = GetProcAddress("DNSServiceRefSockFD");
      dnssd_update_record       = GetProcAddress("DNSServiceUpdateRecord");

      dnssd_txt_create          = GetProcAddress("TXTRecordCreate");
      dnssd_txt_deallocate      = GetProcAddress("TXTRecordDeallocate");
      dnssd_txt_get_bytes_ptr   = GetProcAddress("TXTRecordGetBytesPtr");
      dnssd_txt_get_length      = GetProcAddress("TXTRecordGetLength");
      dnssd_txt_set_value       = GetProcAddress("TXTRecordSetValue");
    }

    dnssd_initialized = 1;
  }
  _cupsMutexUnlock(&dnssd_mutex);
}


// DNSServiceAddRecord
DNSServiceErrorType DNSSD_API DNSServiceAddRecord
    (
    DNSServiceRef                       sdRef,
    DNSRecordRef                        *RecordRef,
    DNSServiceFlags                     flags,
    uint16_t                            rrtype,
    uint16_t                            rdlen,
    const void                          *rdata,
    uint32_t                            ttl
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_add_record)
    return (*dnssd_add_record)(sdRef, RecordRef, flags, rrtype, rdlen, rdata, ttl);
  else
    return (-1);
}


// DNSServiceBrowse
DNSServiceErrorType DNSSD_API DNSServiceBrowse
    (
    DNSServiceRef                       *sdRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    const char                          *regtype,
    const char                          *domain,    /* may be NULL */
    DNSServiceBrowseReply               callBack,
    void                                *context    /* may be NULL */
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_browse)
    return (*dnssd_browse)(sdRef, flags, interfaceIndex, regtype, domain, callBack, context);
  else
    return (-1);
}


// DNSServiceConstructFullName
DNSServiceErrorType DNSSD_API DNSServiceConstructFullName
    (
    char                            * const fullName,
    const char                      * const service,      /* may be NULL */
    const char                      * const regtype,
    const char                      * const domain
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_construct_full_name)
    return (*dnssd_construct_full_name)(fullName, service, regtype, domain);
  else
    return (-1);
}


// DNSServiceCreateConnection
DNSServiceErrorType DNSSD_API DNSServiceCreateConnection(DNSServiceRef *sdRef)
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_create_connection)
    return (*dnssd_create_connection)(sdRef);
  else
    return (-1);
}


// DNSServiceProcessResult
DNSServiceErrorType DNSSD_API DNSServiceProcessResult(DNSServiceRef sdRef)
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_process_result)
    return (*dnssd_process_result)(sdRef);
  else
    return (-1);
}


// DNSServiceQueryRecord
DNSServiceErrorType DNSSD_API DNSServiceQueryRecord
    (
    DNSServiceRef                       *sdRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    const char                          *fullname,
    uint16_t                            rrtype,
    uint16_t                            rrclass,
    DNSServiceQueryRecordReply          callBack,
    void                                *context  /* may be NULL */
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_query_record)
    return (*dnssd_query_record)(sdRef, flags, interfaceIndex, fullname, rrtype, rrclass, callBack, context);
  else
    return (-1);
}


// DNSServiceRefDeallocate
void DNSSD_API DNSServiceRefDeallocate(DNSServiceRef sdRef)
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_deallocate)
    return (*dnssd_deallocate)(sdRef);
  else
    return (-1);
}


// DNSServiceRefSockFD
int DNSSD_API DNSServiceRefSockFD(DNSServiceRef sdRef)
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_sock_fd)
    return (*dnssd_sock_fd)(sdRef);
  else
    return (-1);
}


// DNSServiceRegister
DNSServiceErrorType DNSSD_API DNSServiceRegister
    (
    DNSServiceRef                       *sdRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    const char                          *name,         /* may be NULL */
    const char                          *regtype,
    const char                          *domain,       /* may be NULL */
    const char                          *host,         /* may be NULL */
    uint16_t                            port,          /* In network byte order */
    uint16_t                            txtLen,
    const void                          *txtRecord,    /* may be NULL */
    DNSServiceRegisterReply             callBack,      /* may be NULL */
    void                                *context       /* may be NULL */
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_register)
    return (*dnssd_register)(sdRef, flags, interfaceIndex, name, regtype, domain, host, port, txtLen, txtRecord, callBack, context);
  else
    return (-1);
}


// DNSServiceRemoveRecord
DNSServiceErrorType DNSSD_API DNSServiceRemoveRecord
    (
    DNSServiceRef                 sdRef,
    DNSRecordRef                  RecordRef,
    DNSServiceFlags               flags
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_remove_record)
    return (*dnssd_remove_record)(sdRef, RecordRef, flags);
  else
    return (-1);
}


// DNSServiceResolve
DNSServiceErrorType DNSSD_API DNSServiceResolve
    (
    DNSServiceRef                       *sdRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    const char                          *name,
    const char                          *regtype,
    const char                          *domain,
    DNSServiceResolveReply              callBack,
    void                                *context  /* may be NULL */
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_resolve)
    return (*dnssd_resolve)(sdRef, flags, interfaceIndex, name, regtype, domain, callBack, context);
  else
    return (-1);
}


// DNSServiceUpdateRecord
DNSServiceErrorType DNSSD_API DNSServiceUpdateRecord
    (
    DNSServiceRef                       sdRef,
    DNSRecordRef                        RecordRef,     /* may be NULL */
    DNSServiceFlags                     flags,
    uint16_t                            rdlen,
    const void                          *rdata,
    uint32_t                            ttl
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_update_record)
    return (*dnssd_update_record)(sdRef, RecordRef, flags, rdlen, rdata, ttl);
  else
    return (-1);
}


// TXTRecordCreate
void DNSSD_API TXTRecordCreate
    (
    TXTRecordRef     *txtRecord,
    uint16_t         bufferLen,
    void             *buffer
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_txt_create)
    (*dnssd_txt_create)(txtRecord, bufferLen, buffer);
}


// TXTRecordDeallocate
void DNSSD_API TXTRecordDeallocate
    (
    TXTRecordRef     *txtRecord
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_txt_deallocate)
    (*dnssd_txt_deallocate)(txtRecord);
}


// TXTRecordGetBytesPtr
const void * DNSSD_API TXTRecordGetBytesPtr
    (
    const TXTRecordRef *txtRecord
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_txt_get_bytes_ptr)
    return (*dnssd_txt_get_bytes_ptr)(txtRecord);
  else
    return (NULL);
}


// TXTRecordGetLength
uint16_t DNSSD_API TXTRecordGetLength
    (
    const TXTRecordRef *txtRecord
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_txt_get_length)
    return (*dnssd_txt_get_length)(txtRecord);
  else
    return (0);
}


// TXTRecordSetValue
DNSServiceErrorType DNSSD_API TXTRecordSetValue
    (
    TXTRecordRef     *txtRecord,
    const char       *key,
    uint8_t          valueSize,        /* may be zero */
    const void       *value            /* may be NULL */
    )
{
  if (!dnssd_initialized)
    dnssd_init();

  if (dnssd_txt_set_value)
    return (*dnssd_txt_set_value)(txtRecord, key, valueSize, value);
  else
    return (-1);
}


