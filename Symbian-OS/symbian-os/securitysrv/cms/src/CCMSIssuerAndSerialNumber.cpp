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
#include    "CCMSIssuerAndSerialNumber.h"
#include <x500dn.h>
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KNumberOfSubModules = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::CCMSIssuerAndSerialNumber
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSIssuerAndSerialNumber::CCMSIssuerAndSerialNumber()
    {
    }

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSIssuerAndSerialNumber::ConstructL(
	const CX500DistinguishedName& aIssuerName,
	const TDesC8& aSerialNumber )
    {
	SetIssuerNameL( aIssuerName );
	SetSerialNumberL( aSerialNumber );
    }

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSIssuerAndSerialNumber* CCMSIssuerAndSerialNumber::NewL()
	{
	// creating with empty values
	CArrayPtr<CX520AttributeTypeAndValue>* empty =
				new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue>( 1 );
	CleanupStack::PushL( empty );
	CX500DistinguishedName* issuer = CX500DistinguishedName::NewL( *empty );
	CleanupStack::PushL( issuer );
	CCMSIssuerAndSerialNumber* self = NewL( *issuer, KNullDesC8() );
	CleanupStack::PopAndDestroy( issuer );
	CleanupStack::PopAndDestroy( empty );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSIssuerAndSerialNumber* CCMSIssuerAndSerialNumber::NewL(
	const CX500DistinguishedName& aIssuerName,
	const TDesC8& aSerialNumber )
    {
    CCMSIssuerAndSerialNumber* self =
    	new( ELeave ) CCMSIssuerAndSerialNumber();

    CleanupStack::PushL( self );
    self->ConstructL( aIssuerName, aSerialNumber );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CCMSIssuerAndSerialNumber::~CCMSIssuerAndSerialNumber()
    {
	delete iIssuerName;
	delete iSerialNumber;
    }

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSIssuerAndSerialNumber::DecodeL( const TDesC8& aRawData )
	{
	CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC( aRawData,
															  KNumberOfSubModules,
															  KNumberOfSubModules );
	// we would not get this far if there is not 2 elements

	// decoding name
	CX500DistinguishedName* name =
			CX500DistinguishedName::NewL( itemsData->At( 0 )->Encoding() );
	delete iIssuerName;
	iIssuerName = name;

	// deocoding serial number
	TASN1DecGeneric gen( *itemsData->At( 1 ) );
	gen.InitL();
	HBufC8* tmp = gen.GetContentDER().AllocL();
	delete iSerialNumber;
	iSerialNumber = tmp;

	CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSIssuerAndSerialNumber::EncoderLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();

	// add issuer name
	CASN1EncSequence* issuerName = iIssuerName->EncodeASN1LC();
	root->AddAndPopChildL( issuerName );

	// add serial number
	CASN1EncOctetString* serial = CASN1EncOctetString::NewLC( *iSerialNumber );
	// changing tag to integer
	serial->SetTag( EASN1Integer, EUniversal );
	root->AddAndPopChildL( serial );
	return root;
	}

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::IssuerName
// Issuer name getter
// -----------------------------------------------------------------------------
EXPORT_C const CX500DistinguishedName& CCMSIssuerAndSerialNumber::IssuerName() const
	{
	return *iIssuerName;
	}

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::SerialNumber
// Serial number getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8& CCMSIssuerAndSerialNumber::SerialNumber() const
	{
	return *iSerialNumber;
	}

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::SetIssuerNameL
// Issuer name setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSIssuerAndSerialNumber::SetIssuerNameL(
	const CX500DistinguishedName& aIssuerName )
	{
	CX500DistinguishedName* name = CX500DistinguishedName::NewL( aIssuerName );
	delete iIssuerName;
	iIssuerName = name;
	}

// -----------------------------------------------------------------------------
// CCMSIssuerAndSerialNumber::SetSerialNumber
// Serial number setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSIssuerAndSerialNumber::SetSerialNumberL(
	const TDesC8& aSerialNumber )
	{
	HBufC8* tmp = aSerialNumber.AllocL();
	delete iSerialNumber;
	iSerialNumber = tmp;
	}

//  End of File
