/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for program's defines.
*
*/


#ifndef INTEGRITYCHECKDEFS_H
#define INTEGRITYCHECKDEFS_H

// INCLUDES 
#include    <e32base.h>

// CONSTANTS 

const TInt KBTICCertProvUID = 0x101FB665;          //UID for CertProvisioner       
const TInt KBTICSymbianSWIUpdaterUID = 0x101FB666; //UID for Symbian SWI updater

// For client server

enum TBTICServerCommand
    {    
    EBTICBootReasonSW,    
    };

enum TBTICBootReason
    { 
    EBTICNoSWReason = 200,   
    EBTICNormalBoot = 201,     
    EBTICRestoreFactorySetDeep = 202,            
    };
    
// DATA TYPES 

// FUNCTION PROTOTYPES 

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES 

// CLASS DECLARATION 

#endif

//EOF 
