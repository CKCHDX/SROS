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
* Description:  Definitions for smart card
*
*/


#ifndef SCARDDEFS_H
#define SCARDDEFS_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS (these has to be before data types)
class CScardCommandTimer;

const TInt KScardReaderMaxNameLength = 32; // Maximum data lengths

//  DATA TYPES  

// Database string aliases
typedef TBuf<KScardReaderMaxNameLength> TScardReaderName;

typedef TInt16 TReaderID;
typedef TInt16 TGroupID;

enum TScardServiceStatus
    {
    EScardReseted,
    EScardInserted,
    EScardRemoved,
    EReaderRemoved,
    ECommunicationError
    };

enum TPowerAction
    {
    EScardPowerUp,
    EScardPowerDown,
    EScardReset
    };

enum TScChannelManagement
    {
    EOpenAnyChannel = 1,
    ECloseChannel,
    EChannelStatus
    };

//  CScardAccessControl & CScardMessageRegistry
struct TMessageHandle
    {
    RMessage2    iMessage;
    TInt        iSessionID;
    TReaderID   iReaderID;
    TBool       iCancelled;
    TInt8       iChannel;

    CScardCommandTimer* iTimer;
    TInt        iAdditionalParameter;

    TMessageHandle( const RMessage2& aMessage,
                    const TInt aSessionID,
                    const TReaderID aReaderID,
                    const TInt8 aChannel, 
                    const TInt8 aAddition = 0 );
    TMessageHandle();
    };

//  CScardConnector & CScardEventStack
struct TQueueEvent
    {
    TReaderID   iReaderID;
    TScardServiceStatus iEventType;
    };

// Answer To Reset
const TInt KMaxATRBytes           = 33; 
const TInt KMaxATRHistoricalBytes = 15;
typedef TBuf8<KMaxATRBytes> TScardATR; // ATR bytes

#endif      // SCARDDEFS_H

// End of File
