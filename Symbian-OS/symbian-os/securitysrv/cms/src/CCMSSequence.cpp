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
#include    "CCMSSequence.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSSequence::CCMSSequence
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCMSSequence::CCMSSequence()
    {
    }

// -----------------------------------------------------------------------------
// CCMSSequence::DecodeSequenceLC
// Decodes raw-data to ASN.1 modules, pushes items to cleanupstack
// -----------------------------------------------------------------------------

EXPORT_C CArrayPtr<TASN1DecGeneric>* CCMSSequence::DecodeSequenceLC( const TDesC8& aRawData )
	{
	CArrayPtr<TASN1DecGeneric>* items = NULL;

	// Check we've got a sequence
	TASN1DecGeneric decGen( aRawData );
	decGen.InitL();
	// Accept only sequences or sets
	if( ( decGen.Tag() != EASN1Sequence ) &&
		( decGen.Tag() != EASN1Set ) )
		{
		User::Leave(KErrArgument);
		}
	else
		{
		// Decode the sequence
		TASN1DecSequence decSeq;
		items = decSeq.DecodeDERLC( decGen );
		}
	return items;
	}

// -----------------------------------------------------------------------------
// CCMSSequence::DecodeSequenceLC
// Decodes raw-data to ASN.1 modules, pushes items to cleanupstack
// Validates that there is right number of modules, leaves with KErrArgument if
// not.
// -----------------------------------------------------------------------------

EXPORT_C CArrayPtr<TASN1DecGeneric>* CCMSSequence::DecodeSequenceLC( const TDesC8& aRawData,
														 const TInt aMinTerms,
														 const TInt aMaxTerms)
	{
	CArrayPtr<TASN1DecGeneric>* items = DecodeSequenceLC(aRawData);
	TInt count = items->Count();
	if( ( count < aMinTerms ) ||
		( count > aMaxTerms ) )
		{
		// not in the range, leave
		User::Leave( KErrArgument );
		}

	return items;
	}

// -----------------------------------------------------------------------------
// CCMSSequence::EncodeL
// Encrypts this instance to descriptor
// -----------------------------------------------------------------------------

void CCMSSequence::EncodeL( HBufC8*& aResult ) const
	{
	CASN1EncBase* root = EncoderLC();

	// encode the object in a DER encoding
	HBufC8* der = CreateDerEncodingL( root );
	CleanupStack::PopAndDestroy( root );

	aResult = der;
	}

// -----------------------------------------------------------------------------
// CCMSSequence::CreateDerEncodingLC()
// Creates DER encoding
// -----------------------------------------------------------------------------
EXPORT_C HBufC8* CCMSSequence::CreateDerEncodingL( CASN1EncBase* aEncoding )
	{
	// encode the object in a DER encoding
	HBufC8* der = HBufC8::NewMaxLC( aEncoding->LengthDER() );
	TPtr8 pder( der->Des() );
	TUint pos = 0;
	aEncoding->WriteDERL( pder, pos );
	CleanupStack::Pop( der );
	return der;
	}
//  End of File
