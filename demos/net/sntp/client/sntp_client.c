/**
 ******************************************************************************
 * @file    sntp_client
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   Time sync with MTP server
 ******************************************************************************
 *
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

#include <time.h>
#include "mxos.h"
#include "sntp.h"

#define TIME_SYNC_PERIOD    (60 * SECONDS)

#define sntp_demo_log(M, ...) custom_log("SNTP DEMO", M, ##__VA_ARGS__)

/* Callback function when MiCO UTC time in sync to NTP server */
static void sntp_time_synced( void )
{
    struct tm *     currentTime;
    iso8601_time_t  iso8601_time;
    mxos_utc_time_t utc_time;

    mxos_time_get_iso8601_time( &iso8601_time );
    sntp_demo_log("sntp_time_synced: %.26s", (char*)&iso8601_time);
}

int main( void )
{
    merr_t           err = kNoErr;
    struct tm*         utc_time;
    mxos_utc_time_ms_t utc_time_ms;
    iso8601_time_t     iso8601_time;

    /* Start MiCO system functions according to mxos_config.h*/
    err = mxos_system_init( );
    require_noerr( err, exit );

    /* Start auto sync with NTP server */
    sntp_start_auto_time_sync( TIME_SYNC_PERIOD, sntp_time_synced );

    /* Print current time from MiCO system every 10 seconds */
    while ( 1 )
    {
        mxos_time_get_iso8601_time( &iso8601_time );

        sntp_demo_log("Current time: %.26s", (char*)&iso8601_time);
        mos_msleep( 10 * 1000 );
    }

exit:
    return 0;
}

