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
#include    "CCMSSignerInfo.h"
#include    "TCMSTimeUtil.h"

#include <x500dn.h>
#include <asn1dec.h>
#include <asn1enc.h>
#include <signed.h>

// CONSTANTS
const TInt KMinNumberOfSubModules = 5;
const TInt KMaxNumberOfSubModules = 7;
const TInt KDefaultGranularity = 2;
const TInt KCMSVersion1 = 1;
const TInt KCMSVersion3 = 3;
const TInt KSignedAttrsTag = 0;
const TInt KUnsignedAttrsTag = 1;

// CMS SignedAttributes useful types
_LIT( KContentTypeOID, "1.2.840.113549.1.9.3" );
_LIT( KMessageDigestOID, "1.2.840.113549.1.9.4" );
_LIT( KSignTimeOID, "1.2.840.113549.1.9.5" );

// Additional oid for adding certificates
_LIT( KPKCS9SigCertOID, "1.2.840.113549.1.9.16.2.12" );

// X509 URL certificate OID
_LIT( KURLCertificateOID, "2.23.43.2.1" );

// Defaulta id-data oid
_LIT( KIDDataOID, "1.2.840.113549.1.7.1" );

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CCMSSignerInfo::CSignerInfoData::~CSignerInfoData()
    {
	delete iIssuerAndSerial;
	delete iSubjectKeyIdentifier;

 	if( iSignedAttributes )
 		{
 		iSignedAttributes->ResetAndDestroy();
		delete iSignedAttributes;
		}

	if( iUnsignedAttributes )
		{
		iUnsignedAttributes->ResetAndDestroy();
		delete iUnsignedAttributes;
		}

	delete iDigestAI;
	delete iSignatureAI;
	delete iSignatureValue;
    }

// -----------------------------------------------------------------------------
// CCMSSignerInfo::CCMSSignerInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSSignerInfo::CCMSSignerInfo()
    {
    }

// -----------------------------------------------------------------------------
// CCMSSignerInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSSignerInfo::ConstructL(
	const TDesC8& aCertificateUrl,
	const TDesC8& aSubjectKeyID,
	const TDesC8& aMessageDigest )
    {
	iData = new(ELeave) CSignerInfoData();
	BaseConstructL( aMessageDigest );
	SetCertificateUrlL( aCertificateUrl, aSubjectKeyID );
    }

// -----------------------------------------------------------------------------
// CCMSSignerInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSSignerInfo::ConstructL(
	const CCMSX509Certificate& aCertificate,
	const TDesC8& aMessageDigest )
    {
	iData = new(ELeave) CSignerInfoData();
	BaseConstructL( aMessageDigest );
	SetCertificateL( aCertificate );
    }

