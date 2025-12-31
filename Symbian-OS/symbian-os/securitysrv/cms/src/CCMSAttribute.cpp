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
#include    "CCMSAttribute.h"
#include "TCMSTimeUtil.h"
#include <x500dn.h>
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KMaxNumberOfSubModules = 3;
const TInt KMinNumberOfSubModules = 2;
const TInt KDefaultGranularity = 1;

// CMS SignedAttributes useful types
_LIT( KContentTypeOID, "1.2.840.113549.1.9.3" );
_LIT( KMessageDigestOID, "1.2.840.113549.1.9.4" );
_LIT( KSignTimeOID, "1.2.840.113549.1.9.5" );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMSCreateEncoderLC Creates an encoder based on the gived OID value.
// Returns: Encoder for the given encoding
// -----------------------------------------------------------------------------
//
CASN1EncBase* CMSCreateEncoderLC( const TDesC& aAttributeType, const TDesC8& aEncoding )
    {
    CASN1EncBase* retVal = NULL;
    if( aAttributeType == KContentTypeOID )
        {
        // encoding data, have to decode it before encoding it again
        // because CASN1EncEncoding changes TagType to Constructed
        TASN1DecObjectIdentifier dataDec;
        TInt pos = 0;
        HBufC* data = dataDec.DecodeDERL( aEncoding, pos );
        CleanupStack::PushL( data );
        retVal = CASN1EncObjectIdentifier::NewL( *data );
        CleanupStack::PopAndDestroy( data );
        CleanupStack::PushL( retVal );
        }
    else if( aAttributeType == KMessageDigestOID )
        {
        TASN1DecOctetString dataDec;
        TInt pos = 0;
        HBufC8* data = dataDec.DecodeDERL( aEncoding, pos );
        CleanupStack::PushL( data );
        retVal = CASN1EncOctetString::NewL( *data );
        CleanupStack::PopAndDestroy( data );
        CleanupStack::PushL( retVal );
        }
    else if( aAttributeType == KSignTimeOID )
        {
        TTime time = TCMSTimeUtil::ConvertToTimeL( aEncoding );
        retVal = TCMSTimeUtil::ConvertToEncoderLC( time );
        }
    else
        {
        // default is to wrap inside encencoder
        retVal = CASN1EncEncoding::NewLC( aEncoding );
        }
    return retVal;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSAttribute::CCMSAttribute
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAttribute::CCMSAttribute()
    {
    }

// -----------------------------------------------------------------------------
// CCMSAttribute::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSAttribute::ConstructL()
    {
	// creating with empty values
	iAttributeType = KNullDesC().AllocL();

	iAttributeValues =
		new(ELeave) CDesC8ArrayFlat( KDefaultGranularity );
    }

// -----------------------------------------------------------------------------
// CCMSAttribute::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSAttribute::ConstructL(
	const TDesC& aAttributeType,
	const CDesC8Array& aAttributeValues )
    {
	SetAttributeTypeL( aAttributeType );
	SetAttributeValuesL( aAttributeValues );
    }

// -----------------------------------------------------------------------------
// CCMSAttribute::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSAttribute::ConstructL(
	const TDesC& aAttributeType,
	const TDesC8& aAttributeValue )
    {
	SetAttributeTypeL( aAttributeType );
	iAttributeValues =
		new(ELeave) CDesC8ArrayFlat( KDefaultGranularity );
	iAttributeValues->AppendL( aAttributeValue );
    }

