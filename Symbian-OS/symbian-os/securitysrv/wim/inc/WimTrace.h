/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Defines for RDebug Traces
*
*/


#ifndef WIMTRACE_H
#define WIMTRACE_H


#ifdef __WIM_ENABLE_TRACES
#include <e32svr.h>
#define _WIMTRACE( a ) RDebug::Print( a )
#define _WIMTRACE2( a , b ) RDebug::Print( a , b )
#define _WIMTRACE3( a , b , c ) RDebug::Print( a , b , c )
#define _WIMTRACE4( a , b , c , d ) RDebug::Print( a , b , c , d )
#define _WIMTRACE5( a , b , c , d , e ) RDebug::Print( a , b , c , d , e )
#define _WIMTRACE6( a , b , c , d , e , f ) RDebug::Print( a , b , c , d , e , f )
#define _WIMTRACE7( a , b , c , d , e , f , g ) RDebug::Print( a , b , c , d , e , f , g )
#define _WIMTRACE8( a , b , c , d , e , f , g , h ) RDebug::Print( a , b , c , d , e , f , g , h )
#define _WIMTRACE9( a , b , c , d , e , f , g , h , i ) RDebug::Print( a , b , c , d , e , f , g , h , i )
#define _WIMTRACE10( a , b , c , d , e , f , g , h , i , j ) RDebug::Print( a , b , c , d , e , f , g , h , i , j )
#define _WIMTRACE11( a , b , c , d , e , f , g , h , i , j , k ) RDebug::Print( a , b , c , d , e , f , g , h , i , j , k )
#else
#define _WIMTRACE( a ) 
#define _WIMTRACE2( a , b ) 
#define _WIMTRACE3( a , b , c ) 
#define _WIMTRACE4( a , b , c , d ) 
#define _WIMTRACE5( a , b , c , d , e ) 
#define _WIMTRACE6( a , b , c , d , e , f ) 
#define _WIMTRACE7( a , b , c , d , e , f , g ) 
#define _WIMTRACE8( a , b , c , d , e , f , g , h ) 
#define _WIMTRACE9( a , b , c , d , e , f , g , h , i ) 
#define _WIMTRACE10( a , b , c , d , e , f , g , h , i , j ) 
#define _WIMTRACE11( a , b , c , d , e , f , g , h , i , j , k ) 
#endif

#endif
