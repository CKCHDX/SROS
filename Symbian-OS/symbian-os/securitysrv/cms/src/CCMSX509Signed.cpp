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
#include    "CCMSX509Signed.h"
#include "CCMSX509AlgorithmIdentifier.h"
// CONSTANTS
const TInt KNumberOfSubModules = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509Signed::CCMSX509Signed
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCMSX509Signed::CCMSX509Signed( )
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::BaseConstructL
// Constructs the member variables. Makes copies.
// -----------------------------------------------------------------------------
//
void CCMSX509Signed::BaseConstructL(
    const CCMSX509AlgorithmIdentifier& aAlgorithmIdentifier,
    const TDesC8& aEncrypted )
    {
    SetAlgorithmIdentifierL( aAlgorithmIdentifier );
    SetEncryptedL( aEncrypted );
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::SignAndPopLC
// Creates a signed encoder.
// -----------------------------------------------------------------------------
//
CASN1EncSequence* CCMSX509Signed::SignAndPopLC(
    CASN1EncBase* aToBeSigned ) const
    {
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode toBeSigned
    root->AddChildL( aToBeSigned );

    CleanupStack::Pop( 2 ); // root, aToBeSigned
    CleanupStack::PushL( root );
    
    // encode algorithmIdentifier  AlgorithmIdentifier
    CASN1EncBase* algId = iAlgorithmIdentifier->EncoderLC( );
    root->AddAndPopChildL( algId );

    // encode encrypted            ENCRYPTED-HASH{ToBeSigned}
    CASN1EncBitString* encrypted = CASN1EncBitString::NewLC( *iEncrypted );
    root->AddAndPopChildL( encrypted );

    return root;
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::DecodeSignatureL
// Decodes algorithmIdentifier and encrypted parts from the raw data.
// -----------------------------------------------------------------------------
//
TASN1DecGeneric CCMSX509Signed::DecodeSignatureL(
    const TDesC8& aRawData,
    CCMSX509AlgorithmIdentifier*& aAlgorithmIdentifier,
    HBufC8*& aEncrypted ) const
    {
    CArrayPtr< TASN1DecGeneric >* itemList = DecodeSequenceLC(
        aRawData, KNumberOfSubModules, KNumberOfSubModules );

    TASN1DecGeneric retVal =
        DecodeSignatureArrayL( *itemList, aAlgorithmIdentifier, aEncrypted );
    
    CleanupStack::PopAndDestroy( itemList );
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::DecodeSignatureArrayL
// Decodes algorithmIdentifier and encrypted parts from the array.
// -----------------------------------------------------------------------------
//
TASN1DecGeneric CCMSX509Signed::DecodeSignatureArrayL(
    CArrayPtr< TASN1DecGeneric >& aItemArray,
    CCMSX509AlgorithmIdentifier*& aAlgorithmIdentifier,
    HBufC8*& aEncrypted ) const
    {
    TInt sequenceCounter = 0;

    // ToBeSigned will be returned
    TASN1DecGeneric retVal( *aItemArray.At( sequenceCounter++ ) );
    
    // decode algorithmIdentifier
    CCMSX509AlgorithmIdentifier* algId = CCMSX509AlgorithmIdentifier::NewL( );
    CleanupStack::PushL( algId );
    algId->DecodeL( aItemArray.At( sequenceCounter++ )->Encoding() );
    
    // decode encrypted
    TASN1DecBitString bsDecoder;
    HBufC8* encrypted =
        bsDecoder.ExtractOctetStringL( *( aItemArray.At( sequenceCounter ) ) );

    // change parameter pointers
    aAlgorithmIdentifier = algId;
    aEncrypted = encrypted;
    CleanupStack::Pop( algId );
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::~CCMSX509Signed
// Destructor
// -----------------------------------------------------------------------------
//
CCMSX509Signed::~CCMSX509Signed()
    {
    delete iAlgorithmIdentifier;
    delete iEncrypted;
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::AlgorithmIdentifier
// algorithmIdentifier getter
// -----------------------------------------------------------------------------
//
EXPORT_C const CCMSX509AlgorithmIdentifier&
CCMSX509Signed::AlgorithmIdentifier() const
    {
    return *iAlgorithmIdentifier;
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::Encrypted
// encrypted getter
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CCMSX509Signed::Encrypted() const
    {
    return *iEncrypted;
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::SetAlgorithmIdentifierL
// AlgorithmIdentifier setter
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509Signed::SetAlgorithmIdentifierL(
    const CCMSX509AlgorithmIdentifier& aAlgorithmIdentifier )
    {
    CCMSX509AlgorithmIdentifier* algorithmIdentifier =
        CCMSX509AlgorithmIdentifier::NewL(
            aAlgorithmIdentifier.AlgorithmIdentifier() );
    CleanupStack::PushL( algorithmIdentifier );
    const CAlgorithmIdentifier* digestAlgorithm =
        aAlgorithmIdentifier.DigestAlgorithm();
    if( digestAlgorithm )
        {
        algorithmIdentifier->SetDigestAlgorithmL( digestAlgorithm );
        }
    CleanupStack::Pop( algorithmIdentifier );
    delete iAlgorithmIdentifier;
    iAlgorithmIdentifier = algorithmIdentifier;
    }

// -----------------------------------------------------------------------------
// CCMSX509Signed::SetEncryptedL
// encrypted setter
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509Signed::SetEncryptedL(
    const TDesC8& aEncrypted )
    {
    HBufC8* encrypted = aEncrypted.AllocL();
    delete iEncrypted;
    iEncrypted = encrypted;
    }

//  End of File
