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
#include    "CCMSKeyAgreeRecipientInfo.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include "CCMSOriginatorIdentifierOrKey.h"
#include "CCMSRecipientEncryptedKey.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KMinNumberOfSubModules = 4;
const TInt KMaxNumberOfSubModules = 5;
const TTagType KOriginatorTag = 0;
const TTagType KUkmTag = 1;
const TInt KDefaultGranularity = 1;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::CCMSKeyAgreeRecipientInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyAgreeRecipientInfo::CCMSKeyAgreeRecipientInfo( )
    : CCMSRecipientInfo( KCMSKeyAgreeRecipientInfoVersion )
    {
    }

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSKeyAgreeRecipientInfo::ConstructL( )
    {
    // creates empty/default values
    CCMSX509AlgorithmIdentifier* algorithm = CCMSX509AlgorithmIdentifier::NewL( );
    CleanupStack::PushL( algorithm );
    BaseConstructL( *algorithm );
    CleanupStack::PopAndDestroy( algorithm );

    iOriginator = CCMSOriginatorIdentifierOrKey::NewL( );
    iRecipientEncryptedKeys = new( ELeave )
        CArrayPtrFlat< CCMSRecipientEncryptedKey >( KDefaultGranularity );
    }

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSKeyAgreeRecipientInfo::ConstructL(
    const CCMSOriginatorIdentifierOrKey& aOriginator,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
    const CArrayPtr< CCMSRecipientEncryptedKey >& aRecipientEncryptedKeys )
    {
    BaseConstructL( aKeyEncryptionAlgorithm );
    SetOriginatorL( aOriginator );
    SetRecipientEncryptedKeysL( aRecipientEncryptedKeys );
    }

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSKeyAgreeRecipientInfo::ConstructL(
    const CCMSOriginatorIdentifierOrKey& aOriginator,
    const TDesC8& aUkm,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
    const CArrayPtr< CCMSRecipientEncryptedKey >& aRecipientEncryptedKeys )
    {
    ConstructL( aOriginator, aKeyEncryptionAlgorithm, aRecipientEncryptedKeys );
    SetUserKeyingMaterialL( aUkm );
    }

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyAgreeRecipientInfo* CCMSKeyAgreeRecipientInfo::NewLC()
	{
	// creating with empty/default values
    CCMSKeyAgreeRecipientInfo* self = new( ELeave ) CCMSKeyAgreeRecipientInfo( );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyAgreeRecipientInfo* CCMSKeyAgreeRecipientInfo::NewL()
	{
	// creating with empty/default values
    CCMSKeyAgreeRecipientInfo* self = NewLC();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyAgreeRecipientInfo* CCMSKeyAgreeRecipientInfo::NewL(
    const CCMSOriginatorIdentifierOrKey& aOriginator,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
    const CArrayPtr< CCMSRecipientEncryptedKey >& aRecipientEncryptedKeys )
	{
    CCMSKeyAgreeRecipientInfo* self =
    	new( ELeave ) CCMSKeyAgreeRecipientInfo( );

    CleanupStack::PushL( self );
    self->ConstructL( aOriginator, aKeyEncryptionAlgorithm,
                      aRecipientEncryptedKeys );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyAgreeRecipientInfo* CCMSKeyAgreeRecipientInfo::NewL(
    const CCMSOriginatorIdentifierOrKey& aOriginator,
    const TDesC8& aUkm,
    const CCMSX509AlgorithmIdentifier& aKeyEncryptionAlgorithm,
    const CArrayPtr< CCMSRecipientEncryptedKey >& aRecipientEncryptedKeys )
	{
    CCMSKeyAgreeRecipientInfo* self =
    	new( ELeave ) CCMSKeyAgreeRecipientInfo( );

    CleanupStack::PushL( self );
    self->ConstructL( aOriginator,aUkm, aKeyEncryptionAlgorithm,
                      aRecipientEncryptedKeys );
    CleanupStack::Pop( self );
	return self;
	}


// Destructor
CCMSKeyAgreeRecipientInfo::~CCMSKeyAgreeRecipientInfo()
    {
	delete iOriginator;
    delete iKeyEncryptionAlgorithm;
    if( iRecipientEncryptedKeys )
        {
        iRecipientEncryptedKeys->ResetAndDestroy();
        delete iRecipientEncryptedKeys;
        }
    delete iUserKeyingMaterial;
    }

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSKeyAgreeRecipientInfo::DecodeL( const TDesC8& aRawData )
	{
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    // accept only sequence or implicit [1] tag
    if( ( decGen.Tag() != EASN1Sequence ) &&
        ( decGen.Tag() != KCMSKeyAgreeRecipientInfoTag ) )
        {
        User::Leave( KErrArgument );
        }
    TASN1DecSequence decSeq;
    CArrayPtr< TASN1DecGeneric >* itemsData = decSeq.DecodeDERLC( decGen );
    TInt count = itemsData->Count();
    if( ( count < KMinNumberOfSubModules ) ||
        ( count > KMaxNumberOfSubModules ) )
        {
        User::Leave( KErrArgument );
        }

    TInt sequenceCounter = 0;

    // Decode version
    TASN1DecGeneric versionDec( *itemsData->At( sequenceCounter++ ) );
    versionDec.InitL();
    TASN1DecInteger intDecoder;
    TInt version = intDecoder.DecodeDERShortL( versionDec );

    // decode originator [0] EXPLICIT OriginatorIdentifierOrKey
    TASN1DecGeneric taggedOriginatorDec( *itemsData->At( sequenceCounter++ ) );
    if( taggedOriginatorDec.Tag() != KOriginatorTag )
        {
        User::Leave( KErrArgument );
        }
    CCMSOriginatorIdentifierOrKey* originator =
        CCMSOriginatorIdentifierOrKey::NewL();
    CleanupStack::PushL( originator );
    originator->DecodeL( taggedOriginatorDec.GetContentDER() );

    // decode ukm [1] EXPLICIT UserKeyingMaterial OPTIONAL
    HBufC8* ukm = NULL;
    TASN1DecGeneric taggedUkmDec( *itemsData->At( sequenceCounter ) );
    if( taggedUkmDec.Tag() == KUkmTag )
        {
        TASN1DecOctetString octetStringDecoder;
        TASN1DecGeneric ukmDec( taggedUkmDec.GetContentDER() );
        ukmDec.InitL();
        ukm = octetStringDecoder.DecodeDERL( ukmDec );
        CleanupStack::PushL( ukm );
        sequenceCounter++;
        if( itemsData->Count() < KMaxNumberOfSubModules )
            {
            User::Leave( KErrArgument );
            }
        }

    // decode keyEncryptionAlgorithm
    CCMSX509AlgorithmIdentifier* algId = CCMSX509AlgorithmIdentifier::NewL();
    CleanupStack::PushL( algId );
    algId->DecodeL( itemsData->At( sequenceCounter++ )->Encoding() );

    // decode recipientEncryptedKeys
    CArrayPtr< TASN1DecGeneric >* keysData =
        DecodeSequenceLC( itemsData->At( sequenceCounter )->Encoding() );
    TInt keyCount = keysData->Count();
    CArrayPtr< CCMSRecipientEncryptedKey >* keys = NULL;
    if( keyCount > 0 )
        {
        keys = new( ELeave ) CArrayPtrFlat< CCMSRecipientEncryptedKey >(
            keyCount );
        CleanupStack::PushL( keys );

        for( TInt i = 0; i < keyCount; i++ )
            {
            CCMSRecipientEncryptedKey* key = CCMSRecipientEncryptedKey::NewL();
            CleanupStack::PushL( key );
            key->DecodeL( keysData->At( i )->Encoding() );
            keys->AppendL( key );
            }
        CleanupStack::Pop( keyCount ); // key
        }
    else
        {
        keys = new( ELeave ) CArrayPtrFlat< CCMSRecipientEncryptedKey >( 1 );
        CleanupStack::PushL( keys );
        }

    // everything has been decoded, now we can change state
    iVersion = version;
    delete iOriginator;
    iOriginator = originator;
    delete iUserKeyingMaterial;
    iUserKeyingMaterial = ukm;
    delete iKeyEncryptionAlgorithm;
    iKeyEncryptionAlgorithm = algId;
    if( iRecipientEncryptedKeys )
        {
        iRecipientEncryptedKeys->ResetAndDestroy();
        delete iRecipientEncryptedKeys;
        }
    iRecipientEncryptedKeys = keys;

    CleanupStack::Pop( keys );
    CleanupStack::PopAndDestroy( keysData );
    CleanupStack::Pop( algId );
    if( ukm )
        {
        CleanupStack::Pop( ukm );
        }
    CleanupStack::Pop( originator );
    CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSKeyAgreeRecipientInfo::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode version
    AddVersionL( root );

    // encode originator [0] EXPLICIT OriginatorIdentifierOrKey,
    CASN1EncBase* originator = iOriginator->EncoderLC();

    // CASN1EncExplicitTag takes ownership of originator even when leaving
    CleanupStack::Pop( originator );
    CASN1EncExplicitTag* taggedOriginator =
        CASN1EncExplicitTag::NewLC( originator, KOriginatorTag );
    root->AddAndPopChildL( taggedOriginator );

    // encode ukm [1] EXPLICIT UserKeyingMaterial OPTIONAL
    if( iUserKeyingMaterial )
        {
        CASN1EncOctetString* ukm =
            CASN1EncOctetString::NewL( *iUserKeyingMaterial );
        CASN1EncExplicitTag* taggedUkm =
            CASN1EncExplicitTag::NewLC( ukm, KUkmTag );
        root->AddAndPopChildL( taggedUkm );
        }

    // encode keyEncryptionAlgorithm KeyEncryptionAlgorithmIdentifier
    AddKeyEncryptionAlgorithmL( root );

    // encode RecipientEncryptedKeys ::= SEQUENCE OF RecipientEncryptedKey
    CASN1EncSequence* recipientEncryptedKeys = CASN1EncSequence::NewLC();
    TInt keyCount = iRecipientEncryptedKeys->Count();
    for( TInt i = 0; i < keyCount; i++ )
        {
        HBufC8* encodedKey = NULL;
        iRecipientEncryptedKeys->At( i )->EncodeL( encodedKey );
        CleanupStack::PushL( encodedKey );
        CASN1EncEncoding* singleKey = CASN1EncEncoding::NewLC( *encodedKey );
        recipientEncryptedKeys->AddAndPopChildL( singleKey );
        CleanupStack::PopAndDestroy( encodedKey );
        }
    root->AddAndPopChildL( recipientEncryptedKeys );

    return root;
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::TaggedEncoderLC
// Returns ASN1 encoder for this instance inside tag
// -----------------------------------------------------------------------------
CASN1EncBase* CCMSKeyAgreeRecipientInfo::TaggedEncoderLC() const
    {
    CASN1EncBase* encoder = EncoderLC();
    encoder->SetTag( KCMSKeyAgreeRecipientInfoTag );
    return encoder;
    }

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::Originator
// Originator getter
// -----------------------------------------------------------------------------
EXPORT_C const CCMSOriginatorIdentifierOrKey&
CCMSKeyAgreeRecipientInfo::Originator() const
	{
	return *iOriginator;
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::RecipientEncryptedKeys
// RecipientEncryptedKeys getter
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSRecipientEncryptedKey >&
CCMSKeyAgreeRecipientInfo::RecipientEncryptedKeys() const
	{
	return *iRecipientEncryptedKeys;
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::UserKeyingMaterial
// UserKeyingMaterial getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8*
CCMSKeyAgreeRecipientInfo::UserKeyingMaterial() const
	{
	return iUserKeyingMaterial;
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::SetOriginatorL
// Originator setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKeyAgreeRecipientInfo::SetOriginatorL(
	const CCMSOriginatorIdentifierOrKey& aOriginator )
	{
    CCMSOriginatorIdentifierOrKey* originator = NULL;
    switch( aOriginator.Type() )
        {
        case CCMSOriginatorIdentifierOrKey::ESubjectKeyIdentifier:
            {
            originator = CCMSOriginatorIdentifierOrKey::NewL(
                aOriginator.SubjectKeyIdentifierL() );
            break;
            }
        case CCMSOriginatorIdentifierOrKey::EOriginatorKey:
            {
            originator = CCMSOriginatorIdentifierOrKey::NewL(
                aOriginator.OriginatorKeyL() );
            break;
            }
        case CCMSOriginatorIdentifierOrKey::EIssuerAndSerialNumber:
            {
            originator = CCMSOriginatorIdentifierOrKey::NewL(
                aOriginator.IssuerAndSerialNumberL() );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }
    delete iOriginator;
    iOriginator = originator;
	}

// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::SetRecipientEncryptedKeysL
// RecipientEncryptedKeys setter, deep copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKeyAgreeRecipientInfo::SetRecipientEncryptedKeysL(
	const CArrayPtr< CCMSRecipientEncryptedKey >& aRecipientEncryptedKeys )
	{
    TInt keyCount = aRecipientEncryptedKeys.Count();
    CArrayPtr< CCMSRecipientEncryptedKey >* keys = NULL;
    if( keyCount > 0 )
        {
        keys = new( ELeave ) CArrayPtrFlat< CCMSRecipientEncryptedKey >(
            keyCount );
        CleanupStack::PushL( keys );
        for( TInt i = 0; i < keyCount; i++ )
            {
            const CCMSRecipientEncryptedKey* key = aRecipientEncryptedKeys[ i ];
            const CCMSIssuerAndSerialNumber* issuer = key->IssuerAndSerialNumber();
            const TDesC8& encryptedKey = key->EncryptedKey();
            CCMSRecipientEncryptedKey* newKey = NULL;
            if( issuer )
                {
                newKey = CCMSRecipientEncryptedKey::NewL(
                    *issuer, encryptedKey );
                }
            else
                {
                newKey = CCMSRecipientEncryptedKey::NewL( *( key->RKeyId() ),
                                                          encryptedKey );
                }
            CleanupStack::PushL( newKey );
            keys->AppendL( newKey );
            }
        CleanupStack::Pop( keyCount ); // newKeys
        CleanupStack::Pop( keys );
        }
    else
        {
        keys = new( ELeave ) CArrayPtrFlat< CCMSRecipientEncryptedKey >( 1 );
        }
    if( iRecipientEncryptedKeys )
        {
        iRecipientEncryptedKeys->ResetAndDestroy();
        delete iRecipientEncryptedKeys;
        }
    iRecipientEncryptedKeys = keys;
    }
// -----------------------------------------------------------------------------
// CCMSKeyAgreeRecipientInfo::SetUserKeyingMaterialL
// UserKeyingMaterial setter.
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKeyAgreeRecipientInfo::SetUserKeyingMaterialL(
	const TDesC8& aUkm )
	{
    HBufC8* ukm = aUkm.AllocL();
    delete iUserKeyingMaterial;
    iUserKeyingMaterial = ukm;
	}

//  End of File
