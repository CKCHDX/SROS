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
#include    "CCMSOriginatorIdentifierOrKey.h"
#include "CCMSIssuerAndSerialNumber.h"
#include "CCMSOriginatorPublicKey.h"
#include <asn1dec.h>
#include <asn1enc.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::CCMSOriginatorIdentifierOrKey
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorIdentifierOrKey::CCMSOriginatorIdentifierOrKey()
    {
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSOriginatorIdentifierOrKey::ConstructL(
    const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber )
    {
    SetIssuerAndSerialNumberL( aIssuerAndSerialNumber );
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSOriginatorIdentifierOrKey::ConstructL(
    const TDesC8& aSubjectKeyIdentifier )
    {
    SetSubjectKeyIdentifierL( aSubjectKeyIdentifier );
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSOriginatorIdentifierOrKey::ConstructL(
    const CCMSOriginatorPublicKey& aOriginatorKey )
    {
    SetOriginatorKeyL( aOriginatorKey );
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorIdentifierOrKey* CCMSOriginatorIdentifierOrKey::NewL()
	{
	// creating with empty/default values
    CCMSIssuerAndSerialNumber* issuer = CCMSIssuerAndSerialNumber::NewL( );
	CleanupStack::PushL( issuer );
	CCMSOriginatorIdentifierOrKey* self = NewL( *issuer );
	CleanupStack::PopAndDestroy( issuer );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorIdentifierOrKey* CCMSOriginatorIdentifierOrKey::NewL(
    const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber)
    {
    CCMSOriginatorIdentifierOrKey* self =
    	new( ELeave ) CCMSOriginatorIdentifierOrKey();

    CleanupStack::PushL( self );
    self->ConstructL( aIssuerAndSerialNumber );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorIdentifierOrKey* CCMSOriginatorIdentifierOrKey::NewL(
    const TDesC8& aSubjectKeyIdentifier )
    {
    CCMSOriginatorIdentifierOrKey* self =
    	new( ELeave ) CCMSOriginatorIdentifierOrKey();

    CleanupStack::PushL( self );
    self->ConstructL( aSubjectKeyIdentifier );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorIdentifierOrKey* CCMSOriginatorIdentifierOrKey::NewL(
    const CCMSOriginatorPublicKey& aOriginatorKey )
    {
    CCMSOriginatorIdentifierOrKey* self =
    	new( ELeave ) CCMSOriginatorIdentifierOrKey();

    CleanupStack::PushL( self );
    self->ConstructL( aOriginatorKey );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CCMSOriginatorIdentifierOrKey::~CCMSOriginatorIdentifierOrKey()
    {
    delete iIssuerAndSerialNumber;
    delete iSubjectKeyIdentifier;
    delete iOriginatorKey;
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSOriginatorIdentifierOrKey::DecodeL( const TDesC8& aRawData )
	{
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    switch( decGen.Tag() )
        {
        case EASN1Sequence:
            {
            // check class and decode iasn
            if( decGen.Class() != EUniversal )
                {
                User::Leave( KErrArgument );
                }
            CCMSIssuerAndSerialNumber* issuerAndSerialNumber =
                CCMSIssuerAndSerialNumber::NewL();
            CleanupStack::PushL( issuerAndSerialNumber );
            issuerAndSerialNumber->DecodeL( aRawData );
            DeleteOldValueAndChangeTypeL( EIssuerAndSerialNumber );
            iIssuerAndSerialNumber = issuerAndSerialNumber;
            CleanupStack::Pop( issuerAndSerialNumber );
            break;
            }
        case ESubjectKeyIdentifier:
            {
            // check class and decode ski
            if( decGen.Class() != EContextSpecific )
                {
                User::Leave( KErrArgument );
                }
            TASN1DecOctetString octetDecoder;
            TInt pos = 0;
            HBufC8* subjectKeyIdentifier =
                octetDecoder.DecodeDERL( aRawData, pos );
            CleanupStack::PushL( subjectKeyIdentifier );
            DeleteOldValueAndChangeTypeL( ESubjectKeyIdentifier );
            iSubjectKeyIdentifier = subjectKeyIdentifier;
            CleanupStack::Pop( subjectKeyIdentifier );
            break;
            }
        case EOriginatorKey:
            {
            // check class and decode originatorKey
            if( decGen.Class() != EContextSpecific )
                {
                User::Leave( KErrArgument );
                }
            CCMSOriginatorPublicKey* originatorKey =
                CCMSOriginatorPublicKey::NewL();
            CleanupStack::PushL( originatorKey );
            originatorKey->DecodeL( aRawData );
            DeleteOldValueAndChangeTypeL( EOriginatorKey );
            iOriginatorKey = originatorKey;
            CleanupStack::Pop( originatorKey );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::EncodeL
// Encrypts this instance to descriptor
// -----------------------------------------------------------------------------

void CCMSOriginatorIdentifierOrKey::EncodeL( HBufC8*& aResult ) const
	{
	CASN1EncBase* root = EncoderLC();

	// encode the object in a DER encoding
    HBufC8* der = HBufC8::NewMaxLC( root->LengthDER() );
    TPtr8 pder( der->Des() );
    TUint pos = 0;
    root->WriteDERL( pder, pos );
    CleanupStack::Pop( der );
    aResult = der;

	CleanupStack::PopAndDestroy( root );
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSOriginatorIdentifierOrKey::EncoderLC() const
	{
    switch( iType )
        {
        case EIssuerAndSerialNumber:
            {
            return iIssuerAndSerialNumber->EncoderLC();
            }
        case ESubjectKeyIdentifier:
            {
            return EncoderSubjectKeyIdentifierLC();
            }
        case EOriginatorKey:
            {
            return EncoderOriginatorKeyLC();
            }
        default:
            {
            User::Leave( KErrCorrupt );
			return NULL;
            }
        }
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::Type
// Type getter.
// -----------------------------------------------------------------------------
EXPORT_C CCMSOriginatorIdentifierOrKey::TType
CCMSOriginatorIdentifierOrKey::Type() const
	{
	return iType;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::IssuerAndSerialNumberL
// IssuerAndSerialNumber getter.
// -----------------------------------------------------------------------------
EXPORT_C const CCMSIssuerAndSerialNumber&
CCMSOriginatorIdentifierOrKey::IssuerAndSerialNumberL() const
	{
    if( !iIssuerAndSerialNumber )
        {
        User::Leave( KErrNotFound );
        }
	return *iIssuerAndSerialNumber;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::SubjectKeyIdentifierL
// SubjectKeyIdentifier getter.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8&
CCMSOriginatorIdentifierOrKey::SubjectKeyIdentifierL() const
	{
    if( !iSubjectKeyIdentifier )
        {
        User::Leave( KErrNotFound );
        }
	return *iSubjectKeyIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::SubjectKeyIdentifierL
// SubjectKeyIdentifier getter.
// -----------------------------------------------------------------------------
EXPORT_C const CCMSOriginatorPublicKey&
CCMSOriginatorIdentifierOrKey::OriginatorKeyL() const
	{
    if( !iOriginatorKey )
        {
        User::Leave( KErrNotFound );
        }
	return *iOriginatorKey;
	}



// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::SetIssuerAndSerialNumberL
// IssuerAndSerialNumber setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSOriginatorIdentifierOrKey::SetIssuerAndSerialNumberL(
	const CCMSIssuerAndSerialNumber& aIssuerAndSerialNumber )
	{
    CCMSIssuerAndSerialNumber* issuerAndSerialNumber =
        CCMSIssuerAndSerialNumber::NewL(
            aIssuerAndSerialNumber.IssuerName(),
            aIssuerAndSerialNumber.SerialNumber() );
    CleanupStack::PushL( issuerAndSerialNumber );
    DeleteOldValueAndChangeTypeL( EIssuerAndSerialNumber );
    iIssuerAndSerialNumber = issuerAndSerialNumber;
    CleanupStack::Pop( issuerAndSerialNumber );
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::SetSubjectKeyIdentifierL
// SubjectKeyIdentifier setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSOriginatorIdentifierOrKey::SetSubjectKeyIdentifierL(
	const TDesC8& aSubjectKeyIdentifier )
	{
    HBufC8* subjectKeyIdentifier = aSubjectKeyIdentifier.AllocLC();
    DeleteOldValueAndChangeTypeL( ESubjectKeyIdentifier );
    iSubjectKeyIdentifier = subjectKeyIdentifier;
    CleanupStack::Pop( subjectKeyIdentifier );
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::SetOriginatorKeyL
// OriginatorKey setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSOriginatorIdentifierOrKey::SetOriginatorKeyL(
	const CCMSOriginatorPublicKey& aOriginatorKey )
	{
    CCMSOriginatorPublicKey* originatorKey =
        CCMSOriginatorPublicKey::NewLC( aOriginatorKey );
    DeleteOldValueAndChangeTypeL( EOriginatorKey );
    iOriginatorKey = originatorKey;
    CleanupStack::Pop( originatorKey );
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::DeleteOldValueAndChangeType
// Deletes old value and changes type
// -----------------------------------------------------------------------------
void CCMSOriginatorIdentifierOrKey::DeleteOldValueAndChangeTypeL(
	CCMSOriginatorIdentifierOrKey::TType aNewType )
	{
    switch( iType )
        {
        case EIssuerAndSerialNumber:
            {
            delete iIssuerAndSerialNumber;
            iIssuerAndSerialNumber = NULL;
            break;
            }
        case ESubjectKeyIdentifier:
            {
            delete iSubjectKeyIdentifier;
            iSubjectKeyIdentifier = NULL;
            break;
            }
        case EOriginatorKey:
            {
            delete iOriginatorKey;
            iOriginatorKey = NULL;
            break;
            }
        default:
            {
            User::Leave( KErrCorrupt );
            }
        }
    iType = aNewType;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::EncoderSubjectKeyIdentifierLC
// returns encoder for the subjectKeyIdentifier
// -----------------------------------------------------------------------------
CASN1EncBase*
CCMSOriginatorIdentifierOrKey::EncoderSubjectKeyIdentifierLC() const
	{
    CASN1EncOctetString* subjectKeyIdentifier =
        CASN1EncOctetString::NewLC( *iSubjectKeyIdentifier );
    subjectKeyIdentifier->SetTag( ESubjectKeyIdentifier );

	return subjectKeyIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorIdentifierOrKey::EncoderOriginatorKeyL
// Returns encoder for the OriginatorKey value
// -----------------------------------------------------------------------------
CASN1EncBase* CCMSOriginatorIdentifierOrKey::EncoderOriginatorKeyLC() const
	{
    CASN1EncBase* originatorKey = iOriginatorKey->EncoderLC();
    originatorKey->SetTag( EOriginatorKey );

    return originatorKey;
	}

//  End of File
