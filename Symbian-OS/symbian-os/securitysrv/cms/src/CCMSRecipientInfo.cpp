/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/


// INCLUDE FILES
#include    "CCMSRecipientInfo.h"
#include "CCMSX509AlgorithmIdentifier.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSRecipientInfo::CCMSRecipientInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCMSRecipientInfo::CCMSRecipientInfo( TInt aVersion )
    : iVersion( aVersion )
    {
    }

// -----------------------------------------------------------------------------
// CCMSRecipientInfo::BaseConstructL
// Constructs the member variables. Makes a copy of aKeyEncryptionAlgorithm.
// -----------------------------------------------------------------------------
//
void CCMSRecipientInfo::BaseConstructL(
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm )
    {
    SetKeyEncryptionAlgorithmL( aKeyEncryptionAlgorithm );
    }

// -----------------------------------------------------------------------------
// CCMSRecipientInfo::AddVersionL
// Adds version encoding to root sequence
// -----------------------------------------------------------------------------
//
void CCMSRecipientInfo::AddVersionL( CASN1EncSequence* aRoot ) const
    {
    CASN1EncInt* version = CASN1EncInt::NewLC( iVersion );
    aRoot->AddAndPopChildL( version );
    }

// -----------------------------------------------------------------------------
// CCMSRecipientInfo::AddKeyEncryptionAlgorithmL
// Adds KeyEncryptionAlgorithm encoding to root sequence
// -----------------------------------------------------------------------------
//
void CCMSRecipientInfo::AddKeyEncryptionAlgorithmL( CASN1EncSequence* aRoot ) const
    {
    HBufC8* encodedKeyEncryptionAlgorithm = NULL;
    iKeyEncryptionAlgorithm->EncodeL( encodedKeyEncryptionAlgorithm );
    CleanupStack::PushL( encodedKeyEncryptionAlgorithm );
    CASN1EncEncoding* keyEncryptionAlgorithm =
        CASN1EncEncoding::NewLC( *encodedKeyEncryptionAlgorithm );
    aRoot->AddAndPopChildL( keyEncryptionAlgorithm );
    CleanupStack::PopAndDestroy( encodedKeyEncryptionAlgorithm );
    }

// -----------------------------------------------------------------------------
// CCMSRecipientInfo::~CCMSRecipientInfo
// Destructor
// -----------------------------------------------------------------------------
//
CCMSRecipientInfo::~CCMSRecipientInfo()
    {
    delete iKeyEncryptionAlgorithm;
    }

// -----------------------------------------------------------------------------
// CCMSRecipientInfo::Version
// Version getter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCMSRecipientInfo::Version() const
    {
    return iVersion;
    }

// -----------------------------------------------------------------------------
// CCMSRecipientInfo::KeyEncryptionAlgorithm
// KeyEncryptionAlgorithm getter
// -----------------------------------------------------------------------------
//
EXPORT_C const CCMSX509AlgorithmIdentifier&
CCMSRecipientInfo::KeyEncryptionAlgorithm() const
    {
    return *iKeyEncryptionAlgorithm;
    }

// -----------------------------------------------------------------------------
// CCMSRecipientInfo::SetKeyEncryptionAlgorithmL
// KeyEncryptionAlgorithm setter
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSRecipientInfo::SetKeyEncryptionAlgorithmL(
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm )
    {
    CCMSX509AlgorithmIdentifier* algorithmIdentifier =
        CCMSX509AlgorithmIdentifier::NewL(
            aKeyEncryptionAlgorithm.AlgorithmIdentifier() );
    CleanupStack::PushL( algorithmIdentifier );
    const CAlgorithmIdentifier* digestAlgorithm =
        aKeyEncryptionAlgorithm.DigestAlgorithm();
    if( digestAlgorithm )
        {
        algorithmIdentifier->SetDigestAlgorithmL( digestAlgorithm );
        }
    CleanupStack::Pop( algorithmIdentifier );
    delete iKeyEncryptionAlgorithm;
    iKeyEncryptionAlgorithm = algorithmIdentifier;
    }


//  End of File
