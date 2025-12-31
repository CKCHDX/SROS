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
#include <x500dn.h>
#include <asn1dec.h>
#include <asn1enc.h>

#include "CCMSSignedData.h"

// CONSTANTS
const TInt KMinNumberOfSubModules = 4;
const TInt KMaxNumberOfSubModules = 6;
const TInt KDefaultGranularity = 1;
const TInt KDefaultVersion = 1;
const TInt KAlternativeVersion = 3;
const TUint8 KCertificateSetTag = 0;
const TUint8 KRevokedCertificatesTag = 1;

// Defaulta id-data oid
_LIT( KIDDataOID, "1.2.840.113549.1.7.1" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSSignedData::CCMSSignedData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSSignedData::CCMSSignedData()
    {
    }

// -----------------------------------------------------------------------------
// CCMSSignedData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSSignedData::ConstructL()
    {
	iVersion = KDefaultVersion;
	iDigestAlgorithmIdentifiers =
				new( ELeave )CArrayPtrFlat< CCMSX509AlgorithmIdentifier >
														( KDefaultGranularity );
	iContentInfo = CCMSEncapsulatedContentInfo::NewLC();
	CleanupStack::Pop( iContentInfo );
	iSignerInfos =
				new( ELeave )CArrayPtrFlat< CCMSSignerInfo >
														( KDefaultGranularity );
    }

// -----------------------------------------------------------------------------
// CCMSSignedData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSSignedData::ConstructL(
	const CArrayPtr< CCMSX509AlgorithmIdentifier >& aDigestAlgorithmIdentifiers,
   	const CCMSEncapsulatedContentInfo& aContentInfo,
   	const CArrayPtr< CCMSSignerInfo >& aSignerInfos,
   	const CArrayPtr< CCMSCertificateChoices >* aCertificates,
   	const CArrayPtr< CCMSX509CertificateList >* aRevokedCertificates )
    {
	SetSignerInfosL( aSignerInfos );
	SetDigestAlgorithmIdentifiersL( aDigestAlgorithmIdentifiers );
	SetEncapsulatedContentInfoL( aContentInfo );

	SetCertificatesL( aCertificates );
	SetRevokedCertificatesL( aRevokedCertificates );
    }

// -----------------------------------------------------------------------------
// CCMSSignedData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSSignedData* CCMSSignedData::NewLC()
	{
	CCMSSignedData* self = new( ELeave ) CCMSSignedData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSSignedData* CCMSSignedData::NewLC(
	const CArrayPtr< CCMSX509AlgorithmIdentifier >& aDigestAlgorithmIdentifiers,
   	const CCMSEncapsulatedContentInfo& aContentInfo,
   	const CArrayPtr< CCMSSignerInfo >& aSignerInfos )
    {
    CCMSSignedData* self = new( ELeave ) CCMSSignedData();
    CleanupStack::PushL( self );
    self->ConstructL( aDigestAlgorithmIdentifiers,
    				  aContentInfo,
    				  aSignerInfos,
    				  NULL,
    				  NULL );
    return self;
    }

// -----------------------------------------------------------------------------
// CCMSSignedData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSSignedData* CCMSSignedData::NewLC(
	const CArrayPtr< CCMSX509AlgorithmIdentifier >& aDigestAlgorithmIdentifiers,
   	const CCMSEncapsulatedContentInfo& aContentInfo,
   	const CArrayPtr< CCMSSignerInfo >& aSignerInfos,
   	const CArrayPtr< CCMSCertificateChoices >* aCertificates,
   	const CArrayPtr< CCMSX509CertificateList >* aRevokedCertificates )
    {
    CCMSSignedData* self = new( ELeave ) CCMSSignedData();
    CleanupStack::PushL( self );
    self->ConstructL( aDigestAlgorithmIdentifiers,
    				  aContentInfo,
    				  aSignerInfos,
    				  aCertificates,
    				  aRevokedCertificates );
    return self;
    }

