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
#include    "CCMSRecipientEncryptedKey.h"
#include "CCMSIssuerAndSerialNumber.h"
#include "CCMSKeyIdentifier.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KNumberOfSubModules = 2;
const TTagType KRKeyIdTag = 0;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::CCMSRecipientEncryptedKey
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSRecipientEncryptedKey::CCMSRecipientEncryptedKey()
    {
    }

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSRecipientEncryptedKey::ConstructL(
    const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber,
	const TDesC8& aEncryptedKey )
    {
    SetIssuerAndSerialNumberL( aIssuerAndSerialNumber );
	SetEncryptedKeyL( aEncryptedKey );
    }

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSRecipientEncryptedKey::ConstructL(
    const CCMSKeyIdentifier& aRKeyId,
	const TDesC8& aEncryptedKey )
    {
    SetRKeyIdL( aRKeyId );
	SetEncryptedKeyL( aEncryptedKey );
    }


// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSRecipientEncryptedKey* CCMSRecipientEncryptedKey::NewL()
	{
	// creating with empty/default values
    CCMSIssuerAndSerialNumber* issuer = CCMSIssuerAndSerialNumber::NewL();
    CleanupStack::PushL( issuer );
	CCMSRecipientEncryptedKey* self = NewL( *issuer, KNullDesC8() );
    CleanupStack::PopAndDestroy( issuer );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSRecipientEncryptedKey* CCMSRecipientEncryptedKey::NewL(
    const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber,
    const TDesC8& aEncryptedKey )
	{
    CCMSRecipientEncryptedKey* self =
    	new( ELeave ) CCMSRecipientEncryptedKey();

    CleanupStack::PushL( self );
    self->ConstructL( aIssuerAndSerialNumber, aEncryptedKey );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSRecipientEncryptedKey* CCMSRecipientEncryptedKey::NewL(
    const CCMSKeyIdentifier& aRKeyId,
    const TDesC8& aEncryptedKey )
	{
    CCMSRecipientEncryptedKey* self =
    	new( ELeave ) CCMSRecipientEncryptedKey();

    CleanupStack::PushL( self );
    self->ConstructL( aRKeyId, aEncryptedKey );
    CleanupStack::Pop( self );
	return self;
	}


// Destructor
CCMSRecipientEncryptedKey::~CCMSRecipientEncryptedKey()
    {
	delete iEncryptedKey;
    delete iIssuerAndSerialNumber;
    delete iRKeyId;
    }

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSRecipientEncryptedKey::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* itemsData =
        DecodeSequenceLC( aRawData, KNumberOfSubModules, KNumberOfSubModules );

    // decode rid KeyAgreeRecipientIdentifier
    TASN1DecGeneric gen( *itemsData->At( 0 ) );
    gen.InitL();
    if( gen.Tag() == KRKeyIdTag )
        {
        // decode rKeyId
        CCMSKeyIdentifier* keyId = CCMSKeyIdentifier::NewL();
        CleanupStack::PushL( keyId );
        keyId->DecodeImplicitTagL( gen.Encoding(), KRKeyIdTag );
        CleanupStack::Pop( keyId );
        delete iIssuerAndSerialNumber;
        iIssuerAndSerialNumber = NULL;
        delete iRKeyId;
        iRKeyId = keyId;
        }
    else
        {
        // delete issuerAndSerialNumber
        CCMSIssuerAndSerialNumber* issuer = CCMSIssuerAndSerialNumber::NewL();
        CleanupStack::PushL( issuer );
        issuer->DecodeL( gen.Encoding() );
        CleanupStack::Pop( issuer );
        delete iRKeyId;
        iRKeyId = NULL;
        delete iIssuerAndSerialNumber;
        iIssuerAndSerialNumber = issuer;
        }

    //decode encryptedKey
    TASN1DecGeneric keyDecoder( *itemsData->At( 1 ) );
    keyDecoder.InitL();
    TASN1DecOctetString octetStringDecoder;
    HBufC8* tmp = octetStringDecoder.DecodeDERL( keyDecoder );
    delete iEncryptedKey;
    iEncryptedKey = tmp;
    CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSRecipientEncryptedKey::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode rid KeyAgreeRecipientIdentifier
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
        // encode rKeyId [0] IMPLICIT RecipientKeyIdentifier
        HBufC8* encodedRKeyId = NULL;
        iRKeyId->EncodeL( encodedRKeyId );
        CleanupStack::PushL( encodedRKeyId );
        CASN1EncEncoding* rKeyId = CASN1EncEncoding::NewLC( *encodedRKeyId );
        rKeyId->SetTag( KRKeyIdTag );
        root->AddAndPopChildL( rKeyId );
        CleanupStack::PopAndDestroy( encodedRKeyId );
        }

    // encode encryptedKey
    CASN1EncOctetString* encryptedKey =
        CASN1EncOctetString::NewLC( *iEncryptedKey );
    root->AddAndPopChildL( encryptedKey );

    return root;
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::EncryptedKey
// EncryptedKey getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8&
CCMSRecipientEncryptedKey::EncryptedKey() const
	{
	return *iEncryptedKey;
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::IssuerAndSerialNumber
// IssuerAndSerialNumber getter
// -----------------------------------------------------------------------------
EXPORT_C const CCMSIssuerAndSerialNumber*
CCMSRecipientEncryptedKey::IssuerAndSerialNumber() const
	{
	return iIssuerAndSerialNumber;
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::RecipientKeyIdentifier
// RecipientKeyIdentifier getter
// -----------------------------------------------------------------------------
EXPORT_C const CCMSKeyIdentifier*
CCMSRecipientEncryptedKey::RKeyId() const
	{
	return iRKeyId;
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::SetEncryptedKeyL
// EncryptedKey setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSRecipientEncryptedKey::SetEncryptedKeyL(
	const TDesC8& aEncryptedKey )
	{
    HBufC8* encryptedKey = aEncryptedKey.AllocL();
    delete iEncryptedKey;
    iEncryptedKey = encryptedKey;
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::SetIssuerAndSerialNumberL
// IssuerAndSerialNumber setter, deletes also rKeyId
// -----------------------------------------------------------------------------
EXPORT_C void CCMSRecipientEncryptedKey::SetIssuerAndSerialNumberL(
	const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber )
	{
    CCMSIssuerAndSerialNumber* issuer = CCMSIssuerAndSerialNumber::NewL(
        aIssuerAndSerialNumber.IssuerName(),
        aIssuerAndSerialNumber.SerialNumber() );
    delete iRKeyId;
    iRKeyId = NULL;
    delete iIssuerAndSerialNumber;
    iIssuerAndSerialNumber = issuer;
	}

// -----------------------------------------------------------------------------
// CCMSRecipientEncryptedKey::SetRKeyIdL
// rKeyId setter, deletes also IssuerAndSerialNumber
// -----------------------------------------------------------------------------
EXPORT_C void CCMSRecipientEncryptedKey::SetRKeyIdL(
	const CCMSKeyIdentifier& aRKeyId )
	{
    CCMSKeyIdentifier* keyId = NULL;
    const TTime* keyIdDate = aRKeyId.Date();
    if( keyIdDate )
        {
        keyId = CCMSKeyIdentifier::NewL(
            aRKeyId.KeyIdentifier(), *keyIdDate );
        }
    else
        {
        keyId = CCMSKeyIdentifier::NewL(
            aRKeyId.KeyIdentifier() );
        }
    delete iIssuerAndSerialNumber;
    iIssuerAndSerialNumber = NULL;
    delete iRKeyId;
    iRKeyId = keyId;
	}

//  End of File
