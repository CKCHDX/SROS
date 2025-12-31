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
* Description:  Client header file
*
*/


#ifndef INTEGRITYCHECKCLIENT_H
#define INTEGRITYCHECKCLIENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

// CONSTANTS 

const TUid KBTICServerUid3 = { 0x101FB661 };
_LIT( KBTICServer,"IntegrityCheckServer" );
_LIT( KBTICServerName,"!IntegrityCheckServer" );

// MACROS

// DATA TYPES 

// FUNCTION PROTOTYPES 

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES 

// CLASS DECLARATION 

/**
*  RIntegrityCheckClient class
*  Client for Integrity Check server.
*
*  @lib
*  @since 3.1
*/
class RIntegrityCheckClient : public RSessionBase
    {
    public:
    
        /**
        * Connects Integrity Check Server      
        * @return
        **/
        IMPORT_C TInt Connect();

        /**
        * Closes session to server       
        **/
        IMPORT_C void Close();

        /**
        * Get sw boot reason from system. 
        * @param aBootReason
        * @return System wide error code.
        **/
        //IMPORT_C TInt GetSWBootReason( TDes& aBootReason );
        IMPORT_C TInt GetSWBootReason( TInt& aBootReason );

    };

#endif  // INTEGRITYCHECKCLIENT_H

//EOF
