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
#include    "CCMSOriginatorPublicKey.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KNumberOfSubModules = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::CCMSOriginatorPublicKey
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorPublicKey::CCMSOriginatorPublicKey()
    {
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSOriginatorPublicKey::ConstructL(
	const CCMSX509AlgorithmIdentifier& aAlgorithm,
	const TDesC8& aPublicKey )
    {
	SetAlgorithmL( aAlgorithm );
	SetPublicKeyL( aPublicKey );
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorPublicKey* CCMSOriginatorPublicKey::NewL()
	{
	// creating with empty/default values
	CCMSX509AlgorithmIdentifier* algorithm =
        CCMSX509AlgorithmIdentifier::NewL( );
	CleanupStack::PushL( algorithm );
	CCMSOriginatorPublicKey* self = NewL( *algorithm, KNullDesC8() );
	CleanupStack::PopAndDestroy( algorithm );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorPublicKey* CCMSOriginatorPublicKey::NewL(
    const CCMSOriginatorPublicKey& aOriginatorPublicKey )
	{
	CCMSOriginatorPublicKey* self = NewLC( aOriginatorPublicKey );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::NewLC
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorPublicKey* CCMSOriginatorPublicKey::NewLC(
    const CCMSOriginatorPublicKey& aOriginatorPublicKey )
	{
	CCMSOriginatorPublicKey* self = NewLC(
        aOriginatorPublicKey.Algorithm(), aOriginatorPublicKey.PublicKey() );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorPublicKey* CCMSOriginatorPublicKey::NewL(
	const CCMSX509AlgorithmIdentifier& aAlgorithm,
	const TDesC8& aPublicKey )
    {
    CCMSOriginatorPublicKey* self = NewLC( aAlgorithm, aPublicKey );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorPublicKey* CCMSOriginatorPublicKey::NewLC(
	const CCMSX509AlgorithmIdentifier& aAlgorithm,
	const TDesC8& aPublicKey )
    {
    CCMSOriginatorPublicKey* self =
    	new( ELeave ) CCMSOriginatorPublicKey();

    CleanupStack::PushL( self );
    self->ConstructL( aAlgorithm, aPublicKey );
    return self;
    }


// Destructor
CCMSOriginatorPublicKey::~CCMSOriginatorPublicKey()
    {
	delete iAlgorithm;
	delete iPublicKey;
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSOriginatorPublicKey::DecodeL( const TDesC8& aRawData )
	{
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    TASN1DecSequence decSeq;
    CArrayPtr< TASN1DecGeneric >* itemsData =
        decSeq.DecodeDERLC( decGen );
    TInt count = itemsData->Count();
    if( count != KNumberOfSubModules )
        {
        User::Leave( KErrArgument );
        }
	// we would not get this far if there is not 2 elements

	// decoding algorithm identifier
    CCMSX509AlgorithmIdentifier* algorithm =
        CCMSX509AlgorithmIdentifier::NewL( );
    CleanupStack::PushL( algorithm );
    algorithm->DecodeL( itemsData->At( 0 )->Encoding() );
    CleanupStack::Pop( algorithm );
    delete iAlgorithm;
    iAlgorithm = algorithm;

	// decoding public key
	TASN1DecGeneric gen( *itemsData->At( 1 ) );
	gen.InitL();
    // Symbian decoder can't handle zero length bit strings
    if( gen.LengthDERContent() > 1 )
        {
        TASN1DecBitString bitStringDecoder;
        HBufC8* tmp = bitStringDecoder.ExtractOctetStringL( gen );
        delete iPublicKey;
        iPublicKey = tmp;
        }
    else
        {
        HBufC8* tmp = KNullDesC8().AllocL();
        delete iPublicKey;
        iPublicKey = tmp;
        }

	CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSOriginatorPublicKey::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode the algorithm identifier
    HBufC8* encodedAlgorithm = NULL;
    iAlgorithm->EncodeL( encodedAlgorithm );
    CleanupStack::PushL( encodedAlgorithm );
    CASN1EncEncoding* algorithm = CASN1EncEncoding::NewLC( *encodedAlgorithm );
    root->AddAndPopChildL( algorithm );
    CleanupStack::PopAndDestroy( encodedAlgorithm );

    // add public key
    CASN1EncBitString* publicKey = CASN1EncBitString::NewLC( *iPublicKey );
    root->AddAndPopChildL( publicKey );

	return root;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::Algorithm
// Algorithm identifier getter
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AlgorithmIdentifier&
CCMSOriginatorPublicKey::Algorithm() const
	{
	return *iAlgorithm;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::PublicKey
// PublicKey getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8& CCMSOriginatorPublicKey::PublicKey() const
	{
	return *iPublicKey;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::SetAlgorithmL
// Algorithm setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSOriginatorPublicKey::SetAlgorithmL(
	const CCMSX509AlgorithmIdentifier& aAlgorithm )
	{
	CCMSX509AlgorithmIdentifier* algorithm = NULL;
    const CAlgorithmIdentifier& algorithmId = aAlgorithm.AlgorithmIdentifier();
    const CAlgorithmIdentifier* digestAlgorithm = aAlgorithm.DigestAlgorithm();
    if( digestAlgorithm )
        {
        algorithm = CCMSX509AlgorithmIdentifier::NewL( algorithmId,
                                                       *digestAlgorithm );
        }
    else
        {
        algorithm = CCMSX509AlgorithmIdentifier::NewL( algorithmId );
        }
	delete iAlgorithm;
	iAlgorithm = algorithm;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorPublicKey::SetPublicKeyL
// PublicKey setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSOriginatorPublicKey::SetPublicKeyL(
	const TDesC8& aPublicKey )
	{
	HBufC8* tmp = aPublicKey.AllocL();
	delete iPublicKey;
	iPublicKey = tmp;
	}

//  End of File
