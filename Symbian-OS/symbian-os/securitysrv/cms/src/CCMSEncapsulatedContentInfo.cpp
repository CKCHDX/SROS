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
#include    "CCMSEncapsulatedContentInfo.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KMinNumberOfSubModules = 1;
const TInt KMaxNumberOfSubModules = 2;
const TUint8 KContentTag = 0;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::CCMSEncapsulatedContentInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSEncapsulatedContentInfo::CCMSEncapsulatedContentInfo()
    {
    }

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSEncapsulatedContentInfo::ConstructL()
    {
	// creating with empty values
	CCMSContentInfo::ConstructL();
	SetContentL( NULL );
    }

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSEncapsulatedContentInfo::ConstructL(
	const TDesC& aContentType,
	const TDesC8* aContent )
    {
	CCMSContentInfo::ConstructL( aContentType );
	SetContentL( aContent );
    }

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSEncapsulatedContentInfo* CCMSEncapsulatedContentInfo::NewL()
	{
	CCMSEncapsulatedContentInfo* self = NewLC();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSEncapsulatedContentInfo* CCMSEncapsulatedContentInfo::NewLC()
	{
	CCMSEncapsulatedContentInfo* self = new( ELeave ) CCMSEncapsulatedContentInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSEncapsulatedContentInfo* CCMSEncapsulatedContentInfo::NewL(
	const TDesC& aContentType,
	const TDesC8* aContent )
    {
    CCMSEncapsulatedContentInfo* self = NewLC( aContentType, aContent );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSEncapsulatedContentInfo* CCMSEncapsulatedContentInfo::NewLC(
	const TDesC& aContentType,
	const TDesC8* aContent )
    {
    CCMSEncapsulatedContentInfo* self = new( ELeave ) CCMSEncapsulatedContentInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aContentType, aContent );
    return self;
    }

// Destructor
CCMSEncapsulatedContentInfo::~CCMSEncapsulatedContentInfo()
    {
    }

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSEncapsulatedContentInfo::DecodeL( const TDesC8& aRawData )
	{
	CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC( aRawData,
															  KMinNumberOfSubModules,
															  KMaxNumberOfSubModules );
	// we would not get this far if there is not atleast one
	// decoding attribute type
	TASN1DecObjectIdentifier decOid;
	HBufC* oid = decOid.DecodeDERL( *itemsData->At( 0 ) );
	delete iContentType;
	iContentType = oid;

	// decoding possible content
	HBufC8* contDesc = NULL;
	if( itemsData->Count() > 1 )
		{
		TASN1DecGeneric taggedContent( *itemsData->At( 1 ) );
		if( taggedContent.Tag() != KContentTag )
			{
			User::Leave( KErrArgument );
			}
		TASN1DecOctetString content;
		TInt pos = 0;
		contDesc = content.DecodeDERL( taggedContent.GetContentDER(), pos );
		}
	delete iContent;
	iContent = contDesc;
	CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSEncapsulatedContentInfo::EncoderLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();

	// add attribute type
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC( *iContentType );
	root->AddAndPopChildL( oid );

	// add possible content
	if( iContent )
		{
		CASN1EncOctetString* content = CASN1EncOctetString::NewL( *iContent );
		// explicitly tagged to 0
		// Takes ownership of the encoder, *including* the case when
		// this method leaves.
		CASN1EncExplicitTag* explicitTag =
			CASN1EncExplicitTag::NewLC( content, KContentTag );
		root->AddAndPopChildL( explicitTag );
		}
	return root;
	}
// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::Content
// Getter for content
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8* CCMSEncapsulatedContentInfo::Content() const
	{
	return iContent;
	}

// -----------------------------------------------------------------------------
// CCMSEncapsulatedContentInfo::SetContentL
// Setter for content, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSEncapsulatedContentInfo::SetContentL( const TDesC8* aContent )
	{
	HBufC8* tmp = NULL;
	if( aContent )
		{
		tmp = aContent->AllocL();
		}
	delete iContent;
	iContent = tmp;
	}

//  End of File
