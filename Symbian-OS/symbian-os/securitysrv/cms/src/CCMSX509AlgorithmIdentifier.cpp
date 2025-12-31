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
#include    "CCMSX509AlgorithmIdentifier.h"
#include <x509cert.h>
#include <asn1dec.h>
#include <asn1enc.h>

const TInt KMinNumberOfSubModules = 1;
const TInt KMaxNumberOfSubModules = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::CCMSX509AlgorithmIdentifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AlgorithmIdentifier::CCMSX509AlgorithmIdentifier()
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509AlgorithmIdentifier::ConstructL(
	const TAlgorithmId& aAlgorithmId )
    {
	TAlgorithmId id = aAlgorithmId;
	iAlgorithmIdentifier = CAlgorithmIdentifier::NewL( id, KNullDesC8() );
    }

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509AlgorithmIdentifier::ConstructL(
	const CAlgorithmIdentifier& aAlgorithmIdentifier )
    {
	iAlgorithmIdentifier = CAlgorithmIdentifier::NewL( aAlgorithmIdentifier );
    }

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AlgorithmIdentifier* CCMSX509AlgorithmIdentifier::NewL()
	{
    return NewL( ERSA );
	}

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AlgorithmIdentifier* CCMSX509AlgorithmIdentifier::NewL(
	const TAlgorithmId& aAlgorithmId )
    {
    CCMSX509AlgorithmIdentifier* self = new( ELeave ) CCMSX509AlgorithmIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL( aAlgorithmId );
	CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AlgorithmIdentifier* CCMSX509AlgorithmIdentifier::NewL(
	const CAlgorithmIdentifier& aAlgorithmIdentifier )
	{
	CCMSX509AlgorithmIdentifier* self = new( ELeave ) CCMSX509AlgorithmIdentifier();
	CleanupStack::PushL( self );
	self->ConstructL( aAlgorithmIdentifier );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AlgorithmIdentifier* CCMSX509AlgorithmIdentifier::NewL(
	const CAlgorithmIdentifier& aAlgorithmIdentifier,
	const CAlgorithmIdentifier& aDigestIdentifier )
	{
	CCMSX509AlgorithmIdentifier* self = new( ELeave ) CCMSX509AlgorithmIdentifier();
	CleanupStack::PushL( self );
	self->ConstructL( aAlgorithmIdentifier );
	self->SetDigestAlgorithmL( &aDigestIdentifier );
	CleanupStack::Pop( self );
    return self;
	}

// Destructor
CCMSX509AlgorithmIdentifier::~CCMSX509AlgorithmIdentifier()
    {
	delete iAlgorithmIdentifier;
	delete iDigestAlgorithmIdentifier;
    }

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509AlgorithmIdentifier::DecodeL( const TDesC8& aRawData )
	{

    CAlgorithmIdentifier* tmpAlgorithmIdentifier = NULL;
    CAlgorithmIdentifier* tmpDigestAI = NULL;

    // try simple algorithm
	TRAPD( error, tmpAlgorithmIdentifier =
					CX509AlgorithmIdentifier::NewL( aRawData ) ); 

	// it is not simple algorithm identifier, trying with SigningAlgorithm
	if( error == KErrNotSupported )
		{
        CX509SigningAlgorithmIdentifier* tmpSigning = NULL;
		TRAPD( error2, tmpSigning =
               CX509SigningAlgorithmIdentifier::NewL( aRawData ) );
        if( error2 == KErrNone )
            {
            CleanupStack::PushL( tmpSigning );
            tmpAlgorithmIdentifier = CAlgorithmIdentifier::NewLC(
                tmpSigning->AsymmetricAlgorithm() );
            tmpDigestAI =
                CAlgorithmIdentifier::NewL( tmpSigning->DigestAlgorithm() );
            CleanupStack::Pop( tmpAlgorithmIdentifier );
            CleanupStack::PopAndDestroy( tmpSigning );
            }
        // it is not SigningAlgorithm, decode manually
        else if( error2 == KErrNotSupported )
            {
            CArrayPtr<TASN1DecGeneric>* itemsData =
							DecodeSequenceLC( aRawData,
											  KMinNumberOfSubModules,
											  KMaxNumberOfSubModules );
            TInt pos = 0;
            TInt count = itemsData->Count();
            TASN1DecObjectIdentifier decOID;
            HBufC* oid = decOID.DecodeDERL( *( itemsData->At( pos++ ) ) );
            CleanupStack::PushL( oid );
            if( *oid == KSHA1 )
                {
                TAlgorithmId algId( ESHA1 );
                if( count > pos )
                    {
                    TASN1DecGeneric* gen = itemsData->At( pos );
                    TPtrC8 encodedParams( gen->Encoding() );
                    tmpAlgorithmIdentifier =
                        CAlgorithmIdentifier::NewL( algId, encodedParams );
                    }
                else
                    {
                    TPtrC8 encodedParams( KNullDesC8() );
                    tmpAlgorithmIdentifier =
                        CAlgorithmIdentifier::NewL( algId, encodedParams );
                    }
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            CleanupStack::PopAndDestroy( 2 ); // oid, itemsData
            }
        else
            {
            User::Leave( error2 );
            }
		}
	else if( error != KErrNone )
		{
		User::Leave( error );
		}
	delete iAlgorithmIdentifier;
	iAlgorithmIdentifier = tmpAlgorithmIdentifier;
	delete iDigestAlgorithmIdentifier;
	iDigestAlgorithmIdentifier = tmpDigestAI;
	}

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509AlgorithmIdentifier::EncoderLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();
	CASN1EncObjectIdentifier* oid = NULL;
	if( !iDigestAlgorithmIdentifier )
		{
		switch ( iAlgorithmIdentifier->Algorithm() )
			{
			case ERSA:
				{
				oid = CASN1EncObjectIdentifier::NewLC( KRSA );
				break;
				}
			case EDSA:
				{
				oid = CASN1EncObjectIdentifier::NewLC( KDSA );
				break;
				}
			case EDH:
				{
				oid = CASN1EncObjectIdentifier::NewLC( KDH );
				break;
				}
			case EMD2:
				{
				oid = CASN1EncObjectIdentifier::NewLC( KMD2 );
				break;
				}
			case EMD5:
				{
				oid = CASN1EncObjectIdentifier::NewLC( KMD5 );
				break;
				}
			case ESHA1:
				{
				oid = CASN1EncObjectIdentifier::NewLC( KSHA1 );
				break;
				}
			default:
				{
				User::Leave( KErrArgument );
				}
			}
		}
	else
		{
		// only valid combinations are MD2WithRSA, MD5WithRSA
		// SHA1WithRSA and DSAWithSHA1
		if( iAlgorithmIdentifier->Algorithm() == ERSA )
			{
			switch ( iDigestAlgorithmIdentifier->Algorithm() )
				{
				case EMD2:
					{
					oid = CASN1EncObjectIdentifier::NewLC( KMD2WithRSA );
					break;
					}
				case EMD5:
					{
					oid = CASN1EncObjectIdentifier::NewLC( KMD5WithRSA );
					break;
					}
				case ESHA1:
					{
					oid = CASN1EncObjectIdentifier::NewLC( KSHA1WithRSA );
					break;
					}
				default:
					{
					User::Leave( KErrArgument );
					}
				}
			}
		else if( ( iAlgorithmIdentifier->Algorithm() == EDSA ) &&
				 ( iDigestAlgorithmIdentifier->Algorithm() == ESHA1 ) )
			{
			oid = CASN1EncObjectIdentifier::NewLC( KDSAWithSHA1 );
			}
		else
			{
			// not valid combination
			User::Leave( KErrArgument );
			}
		}

	// add algorithm id
	root->AddAndPopChildL( oid );

	// add parameters
	CASN1EncBase* parameters = NULL;
	if( iAlgorithmIdentifier->EncodedParams() != KNullDesC8() )
		{
		parameters = CASN1EncEncoding::NewLC( iAlgorithmIdentifier->EncodedParams() );
		}
	else
		{
		parameters = CASN1EncNull::NewLC();
		}
	root->AddAndPopChildL( parameters );

	return root;
	}

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::AlgorithmIdentifier
// Getter for AlgorithIdentifier
// -----------------------------------------------------------------------------
EXPORT_C const CAlgorithmIdentifier& CCMSX509AlgorithmIdentifier::AlgorithmIdentifier() const
	{
	return *iAlgorithmIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::SetAlgorithmIdentifierL
// Setter for attribute type, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AlgorithmIdentifier::SetAlgorithmIdentifierL(
	const CAlgorithmIdentifier& aAlgorithmIdentifier )
	{
	CAlgorithmIdentifier* tmpAi = CAlgorithmIdentifier::NewL( aAlgorithmIdentifier );
	delete iAlgorithmIdentifier;
	iAlgorithmIdentifier = tmpAi;
	}

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::DigestAlgorithm
// Getter for DigestAlgorithIdentifier
// -----------------------------------------------------------------------------
EXPORT_C const CAlgorithmIdentifier* CCMSX509AlgorithmIdentifier::DigestAlgorithm() const
	{
	return iDigestAlgorithmIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSX509AlgorithmIdentifier::SetDigestAlgorithmL
// Setter for digest algorithm identifier, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AlgorithmIdentifier::SetDigestAlgorithmL(
	const CAlgorithmIdentifier* aAlgorithmIdentifier )
	{
	CAlgorithmIdentifier* tmpAi = CAlgorithmIdentifier::NewL( *aAlgorithmIdentifier );
	delete iDigestAlgorithmIdentifier;
	iDigestAlgorithmIdentifier = tmpAi;
	}
//  End of File
