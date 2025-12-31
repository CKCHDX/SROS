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
* Description:  Implementation of CFileLog
*
*/


#include "DevEncLog.h"
#include <flogger.h>

#if defined( _DEBUG ) && defined ( DEVENC_LOG )

_LIT( KDevEncLogDir, "DevEnc");
_LIT( KDevEncLogFile, "DevEncdiskutils.log");

void CFileLog::Printf( TRefByValue<const TDesC> aFmt, ... )
	{
	VA_LIST list;
	VA_START( list, aFmt );
	RFileLogger::WriteFormat( KDevEncLogDir,
                              KDevEncLogFile,
	                          EFileLoggingModeAppend,
	                          aFmt,
	                          list );
	}

#endif

// End of File
