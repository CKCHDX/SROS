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
#include    "CCMSContentInfo.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KMinNumberOfSubModules = 2;
const TInt KMaxNumberOfSubModules = 2;
const TUint8 KContentTag = 0;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSContentInfo::CCMSContentInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSContentInfo::CCMSContentInfo()
    {
    }

// -----------------------------------------------------------------------------
// CCMSContentInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSContentInfo::ConstructL()
    {
	// creating with empty values
	SetContentTypeL( KNullDesC() );
    SetContentL( KNullDesC8() );
    }

// -----------------------------------------------------------------------------
// CCMSContentInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSContentInfo::ConstructL(
	const TDesC& aContentType )
    {
	SetContentTypeL( aContentType );
    }

// -----------------------------------------------------------------------------
// CCMSContentInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSContentInfo::ConstructL(
	const TDesC& aContentType,
    const TDesC8& aContent )
    {
	SetContentTypeL( aContentType );
    SetContentL( aContent );
    }

// -----------------------------------------------------------------------------
// CCMSContentInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSContentInfo* CCMSContentInfo::NewLC()
	{
    CCMSContentInfo* self = new( ELeave ) CCMSContentInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSContentInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSContentInfo* CCMSContentInfo::NewLC(
    const TDesC& aContentType,
    const TDesC8& aContent )
	{
    CCMSContentInfo* self = new( ELeave ) CCMSContentInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aContentType, aContent );
    return self;
	}

// Destructor
CCMSContentInfo::~CCMSContentInfo()
    {
    delete iContent;
	delete iContentType;
    }

// -----------------------------------------------------------------------------
// CCMSContentInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSContentInfo::DecodeL( const TDesC8& aRawData )
	{
	CArrayPtr<TASN1DecGeneric>* itemsData = DecodeSequenceLC( aRawData,
															  KMinNumberOfSubModules,
															  KMaxNumberOfSubModules );
	// we would not get this far if there is not 2
	// decoding attribute type
	TASN1DecObjectIdentifier decOid;
	HBufC* oid = decOid.DecodeDERL( *itemsData->At( 0 ) );
	delete iContentType;
	iContentType = oid;

	// decoding content
	HBufC8* contDesc = NULL;
    TASN1DecGeneric taggedContent( *itemsData->At( 1 ) );
    if( taggedContent.Tag() != KContentTag )
        {
        User::Leave( KErrArgument );
        }
    contDesc = taggedContent.GetContentDER().AllocL();

	delete iContent;
	iContent = contDesc;
	CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSContentInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------
CASN1EncBase* CCMSContentInfo::EncoderLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();

	// add attribute type
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC( *iContentType );
	root->AddAndPopChildL( oid );

	// add content
    CASN1EncEncoding* content = CASN1EncEncoding::NewL( *iContent );
    // explicitly tagged to 0
    // Takes ownership of the encoder, *including* the case when
    // this method leaves.
    CASN1EncExplicitTag* explicitTag =
        CASN1EncExplicitTag::NewLC( content, KContentTag );
    root->AddAndPopChildL( explicitTag );
	return root;
	}

// -----------------------------------------------------------------------------
// CCMSContentInfo::Content
// Getter for content
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8& CCMSContentInfo::Content() const
	{
	return *iContent;
	}

// -----------------------------------------------------------------------------
// CCMSContentInfo::SetContentL
// Setter for content, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSContentInfo::SetContentL( const TDesC8& aContent )
	{
	HBufC8* tmp = aContent.AllocL();
	delete iContent;
	iContent = tmp;
	}


// -----------------------------------------------------------------------------
// CCMSContentInfo::ContentType
// Getter for content type
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CCMSContentInfo::ContentType() const
	{
	return *iContentType;
	}

// -----------------------------------------------------------------------------
// CCMSContentInfo::SetContentTypeL
// Setter for content type, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSContentInfo::SetContentTypeL( const TDesC& aContentType )
	{
	HBufC* tmp = aContentType.AllocL();
	delete iContentType;
	iContentType = tmp;
	}
//  End of File