// Destructor
CCMSSignedData::~CCMSSignedData()
    {
	if( iDigestAlgorithmIdentifiers )
		{
		iDigestAlgorithmIdentifiers->ResetAndDestroy();
		delete iDigestAlgorithmIdentifiers;
		}
	delete iContentInfo;
	if( iSignerInfos )
		{
		iSignerInfos->ResetAndDestroy();
		delete iSignerInfos;
		}
	if( iCertificates )
		{
		iCertificates->ResetAndDestroy();
		delete iCertificates;
		}
	if( iRevokedCertificates )
		{
		iRevokedCertificates->ResetAndDestroy();
		delete iRevokedCertificates;
		}
    }

// -----------------------------------------------------------------------------
// CCMSSignedData::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSSignedData::DecodeL( const TDesC8& aRawData )
	{
	CArrayPtr<TASN1DecGeneric>* itemsData =
							DecodeSequenceLC( aRawData,
											  KMinNumberOfSubModules,
											  KMaxNumberOfSubModules );
	TInt pos = 0;
	// decode Version
	TASN1DecInteger version;
	TInt tmpVersion;
	tmpVersion = version.DecodeDERShortL( *itemsData->At( pos++ ) );

	// decode digest algorithms
	CArrayPtrFlat< CCMSX509AlgorithmIdentifier >* tmpAlgs =
		new( ELeave )CArrayPtrFlat< CCMSX509AlgorithmIdentifier >
													( KDefaultGranularity );
	CleanupStack::PushL( tmpAlgs );
	CleanupResetAndDestroyPushL( *tmpAlgs );

	//there must be atleast 1 algorithm
	CArrayPtr<TASN1DecGeneric>* digestAlgorithms =
		DecodeSequenceLC( itemsData->At( pos++ )->Encoding(),
						  1,
						  KMaxTInt );
	TInt algCount = digestAlgorithms->Count();
	for( TInt i = 0; i < algCount; i++ )
		{
		CCMSX509AlgorithmIdentifier* alg =
										CCMSX509AlgorithmIdentifier::NewL();
		CleanupStack::PushL( alg );
		alg->DecodeL( digestAlgorithms->At( i )->Encoding() );
		tmpAlgs->AppendL( alg );
		CleanupStack::Pop( alg );
		}
	CleanupStack::PopAndDestroy( digestAlgorithms );

	// decode encapsulated content info
	CCMSEncapsulatedContentInfo* tmpInfo = CCMSEncapsulatedContentInfo::NewLC();
	tmpInfo->DecodeL( itemsData->At( pos++ )->Encoding() );

	CArrayPtrFlat< CCMSCertificateChoices >* tmpCerts = NULL;

	// decode possible certificate set
	if( itemsData->At( pos )->Tag() == KCertificateSetTag )
		{
		tmpCerts =
			new( ELeave )CArrayPtrFlat< CCMSCertificateChoices >
													( KDefaultGranularity );
		CleanupStack::PushL( tmpCerts );
		CleanupResetAndDestroyPushL( *tmpCerts );
		TASN1DecSequence decSeq;
		CArrayPtr<TASN1DecGeneric>* certs =
			decSeq.DecodeDERLC( *itemsData->At( pos++ ) );
		TInt certCount = certs->Count();
		for( TInt i = 0; i < certCount; i++ )
			{
			CCMSCertificateChoices* tmpCert =
				CCMSCertificateChoices::NewLC();
			tmpCert->DecodeL( certs->At( i )->Encoding() );
			tmpCerts->AppendL( tmpCert );
			CleanupStack::Pop( tmpCert );
			}
		CleanupStack::PopAndDestroy( certs );
		}

	// decode possible revoked certificates
	CArrayPtrFlat< CCMSX509CertificateList >* tmpRevokedCertificates = NULL;
	if( ( pos < itemsData->Count() ) &&
		( itemsData->At( pos )->Tag() == KRevokedCertificatesTag ) )
		{
		tmpRevokedCertificates =
			new( ELeave )CArrayPtrFlat< CCMSX509CertificateList >
													( KDefaultGranularity );
		CleanupStack::PushL( tmpRevokedCertificates );
		CleanupResetAndDestroyPushL( *tmpRevokedCertificates );

		TASN1DecSequence decSeq;
		CArrayPtr<TASN1DecGeneric>* certs =
			decSeq.DecodeDERLC( *itemsData->At( pos++ ) );

		TInt certCount = certs->Count();
		for( TInt i = 0; i < certCount; i++ )
			{
			CCMSX509CertificateList* tmpCert = CCMSX509CertificateList::NewLC();
			tmpCert->DecodeL( certs->At( i )->Encoding() );
			tmpRevokedCertificates->AppendL( tmpCert );
			CleanupStack::Pop( tmpCert );
			}
		CleanupStack::PopAndDestroy( certs );
		}

	// decode Signer Infos
	if( pos >= itemsData->Count() )
		{
		// Missing mandatory signer infos
		User::Leave( KErrArgument );
		}

	CArrayPtrFlat< CCMSSignerInfo >* tmpSignerInfos =
		new( ELeave )CArrayPtrFlat< CCMSSignerInfo >( KDefaultGranularity );
	CleanupStack::PushL( tmpSignerInfos );
	CleanupResetAndDestroyPushL( *tmpSignerInfos );

	CArrayPtr<TASN1DecGeneric>* sInfos =
		DecodeSequenceLC( itemsData->At( pos )->Encoding() );

	TInt sInfoCount = sInfos->Count();
	for( TInt j = 0; j < sInfoCount; j++ )
		{
		CCMSSignerInfo* tmpInfo = CCMSSignerInfo::NewL();
		CleanupStack::PushL( tmpInfo );
		tmpInfo->DecodeL( sInfos->At( j )->Encoding() );
		tmpSignerInfos->AppendL( tmpInfo );
		CleanupStack::Pop( tmpInfo );
		}
	CleanupStack::PopAndDestroy( sInfos );

	// All done, setting new values
	iSignerInfos->ResetAndDestroy();
	delete iSignerInfos;
	iSignerInfos = tmpSignerInfos;

	CleanupStack::Pop( tmpSignerInfos ); // ResetAndDestroy
	CleanupStack::Pop( tmpSignerInfos ); // Normal cleanup

	if( iRevokedCertificates )
		{
		iRevokedCertificates->ResetAndDestroy();
		delete iRevokedCertificates;
		}
	iRevokedCertificates = tmpRevokedCertificates;
	if( tmpRevokedCertificates )
		{
		CleanupStack::Pop( tmpRevokedCertificates ); // ResetAndDestroy
		CleanupStack::Pop( tmpRevokedCertificates ); // Normal cleanup
		}

	if( iCertificates )
		{
		iCertificates->ResetAndDestroy();
		delete iCertificates;
		}
	iCertificates = tmpCerts;
	if( tmpCerts )
		{
		CleanupStack::Pop( tmpCerts ); // ResetAndDestory
		CleanupStack::Pop( tmpCerts ); // Normal cleanup
		}

	delete iContentInfo;
	iContentInfo = tmpInfo;
	CleanupStack::Pop( tmpInfo );

	iDigestAlgorithmIdentifiers->ResetAndDestroy();
	delete iDigestAlgorithmIdentifiers;
	iDigestAlgorithmIdentifiers = tmpAlgs;
	CleanupStack::Pop( tmpAlgs ); // ResetAndDestroy
	CleanupStack::Pop( tmpAlgs ); // Normal cleanup

	iVersion = tmpVersion;

	CleanupStack::PopAndDestroy( itemsData );
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSSignedData::EncoderLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();

	// Add version
	CASN1EncInt* version = CASN1EncInt::NewLC( iVersion );
	root->AddAndPopChildL( version );

	// Add digest algorithms
	CASN1EncSequence* digestAlgorithms = CASN1EncSequence::NewLC();
	digestAlgorithms->SetTag( EASN1Set, EUniversal );
	TInt count = iDigestAlgorithmIdentifiers->Count();
	if( count == 0 )
		{
		// there is no mandatory algorithms
		User::Leave( KErrArgument );
		}
	for( TInt i = 0; i < count; i++ )
		{
		CCMSX509AlgorithmIdentifier* alg =
			iDigestAlgorithmIdentifiers->At( i );
		CASN1EncBase* algEnc = alg->EncoderLC();
		digestAlgorithms->AddAndPopChildL( algEnc );
		}
	root->AddAndPopChildL( digestAlgorithms );

	// add encapsulated content info
	CASN1EncBase* encContentInfo = iContentInfo->EncoderLC();
	root->AddAndPopChildL( encContentInfo );

	// add possible certificates
	if( iCertificates &&
		iCertificates->Count() > 0 )
		{
		CASN1EncSequence* certificates = CASN1EncSequence::NewLC();
		certificates->SetTag( KCertificateSetTag );
		TInt certCount = iCertificates->Count();
		for( TInt i = 0; i < certCount; i++ )
			{
			CCMSCertificateChoices* cert = iCertificates->At( i );
			CASN1EncBase* certEnc = cert->EncoderLC();
			certificates->AddAndPopChildL( certEnc );
			}
		root->AddAndPopChildL( certificates );
		}

	// add possible revoked certificates
	if( iRevokedCertificates &&
		iRevokedCertificates->Count() > 0 )
		{
		CASN1EncSequence* certificates = CASN1EncSequence::NewLC();
		certificates->SetTag( KRevokedCertificatesTag );
		TInt certCount = iRevokedCertificates->Count();
		for( TInt i = 0; i < certCount; i++ )
			{
			CCMSX509CertificateList* list = iRevokedCertificates->At( i );
			CASN1EncBase* rCerts = list->EncoderLC();
			certificates->AddAndPopChildL( rCerts );
			}
		root->AddAndPopChildL( certificates );
		}

	// add signer infos
	CASN1EncSequence* signerInfos = CASN1EncSequence::NewLC();
	signerInfos->SetTag( EASN1Set, EUniversal );
	TInt sCount = iSignerInfos->Count();
	if( sCount == 0 )
		{
		// there is no mandatory signer infos
		User::Leave( KErrArgument );
		}

	for( TInt j = 0; j < sCount; j++ )
		{
		CASN1EncBase* sInfo = iSignerInfos->At( j )->EncoderLC();
		signerInfos->AddAndPopChildL( sInfo );
		}
	root->AddAndPopChildL( signerInfos );
	return root;
	}
