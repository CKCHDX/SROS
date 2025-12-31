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
#include    "CCMSKeyTransRecipientInfo.h"
#include "CCMSIssuerAndSerialNumber.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KNumberOfSubModules = 4;
const TTagType KSubjectKeyIdentifierTag = 0;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::CCMSKeyTransRecipientInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyTransRecipientInfo::CCMSKeyTransRecipientInfo( TInt aVersion)
    : CCMSRecipientInfo( aVersion )
    {
    }

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSKeyTransRecipientInfo::ConstructL(
    const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
	const TDesC8& aEncryptedKey )
    {
    BaseConstructL( aKeyEncryptionAlgorithm );
    SetIssuerAndSerialNumberL( aIssuerAndSerialNumber );
	SetEncryptedKeyL( aEncryptedKey );
    }

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSKeyTransRecipientInfo::ConstructL(
    const TDesC8& aSubjectKeyIdentifier,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
	const TDesC8& aEncryptedKey )
    {
    BaseConstructL( aKeyEncryptionAlgorithm );
    SetSubjectKeyIdentifierL( aSubjectKeyIdentifier );
	SetEncryptedKeyL( aEncryptedKey );
    }

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyTransRecipientInfo* CCMSKeyTransRecipientInfo::NewLC()
	{
	// creating with empty/default values
    CCMSKeyTransRecipientInfo* self =
    	new( ELeave ) CCMSKeyTransRecipientInfo(
            KCMSKeyTransRecipientInfoSubjectKeyIdentifierVersion );
    CleanupStack::PushL( self );
    CCMSX509AlgorithmIdentifier* algorithmIdentifier =
        CCMSX509AlgorithmIdentifier::NewL();
    CleanupStack::PushL( algorithmIdentifier );
	self->ConstructL( KNullDesC8(), *algorithmIdentifier, KNullDesC8() );
    CleanupStack::PopAndDestroy( algorithmIdentifier );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyTransRecipientInfo* CCMSKeyTransRecipientInfo::NewL()
	{
	// creating with empty/default values
	CCMSKeyTransRecipientInfo* self = NewLC();
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyTransRecipientInfo* CCMSKeyTransRecipientInfo::NewL(
    const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
    const TDesC8& aEncryptedKey )
	{
    CCMSKeyTransRecipientInfo* self =
    	new( ELeave ) CCMSKeyTransRecipientInfo(
            KCMSKeyTransRecipientInfoIssuerAndSerialNumberVersion );

    CleanupStack::PushL( self );
    self->ConstructL( aIssuerAndSerialNumber, aKeyEncryptionAlgorithm,
                      aEncryptedKey );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyTransRecipientInfo* CCMSKeyTransRecipientInfo::NewLC(
    const TDesC8& aSubjectKeyIdentifier,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
    const TDesC8& aEncryptedKey )
	{
    CCMSKeyTransRecipientInfo* self =
    	new( ELeave ) CCMSKeyTransRecipientInfo(
            KCMSKeyTransRecipientInfoSubjectKeyIdentifierVersion );

    CleanupStack::PushL( self );
    self->ConstructL( aSubjectKeyIdentifier, aKeyEncryptionAlgorithm,
                      aEncryptedKey );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyTransRecipientInfo* CCMSKeyTransRecipientInfo::NewL(
    const TDesC8& aSubjectKeyIdentifier,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
    const TDesC8& aEncryptedKey )
	{
    CCMSKeyTransRecipientInfo* self =
        NewLC( aSubjectKeyIdentifier, aKeyEncryptionAlgorithm, aEncryptedKey );

    CleanupStack::Pop( self );
	return self;
	}


// Destructor
CCMSKeyTransRecipientInfo::~CCMSKeyTransRecipientInfo()
    {
	delete iEncryptedKey;
    delete iIssuerAndSerialNumber;
    delete iSubjectKeyIdentifier;
    }

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSKeyTransRecipientInfo::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* itemsData =
        DecodeSequenceLC( aRawData, KNumberOfSubModules, KNumberOfSubModules );

    // Decode version
    TASN1DecGeneric versionDec( *itemsData->At( 0 ) );
    versionDec.InitL();
    TASN1DecInteger intDecoder;
    TInt version = intDecoder.DecodeDERShortL( versionDec );

    // decode rid RecipientIdentifier
    TASN1DecOctetString octetStringDecoder;
    TASN1DecGeneric ridDec( *itemsData->At( 1 ) );
    HBufC8* keyIdentifier = NULL;
    CCMSIssuerAndSerialNumber* issuer = NULL;
    switch( version )
        {
        case KCMSKeyTransRecipientInfoSubjectKeyIdentifierVersion:
            {
            if( ridDec.Tag() != KSubjectKeyIdentifierTag )
                {
                User::Leave( KErrArgument );
                }
            TInt pos = 0;
            keyIdentifier = octetStringDecoder.DecodeDERL(
                ridDec.GetContentDER(), pos );
            CleanupStack::PushL( keyIdentifier );
            break;
            }
        case KCMSKeyTransRecipientInfoIssuerAndSerialNumberVersion:
            {
            issuer = CCMSIssuerAndSerialNumber::NewL();
            CleanupStack::PushL( issuer );
            issuer->DecodeL( ridDec.Encoding() );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }

    // decode keyEncryptionAlgorithm
    CCMSX509AlgorithmIdentifier* algId = CCMSX509AlgorithmIdentifier::NewL();
    CleanupStack::PushL( algId );
    algId->DecodeL( itemsData->At( 2 )->Encoding() );

    // decode encryptedKey
    TASN1DecGeneric keyDecoder( *itemsData->At( 3 ) );
    keyDecoder.InitL();
    HBufC8* tmp = octetStringDecoder.DecodeDERL( keyDecoder );

    // now we have created all new members, so we can change state
    iVersion = version;
    delete iSubjectKeyIdentifier;
    iSubjectKeyIdentifier = keyIdentifier;
    delete iIssuerAndSerialNumber;
    iIssuerAndSerialNumber = issuer;
    delete iKeyEncryptionAlgorithm;
    iKeyEncryptionAlgorithm = algId;
    delete iEncryptedKey;
    iEncryptedKey = tmp;
    CleanupStack::Pop( 2 ); // keyIdentifier/issuer, algId
    CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSKeyTransRecipientInfo::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode version
    AddVersionL( root );

    // encode rid RecipientIdentifier
    if( iIssuerAndSerialNumber )
        {
        // encode issuerAndSerialNumber
        HBufC8* encodedIssuer = NULL;
        iIssuerAndSerialNumber->EncodeL( encodedIssuer );
        CleanupStack::PushL( encodedIssuer );
        CASN1EncEncoding* issuer = CASN1EncEncoding::NewLC( *encodedIssuer );
        root->AddAndPopChildL( issuer );
        CleanupStack::PopAndDestroy( encodedIssuer );
        }
    else
        {
        // encode subjectKeyIdentifier [0] SubjectKeyIdentifier
        CASN1EncOctetString* subjectKeyIdentifier =
            CASN1EncOctetString::NewL( *iSubjectKeyIdentifier );
        CASN1EncExplicitTag* taggedIdentifier =
            CASN1EncExplicitTag::NewLC( subjectKeyIdentifier, KSubjectKeyIdentifierTag );
        root->AddAndPopChildL( taggedIdentifier );
        }

    // encode keyEncryptionAlgorithm KeyEncryptionAlgorithmIdentifier
    AddKeyEncryptionAlgorithmL( root );

    // encode encryptedKey
    CASN1EncOctetString* encryptedKey =
        CASN1EncOctetString::NewLC( *iEncryptedKey );
    root->AddAndPopChildL( encryptedKey );

    return root;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::TaggedEncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------
CASN1EncBase* CCMSKeyTransRecipientInfo::TaggedEncoderLC() const
    {
    return EncoderLC();
    }

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::EncryptedKey
// EncryptedKey getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8&
CCMSKeyTransRecipientInfo::EncryptedKey() const
	{
	return *iEncryptedKey;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::IssuerAndSerialNumber
// IssuerAndSerialNumber getter
// -----------------------------------------------------------------------------
EXPORT_C const CCMSIssuerAndSerialNumber*
CCMSKeyTransRecipientInfo::IssuerAndSerialNumber() const
	{
	return iIssuerAndSerialNumber;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::SubjectKeyIdentifier
// SubjectKeyIdentifier getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8*
CCMSKeyTransRecipientInfo::SubjectKeyIdentifier() const
	{
	return iSubjectKeyIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::SetEncryptedKeyL
// EncryptedKey setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKeyTransRecipientInfo::SetEncryptedKeyL(
	const TDesC8& aEncryptedKey )
	{
    HBufC8* encryptedKey = aEncryptedKey.AllocL();
    delete iEncryptedKey;
    iEncryptedKey = encryptedKey;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::SetIssuerAndSerialNumberL
// IssuerAndSerialNumber setter, deletes also SubjectKeyIdentifier and
// sets version to KVersionIssuerAndSerialNumber
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKeyTransRecipientInfo::SetIssuerAndSerialNumberL(
	const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber )
	{
    CCMSIssuerAndSerialNumber* issuer = CCMSIssuerAndSerialNumber::NewL(
        aIssuerAndSerialNumber.IssuerName(),
        aIssuerAndSerialNumber.SerialNumber() );
    delete iSubjectKeyIdentifier;
    iSubjectKeyIdentifier = NULL;
    delete iIssuerAndSerialNumber;
    iIssuerAndSerialNumber = issuer;
    iVersion = KCMSKeyTransRecipientInfoIssuerAndSerialNumberVersion;
	}

// -----------------------------------------------------------------------------
// CCMSKeyTransRecipientInfo::SetSubjectKeyIdentifierL
// SubjectKeyIdentifier setter. Copies parameters, deletes also
// IssuerAndSerialNumber and set version to KVersionSubjectKeyIdentifier
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKeyTransRecipientInfo::SetSubjectKeyIdentifierL(
	const TDesC8& aSubjectKeyIdentifier )
	{
    HBufC8* identifier = aSubjectKeyIdentifier.AllocL();
    delete iIssuerAndSerialNumber;
    iIssuerAndSerialNumber = NULL;
    delete iSubjectKeyIdentifier;
    iSubjectKeyIdentifier = identifier;
    iVersion = KCMSKeyTransRecipientInfoSubjectKeyIdentifierVersion;
	}

//  End of File
