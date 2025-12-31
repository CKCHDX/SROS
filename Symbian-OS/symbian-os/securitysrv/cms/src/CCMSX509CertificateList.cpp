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
#include    "CCMSX509CertificateList.h"
#include    "TCMSTimeUtil.h"
#include    "CCMSX509AlgorithmIdentifier.h"
#include <x509cert.h>
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KDefaultGranularity = 1;
const TInt KCertificateListParams = 3;
const TInt KMinTBSCertListParams = 3;
const TInt KMaxTBSCertListParams = 7;
const TInt KDefaultVersion = 1;
const TInt KMinRevokedCertParams = 2;
const TInt KMaxRevokedCertParams = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509RevokedCertificate::CCMSX509RevokedCertificate
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

EXPORT_C CCMSX509RevokedCertificate::CCMSX509RevokedCertificate()
	{
	}

// Destructor
CCMSX509RevokedCertificate::~CCMSX509RevokedCertificate()
	{
	if( iExtensions )
		{
		iExtensions->ResetAndDestroy();
		delete iExtensions;
		}
	}

// -----------------------------------------------------------------------------
// CCMSX509RevokedCertificate::CCMSX509RevokedCertificate
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
EXPORT_C CCMSX509CertificateListParameters::CCMSX509CertificateListParameters()
	{
	}

