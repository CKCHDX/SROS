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
#include    "CCMSKeyIdentifier.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KMinNumberOfSubModules = 1;
const TInt KMaxNumberOfSubModules = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::CCMSKeyIdentifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyIdentifier::CCMSKeyIdentifier()
    {
    }

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSKeyIdentifier::ConstructL(
	const TDesC8& aKeyIdentifier )
    {
	SetKeyIdentifierL( aKeyIdentifier );
    }

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSKeyIdentifier::ConstructL(
	const TDesC8& aKeyIdentifier,
    const TTime& aDate )
    {
	SetKeyIdentifierL( aKeyIdentifier );
    SetDateL( aDate );
    }

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyIdentifier* CCMSKeyIdentifier::NewL()
	{
	// creating with empty/default values
	CCMSKeyIdentifier* self = NewL( KNullDesC8() );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyIdentifier* CCMSKeyIdentifier::NewL(
    const TDesC8& aKeyIdentifier )
	{
    CCMSKeyIdentifier* self =
    	new( ELeave ) CCMSKeyIdentifier();

    CleanupStack::PushL( self );
    self->ConstructL( aKeyIdentifier );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSKeyIdentifier* CCMSKeyIdentifier::NewL(
    const TDesC8& aKeyIdentifier,
    const TTime& aDate )
	{
    CCMSKeyIdentifier* self =
    	new( ELeave ) CCMSKeyIdentifier();

    CleanupStack::PushL( self );
    self->ConstructL( aKeyIdentifier, aDate );
    CleanupStack::Pop( self );
	return self;
	}

// Destructor
CCMSKeyIdentifier::~CCMSKeyIdentifier()
    {
	delete iKeyIdentifier;
	delete iDate;
    }

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSKeyIdentifier::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* itemsData =
        DecodeSequenceLC( aRawData, KMinNumberOfSubModules,
                          KMaxNumberOfSubModules );

    DecodeArrayL( itemsData );

    CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSKeyIdentifier::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode the subjectKeyIdentifier
    CASN1EncOctetString* subjectKeyIdentifier =
        CASN1EncOctetString::NewLC( *iKeyIdentifier );
    root->AddAndPopChildL( subjectKeyIdentifier );

    // encode date if existing
    if( iDate )
        {
        CASN1EncGeneralizedTime* date =
            CASN1EncGeneralizedTime::NewLC( *iDate );
        root->AddAndPopChildL( date );
        }

    return root;
	}

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::KeyIdentifier
// Subject Key identifier getter
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8&
CCMSKeyIdentifier::KeyIdentifier() const
	{
	return *iKeyIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::Date
// Date getter
// -----------------------------------------------------------------------------
EXPORT_C const TTime* CCMSKeyIdentifier::Date() const
	{
	return iDate;
	}

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::SetKeyIdentifierL
// KeyIdentifier setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKeyIdentifier::SetKeyIdentifierL(
	const TDesC8& aKeyIdentifier )
	{
    HBufC8* subjectKeyIdentifier = aKeyIdentifier.AllocL();
    delete iKeyIdentifier;
    iKeyIdentifier = subjectKeyIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::SetDateL
// Date setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSKeyIdentifier::SetDateL(
	const TTime& aDate )
	{
    TTime* date = new( ELeave ) TTime( aDate );
    delete iDate;
    iDate = date;
	}

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::DecodeImplicitTagL
// Decrypts raw data with implicit tag
// -----------------------------------------------------------------------------
void CCMSKeyIdentifier::DecodeImplicitTagL(
    const TDesC8& aRawData,
    const TTagType aImplicitTag )
    {
    CArrayPtr< TASN1DecGeneric >* items = NULL;

    // Check the tag
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    // Accept only given tag
    if( decGen.Tag() != aImplicitTag )
        {
        User::Leave( KErrArgument );
        }
    TASN1DecSequence decSeq;
    items = decSeq.DecodeDERLC( decGen );
    TInt itemCount = items->Count();
    if( ( itemCount > KMaxNumberOfSubModules ) ||
        ( itemCount < KMinNumberOfSubModules ) )
        {
        User::Leave( KErrArgument );
        }
    DecodeArrayL( items );
    CleanupStack::PopAndDestroy( items );
    }

// -----------------------------------------------------------------------------
// CCMSKeyIdentifier::DecodeArrayL
// Decodes data from an array of decoders
// -----------------------------------------------------------------------------
void CCMSKeyIdentifier::DecodeArrayL(
    CArrayPtr< TASN1DecGeneric >* aItems )
    {
    // Caller should have made sure that there is at least one item
    __ASSERT_DEBUG( ( aItems->Count() > 0 ), User::Invariant() );
    
    // decode subjectKeyIdentifier
    TASN1DecGeneric gen( *aItems->At( 0 ) );
    gen.InitL();
    TASN1DecOctetString octetStringDecoder;
    HBufC8* tmp =
        octetStringDecoder.DecodeDERL( gen );
    delete iKeyIdentifier;
    iKeyIdentifier = tmp;

    // decode date if exists
    if( aItems->Count() > 1 )
        {
        TInt pos = 0;
        TASN1DecGeneralizedTime dateDec;
        TTime date = dateDec.DecodeDERL( aItems->At( 1 )->Encoding(), pos );
        SetDateL( date );
        }
    else
        {
        delete iDate;
        iDate = NULL;
        }

    // possible OtherKeyAttribute is not supported / ignored.
    }

//  End of File
