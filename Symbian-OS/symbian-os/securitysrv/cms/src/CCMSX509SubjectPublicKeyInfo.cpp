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
#include    "CCMSX509SubjectPublicKeyInfo.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KNumberOfSubModules = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::CCMSX509SubjectPublicKeyInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509SubjectPublicKeyInfo::CCMSX509SubjectPublicKeyInfo()
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509SubjectPublicKeyInfo::ConstructL(
    const CCMSX509AlgorithmIdentifier& aAlgorithm,
    const TDesC8& aSubjectPublicKey )
    {
    SetAlgorithmL( aAlgorithm );
    SetSubjectPublicKeyL( aSubjectPublicKey );
    }

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509SubjectPublicKeyInfo::ConstructL(
    const CSubjectPublicKeyInfo& aSubjectPublicKeyInfo )
    {
    TAlgorithmId algIdType = aSubjectPublicKeyInfo.AlgorithmId();
    CAlgorithmIdentifier* algId = CAlgorithmIdentifier::NewLC(
        algIdType,
        aSubjectPublicKeyInfo.EncodedParams() );
    CCMSX509AlgorithmIdentifier* cmsAlgId =
        CCMSX509AlgorithmIdentifier::NewL( *algId );
    CleanupStack::PushL( cmsAlgId );
    SetAlgorithmL( *cmsAlgId );
    CleanupStack::PopAndDestroy( 2 ); // cmsAlgId, algId
    SetSubjectPublicKeyL( aSubjectPublicKeyInfo.KeyData() );
    }

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509SubjectPublicKeyInfo::ConstructL( )
    {
    // creating empty/default values
    iAlgorithm = CCMSX509AlgorithmIdentifier::NewL();
    iSubjectPublicKey = KNullDesC8().AllocL();
    }
    

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509SubjectPublicKeyInfo*
CCMSX509SubjectPublicKeyInfo::NewL()
	{
	// creating with empty values
    CCMSX509SubjectPublicKeyInfo* self =
        new( ELeave ) CCMSX509SubjectPublicKeyInfo();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509SubjectPublicKeyInfo*
CCMSX509SubjectPublicKeyInfo::NewL(
    const CCMSX509AlgorithmIdentifier& aAlgorithm,
    const TDesC8& aSubjectPublicKey )
    {
    CCMSX509SubjectPublicKeyInfo* self =
        new( ELeave ) CCMSX509SubjectPublicKeyInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aAlgorithm, aSubjectPublicKey );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509SubjectPublicKeyInfo*
CCMSX509SubjectPublicKeyInfo::NewL(
    const CSubjectPublicKeyInfo& aSubjectPublicKeyInfo )
    {
    CCMSX509SubjectPublicKeyInfo* self =
        new( ELeave ) CCMSX509SubjectPublicKeyInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aSubjectPublicKeyInfo );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CCMSX509SubjectPublicKeyInfo::~CCMSX509SubjectPublicKeyInfo()
    {
	delete iAlgorithm;
    delete iSubjectPublicKey;
    }

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509SubjectPublicKeyInfo::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* itemList = DecodeSequenceLC(
        aRawData, KNumberOfSubModules, KNumberOfSubModules );

    TInt sequenceCounter = 0;
    
    // decode algorithm
    CCMSX509AlgorithmIdentifier* algId = CCMSX509AlgorithmIdentifier::NewL( );
    CleanupStack::PushL( algId );
    algId->DecodeL( itemList->At( sequenceCounter++ )->Encoding() );

    // decode subjectPublicKey
    TASN1DecBitString bsDecoder;
    HBufC8* subjectPublicKey =
        bsDecoder.ExtractOctetStringL( *( itemList->At( sequenceCounter ) ) );

    // all done, change state
    delete iAlgorithm;
    iAlgorithm = algId;
    delete iSubjectPublicKey;
    iSubjectPublicKey = subjectPublicKey;
    CleanupStack::Pop( algId );
    CleanupStack::PopAndDestroy( itemList );
	}

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509SubjectPublicKeyInfo::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode algorithm         AlgorithmIdentifier
    CASN1EncBase* algId = iAlgorithm->EncoderLC();
    root->AddAndPopChildL( algId );

    // encode subjectPublicKey  BIT STRING
    CASN1EncBitString* spKey = CASN1EncBitString::NewLC( *iSubjectPublicKey );
    root->AddAndPopChildL( spKey );

    return root;
	}

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::Algorithm()
// Getter for algorithm
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AlgorithmIdentifier&
CCMSX509SubjectPublicKeyInfo::Algorithm() const
	{
	return *iAlgorithm;
	}

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::SubjectPublicKey()
// Getter for subjectPublicKey
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8&
CCMSX509SubjectPublicKeyInfo::SubjectPublicKey() const
	{
	return *iSubjectPublicKey;
	}

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::SetAlgorithmL()
// Setter for algorithm
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509SubjectPublicKeyInfo::SetAlgorithmL(
    const CCMSX509AlgorithmIdentifier& aAlgorithm )
	{
    CCMSX509AlgorithmIdentifier* algorithm =
        CCMSX509AlgorithmIdentifier::NewL( aAlgorithm.AlgorithmIdentifier() );
    CleanupStack::PushL( algorithm );
    const CAlgorithmIdentifier* digestIdentifier =
        aAlgorithm.DigestAlgorithm();
    if( digestIdentifier )
        {
        algorithm->SetDigestAlgorithmL( digestIdentifier );
        }
    CleanupStack::Pop( algorithm );
    delete iAlgorithm;
    iAlgorithm = algorithm;
	}

// -----------------------------------------------------------------------------
// CCMSX509SubjectPublicKeyInfo::SetSubjectPublicKeyL()
// Setter for subjectPublicKey
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509SubjectPublicKeyInfo::SetSubjectPublicKeyL(
    const TDesC8& aSubjectPublicKey )
	{
    HBufC8* key = aSubjectPublicKey.AllocL();
    delete iSubjectPublicKey;
    iSubjectPublicKey = key;
	}

//  End of File
