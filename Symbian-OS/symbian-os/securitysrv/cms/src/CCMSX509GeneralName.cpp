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
#include    "CCMSX509GeneralName.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TTagType KMaxTag = 8;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::CCMSX509GeneralName
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509GeneralName::CCMSX509GeneralName( )
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509GeneralName::ConstructL(
    const TTagType aTag,
    const TDesC8& aData )
    {
    SetTagL( aTag );
    SetDataL( aData );
    }

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509GeneralName* CCMSX509GeneralName::NewL()
	{
	// creating with empty/default values
	CCMSX509GeneralName* self = NewL( 0, KNullDesC8() );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509GeneralName* CCMSX509GeneralName::NewL(
    const TTagType aTag,
    const TDesC8& aData )
    {
    CCMSX509GeneralName* self =
    	new( ELeave ) CCMSX509GeneralName( );

    CleanupStack::PushL( self );
    self->ConstructL( aTag, aData );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CCMSX509GeneralName::~CCMSX509GeneralName()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509GeneralName::DecodeL( const TDesC8& aRawData )
	{
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    TTagType tag = decGen.Tag();
    if( tag > KMaxTag )
        {
        User::Leave( KErrArgument );
        }
    HBufC8* data = decGen.GetContentDER().AllocL();
    iTag = tag;
    delete iData;
    iData = data;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::EncodeL
// Encrypts this instance to descriptor
// -----------------------------------------------------------------------------

void CCMSX509GeneralName::EncodeL( HBufC8*& aResult ) const
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
// CCMSX509GeneralName::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509GeneralName::EncoderLC() const
	{
    TASN1DecGeneric genDec( *iData );
    genDec.InitL();
    TTagType dataTag( genDec.Tag() );
    TASN1Class dataClass( genDec.Class() );
    TUint8 fullTag = *( iData->Ptr() );
    CASN1EncBase* baseEnc = NULL;

    // is the data constructed?
    if( ( fullTag & 0x20 ) != 0 )
        {
        baseEnc = CASN1EncEncoding::NewL( *iData );
        }
    else
        {
        TASN1DecOctetString dataDec;
        TInt pos = 0;
        HBufC8* data = dataDec.DecodeDERL( *iData, pos );
        CleanupStack::PushL( data );
        baseEnc = CASN1EncOctetString::NewL( *data );
        CleanupStack::PopAndDestroy( data );
        }
    baseEnc->SetTag( dataTag, dataClass );
    // CASN1EncExplicitTag takes ownership of dataEnc, even when Leaving
    CASN1EncExplicitTag* taggedEnc =
        CASN1EncExplicitTag::NewLC( baseEnc, iTag );

    return taggedEnc;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::Tag
// Tag getter.
// -----------------------------------------------------------------------------
EXPORT_C TTagType CCMSX509GeneralName::Tag() const
	{
	return iTag;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::Data
// Data getter.
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8& CCMSX509GeneralName::Data() const
	{
	return *iData;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::SetTagL
// Tag setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509GeneralName::SetTagL(
	const TTagType aTag )
	{
    if( aTag > KMaxTag )
        {
        User::Leave( KErrArgument );
        }
    iTag = aTag;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralName::SetDataL
// Data setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509GeneralName::SetDataL(
	const TDesC8& aData )
	{
    HBufC8* data = aData.AllocL();
    delete iData;
    iData = data;
	}

//  End of File