// -----------------------------------------------------------------------------
// CCMSAttribute::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAttribute* CCMSAttribute::NewLC()
	{
	CCMSAttribute* self = new( ELeave ) CCMSAttribute();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAttribute* CCMSAttribute::NewL()
	{
	CCMSAttribute* self = NewLC();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAttribute* CCMSAttribute::NewL(
	const TDesC& aAttributeType,
	const CDesC8Array& aAttributeValues )
	{
	CCMSAttribute* self = NewLC( aAttributeType, aAttributeValues );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAttribute* CCMSAttribute::NewLC(
	const TDesC& aAttributeType,
	const CDesC8Array& aAttributeValues )
    {
    CCMSAttribute* self = new( ELeave ) CCMSAttribute();
    CleanupStack::PushL( self );
    self->ConstructL( aAttributeType, aAttributeValues );
    return self;
    }

// -----------------------------------------------------------------------------
// CCMSAttribute::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAttribute* CCMSAttribute::NewLC(
	const TDesC& aAttributeType,
	const TDesC8& aAttributeValue )
	{
	CCMSAttribute* self = new( ELeave ) CCMSAttribute();
	CleanupStack::PushL( self );
	self->ConstructL( aAttributeType, aAttributeValue );
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAttribute* CCMSAttribute::NewL(
	const TDesC& aAttributeType,
	const TDesC8& aAttributeValue )
	{
	CCMSAttribute* self = NewLC( aAttributeType, aAttributeValue );
	CleanupStack::Pop( self );
    return self;
	}

// Destructor
CCMSAttribute::~CCMSAttribute()
    {
	delete iAttributeType;
	delete iAttributeValues;
    }

// -----------------------------------------------------------------------------
// CCMSAttribute::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSAttribute::DecodeL( const TDesC8& aRawData )
	{
	CArrayPtr<TASN1DecGeneric>* itemsData =
        DecodeSequenceLC( aRawData,
                          KMinNumberOfSubModules,
                          KMaxNumberOfSubModules );
	// we would not get this far if there is not 2 or 3 elements
	// decoding attribute type
	TASN1DecObjectIdentifier decOid;
	HBufC* oid = decOid.DecodeDERL( *itemsData->At( 0 ) );
	delete iAttributeType;
	iAttributeType = oid;

	// deocoding attribute values
	TASN1DecSet decSet;
	CArrayPtrFlat<TASN1DecGeneric>* attValues =
					decSet.DecodeDERLC( *itemsData->At( 1 ) );
	TInt numOfAttValues = attValues->Count();
	CDesC8ArrayFlat* tmpArray = new( ELeave ) CDesC8ArrayFlat( numOfAttValues );
	CleanupStack::PushL( tmpArray );
	for( TInt i = 0; i < numOfAttValues; i++ )
		{
		tmpArray->AppendL( attValues->At( i )->Encoding() );
		}
	delete iAttributeValues;
	iAttributeValues = tmpArray;
	// compress to get rid of possible unneccessary slots
	iAttributeValues->Compress();

    // possible context values are ignored

	CleanupStack::Pop( tmpArray );
	CleanupStack::PopAndDestroy( attValues );
	CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSAttribute::EncoderLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();

	// add attribute type
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC( *iAttributeType );
	root->AddAndPopChildL( oid );

	// add attribute values
	CASN1EncSequence* values = CASN1EncSequence::NewLC();

	// change tag to SET
	values->SetTag( EASN1Set, EUniversal );

	TInt numOfValues = iAttributeValues->Count();
	for( TInt i = 0; i < numOfValues; i++ )
		{
        CASN1EncBase* enc = CMSCreateEncoderLC( *iAttributeType, ( *iAttributeValues )[ i ] );
		values->AddAndPopChildL( enc );
		}

	root->AddAndPopChildL( values );
	return root;
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::AttributeType
// Getter for attribute type
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CCMSAttribute::AttributeType() const
	{
	return *iAttributeType;
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::AttributeValues
// Getter for attribute values
// -----------------------------------------------------------------------------
EXPORT_C const CDesC8Array& CCMSAttribute::AttributeValues() const
	{
	return *iAttributeValues;
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::SetAttributeTypeL
// Setter for attribute type, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAttribute::SetAttributeTypeL( const TDesC& aAttributeType )
	{
	HBufC* tmp = aAttributeType.AllocL();
	delete iAttributeType;
	iAttributeType = tmp;
	}

// -----------------------------------------------------------------------------
// CCMSAttribute::SetAttributeTypeL
// Setter for attribute values, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAttribute::SetAttributeValuesL(
	const CDesC8Array& aAttributeValues )
	{
	TInt numOfValues = aAttributeValues.Count();
	// Making 1 extra slot
	CDesC8ArrayFlat* tmp = new(ELeave) CDesC8ArrayFlat( numOfValues + 1 );
	CleanupStack::PushL( tmp );
	for( TInt i = 0; i < numOfValues; i++ )
		{
		tmp->AppendL( aAttributeValues[ i ] );
		}

	// Compressing
	tmp->Compress();
	CleanupStack::Pop( tmp );
	delete iAttributeValues;
	iAttributeValues = tmp;
	}

//  End of File
