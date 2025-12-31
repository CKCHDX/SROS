/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for direct use of cert provisioner info server.
*
*/


#ifndef INFOSERVERCLIENT_H
#define INFOSERVERCLIENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>
#include <etelmm.h>
#include <f32file.h>


// CONSTANTS
_LIT( KBTICCertProvInfoServer, "CertProvisionerInfoServer" );
_LIT( KBTICCPEmulatorSn,  "123456789123456789" );

const TUid KBTICCertProvInfoServerUid3 = { 0x1027508A };

enum TPSMessages
  {
  EPSSerialNumber
  };

// CLASS DECLARATION

/**
*  RInfoServerClient class
*  Class for using directly Certificate Provisioner Info Server.
*
*  @lib
*  @since
*/
class RInfoServerClient : public RSessionBase
  {
    public:
        /**
        * Connects Certificate Provisioner Server
        * @param
        * @return
        **/
        TInt Connect();

        /**
        * Closes session to Certificate Provisioner Server
        * @param
        * @return
        **/
        void Close();

        /**
        * Gets Serial Number from Certificate Provisioner Info Server
        * @param
        * @return
        **/
        TInt GetSerialNumber( TDes& aSerialNumber );

    };

#endif  // INFOSERVERCLIENT_H