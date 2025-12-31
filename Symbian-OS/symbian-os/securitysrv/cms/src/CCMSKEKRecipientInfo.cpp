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
#include    "CCMSKEKRecipientInfo.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include "CCMSKeyIdentifier.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KNumberOfSubModules = 4;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::CCMSKEKRecipientInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKEKRecipientInfo::CCMSKEKRecipientInfo( )
    : CCMSRecipientInfo( KCMSKEKRecipientInfoVersion )
    {
    }

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSKEKRecipientInfo::ConstructL(
    const CCMSKeyIdentifier& aKekid,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
	const TDesC8& aEncryptedKey )
    {
    BaseConstructL( aKeyEncryptionAlgorithm );
    SetKekidL( aKekid );
	SetEncryptedKeyL( aEncryptedKey );
    }

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKEKRecipientInfo* CCMSKEKRecipientInfo::NewLC()
	{
	CCMSKEKRecipientInfo* self =
    	new( ELeave ) CCMSKEKRecipientInfo( );

    CleanupStack::PushL( self );
    // creating with empty/default values
    CCMSX509AlgorithmIdentifier* algorithmIdentifier =
        CCMSX509AlgorithmIdentifier::NewL();
    CleanupStack::PushL( algorithmIdentifier );
    CCMSKeyIdentifier* kekid = CCMSKeyIdentifier::NewL();
    CleanupStack::PushL( kekid );
	self->ConstructL( *kekid, *algorithmIdentifier, KNullDesC8() );
    CleanupStack::PopAndDestroy( 2 ); // kekid, algorithmIdentifier
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKEKRecipientInfo* CCMSKEKRecipientInfo::NewL()
	{
    CCMSKEKRecipientInfo* self = NewLC();
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKEKRecipientInfo* CCMSKEKRecipientInfo::NewL(
    const CCMSKeyIdentifier& aKekid,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
    const TDesC8& aEncryptedKey )
	{
    CCMSKEKRecipientInfo* self =
    	new( ELeave ) CCMSKEKRecipientInfo( );

    CleanupStack::PushL( self );
    self->ConstructL( aKekid, aKeyEncryptionAlgorithm,
                      aEncryptedKey );
    CleanupStack::Pop( self );
	return self;
	}

// Destructor
CCMSKEKRecipientInfo::~CCMSKEKRecipientInfo()
    {
	delete iEncryptedKey;
    delete iKekid;
    }

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSKEKRecipientInfo::DecodeL( const TDesC8& aRawData )
	{
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    // accept only sequence or implicit [2] tag
    if( ( decGen.Tag() != EASN1Sequence ) &&
        ( decGen.Tag() != KCMSKEKRecipientInfoTag ) )
        {
        User::Leave( KErrArgument );
        }
    TASN1DecSequence decSeq;
    CArrayPtr< TASN1DecGeneric >* itemsData = decSeq.DecodeDERLC( decGen );
    TInt count = itemsData->Count();
    if( count != KNumberOfSubModules )
        {
        User::Leave( KErrArgument );
        }

    // Decode version
    TASN1DecGeneric versionDec( *itemsData->At( 0 ) );
    versionDec.InitL();
    TASN1DecInteger intDecoder;
    TInt version = intDecoder.DecodeDERShortL( versionDec );

    // decode kekid KEKIdentifier
    CCMSKeyIdentifier* keyId = CCMSKeyIdentifier::NewL();
    CleanupStack::PushL( keyId );
    keyId->DecodeL( itemsData->At( 1 )->Encoding() );

    // decode keyEncryptionAlgorithm KeyEncryptionAlgorithmIdentifier
    CCMSX509AlgorithmIdentifier* algId = CCMSX509AlgorithmIdentifier::NewL();
    CleanupStack::PushL( algId );
    algId->DecodeL( itemsData->At( 2 )->Encoding() );

    // decode encryptedKey
    TASN1DecGeneric keyDecoder( *itemsData->At( 3 ) );
    TASN1DecOctetString octetStringDecoder;
    keyDecoder.InitL();
    HBufC8* tmp = octetStringDecoder.DecodeDERL( keyDecoder );

    // now we have created all new members, so we can change state
    iVersion = version;
    delete iKekid;
    iKekid = keyId;
    delete iKeyEncryptionAlgorithm;
    iKeyEncryptionAlgorithm = algId;
    delete iEncryptedKey;
    iEncryptedKey = tmp;
    CleanupStack::Pop( 2 ); // algId, keyId
    CleanupStack::PopAndDestroy( itemsData );
	}


// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSKEKRecipientInfo::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode version
    AddVersionL( root );

    // encode kekid
    HBufC8* encodedKekId = NULL;
    iKekid->EncodeL( encodedKekId );
    CleanupStack::PushL( encodedKekId );
    CASN1EncEncoding* kekId = CASN1EncEncoding::NewLC( *encodedKekId );
    root->AddAndPopChildL( kekId );
    CleanupStack::PopAndDestroy( encodedKekId );

    // encode keyEncryptionAlgorithm
    AddKeyEncryptionAlgorithmL( root );

    // encode encryptedKey
    CASN1EncOctetString* encryptedKey =
        CASN1EncOctetString::NewLC( *iEncryptedKey );
    root->AddAndPopChildL( encryptedKey );

    return root;
	}

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::TaggedEncoderLC
// Returns ASN1 encoder for this instance inside tag
// -----------------------------------------------------------------------------
CASN1EncBase* CCMSKEKRecipientInfo::TaggedEncoderLC() const
    {
    CASN1EncBase* encoder = EncoderLC();
    encoder->SetTag( KCMSKEKRecipientInfoTag );
    return encoder;
    }

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::EncryptedKey
// EncryptedKey getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8&
CCMSKEKRecipientInfo::EncryptedKey() const
	{
	return *iEncryptedKey;
	}

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::Kekid
// KEKIdentifier getter
// -----------------------------------------------------------------------------
EXPORT_C const CCMSKeyIdentifier&
CCMSKEKRecipientInfo::Kekid() const
	{
	return *iKekid;
	}

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::SetEncryptedKeyL
// EncryptedKey setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKEKRecipientInfo::SetEncryptedKeyL(
	const TDesC8& aEncryptedKey )
	{
    HBufC8* encryptedKey = aEncryptedKey.AllocL();
    delete iEncryptedKey;
    iEncryptedKey = encryptedKey;
	}

// -----------------------------------------------------------------------------
// CCMSKEKRecipientInfo::SetKekidL
// iKekid setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKEKRecipientInfo::SetKekidL(
	const CCMSKeyIdentifier& aKekid )
	{
    CCMSKeyIdentifier* keyId = CCMSKeyIdentifier::NewL( aKekid.KeyIdentifier() );
    CleanupStack::PushL( keyId );
    const TTime* date = keyId->Date();
    if( date )
        {
        keyId->SetDateL( *date );
        }
    CleanupStack::Pop( keyId );
    delete iKekid;
    iKekid = keyId;
	}

//  End of File
