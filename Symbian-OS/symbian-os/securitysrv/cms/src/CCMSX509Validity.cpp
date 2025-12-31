/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
#include    "CCMSX509Validity.h"
#include "TCMSTimeUtil.h"
#include <signed.h>
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KNumberOfSubModules = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509Validity::CCMSX509Validity
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Validity::CCMSX509Validity(
    const TTime& aNotBefore,
    const TTime& aNotAfter )
    : iNotBefore( aNotBefore ), iNotAfter( aNotAfter )
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509Validity::CCMSX509Validity
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Validity::CCMSX509Validity( )
    {
    // CBase initilises member data to zero, which is good enough
    }

// -----------------------------------------------------------------------------
// CCMSX509Validity::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Validity*
CCMSX509Validity::NewL()
	{
	// creating with empty values
    CCMSX509Validity* self =
        new( ELeave ) CCMSX509Validity();
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509Validity::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Validity*
CCMSX509Validity::NewL(
    const TTime& aNotBefore,
    const TTime& aNotAfter )
    {
    CCMSX509Validity* self =
        new( ELeave ) CCMSX509Validity( aNotBefore, aNotAfter ); 
    return self;
    }

// -----------------------------------------------------------------------------
// CCMSX509Validity::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Validity*
CCMSX509Validity::NewL( const CValidityPeriod& aValidityPeriod )
    {
    CCMSX509Validity* self =
        new( ELeave ) CCMSX509Validity( aValidityPeriod.Start(),
                                        aValidityPeriod.Finish() ); 
    return self;
    }

// Destructor
CCMSX509Validity::~CCMSX509Validity()
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509Validity::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509Validity::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* itemList = DecodeSequenceLC(
        aRawData, KNumberOfSubModules, KNumberOfSubModules );

    TInt sequenceCounter = 0;

    // decode notBefore
    TTime notBefore = TCMSTimeUtil::ConvertToTimeL(
        itemList->At( sequenceCounter++ )->Encoding() );

    // decode notAfter
    TTime notAfter = TCMSTimeUtil::ConvertToTimeL(
        itemList->At( sequenceCounter )->Encoding() );

    // all done, change state
    iNotBefore = notBefore;
    iNotAfter = notAfter;

    CleanupStack::PopAndDestroy( itemList );
	}

// -----------------------------------------------------------------------------
// CCMSX509Validity::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509Validity::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode notBefore
    CASN1EncBase* notBefore = TCMSTimeUtil::ConvertToEncoderLC( iNotBefore );
    root->AddAndPopChildL( notBefore );

    // encode notAfter
    CASN1EncBase* notAfter = TCMSTimeUtil::ConvertToEncoderLC( iNotAfter );
    root->AddAndPopChildL( notAfter );

    return root;
	}

// -----------------------------------------------------------------------------
// CCMSX509Validity::NotBefore()
// Getter for notBefore
// -----------------------------------------------------------------------------
EXPORT_C const TTime&
CCMSX509Validity::NotBefore() const
	{
	return iNotBefore;
	}

// -----------------------------------------------------------------------------
// CCMSX509Validity::NotAfter()
// Getter for notAfter
// -----------------------------------------------------------------------------
EXPORT_C const TTime&
CCMSX509Validity::NotAfter() const
	{
	return iNotAfter;
	}

// -----------------------------------------------------------------------------
// CCMSX509Validity::SetNotBefore()
// Setter for notBefore
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Validity::SetNotBefore(
    const TTime& aNotBefore )
	{
    iNotBefore = aNotBefore;
	}

// -----------------------------------------------------------------------------
// CCMSX509Validity::SetNotAfter()
// Setter for notAfter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Validity::SetNotAfter(
    const TTime& aNotAfter )
	{
    iNotAfter = aNotAfter;
	}

//  End of File
