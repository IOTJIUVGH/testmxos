/**
 ******************************************************************************
 * @file    http_client.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   MiCO http client demo to read data from www.baidu.com
 ******************************************************************************
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include "mxos.h"
#include "HTTPUtils.h"
#include "SocketUtils.h"
#include "StringUtils.h"

#define http_client_log(M, ...) custom_log("HTTP", M, ##__VA_ARGS__)

static merr_t onReceivedData( struct _HTTPHeader_t * httpHeader, uint32_t pos, uint8_t *data, size_t len, void * userContext );
static void onClearData( struct _HTTPHeader_t * inHeader, void * inUserContext );

static mos_semphr_id_t wait_sem = NULL;

typedef struct _http_context_t
{
    char *content;
    uint64_t content_length;
} http_context_t;

merr_t simple_http_get( char* host, char* query );
merr_t simple_https_get( char* host, char* query );

#define SIMPLE_GET_REQUEST \
    "GET / HTTP/1.1\r\n" \
    "Host: www.baidu.com\r\n" \
    "Connection: close\r\n" \
    "\r\n"

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    UNUSED_PARAMETER( inContext );
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            if ( wait_sem != NULL )
            {
                mos_semphr_release( wait_sem );
            }
            break;
        case NOTIFY_STATION_DOWN:
            case NOTIFY_AP_UP:
            case NOTIFY_AP_DOWN:
            break;
    }
}

/* Only return the first address of a domain */
static merr_t dns_lookup_by_family( char* host, char* port, int ai_family, struct sockaddr_storage* sockaddr )
{
    merr_t err = kNoErr;
    struct addrinfo hints;
    struct addrinfo *result = NULL;

    /* Resolve DNS address */
    memset( &hints, 0, sizeof(struct addrinfo) );
    hints.ai_family = ai_family; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0; /* Any protocol */

    err = getaddrinfo( host, port, &hints, &result );
    require_noerr( err, exit );
    require_action( result, exit, err = kNotFoundErr );

    memcpy(sockaddr, result->ai_addr, sizeof(struct sockaddr_storage) );

exit:
    freeaddrinfo( result ); /* No longer needed */
    return err;
}

static merr_t dns_lookup(char* host, char* port, struct sockaddr_storage* sockaddr)
{
    merr_t err = kNoErr;
    char ipstr[INET6_ADDRSTRLEN];
    struct sockaddr_in *sockaddr_v4 = (struct sockaddr_in *)sockaddr;
    struct sockaddr_in6 *sockaddr_v6 = (struct sockaddr_in6 *)sockaddr;

    /* Lookup IPv6 address */
    if ( kNoErr == dns_lookup_by_family(host, port, AF_INET6, sockaddr) ) {
        inet_ntop(AF_INET6, sockaddr_v6->sin6_addr.s6_addr, ipstr, INET6_ADDRSTRLEN);
        http_client_log("Server %s address: %s, port: %d", host, ipstr, ntohs(sockaddr_v6->sin6_port));
    }
    else if ( kNoErr == dns_lookup_by_family(host, port, AF_INET, sockaddr) ) {
        inet_ntop(AF_INET, &sockaddr_v4->sin_addr.s_addr, ipstr, INET_ADDRSTRLEN);
        http_client_log("Server %s address: %s, port: %d", host, ipstr, ntohs(sockaddr_v4->sin_port));
    }
    else {
        http_client_log("Server not available");
        goto exit;
    }
exit:
    return err;
}


int main( void )
{
    merr_t err = kNoErr;

    wait_sem = mos_semphr_new(1);

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) micoNotify_WifiStatusHandler, NULL );
    require_noerr( err, exit );

    /* Start MiCO system functions according to mxos_config.h */
    err = mxos_system_init( );
    require_noerr( err, exit );

    /* Wait for wlan connection*/
    mos_semphr_acquire(wait_sem, MOS_WAIT_FOREVER);
    http_client_log( "wifi connected successful" );

    /* Read http data from server */
    simple_http_get( "www.baidu.com", SIMPLE_GET_REQUEST );
    simple_https_get( "www.baidu.com", SIMPLE_GET_REQUEST );

exit:
    mxos_system_notify_remove( mxos_notify_WIFI_STATUS_CHANGED, (void *) micoNotify_WifiStatusHandler );
    if ( wait_sem != NULL )
    {
        mos_semphr_delete( wait_sem );
    }

    mos_thread_delete( NULL );
    return err;
}