void CCMSSignerInfo::BaseConstructL( const TDesC8& aMessageDigest )
	{
	iData->iDigestAI = CCMSX509AlgorithmIdentifier::NewL( ESHA1 );
	iData->iSignatureAI = CCMSX509AlgorithmIdentifier::NewL();
	TTime time;
	time.UniversalTime();
	iData->iSignedAttributes = new(ELeave)CArrayPtrFlat<CCMSAttribute>( KDefaultGranularity );
	SetSignedAttributesL( KIDDataOID(),
						  aMessageDigest,
						  time,
						  NULL );
	iData->iUnsignedAttributes = new(ELeave)CArrayPtrFlat<CCMSAttribute>( KDefaultGranularity );
    SetSignatureValueL( aMessageDigest );
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSSignerInfo* CCMSSignerInfo::NewL()
	{
	// creating with empty values
	CCMSSignerInfo* self = NewL( KNullDesC8(),
								 KNullDesC8(),
								 KNullDesC8() );

	return self;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSSignerInfo* CCMSSignerInfo::NewL(
	const TDesC8& aCertificateUrl,
	const TDesC8& aSubjectKeyID,
	const TDesC8& aMessageDigest )
    {
    CCMSSignerInfo* self = new( ELeave ) CCMSSignerInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aCertificateUrl, aSubjectKeyID, aMessageDigest );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSSignerInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSSignerInfo* CCMSSignerInfo::NewL(
	const CCMSX509Certificate& aCertificate,
	const TDesC8& aMessageDigest )
    {
    CCMSSignerInfo* self = new( ELeave ) CCMSSignerInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aCertificate, aMessageDigest );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CCMSSignerInfo::~CCMSSignerInfo()
    {
	delete iData;
    }

// -----------------------------------------------------------------------------
// CCMSSignerInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSSignerInfo::DecodeL( const TDesC8& aRawData )
	{
	CSignerInfoData* tmpData = new(ELeave) CSignerInfoData();
	CleanupStack::PushL( tmpData );
	CArrayPtr<TASN1DecGeneric>* signerInfo = DecodeSequenceLC( aRawData,
															  KMinNumberOfSubModules,
															  KMaxNumberOfSubModules );
	// we would not get this far if there is not min 5 elements
	TInt pos = 0;
	// decode Version
	TASN1DecInteger version;
	tmpData->iVersion = version.DecodeDERShortL( *signerInfo->At( pos++ ) );

	if( tmpData->iVersion == KCMSVersion1 )
		{
		// decode IssuerAndSerialNumber
		tmpData->iIssuerAndSerial =
			CCMSIssuerAndSerialNumber::NewL();
		tmpData->iIssuerAndSerial->DecodeL( signerInfo->At( pos++ )->Encoding() );
		}
	else
		{
		// decode SubjectKeyIdentifier
		TASN1DecOctetString subjectKey;
		tmpData->iSubjectKeyIdentifier =
			subjectKey.DecodeDERL( *signerInfo->At( pos++ ) );
		}

	// decode DigestAlgorithIdentifier
	tmpData->iDigestAI = CCMSX509AlgorithmIdentifier::NewL();
	tmpData->iDigestAI->DecodeL( signerInfo->At( pos++ )->Encoding() );

	// decode possible SignedAttributes
	tmpData->iSignedAttributes =
		new( ELeave )CArrayPtrFlat<CCMSAttribute>( KDefaultGranularity );
	if( signerInfo->At( pos )->Tag() == KSignedAttrsTag )
		{
		DecodeAttributesL( signerInfo->At( pos++ )->Encoding(),
						   tmpData->iSignedAttributes );
		}

	// decode SignatureAlgorithIdentifier
	tmpData->iSignatureAI = CCMSX509AlgorithmIdentifier::NewL();
	tmpData->iSignatureAI->DecodeL( signerInfo->At( pos++ )->Encoding() );

	// check that we have enough parameters for mandatory fields
	if( pos >= signerInfo->Count() )
		{
		User::Leave( KErrArgument );
		}

	// decode SignatureValue
	TASN1DecOctetString signValue;
	tmpData->iSignatureValue = signValue.DecodeDERL( *signerInfo->At( pos++ ) );

	// decode possible UnsignedAttributes
	tmpData->iUnsignedAttributes =
		new( ELeave )CArrayPtrFlat<CCMSAttribute>( KDefaultGranularity );
	if( ( pos < signerInfo->Count() ) &&
		( signerInfo->At( pos )->Tag() == KUnsignedAttrsTag ) )
		{
		DecodeAttributesL( signerInfo->At( pos++ )->Encoding(),
						   tmpData->iUnsignedAttributes );
		}

	CleanupStack::PopAndDestroy( signerInfo );

	delete iData;
	iData = tmpData;
	CleanupStack::Pop( tmpData );
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSSignerInfo::EncoderLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();

	// Add version
	CASN1EncInt* version = CASN1EncInt::NewLC( iData->iVersion );
	root->AddAndPopChildL( version );

	// Add IssuerAndSerialNumber or SubjectKeyIdentifier
	if( iData->iIssuerAndSerial )
		{
		CASN1EncBase* issuer = iData->iIssuerAndSerial->EncoderLC();
		root->AddAndPopChildL( issuer );
		}
	else
		{
		CASN1EncOctetString* subjectKey =
			CASN1EncOctetString::NewLC( *iData->iSubjectKeyIdentifier );
		root->AddAndPopChildL( subjectKey );
		}

	// Add DigestAlgorithmIdentifier
	CASN1EncBase* digestAlg = iData->iDigestAI->EncoderLC();
	root->AddAndPopChildL( digestAlg );

	// Add SignedAttributes if they exists
	TInt signedAttributes = iData->iSignedAttributes->Count();
	if( signedAttributes > 0 )
		{
		CASN1EncSequence* signedAttributesSet
				= CASN1EncSequence::NewLC();
		signedAttributesSet->SetTag( KSignedAttrsTag );

		for( TInt i = 0; i < signedAttributes; i++ )
			{
			CASN1EncBase* attribute = iData->iSignedAttributes->At( i )->EncoderLC();
			signedAttributesSet->AddAndPopChildL( attribute );
			}
		root->AddAndPopChildL( signedAttributesSet );
		}

	// Add SignatureAlgorithmIdentifier
	CASN1EncBase* signatureAlg =  iData->iSignatureAI->EncoderLC();
	root->AddAndPopChildL( signatureAlg );

	// Add SignatureValue
	CASN1EncOctetString* signValue =
			CASN1EncOctetString::NewLC( *iData->iSignatureValue );
	root->AddAndPopChildL( signValue );

	// Add UnsignedAttributes if they exists
	TInt unsignedAttributes = iData->iUnsignedAttributes->Count();
	if( unsignedAttributes > 0 )
		{
		CASN1EncSequence* unsignedAttributesSet
			= CASN1EncSequence::NewLC();
		unsignedAttributesSet->SetTag( KUnsignedAttrsTag );

		for( TInt i = 0; i < unsignedAttributes; i++ )
			{
			CASN1EncBase* attribute = iData->iUnsignedAttributes->At( i )->EncoderLC();
			unsignedAttributesSet->AddAndPopChildL( attribute );
			}
		root->AddAndPopChildL( unsignedAttributesSet );
		}

	return root;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::CMSVersion()
// Getter for Version
// -----------------------------------------------------------------------------
EXPORT_C TInt CCMSSignerInfo::CMSVersion() const
	{
	return iData->iVersion;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::IssuerAndSerialNumber()
// Getter for IssuerAndSerialNumber
// -----------------------------------------------------------------------------
EXPORT_C const CCMSIssuerAndSerialNumber* CCMSSignerInfo::IssuerAndSerialNumber() const
	{
	return iData->iIssuerAndSerial;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SubjectKeyIdentifier()
// Getter for SubjectKeyIdentifier
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8* CCMSSignerInfo::SubjectKeyIdentifier() const
	{
	return iData->iSubjectKeyIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::DigestAlgorithmIdentifier()
// Getter for DigestAlgorithmIdentifier
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AlgorithmIdentifier& CCMSSignerInfo::DigestAlgorithmIdentifier() const
	{
	return *iData->iDigestAI;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SignedAttributes()
// Getter for SignedAttributes
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtrFlat<CCMSAttribute>& CCMSSignerInfo::SignedAttributes() const
	{
	return *iData->iSignedAttributes;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SignedAttributesEncodedL()
// Getter for encoded SignedAttributes
// -----------------------------------------------------------------------------
EXPORT_C HBufC8* CCMSSignerInfo::SignedAttributesEncodedL() const
	{
    HBufC8* retVal = NULL;
	TInt signedAttributes = iData->iSignedAttributes->Count();
	if( signedAttributes > 0 )
		{
		CASN1EncSequence* signedAttributesSet
				= CASN1EncSequence::NewLC();
		signedAttributesSet->SetTag( EASN1Set, EUniversal );

		for( TInt i = 0; i < signedAttributes; i++ )
			{
			CASN1EncBase* attribute = iData->iSignedAttributes->At( i )->EncoderLC();
			signedAttributesSet->AddAndPopChildL( attribute );
			}
        retVal = CreateDerEncodingL( signedAttributesSet );
        CleanupStack::PopAndDestroy( signedAttributesSet );
		}
    return retVal;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SignatureAlgorithmIdentifier()
// Getter for SignatureAlgorithmIdentifier
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AlgorithmIdentifier& CCMSSignerInfo::SignatureAlgorithmIdentifier() const
	{
	return *iData->iSignatureAI;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SignatureValue()
// Getter for SignatureValue
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8& CCMSSignerInfo::SignatureValue()	 const
	{
	return *iData->iSignatureValue;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::UnsignedAttributes()
// Getter for UnsignedAttributes
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtrFlat<CCMSAttribute>& CCMSSignerInfo::UnsignedAttributes() const
	{
	return *iData->iUnsignedAttributes;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SetCertificateL()
// Setter for Certificate
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignerInfo::SetCertificateL( const CCMSX509Certificate& aCertificate )
	{
	// creating Issuer and serial
	CCMSIssuerAndSerialNumber* tmpIssuer = CCMSIssuerAndSerialNumber::NewL(
														aCertificate.Issuer(),
														aCertificate.SerialNumber() );

	delete iData->iIssuerAndSerial;
	iData->iIssuerAndSerial = tmpIssuer;

	// setting normal certificate, deleting possible certificate url
	// and subject key identifier
	TInt unsignedAttCount = iData->iUnsignedAttributes->Count();
	for( TInt i = 0; i < unsignedAttCount; i++ )
		{
		CCMSAttribute* att = iData->iUnsignedAttributes->At( i );
		if( att->AttributeType() == KURLCertificateOID() )
			{
			iData->iUnsignedAttributes->Delete( i );
			delete att;
			}
		}
	delete iData->iSubjectKeyIdentifier;
	iData->iSubjectKeyIdentifier = NULL;

	// changing version to 1
	iData->iVersion = KCMSVersion1;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SetCertificateUrlL()
// Setter for certificate URL
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignerInfo::SetCertificateUrlL( const TDesC8& aCertificateUrl,
												  const TDesC8& aSubjectKeyID )
	{
	CCMSAttribute* certUrl = CreateCertificateUrlLC( aCertificateUrl );
	
	// remove possible URL from unsigned attributes
	TInt unsignedAttCount = iData->iUnsignedAttributes->Count();
	for( TInt i = 0; i < unsignedAttCount; i++ )
		{
		CCMSAttribute* att = iData->iUnsignedAttributes->At( i );
		if( att->AttributeType() == KURLCertificateOID() )
			{
			iData->iUnsignedAttributes->Delete( i );
			delete att;
			}
		}
	iData->iUnsignedAttributes->AppendL( certUrl );
	CleanupStack::Pop( certUrl );

	// taking copy of subject key identifier
	HBufC8* tmpSubjectKeyID = aSubjectKeyID.AllocL();

	delete iData->iSubjectKeyIdentifier;
	iData->iSubjectKeyIdentifier = tmpSubjectKeyID;

	// Issuer and serial is only for real certificates
	delete iData->iIssuerAndSerial;
	iData->iIssuerAndSerial = NULL;

	// changing version to 3
	iData->iVersion = KCMSVersion3;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SetDigestAlgorithmIdentifier()
// Setter for DigestAlgorithmIdentifier
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignerInfo::SetDigestAlgorithmIdentifier(
				CCMSX509AlgorithmIdentifier& aDigestAI )
	{
	delete iData->iDigestAI;
	iData->iDigestAI = &aDigestAI;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SetSignatureAlgorithmIdentifier()
// Setter for SignatureAlgorithmIdentifier
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignerInfo::SetSignatureAlgorithmIdentifier(
				CCMSX509AlgorithmIdentifier& aSignatureAI )
	{
	delete iData->iSignatureAI;
	iData->iSignatureAI = &aSignatureAI;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SetSignatureValueL()
// Setter for SignatureValue
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignerInfo::SetSignatureValueL( const TDesC8& aSignatureValue )
	{
	HBufC8* tmpSignValue = aSignatureValue.AllocL();
	delete iData->iSignatureValue;
	iData->iSignatureValue = tmpSignValue;
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SetSignedAttributesL()
// Setter for Signed attributes
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignerInfo::SetSignedAttributesL( const TDesC& aContentType,
													const TDesC8& aMessageDigest,
													const TTime& aSigningTime,
													const TDesC8* aCertHash )
	{
	CArrayPtrFlat<CCMSAttribute>* tmpAttributes =
		new(ELeave)CArrayPtrFlat<CCMSAttribute>( KDefaultGranularity );
	CleanupStack::PushL( tmpAttributes );
	CleanupResetAndDestroy< CArrayPtrFlat<CCMSAttribute> >::PushL( *tmpAttributes );


	if( aContentType != KNullDesC() )
		{
		// creating content type
		CASN1EncObjectIdentifier* contOid =
			CASN1EncObjectIdentifier::NewLC( aContentType );
		HBufC8* contentType = CreateDerEncodingL( contOid );
		CleanupStack::PushL( contentType );
		CCMSAttribute* contType = CCMSAttribute::NewLC( KContentTypeOID, *contentType );
		tmpAttributes->AppendL( contType );
		CleanupStack::Pop( contType );
		CleanupStack::PopAndDestroy( contentType );
		CleanupStack::PopAndDestroy( contOid );
		}

	if( aMessageDigest != KNullDesC8() )
		{
		// creating MessageDigest
		CASN1EncOctetString* signature =
			CASN1EncOctetString::NewLC( aMessageDigest );
		HBufC8* messageDigest = CreateDerEncodingL( signature );
		CleanupStack::PushL( messageDigest );
		CCMSAttribute* mDigest = CCMSAttribute::NewLC( KMessageDigestOID, *messageDigest );
		tmpAttributes->AppendL( mDigest );
		CleanupStack::Pop( mDigest );
		CleanupStack::PopAndDestroy( messageDigest );
		CleanupStack::PopAndDestroy( signature );
		}

	// creating Signing time
    CASN1EncBase* signTimeDER = TCMSTimeUtil::ConvertToEncoderLC( aSigningTime );
	HBufC8* signTime = CreateDerEncodingL( signTimeDER );
	CleanupStack::PushL( signTime );
	CCMSAttribute* sTime = CCMSAttribute::NewLC( KSignTimeOID, *signTime );
	tmpAttributes->AppendL( sTime );
	CleanupStack::Pop( sTime );
	CleanupStack::PopAndDestroy( signTime );
	CleanupStack::PopAndDestroy( signTimeDER );

	// are we adding certificates
	if( aCertHash )
		{
		CCMSAttribute* signingCert = 
			CCMSAttribute::NewLC( KPKCS9SigCertOID, *aCertHash );
		tmpAttributes->AppendL( signingCert );
		CleanupStack::Pop( signingCert );
		}

	iData->iSignedAttributes->ResetAndDestroy();
	delete iData->iSignedAttributes;
	iData->iSignedAttributes = tmpAttributes;
	CleanupStack::Pop( tmpAttributes ); // ResetAndDestroy
	CleanupStack::Pop( tmpAttributes );
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::SetUnsignedAttributesL()
// Setter for Unsigned attributes
// -----------------------------------------------------------------------------
EXPORT_C void CCMSSignerInfo::SetUnsignedAttributesL(
	CArrayPtrFlat<CCMSAttribute>& aUnsignedAttributes )
	{
	iData->iUnsignedAttributes->ResetAndDestroy();
	delete iData->iUnsignedAttributes;
	iData->iUnsignedAttributes = &aUnsignedAttributes;
	}


// -----------------------------------------------------------------------------
// CCMSSignerInfo::DecodeAttributesL()
// Decodes Attributes to array
// -----------------------------------------------------------------------------
void CCMSSignerInfo::DecodeAttributesL(
	const TDesC8& aRawData,
	CArrayPtrFlat<CCMSAttribute>* aAttributes )
	{
	TASN1DecSet attributeDec;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* attributes =
		attributeDec.DecodeDERLC( aRawData, pos );

	TInt numOfAttributes = attributes->Count();
	for( TInt i = 0; i < numOfAttributes; i++ )
		{
		CCMSAttribute* att = CCMSAttribute::NewLC();
		att->DecodeL( attributes->At( i )->Encoding() );
		aAttributes->AppendL( att );
		CleanupStack::Pop( att );
		}
	CleanupStack::PopAndDestroy( attributes );
	}

// -----------------------------------------------------------------------------
// CCMSSignerInfo::CreateCertificateUrlLC()
// Creates certificate url attribute
// -----------------------------------------------------------------------------
CCMSAttribute* CCMSSignerInfo::CreateCertificateUrlLC( const TDesC8& aCertificateUrl )
	{
	// Add certificate URL to unsigned attributes
	CASN1EncOctetString* certificateUrlEnc =
		CASN1EncOctetString::NewLC( aCertificateUrl );
	//change tag to IA5
	certificateUrlEnc->SetTag( EASN1IA5String );

	HBufC8* certificateURL = CreateDerEncodingL( certificateUrlEnc );
	CleanupStack::PushL( certificateURL );
	CCMSAttribute* certUrl = CCMSAttribute::NewL( KURLCertificateOID, *certificateURL );
	CleanupStack::PopAndDestroy( certificateURL );
	CleanupStack::PopAndDestroy( certificateUrlEnc );
	CleanupStack::PushL( certUrl );
	return certUrl;
	}
//  End of File
