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
#include    "CCMSX509IssuerSerial.h"
#include "CCMSX509GeneralNames.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KMinNumberOfSubModules = 2;
const TInt KMaxNumberOfSubModules = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::CCMSX509IssuerSerial
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509IssuerSerial::CCMSX509IssuerSerial( const TInt aSerial)
    : iSerial( aSerial )
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509IssuerSerial::ConstructL(
    const CCMSX509GeneralNames& aIssuer )
    {
    SetIssuerL( aIssuer );
    }

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509IssuerSerial::ConstructL(
    const CCMSX509GeneralNames& aIssuer,
    const TDesC8& aIssuerUID )
    {
    SetIssuerL( aIssuer );
    SetIssuerUIDL( aIssuerUID );
    }


// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509IssuerSerial* CCMSX509IssuerSerial::NewL()
	{
	// creating with empty/default values
    CCMSX509IssuerSerial* self = new( ELeave ) CCMSX509IssuerSerial( 0 );
    CleanupStack::PushL( self );
    CCMSX509GeneralNames* issuer = CCMSX509GeneralNames::NewL();
    self->iIssuer = issuer;
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509IssuerSerial* CCMSX509IssuerSerial::NewL(
    const CCMSX509GeneralNames& aIssuer,
    const TInt aSerial ) 
	{
    CCMSX509IssuerSerial* self =
    	new( ELeave ) CCMSX509IssuerSerial( aSerial );

    CleanupStack::PushL( self );
    self->ConstructL( aIssuer );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509IssuerSerial* CCMSX509IssuerSerial::NewL(
    const CCMSX509GeneralNames& aIssuer,
    const TInt aSerial,
    const TDesC8& aIssuerUID )
	{
    CCMSX509IssuerSerial* self =
    	new( ELeave ) CCMSX509IssuerSerial( aSerial );

    CleanupStack::PushL( self );
    self->ConstructL( aIssuer, aIssuerUID );
    CleanupStack::Pop( self );
	return self;
	}


// Destructor
CCMSX509IssuerSerial::~CCMSX509IssuerSerial()
    {
	delete iIssuer;
    delete iIssuerUID;
    }

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509IssuerSerial::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* itemList = DecodeSequenceLC(
        aRawData, KMinNumberOfSubModules, KMaxNumberOfSubModules );

    TInt sequenceCounter = 0;
    
    // decode issuer
    CCMSX509GeneralNames* issuer = CCMSX509GeneralNames::NewL();
    CleanupStack::PushL( issuer );
    issuer->DecodeL( itemList->At( sequenceCounter++ )->Encoding() );

    // decode serial
    TASN1DecInteger intDecoder;
    TInt serial =
        intDecoder.DecodeDERShortL( *( itemList->At( sequenceCounter++ ) ) );

    HBufC8* issuerUID = NULL;
    // decode issuerUID, if exists
    if( itemList->Count() > sequenceCounter )
        {
        TASN1DecBitString bsDecoder;
        issuerUID = bsDecoder.ExtractOctetStringL(
            *( itemList->At( sequenceCounter ) ) );
        }

    // Everything has been created, we can change state
    delete iIssuer;
    iIssuer = issuer;
    iSerial = serial;
    delete iIssuerUID;
    iIssuerUID = issuerUID;

    CleanupStack::Pop( issuer );
    CleanupStack::PopAndDestroy( itemList );
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509IssuerSerial::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode issuer
    CASN1EncBase* issuer = iIssuer->EncoderLC();
    root->AddAndPopChildL( issuer );

    // encode serial
    CASN1EncInt* serial = CASN1EncInt::NewLC( iSerial );
    root->AddAndPopChildL( serial );

    // encode issuerUID
    if( iIssuerUID )
        {
        CASN1EncBitString* issuerUID = CASN1EncBitString::NewLC( *iIssuerUID );
        root->AddAndPopChildL( issuerUID );
        }
    
    return root;
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::Issuer
// Issuer getter
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509GeneralNames&
CCMSX509IssuerSerial::Issuer() const
	{
	return *iIssuer;
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::Serial
// Serial getter
// -----------------------------------------------------------------------------
EXPORT_C TInt
CCMSX509IssuerSerial::Serial() const
	{
	return iSerial;
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::IssuerUID
// IssuerUID getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8*
CCMSX509IssuerSerial::IssuerUID() const
	{
	return iIssuerUID;
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::SetIssuerL
// Issuer setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509IssuerSerial::SetIssuerL(
	const CCMSX509GeneralNames& aIssuer )
	{
    CCMSX509GeneralNames* issuer = CCMSX509GeneralNames::NewL( aIssuer );
    delete iIssuer;
    iIssuer = issuer;
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::SetSerial
// Serial setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509IssuerSerial::SetSerial(
	const TInt aSerial )
	{
    iSerial = aSerial;
	}

// -----------------------------------------------------------------------------
// CCMSX509IssuerSerial::SetIssuerUIDL
// IssuerUID setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509IssuerSerial::SetIssuerUIDL(
	const TDesC8& aIssuerUID )
	{
    HBufC8* issuerUID = aIssuerUID.AllocL();
    delete iIssuerUID;
    iIssuerUID = issuerUID;
	}

//  End of File