// Destructor
CCMSX509CertificateListParameters::~CCMSX509CertificateListParameters()
	{
	delete iIssuer;
	delete iSignatureAlgorithm;
	delete iSignature;
	if( iRevokedCertificates )
		{
		iRevokedCertificates->ResetAndDestroy();
		delete iRevokedCertificates;
		}

	if( iExtensions )
		{
		iExtensions->ResetAndDestroy();
		delete iExtensions;
		}
	}

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::CCMSX509CertificateList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509CertificateList::CCMSX509CertificateList()
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509CertificateList::ConstructL()
    {
	iParams = new(ELeave) CCMSX509CertificateListParameters();
	// version is always v2(1)
	iParams->iVersion = KDefaultVersion;
    }

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509CertificateList::ConstructL(
	const CX509Certificate& aSigningCertificate,
	const CArrayPtrFlat<CX509Certificate>& aRevokedCertificates )
    {
	ConstructL();

	// setting signature
	const CSigningAlgorithmIdentifier& signAlgorithm =
		aSigningCertificate.SigningAlgorithm();
	iParams->iSignatureAlgorithm =
		CCMSX509AlgorithmIdentifier::NewL( signAlgorithm.AsymmetricAlgorithm(),
										   signAlgorithm.DigestAlgorithm() );

	// setting issuer
	iParams->iIssuer =
		CX500DistinguishedName::NewL( aSigningCertificate.IssuerName() );

	// setting validity
	const CValidityPeriod& validity = aSigningCertificate.ValidityPeriod();
	iParams->iThisUpdate = validity.Start();
	iParams->iNextUpdate = validity.Finish();


	// copying revoked certificates
	TInt revCerts = aRevokedCertificates.Count();
	if( revCerts > 0 )
		{
		iParams->iRevokedCertificates =
			new( ELeave )CArrayPtrFlat< CCMSX509RevokedCertificate >
												( KDefaultGranularity );
		for( TInt i = 0; i < revCerts; i++ )
			{
			CX509Certificate* cert = aRevokedCertificates[ i ];
			CCMSX509RevokedCertificate* tmpRevCer =
				new(ELeave) CCMSX509RevokedCertificate();
			CleanupStack::PushL( tmpRevCer );
			// convert serial from des to int
            TPtrC8 serialDes = cert->SerialNumber();
            TInt length = serialDes.Size();
            const TUint8* ptr = serialDes.Ptr();
            TInt serial = ( ptr[ 0 ] & 0x80 ) ? -1 : 0;
            for( TInt j = 0; j < length; j++ )
                {
                serial <<= 8;
                serial += *ptr++;
                }
			tmpRevCer->iUserCertificateSerialNumber = serial;

			const CValidityPeriod& reValidity = cert->ValidityPeriod();
			tmpRevCer->iRevokationDate = reValidity.Start();

			// copying extensions
			const CArrayPtrFlat<CX509CertExtension>& extensions =
				cert->Extensions();
			TInt extensionCount = extensions.Count();
			if( extensionCount > 0 )
				{
				tmpRevCer->iExtensions =
					new(ELeave) CArrayPtrFlat<CX509CertExtension>
														( KDefaultGranularity );
				for( TInt j = 0; j < extensionCount; j++ )
					{
					CX509CertExtension* ext = extensions[ j ];
					CX509CertExtension* tmpExt =
									CX509CertExtension::NewLC( *ext );
					tmpRevCer->iExtensions->AppendL( tmpExt );
					CleanupStack::Pop( tmpExt );
					}
				}
			iParams->iRevokedCertificates->AppendL( tmpRevCer );
			CleanupStack::Pop( tmpRevCer );
			}
		}
	// copying possible extensions
	TInt extensionCount = aSigningCertificate.Extensions().Count();
	if( extensionCount > 0 )
		{
		iParams->iExtensions =
			new( ELeave )CArrayPtrFlat< CX509CertExtension >
														( KDefaultGranularity );
		const CArrayPtrFlat< CX509CertExtension >& extensions =
			aSigningCertificate.Extensions();

		for( TInt i = 0; i < extensionCount; i++ )
			{
			CX509CertExtension* copy = CX509CertExtension::NewL( *extensions[ i ] );
			CleanupStack::PushL( copy );
			iParams->iExtensions->AppendL( copy );
			CleanupStack::Pop( copy );
			}
		}

	// copying signature
	iParams->iSignature = aSigningCertificate.Signature().AllocL();
    }

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509CertificateList* CCMSX509CertificateList::NewLC()
	{
    CCMSX509CertificateList* self = new( ELeave ) CCMSX509CertificateList();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509CertificateList* CCMSX509CertificateList::NewLC(
	const CX509Certificate& aSigningCertificate,
	const CArrayPtrFlat<CX509Certificate>& aRevokedCertificates  )
    {
    CCMSX509CertificateList* self = new( ELeave ) CCMSX509CertificateList();
    CleanupStack::PushL( self );
    self->ConstructL( aSigningCertificate,
    				  aRevokedCertificates );
    return self;
    }

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509CertificateList* CCMSX509CertificateList::NewLC(
	CCMSX509CertificateListParameters& aParameters )
    {
    CCMSX509CertificateList* self = new( ELeave ) CCMSX509CertificateList();
    CleanupStack::PushL( self );
    self->SetParametersL( aParameters );
    return self;
    }

// Destructor
CCMSX509CertificateList::~CCMSX509CertificateList()
    {
	delete iParams;
    }

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509CertificateList::DecodeL( const TDesC8& aRawData )
	{
	CCMSX509CertificateListParameters* params =
		new(ELeave) CCMSX509CertificateListParameters();
	CleanupStack::PushL( params );

	CArrayPtr<TASN1DecGeneric>* certificateList = DecodeSequenceLC( aRawData,
															  KCertificateListParams, // 3
															  KCertificateListParams );


	// decode tbsCertList
	CArrayPtr<TASN1DecGeneric>* tbsCertList =
		DecodeSequenceLC( certificateList->At( 0 )->Encoding(),
						  KMinTBSCertListParams,	// 3
						  KMaxTBSCertListParams );	// 7

	TInt pos = 0;
	// decode Version (optional)
	TASN1DecGeneric ver( *tbsCertList->At( pos ) );
	if( ver.Tag() == EASN1Integer )
		{
		TASN1DecInteger version;
		params->iVersion = version.DecodeDERShortL( ver );
		pos++;
		}
	else
		{
		// always ver v2( 1 )
		params->iVersion = KDefaultVersion;
		}

	// decode Signature algorithm
	CCMSX509AlgorithmIdentifier* signAlg = CCMSX509AlgorithmIdentifier::NewL();
	CleanupStack::PushL( signAlg );
	signAlg->DecodeL( tbsCertList->At( pos++ )->Encoding() );
	params->iSignatureAlgorithm = signAlg;
	CleanupStack::Pop( signAlg );

	// decode Issuer
	params->iIssuer =
		CX500DistinguishedName::NewL( tbsCertList->At( pos++ )->Encoding() );

	// check that we have enough parameters for mandatory thisUpdate
	if( tbsCertList->Count() <= pos )
		{
		User::Leave( KErrArgument );
		}

	// decode thisUpdate
	params->iThisUpdate = TCMSTimeUtil::ConvertToTimeL(
									  tbsCertList->At( pos++ )->Encoding() );

	// decode nextUpdate (optional)
	if( ( tbsCertList->Count() > pos ) &&
		( ( tbsCertList->At( pos )->Tag() == EASN1UTCTime ) ||
		  ( tbsCertList->At( pos )->Tag() == EASN1GeneralizedTime ) ) )
		{
		params->iNextUpdate = TCMSTimeUtil::ConvertToTimeL(
									  tbsCertList->At( pos++ )->Encoding() );
		}

	// decode revokedCerts (optional)
	if( ( tbsCertList->Count() > pos ) &&
		( tbsCertList->At( pos )->Tag() == EASN1Sequence ) )
		{
		// if it is sequence then it must be set of revoked certificates
		CArrayPtr<TASN1DecGeneric>* revokedCerts =
				 DecodeSequenceLC( tbsCertList->At( pos++ )->Encoding() );
		TInt rCertCount = revokedCerts->Count();
		params->iRevokedCertificates =
			new(ELeave)CArrayPtrFlat< CCMSX509RevokedCertificate >
													( KDefaultGranularity );
		for( TInt i = 0; i < rCertCount; i++ )
			{
			// decode certificate
			CArrayPtr<TASN1DecGeneric>* rCert =
					 DecodeSequenceLC( revokedCerts->At( i )->Encoding(),
					 KMinRevokedCertParams, // 2
					 KMaxRevokedCertParams ); // 3
			CCMSX509RevokedCertificate* tmpCert =
				new( ELeave )CCMSX509RevokedCertificate;
			CleanupStack::PushL( tmpCert );

			// decode serial number
			TASN1DecInteger serial;
			tmpCert->iUserCertificateSerialNumber =
									serial.DecodeDERShortL( *rCert->At( 0 ) );

			// decode revocationDate
			tmpCert->iRevokationDate = TCMSTimeUtil::ConvertToTimeL(
										rCert->At( 1 )->Encoding() );

			// decode possible extensions
			if( rCert->Count() == KMaxRevokedCertParams )
				{
				tmpCert->iExtensions = DecodeExtensionsL( rCert->At( 2 ) );
				}
			params->iRevokedCertificates->AppendL( tmpCert );

			CleanupStack::Pop( tmpCert );
			CleanupStack::PopAndDestroy( rCert );
			}
		CleanupStack::PopAndDestroy( revokedCerts );
		}
	// decode extensions (optional), tagged with 0
	if( ( tbsCertList->Count() > pos ) &&
		( tbsCertList->At( pos )->Tag() == 0 ) )
		{
		TASN1DecGeneric extens( tbsCertList->At( pos )->GetContentDER() );
        extens.InitL();
		params->iExtensions = DecodeExtensionsL( &extens );
		}

	CleanupStack::PopAndDestroy( tbsCertList );

	// decode signature algorithm,
	// just check that it is same as
	// already set one
	signAlg = CCMSX509AlgorithmIdentifier::NewL();
	CleanupStack::PushL( signAlg );
	signAlg->DecodeL( certificateList->At( 1 )->Encoding() );
	if( !( signAlg->AlgorithmIdentifier() ==
		   params->iSignatureAlgorithm->AlgorithmIdentifier() ) )
		{
		User::Leave( KErrArgument );
		}
	CleanupStack::PopAndDestroy( signAlg );

	// decode signature
	TASN1DecBitString signature;
	params->iSignature = signature.ExtractOctetStringL( *certificateList->At( 2 ) );
	CleanupStack::PopAndDestroy( certificateList );

	delete iParams;
	iParams = params;
	CleanupStack::Pop( params );
	}
// -----------------------------------------------------------------------------
// CCMSX509CertificateList::DecodeExtensionsL
// Decodes Extensions
// -----------------------------------------------------------------------------
CArrayPtrFlat<CX509CertExtension>* CCMSX509CertificateList::DecodeExtensionsL(
	const TASN1DecGeneric* aExtensions )
	{
	CArrayPtr<TASN1DecGeneric>* extens =
			 DecodeSequenceLC( aExtensions->Encoding() );
	TInt extensionCount = extens->Count();
	CArrayPtrFlat< CX509CertExtension >* tmpExtensions =
		new(ELeave)CArrayPtrFlat< CX509CertExtension >( KDefaultGranularity );
	CleanupStack::PushL( tmpExtensions );
	CleanupResetAndDestroyPushL( *tmpExtensions );
	for( TInt j = 0; j < extensionCount; j++ )
		{
		CX509CertExtension* extension =
			CX509CertExtension::NewLC( extens->At( j )->Encoding() );
		tmpExtensions->AppendL( extension );
		CleanupStack::Pop( extension );
		}
	CleanupStack::Pop( tmpExtensions ); // ResetAndDestroy
	CleanupStack::Pop( tmpExtensions ); // normal cleanup
	CleanupStack::PopAndDestroy( extens );
	return tmpExtensions;
	}

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509CertificateList::EncoderLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();

	// encoding tbsCertList
	CASN1EncSequence* tbsCertList = CASN1EncSequence::NewLC();

	// encoding version
	CASN1EncInt* version = CASN1EncInt::NewLC( KDefaultVersion );
	tbsCertList->AddAndPopChildL( version );

	// encoding signature
	HBufC8* signDes;
	iParams->iSignatureAlgorithm->EncodeL( signDes );
	CleanupStack::PushL( signDes );
	CASN1EncEncoding* signatureEnc =
		CASN1EncEncoding::NewLC( *signDes );
	tbsCertList->AddAndPopChildL( signatureEnc );
	CleanupStack::PopAndDestroy( signDes );

	// encoding issuer
	CASN1EncSequence* issuer = iParams->iIssuer->EncodeASN1LC();
	tbsCertList->AddAndPopChildL( issuer );

	// encoding thisUpdate
	CASN1EncBase* thisUpdate =
		TCMSTimeUtil::ConvertToEncoderLC( iParams->iThisUpdate );
	tbsCertList->AddAndPopChildL( thisUpdate );

	// encoding nextUpdate
	CASN1EncBase* nextUpdate =
		TCMSTimeUtil::ConvertToEncoderLC( iParams->iNextUpdate );
	tbsCertList->AddAndPopChildL( nextUpdate );

	// encoding possible revokedCerts
	if( iParams->iRevokedCertificates &&
		iParams->iRevokedCertificates->Count() )
		{
		CASN1EncSequence* revokedCerts = CASN1EncSequence::NewLC();
		TInt numOfRevokedCerts = iParams->iRevokedCertificates->Count();

		for( TInt i = 0; i < numOfRevokedCerts; i++ )
			{
			// encoding certificate
			CASN1EncSequence* revokedCertificate = CASN1EncSequence::NewLC();

			CCMSX509RevokedCertificate* cert = iParams->iRevokedCertificates->At( i );
			// encoding serial number
			CASN1EncInt* serialEnc =
				CASN1EncInt::NewLC( cert->iUserCertificateSerialNumber );
			revokedCertificate->AddAndPopChildL( serialEnc );

			// encoding revocationDate
			CASN1EncBase* revocationDate =
				TCMSTimeUtil::ConvertToEncoderLC( cert->iRevokationDate );
			revokedCertificate->AddAndPopChildL( revocationDate );

			// encoding possible extensions
			CASN1EncSequence* extensions = EncodeExtensionsLC( cert->iExtensions );
			if( extensions )
				{
				revokedCertificate->AddAndPopChildL( extensions );
				}
			revokedCerts->AddAndPopChildL( revokedCertificate );
			}
		tbsCertList->AddAndPopChildL( revokedCerts );
		}

	// encoding possible extensions
	CASN1EncSequence* ext = EncodeExtensionsLC( iParams->iExtensions );
	if( ext )
		{
		CleanupStack::Pop( ext );
		// this will take ownership of the extensions
		// tagging extensions with value 0
		CASN1EncExplicitTag* explExt =
			CASN1EncExplicitTag::NewLC( ext, 0 );
		tbsCertList->AddAndPopChildL( explExt );
		}

	root->AddAndPopChildL( tbsCertList );

	// encoding signatureValue
	iParams->iSignatureAlgorithm->EncodeL( signDes );
	CleanupStack::PushL( signDes );
	signatureEnc =
		CASN1EncEncoding::NewLC( *signDes );
	root->AddAndPopChildL( signatureEnc );
	CleanupStack::PopAndDestroy( signDes );

	// encoding signature
	CASN1EncBitString* signatureBitString =
		CASN1EncBitString::NewLC( *iParams->iSignature );
	root->AddAndPopChildL( signatureBitString );

	return root;
	}

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::EncodeExtensionsLC
// Encodes Extensions
// -----------------------------------------------------------------------------
CASN1EncSequence* CCMSX509CertificateList::EncodeExtensionsLC(
	const CArrayPtrFlat<CX509CertExtension>* aExtensions ) const
	{
	CASN1EncSequence* extensionsEnc = NULL;
	if(	  aExtensions &&
		( aExtensions->Count() > 0 ) )
		{
		TInt extensionCount = aExtensions->Count();
		extensionsEnc = CASN1EncSequence::NewLC();
		for( TInt i = 0; i < extensionCount; i++ )
			{
			CASN1EncSequence* extEnc = CASN1EncSequence::NewLC();
			CX509CertExtension* extension = aExtensions->At( i );
			// encoding id
			CASN1EncObjectIdentifier* id =
				CASN1EncObjectIdentifier::NewLC( extension->Id() );
			extEnc->AddAndPopChildL( id );

			// encoding possible critical flag
			if( extension->Critical() )
				{
				CASN1EncBoolean* critical =
					CASN1EncBoolean::NewLC( ETrue );
				extEnc->AddAndPopChildL( critical );
				}
			// encoding data, have to decode it before encoding it again
			// because CASN1EncEncoding changes TagType to Constructed
			TASN1DecOctetString dataDec;
			TInt pos = 0;
			HBufC8* data = dataDec.DecodeDERL( extension->Data(), pos );
			CleanupStack::PushL( data );
			CASN1EncOctetString* dataEnc =
				CASN1EncOctetString::NewLC( *data );
			extEnc->AddAndPopChildL( dataEnc );
			CleanupStack::PopAndDestroy( data );

			extensionsEnc->AddAndPopChildL( extEnc );
			}
		}
	return extensionsEnc;
	}

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::Parameters
// Getter for Signing Certificate
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509CertificateListParameters&
	CCMSX509CertificateList::Parameters() const
	{
	return *iParams;
	}

// -----------------------------------------------------------------------------
// CCMSX509CertificateList::SetParametersL
// Setter for signing certificate, takes ownership
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509CertificateList::SetParametersL(
	CCMSX509CertificateListParameters& aParameters  )
	{
	// some sanity checks
	if( !aParameters.iIssuer ||
		!aParameters.iSignature ||
		!aParameters.iSignatureAlgorithm ||
	   ( aParameters.iVersion != KDefaultVersion ) )
		{
		User::Leave( KErrArgument );
		}
	delete iParams;
	iParams = &aParameters;
	}

//  End of File