// -----------------------------------------------------------------------------
// CCMSSignedData::Version
// Getter for Version
// -----------------------------------------------------------------------------
EXPORT_C TInt CCMSSignedData::Version() const
	{
	return iVersion;
	}
// -----------------------------------------------------------------------------
// CCMSSignedData::DigestAlgorithmIdentifiers
// Getter for DigestAlgorithmIdentifiers
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSX509AlgorithmIdentifier >&
	CCMSSignedData::DigestAlgorithmIdentifiers() const
	{
	return *iDigestAlgorithmIdentifiers;
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::EncapsulatedContentInfo
// Getter for EncapsulatedContentInfo
// -----------------------------------------------------------------------------
EXPORT_C const CCMSEncapsulatedContentInfo&
	CCMSSignedData::EncapsulatedContentInfo() const
	{
	return *iContentInfo;
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::SignerInfos
// Getter for SignerInfos
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSSignerInfo >&
	CCMSSignedData::SignerInfos() const
	{
	return *iSignerInfos;
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::Certificates
// Getter for Certificates
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSCertificateChoices >*
	CCMSSignedData::Certificates() const
	{
	return iCertificates;
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::RevokedCertificates
// Getter for RevokedCertificates
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSX509CertificateList >*
	CCMSSignedData::RevokedCertificates() const
	{
	return iRevokedCertificates;
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::SetDigestAlgorithmIdentifiersL
// Setter for DigestAlgorithmIdentifiers, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignedData::SetDigestAlgorithmIdentifiersL(
	const CArrayPtr< CCMSX509AlgorithmIdentifier >& aDigestAlgorithmIdentifiers )
	{
	CArrayPtrFlat< CCMSX509AlgorithmIdentifier >* tmpAlgs =
		new( ELeave )CArrayPtrFlat< CCMSX509AlgorithmIdentifier >( KDefaultGranularity );
	CleanupStack::PushL( tmpAlgs );
	TInt algCount = aDigestAlgorithmIdentifiers.Count();
	for( TInt i = 0; i < algCount; i++ )
		{
		CCMSX509AlgorithmIdentifier* origAlg = aDigestAlgorithmIdentifiers[ i ];
		CCMSX509AlgorithmIdentifier* tmpAlg = NULL;
		if( origAlg->DigestAlgorithm() )
			{
			tmpAlg =
			CCMSX509AlgorithmIdentifier::NewL( origAlg->AlgorithmIdentifier(),
											   *origAlg->DigestAlgorithm() );
			}
		else
			{
			tmpAlg =
			CCMSX509AlgorithmIdentifier::NewL( origAlg->AlgorithmIdentifier() );
			}
		CleanupStack::PushL( tmpAlg );
		tmpAlgs->AppendL( tmpAlg );
		CleanupStack::Pop( tmpAlg );
		}
	delete iDigestAlgorithmIdentifiers;
	iDigestAlgorithmIdentifiers = tmpAlgs;
	CleanupStack::Pop( tmpAlgs );
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::SetEncapsulatedContentInfoL
// Setter for EncapsulatedContentInfo, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignedData::SetEncapsulatedContentInfoL(
	const CCMSEncapsulatedContentInfo& aContentInfo )
	{
	CCMSEncapsulatedContentInfo* copy =
		CCMSEncapsulatedContentInfo::NewLC( aContentInfo.ContentType(),
											aContentInfo.Content() );
	delete iContentInfo;
	iContentInfo = copy;
	CleanupStack::Pop( copy );
	ValidateVersion();
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::SetSignerInfosL
// Setter for SignerInfos, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignedData::SetSignerInfosL(
	const CArrayPtr< CCMSSignerInfo >& aSignerInfos )
	{
	TInt sigCount = aSignerInfos.Count();
	// creating right size array directly, adding 1 to avoid panic in case that
	// sigCount is zero.
	CArrayPtrFlat< CCMSSignerInfo >* tmpSignInfos =
		new( ELeave )CArrayPtrFlat< CCMSSignerInfo >( sigCount + 1 );
	CleanupStack::PushL( tmpSignInfos );
	CleanupResetAndDestroyPushL( *tmpSignInfos );
	for( TInt i = 0; i < sigCount; i++ )
		{
		HBufC8* tmpEncoding;
		aSignerInfos[ i ]->EncodeL( tmpEncoding );
		CleanupStack::PushL( tmpEncoding );
		CCMSSignerInfo* tmpSig = CCMSSignerInfo::NewL();
		CleanupStack::PushL( tmpSig );
		tmpSig->DecodeL( *tmpEncoding );
		tmpSignInfos->AppendL( tmpSig );
		CleanupStack::Pop( tmpSig );
		CleanupStack::PopAndDestroy( tmpEncoding );
		}
	if( iSignerInfos )
		{
		iSignerInfos->ResetAndDestroy();
		delete iSignerInfos;
		}
	iSignerInfos = tmpSignInfos;
	CleanupStack::Pop( tmpSignInfos ); // ResetAndDestroy
	CleanupStack::Pop( tmpSignInfos ); // normal cleanup
	ValidateVersion();
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::SetCertificatesL
// Setter for Certificates, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignedData::SetCertificatesL(
	const CArrayPtr< CCMSCertificateChoices >* aCertificates )
	{
	CArrayPtrFlat< CCMSCertificateChoices >* tmpCertificates = NULL;
	if( aCertificates )
		{
		TInt certCount = aCertificates->Count();
		// creating right size array directly, adding 1 to avoid panic in case 
		// that certCount is zero.
		tmpCertificates =
			new( ELeave )CArrayPtrFlat< CCMSCertificateChoices >( certCount + 1 );
		CleanupStack::PushL( tmpCertificates );
		CleanupResetAndDestroyPushL( *tmpCertificates );
		for( TInt i = 0; i < certCount; i++ )
			{
			CCMSCertificateChoices* copy =
				CCMSCertificateChoices::NewLC();
			CCMSCertificateChoices* orig = aCertificates->At( i );
			if( orig->AttrCert() )
				{
				copy->SetAttrCertL( *orig->AttrCert() );
				}
			else if( orig->Certificate() )
				{
				copy->SetCertificateL( *orig->Certificate() );
				}
			else
				{
				User::Leave( KErrArgument );
				}
			tmpCertificates->AppendL( copy );
			CleanupStack::Pop( copy );
			}
		CleanupStack::Pop( tmpCertificates ); // ResetAndDestroy
		CleanupStack::Pop( tmpCertificates ); // Normal cleanup
		}
	if( iCertificates )
		{
		iCertificates->ResetAndDestroy();
		delete iCertificates;
		}
	iCertificates = tmpCertificates;
	ValidateVersion();
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::SetRevokedCertificatesL
// Setter for RevokedCertificates, takes copy
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignedData::SetRevokedCertificatesL(
	const CArrayPtr< CCMSX509CertificateList >* aRevokedCertificates )
	{
	CArrayPtrFlat< CCMSX509CertificateList >* tmpCerts = NULL;
	if( aRevokedCertificates )
		{
		TInt certCount = aRevokedCertificates->Count();
		// creating right size array directly, adding 1 to avoid panic in case 
		// that certCount is zero.
		tmpCerts =
			new( ELeave )CArrayPtrFlat< CCMSX509CertificateList >
														( certCount + 1 );
		CleanupStack::PushL( tmpCerts );
		CleanupResetAndDestroyPushL( *tmpCerts );
		for( TInt i = 0; i < certCount; i++ )
			{
			CCMSX509CertificateList* copy =
				CCMSX509CertificateList::NewLC();

			HBufC8* tmpData;
			aRevokedCertificates->At( i )->EncodeL( tmpData );
			CleanupStack::PushL( tmpData );
			copy->DecodeL( *tmpData );
			CleanupStack::PopAndDestroy( tmpData );
			tmpCerts->AppendL( copy );
			CleanupStack::Pop( copy );
			}
		CleanupStack::Pop( tmpCerts ); // ResetAndDestroy
		CleanupStack::Pop( tmpCerts ); // Normal cleanup
		}

	if( iRevokedCertificates )
		{
		iRevokedCertificates->ResetAndDestroy();
		delete iRevokedCertificates;
		}
	iRevokedCertificates = tmpCerts;
	}

// -----------------------------------------------------------------------------
// CCMSSignedData::ValidateVersion
// Validates that iVersion is correct
// -----------------------------------------------------------------------------
void CCMSSignedData::ValidateVersion()
	{
	iVersion = KDefaultVersion;
	// if the encapsulated content type is other than id-data
	// then the value of version shall be 3.
	if( iContentInfo &&
		( iContentInfo->ContentType() != KIDDataOID ) )
		{
		iVersion = KAlternativeVersion;
		return; // no reason to continue
		}

	// if any of the elements of SignerInfos are version 3,
	// then the value of version shall be 3.
	if( iSignerInfos->Count() )
		{
		TInt sigCount = iSignerInfos->Count();
		for( TInt i = 0; i < sigCount; i++ )
			{
			if( iSignerInfos->At( i )->CMSVersion() == KAlternativeVersion )
				{
				iVersion = KAlternativeVersion;
				return; // no reason to continue
				}
			}
		}

	// if attribute certificates are present, the
	// then the value of version shall be 3.
	if( iCertificates )
		{
		TInt certCount = iCertificates->Count();
		for( TInt i = 0; i < certCount; i++ )
			{
			if( iCertificates->At( i )->AttrCert() )
				{
				iVersion = KAlternativeVersion;
				return; // no reason to continue
				}
			}
		}
	}
//  End of File