merr_t simple_http_get( char* host, char* query )
{
    merr_t err = kNoErr;
    int client_fd = -1;
    fd_set readfds;
    struct sockaddr_storage sockaddr;

    HTTPHeader_t *httpHeader = NULL;
    http_context_t context = { NULL, 0 };

    /* Lookup server address */
    err = dns_lookup(host, "80", &sockaddr);
    require_noerr( err, exit);

    /*HTTPHeaderCreateWithCallback set some callback functions */
    httpHeader = HTTPHeaderCreateWithCallback( 1024, onReceivedData, onClearData, &context );
    require_action( httpHeader, exit, err = kNoMemoryErr );

    client_fd = socket( sockaddr.ss_family, SOCK_STREAM, IPPROTO_TCP );
    err = connect( client_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr) );
    require_noerr_string( err, exit, "connect http server failed" );

    /* Send HTTP Request */
    send( client_fd, query, strlen( query ), 0 );

    FD_ZERO( &readfds );
    FD_SET( client_fd, &readfds );

    select( client_fd + 1, &readfds, NULL, NULL, NULL );
    if ( FD_ISSET( client_fd, &readfds ) )
    {
        /*parse header*/
        err = SocketReadHTTPHeader( client_fd, httpHeader );
        switch ( err )
        {
            case kNoErr:
                PrintHTTPHeader( httpHeader );
                err = SocketReadHTTPBody( client_fd, httpHeader );/*get body data*/
                require_noerr( err, exit );
                /*get data and print*/
                http_client_log( "Content Data: %s", context.content );
                break;
            case EWOULDBLOCK:
                case kNoSpaceErr:
                case kConnectionErr:
                default:
                http_client_log("ERROR: HTTP Header parse error: %d", err);
                break;
        }
    }

exit:
    http_client_log( "Exit: Client exit with err = %d, fd: %d", err, client_fd );
    SocketClose( &client_fd );
    HTTPHeaderDestory( &httpHeader );
    return err;
}

int ssl_errno = 0;

merr_t simple_https_get( char* host, char* query )
{
    merr_t err = kNoErr;
    int client_fd = -1;
    fd_set readfds;
    struct sockaddr_storage sockaddr;

    mtls_t client_ssl = NULL;

    HTTPHeader_t *httpHeader = NULL;
    http_context_t context = { NULL, 0 };

    /* Lookup server address */
    err = dns_lookup(host, "443", &sockaddr);
    require_noerr( err, exit);

    /*HTTPHeaderCreateWithCallback set some callback functions */
    httpHeader = HTTPHeaderCreateWithCallback( 1024, onReceivedData, onClearData, &context );
    require_action( httpHeader, exit, err = kNoMemoryErr );

    client_fd = socket( sockaddr.ss_family, SOCK_STREAM, IPPROTO_TCP );
    err = connect( client_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr) );
    require_noerr_string( err, exit, "connect HTTPs server failed" );

    mtls_set_ver(TLS_V1_2_MODE);
    client_ssl = mtls_connect( client_fd, 0, NULL, &ssl_errno );
    require_string( client_ssl != NULL, exit, "ERROR: ssl disconnect" );

    /* Send HTTP Request */
    mtls_send( client_ssl, query, strlen( query ) );

    FD_ZERO( &readfds );
    FD_SET( client_fd, &readfds );

    select( client_fd + 1, &readfds, NULL, NULL, NULL );
    if ( FD_ISSET( client_fd, &readfds ) )
    {
        /*parse header*/
        err = SocketReadHTTPSHeader( client_ssl, httpHeader );
        switch ( err )
        {
            case kNoErr:
                PrintHTTPHeader( httpHeader );
                err = SocketReadHTTPSBody( client_ssl, httpHeader );/*get body data*/
                require_noerr( err, exit );
                /*get data and print*/
                http_client_log( "Content Data: %s", context.content );
                break;
            case EWOULDBLOCK:
                case kNoSpaceErr:
                case kConnectionErr:
                default:
                http_client_log("ERROR: HTTP Header parse error: %d", err);
                break;
        }
    }

exit:
    http_client_log( "Exit: Client exit with err = %d, fd: %d", err, client_fd );
    if ( client_ssl ) mtls_close( client_ssl );
    SocketClose( &client_fd );
    HTTPHeaderDestory( &httpHeader );
    return err;
}

/*one request may receive multi reply*/
static merr_t onReceivedData( struct _HTTPHeader_t * inHeader, uint32_t inPos, uint8_t * inData,
                                size_t inLen, void * inUserContext )
{
    merr_t err = kNoErr;
    http_context_t *context = inUserContext;
    if ( inHeader->chunkedData == false )
    { //Extra data with a content length value
        if ( inPos == 0 && context->content == NULL )
        {
            context->content = calloc( inHeader->contentLength + 1, sizeof(uint8_t) );
            require_action( context->content, exit, err = kNoMemoryErr );
            context->content_length = inHeader->contentLength;

        }
        memcpy( context->content + inPos, inData, inLen );
    } else
    { //extra data use a chunked data protocol
        http_client_log("This is a chunked data, %d", inLen);
        if ( inPos == 0 )
        {
            context->content = calloc( inHeader->contentLength + 1, sizeof(uint8_t) );
            require_action( context->content, exit, err = kNoMemoryErr );
            context->content_length = inHeader->contentLength;
        } else
        {
            context->content_length += inLen;
            context->content = realloc( context->content, context->content_length + 1 );
            require_action( context->content, exit, err = kNoMemoryErr );
        }
        memcpy( context->content + inPos, inData, inLen );
    }

    exit:
    return err;
}

/* Called when HTTPHeaderClear is called */
static void onClearData( struct _HTTPHeader_t * inHeader, void * inUserContext )
{
    UNUSED_PARAMETER( inHeader );
    http_context_t *context = inUserContext;
    if ( context->content )
    {
        free( context->content );
        context->content = NULL;
    }
}

