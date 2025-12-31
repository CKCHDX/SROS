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
* Description:  X.509 GeneralNames type
*
*/


// INCLUDE FILES
#include    "CCMSX509GeneralNames.h"
#include "CCMSX509GeneralName.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KCMSGranularity = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::CCMSX509GeneralNames
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509GeneralNames::CCMSX509GeneralNames()
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509GeneralNames::ConstructL(
    const CArrayPtr< CCMSX509GeneralName >& aGeneralNames )
    {
    SetGeneralNamesL( aGeneralNames );
    }

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509GeneralNames* CCMSX509GeneralNames::NewL()
	{
	// creating with empty/default values
    CCMSX509GeneralNames* self = new( ELeave ) CCMSX509GeneralNames();
    CleanupStack::PushL( self );
    CArrayPtr< CCMSX509GeneralName >* nameArray =
        new( ELeave ) CArrayPtrFlat< CCMSX509GeneralName >( KCMSGranularity );
    self->iGeneralNames = nameArray;
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509GeneralNames* CCMSX509GeneralNames::NewL(
    const CArrayPtr< CCMSX509GeneralName >& aGeneralNames )
	{
    CCMSX509GeneralNames* self =
    	new( ELeave ) CCMSX509GeneralNames();

    CleanupStack::PushL( self );
    self->ConstructL( aGeneralNames );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509GeneralNames* CCMSX509GeneralNames::NewL(
    const CCMSX509GeneralNames& aGeneralNames )
	{
    CCMSX509GeneralNames* self =
    	new( ELeave ) CCMSX509GeneralNames();

    CleanupStack::PushL( self );
    self->ConstructL( aGeneralNames.GeneralNames() );
    CleanupStack::Pop( self );
	return self;
	}

// Destructor
CCMSX509GeneralNames::~CCMSX509GeneralNames()
    {
    if( iGeneralNames )
        {
        iGeneralNames->ResetAndDestroy();
        delete iGeneralNames;
        }
    }

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509GeneralNames::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* nameData =
        DecodeSequenceLC( aRawData );
    TInt nameCount = nameData->Count();
    if( nameCount == 0 )
        {
        User::Leave( KErrArgument );
        }
    CArrayPtr< CCMSX509GeneralName >* names =
        new( ELeave ) CArrayPtrFlat< CCMSX509GeneralName >( nameCount );
    CleanupStack::PushL( names );
    for( TInt i = 0; i < nameCount; i++ )
        {
        CCMSX509GeneralName* name = CCMSX509GeneralName::NewL( );
        CleanupStack::PushL( name );
        name->DecodeL( nameData->At( i )->Encoding() );
        names->AppendL( name );
        }
    CleanupStack::Pop( nameCount ); // names
    CleanupStack::Pop( names );
    CleanupStack::PopAndDestroy( nameData );
    if( iGeneralNames )
        {
        iGeneralNames->ResetAndDestroy();
        delete iGeneralNames;
        }
    iGeneralNames = names;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509GeneralNames::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();
    TInt nameCount = iGeneralNames->Count();
    for( TInt i = 0; i < nameCount; i++ )
        {
        HBufC8* encodedName = NULL;
        iGeneralNames->At( i )->EncodeL( encodedName );
        CleanupStack::PushL( encodedName );
        CASN1EncEncoding* singleName = CASN1EncEncoding::NewLC( *encodedName );
        root->AddAndPopChildL( singleName );
        CleanupStack::PopAndDestroy( encodedName );
        }

   	return root;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::GeneralNames
// GeneralNames getter
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSX509GeneralName >&
CCMSX509GeneralNames::GeneralNames() const
	{
	return *iGeneralNames;
	}

// -----------------------------------------------------------------------------
// CCMSX509GeneralNames::SetGeneralNamesL
// GeneralNames setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509GeneralNames::SetGeneralNamesL(
	const CArrayPtr< CCMSX509GeneralName >& aGeneralNames )
	{
    TInt nameCount = aGeneralNames.Count();
    if( nameCount == 0 )
        {
        User::Leave ( KErrArgument );
        }
    CArrayPtr< CCMSX509GeneralName >* names =
        new( ELeave ) CArrayPtrFlat< CCMSX509GeneralName >( nameCount );
    CleanupStack::PushL( names );
    for( TInt i = 0; i < nameCount; i++ )
        {
        CCMSX509GeneralName* origName = aGeneralNames[ i ];
        CCMSX509GeneralName* name =
            CCMSX509GeneralName::NewL( origName->Tag(), origName->Data() );
        CleanupStack::PushL( name );
        names->AppendL( name );
        }
    CleanupStack::Pop( nameCount ); // names
    CleanupStack::Pop( names );
    if( iGeneralNames )
        {
        iGeneralNames->ResetAndDestroy();
        delete iGeneralNames;
        }
    iGeneralNames = names;
	}

//  End of File
