/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of loggers
*
*/


#ifdef _DEBUG

#include    "GBALogger.h"
#include    <flogger.h>

const TInt KDebugBufferSize1024         = 1024;
const TInt KDebugBufferSize512          = 512;
const TInt KDebugBufferSize256          = 256;
const TInt KDebugBufferFragmentSize     = 120;
const TInt KDebugBufferSize128          = 128;

_LIT(KTimeFormat, "%F%D/%M/%Y %J:%T:%S");

void debugline(const char *aFormatString, void *aArg1, void* aArg2, void *aArg3, void *aArg4, const char *aArg5)
    {
    TBuf8<KDebugBufferSize1024> logbuffer;
    TPtrC8 p((const TUint8 *)aFormatString);
    TPtrC8 tempArg5((const TUint8 *)aArg5);
    logbuffer.Format(p, aArg1,aArg2, aArg3, aArg4);
    
    if( KDebugBufferSize1024 < logbuffer.Size() + tempArg5.Size() )
        return;
    
    logbuffer.Append(tempArg5);
    
    TInt iterator = 0;
    
    TBuf8<KDebugBufferSize256> tempbuffer;
    TInt bufferSize = logbuffer.Size();
    while( iterator < bufferSize )
        {
        if( (bufferSize - iterator) > KDebugBufferFragmentSize )
            {
            tempbuffer.Copy(logbuffer.Mid(iterator, KDebugBufferFragmentSize));
            RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                        EFileLoggingModeAppend,
                                        _L8("%S"), &tempbuffer);
            }
        else
            {
            tempbuffer.Copy(logbuffer.Mid(iterator, bufferSize - iterator));
            RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                      EFileLoggingModeAppend,
                                      _L8("%S"), &tempbuffer);
            }
        iterator += KDebugBufferFragmentSize;
        }
    }

void debugline(const char *aFormatString, void *aArg1, void* aArg2, void *aArg3, void *aArg4, const TDesC &aArg5)
    {
    TBuf8<KDebugBufferSize1024> logbuffer;
    TPtrC8 p((const TUint8 *)aFormatString);
    logbuffer.Format(p, aArg1,aArg2, aArg3, aArg4);
    
    if( KDebugBufferSize1024 < logbuffer.Size() + aArg5.Length() )
        return;
    
    logbuffer.Append(aArg5);
    
    TInt iterator = 0;
    
    TBuf8<KDebugBufferSize256> tempbuffer;
    TInt bufferSize = logbuffer.Size();
    while( iterator < bufferSize )
        {
        if( (bufferSize - iterator) > KDebugBufferFragmentSize )
            {
            tempbuffer.Copy(logbuffer.Mid(iterator, KDebugBufferFragmentSize));
            RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                        EFileLoggingModeAppend,
                                        _L8("%S"), &tempbuffer);
            }
        else
            {
            tempbuffer.Copy(logbuffer.Mid(iterator, bufferSize - iterator));
            RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                        EFileLoggingModeAppend,
                                        _L8("%S"), &tempbuffer);
            }
        iterator += KDebugBufferFragmentSize;
        }
    }

void debugline(const char *aFormatString, void *aArg1, void *aArg2, void *aArg3, void *aArg4, const TDesC8 &aArg5)
    {
    TBuf8<KDebugBufferSize1024> logbuffer;
    TPtrC8 p((const TUint8 *)aFormatString);
    logbuffer.Format(p, aArg1,aArg2, aArg3, aArg4);
    
    if( KDebugBufferSize1024 < logbuffer.Size() + aArg5.Size() )
        return;
    
    logbuffer.Append(aArg5);
    
    TInt iterator = 0;
    
    TBuf8<KDebugBufferSize256> tempbuffer;
    TInt bufferSize = logbuffer.Size();
    while( iterator < bufferSize )
        {
        if( (bufferSize - iterator) > KDebugBufferFragmentSize )
            {
            tempbuffer.Copy(logbuffer.Mid(iterator, KDebugBufferFragmentSize));
            RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                        EFileLoggingModeAppend,
                                        _L8("%S"), &tempbuffer);
            }
        else
            {
            tempbuffer.Copy(logbuffer.Mid(iterator, bufferSize - iterator));
            RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                        EFileLoggingModeAppend,
                                        _L8("%S"), &tempbuffer);
            }
        iterator += KDebugBufferFragmentSize;
        }
    }

void debugline(const char *aFormatString, void *aArg1, void *aArg2, void *aArg3, void *aArg4, const char *aArg5, const TInt& aNum )
    {
    TBuf8<KDebugBufferSize512> logbuffer;
    TPtrC8 tempArg5((const TUint8 *)aArg5);
    if( tempArg5.Size() > KDebugBufferSize512 - KDebugBufferSize128 )
        return;
    
    TPtrC8 p((const TUint8 *)aFormatString);
    logbuffer.Format(p, aArg1,aArg2, aArg3, aArg4);
    //Print in two seperate lines
    RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                                EFileLoggingModeAppend,
                                                _L8("%S"), &logbuffer);
    RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                            EFileLoggingModeAppend,
                                            _L8(aArg5), aNum);
    }  

void debugBinary( const TDesC8 &aBuffer )
    {
    RFileLogger::HexDump( KGBALogDir, KGBALogFileName,
                                EFileLoggingModeAppend,
                                NULL, NULL , aBuffer.Ptr(), aBuffer.Length() );
    }

void debugTTime( TTime& aTime )
    {
    TBuf<KDebugBufferSize256> buf;
    TRAPD(error , aTime.FormatL( buf, KTimeFormat));
    if(error != KErrNone)
        return;
    RFileLogger::WriteFormat( KGBALogDir, KGBALogFileName,
                                EFileLoggingModeAppend,
                                buf );
    }

#endif

//EOF
