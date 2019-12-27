/**
******************************************************************************
* @file    mxos_config.h
* @author  William Xu
* @version V1.0.0
* @date    08-Aug-2018
* @brief   This file provide application options diff to default.
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

#ifndef __MXOS_CONFIG_H
#define __MXOS_CONFIG_H

#define MXOS_DEFAULT_APPLICATION_STACK_SIZE     4000

#define CONFIG_LWS_DEBUG     MXOS_DEBUG_OFF
#define MXOS_DEBUG_MIN_LEVEL MXOS_DEBUG_LEVEL_ALL


#define CONFIG_LWS_DEBUG_PARSER     MXOS_DEBUG_OFF
#define CONFIG_LWS_DEBUG_HEADER     MXOS_DEBUG_OFF
#define CONFIG_LWS_DEBUG_EXT        MXOS_DEBUG_OFF
#define CONFIG_LWS_DEBUG_CLIENT     MXOS_DEBUG_OFF
#define CONFIG_LWS_DEBUG_LATENCY    MXOS_DEBUG_OFF

#endif
