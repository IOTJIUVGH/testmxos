/**
 ******************************************************************************
 * @file    dns.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   Get the IP address from a host name.(DNS)
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
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

#define dns_log(M, ...) custom_log("DNS", M, ##__VA_ARGS__)

#define DOMAIN  "www.baidu.com"
#define DOMAIN2  "www.sohu.com"

static mos_semphr_id_t wait_sem = NULL;

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            mos_semphr_release( wait_sem );
            break;
        case NOTIFY_STATION_DOWN:
            case NOTIFY_AP_UP:
            case NOTIFY_AP_DOWN:
            break;
    }
}

/* Only return the first address of a domain */
static merr_t dns_lookup( char* domain, char* port, int ai_family, char* buf, int len )
{
    merr_t err = kNoErr;
    struct addrinfo hints;
    struct addrinfo *result = NULL, *rp;
    struct sockaddr_in6 *lookup_addr_v6;
    struct sockaddr_in *lookup_addr_v4;
    const char *ip_str;

    /* Resolve DNS address */
    memset( &hints, 0, sizeof(struct addrinfo) );
    hints.ai_family = ai_family; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0; /* Any protocol */

    err = getaddrinfo( domain, port, &hints, &result );
    require_noerr( err, exit );
    require_action( result, exit, err = kNotFoundErr );

    /* LWIP stack only get one address. */
    rp = result;
    if ( rp->ai_family == AF_INET6 ) {
        lookup_addr_v6 = (struct sockaddr_in6 *) rp->ai_addr;
        ip_str = inet_ntop( rp->ai_family, lookup_addr_v6->sin6_addr.s6_addr, buf, len );
        require_action( ip_str, exit, err = kMalformedErr );
    }
    else if ( rp->ai_family == AF_INET ) {
        lookup_addr_v4 = (struct sockaddr_in *) rp->ai_addr;
        ip_str = inet_ntop( rp->ai_family, &lookup_addr_v4->sin_addr.s_addr, buf, len );
        require_action( ip_str, exit, err = kMalformedErr );
    }

exit:
    freeaddrinfo( result ); /* No longer needed */
    return err;
}

int main( void )
{
    merr_t err = kNoErr;
    char ip_str_buf[INET6_ADDRSTRLEN];

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) micoNotify_WifiStatusHandler,
                                       NULL );
    require_noerr( err, exit );

    /* Start MiCO system functions according to mxos_config.h*/
    err = mxos_system_init( );
    require_noerr( err, exit );

    wait_sem = mos_semphr_new( 1 );
    require(NULL != wait_sem, exit);

    /* Wait for wlan connection*/
    while(kNoErr != mos_semphr_acquire( wait_sem, 30000 ))
    {
        dns_log("*** wifi connect 30s timeout!");
    }
    dns_log( "wifi connected successful." );

    while(1)
    {
    /* Lookup IPV4 address */
    if( kNoErr == dns_lookup( DOMAIN2, NULL, AF_INET, ip_str_buf, INET6_ADDRSTRLEN ) )
        dns_log( "%s ipv4 address: %s", DOMAIN, ip_str_buf);
    else
        dns_log( "%s ipv4 address not found", DOMAIN );

    /* Lookup IPV6 address */
    if( kNoErr == dns_lookup( DOMAIN, NULL, AF_INET6, ip_str_buf, INET6_ADDRSTRLEN ) )
        dns_log( "%s ipv6 address: %s", DOMAIN, ip_str_buf);
    else
        dns_log( "%s ipv6 address not found", DOMAIN );

    mos_sleep(10);
    }

exit:
    mxos_system_notify_remove( mxos_notify_WIFI_STATUS_CHANGED, (void *) micoNotify_WifiStatusHandler );
    if ( wait_sem != NULL ) mos_semphr_delete( wait_sem );
    return err;
}
