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
* Description:  X.509 Certificate type
*
*/


// INCLUDE FILES
#include    "CCMSX509Certificate.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include "CCMSX509Validity.h"
#include "CCMSX509SubjectPublicKeyInfo.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KVersion2 = 1;
const TInt KVersion3 = 2;
const TTagType KVersionTag = 0;
const TTagType KIssuerUniqueIdentifierTag = 1;
const TTagType KSubjectUniqueIdentifierTag = 2;
const TInt KToBeSignedItemsMin = 6;
const TInt KToBeSignedItemsMax = 10;
const TInt KDefaultGranularity = 1;

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CCMSX509Certificate::CCertificateData::~CCertificateData()
    {
    delete iSerialNumber;
    delete iSignature;
    delete iIssuer;
    delete iValidity;
    delete iSubject;
    delete iSubjectPublicKeyInfo;
    delete iIssuerUniqueIdentifier;
    delete iSubjectUniqueIdentifier;
    }

// -----------------------------------------------------------------------------
// CCMSX509Certificate::CCMSX509Certificate
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Certificate::CCMSX509Certificate( )
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509Certificate::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509Certificate::ConstructL(
    const TDesC8& aSerialNumber,
    const CCMSX509AlgorithmIdentifier& aSignature,
    const CX500DistinguishedName& aIssuer,
    const CCMSX509Validity& aValidity,
    const CX500DistinguishedName& aSubject,
    const CCMSX509SubjectPublicKeyInfo& aSubjectPublicKeyInfo,
    const CCMSX509AlgorithmIdentifier& aAlgorithmIdentifier,
    const TDesC8& aEncrypted )
    {
    BaseConstructL( aAlgorithmIdentifier, aEncrypted );
    iData = new( ELeave ) CCertificateData;
    SetSerialNumberL( aSerialNumber );
    SetSignatureL( aSignature );
    SetIssuerL( aIssuer );
    SetValidityL( aValidity );
    SetSubjectL( aSubject );
    SetSubjectPublicKeyInfoL( aSubjectPublicKeyInfo );
    }

// -----------------------------------------------------------------------------
// CCMSX509Certificate::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509Certificate::ConstructL(
    const CX509Certificate& aCertificate )
    {
    SetDataL( aCertificate );
    }

// -----------------------------------------------------------------------------
// CCMSX509Certificate::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509Certificate::ConstructL( )
    {
    // creating empty/default values
    CArrayPtrFlat< CX520AttributeTypeAndValue >* elements = new( ELeave )
        CArrayPtrFlat< CX520AttributeTypeAndValue >( KDefaultGranularity );
    CleanupStack::PushL( elements );
    
    iData = new( ELeave ) CCertificateData;
    iData->iSerialNumber = KNullDesC8().AllocL();
    iData->iSignature = CCMSX509AlgorithmIdentifier::NewL();
    iData->iIssuer = CX500DistinguishedName::NewL( *elements );
    iData->iValidity = CCMSX509Validity::NewL();
    iData->iSubject = CX500DistinguishedName::NewL( *elements );
    iData->iSubjectPublicKeyInfo = CCMSX509SubjectPublicKeyInfo::NewL();

    CleanupStack::PopAndDestroy( elements );
    
    iAlgorithmIdentifier = CCMSX509AlgorithmIdentifier::NewL();
    iEncrypted = KNullDesC8().AllocL();
    
    }
    

// -----------------------------------------------------------------------------
// CCMSX509Certificate::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Certificate*
CCMSX509Certificate::NewL()
	{
	// creating with empty values
    CCMSX509Certificate* self =
        new( ELeave ) CCMSX509Certificate();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Certificate*
CCMSX509Certificate::NewL(
    const TDesC8& aSerialNumber,
    const CCMSX509AlgorithmIdentifier& aSignature,
    const CX500DistinguishedName& aIssuer,
    const CCMSX509Validity& aValidity,
    const CX500DistinguishedName& aSubject,
    const CCMSX509SubjectPublicKeyInfo& aSubjectPublicKeyInfo,
    const CCMSX509AlgorithmIdentifier& aAlgorithmIdentifier,
    const TDesC8& aEncrypted )
    {
    CCMSX509Certificate* self =
        new( ELeave ) CCMSX509Certificate();
    CleanupStack::PushL( self );
    self->ConstructL( aSerialNumber, aSignature, aIssuer, aValidity, aSubject,
                      aSubjectPublicKeyInfo, aAlgorithmIdentifier, aEncrypted );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSX509Certificate::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509Certificate*
CCMSX509Certificate::NewL(
    const CX509Certificate& aCertificate )
    {
    CCMSX509Certificate* self =
        new( ELeave ) CCMSX509Certificate();
    CleanupStack::PushL( self );
    self->ConstructL( aCertificate );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CCMSX509Certificate::~CCMSX509Certificate()
    {
	delete iData;
    }

// -----------------------------------------------------------------------------
// CCMSX509Certificate::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509Certificate::DecodeL( const TDesC8& aRawData )
	{
    CCMSX509AlgorithmIdentifier* algId = NULL;
    HBufC8* encrypted = NULL;
    TASN1DecGeneric dataDecoder =
        DecodeSignatureL( aRawData, algId, encrypted );

    CleanupStack::PushL( algId );
    CleanupStack::PushL( encrypted );
    
    CArrayPtr< TASN1DecGeneric >* itemList = DecodeSequenceLC(
        dataDecoder.Encoding(), KToBeSignedItemsMin, KToBeSignedItemsMax );

    CCertificateData* data = new( ELeave ) CCertificateData();
    CleanupStack::PushL( data );

    TInt sequenceCounter = 0;

    // decode version
    TASN1DecGeneric* taggedVersion = itemList->At( sequenceCounter );
    if( ( taggedVersion->Tag() == KVersionTag ) &&
        ( taggedVersion->Class() == EContextSpecific ) )
        {
        TASN1DecGeneric version( taggedVersion->GetContentDER() );
        version.InitL();
        TASN1DecInteger intDecoder;
        data->iVersion =
            intDecoder.DecodeDERShortL( version );
        sequenceCounter++;
        }

    // decode serialNumber
    data->iSerialNumber =
        itemList->At( sequenceCounter++ )->GetContentDER().AllocL();

    // decode signature
    data->iSignature = CCMSX509AlgorithmIdentifier::NewL();
    data->iSignature->DecodeL( itemList->At( sequenceCounter++)->Encoding() );

    // decode issuer
    data->iIssuer = CX500DistinguishedName::NewL(
        itemList->At( sequenceCounter++ )->Encoding() );

    // decode validity
    data->iValidity = CCMSX509Validity::NewL();
    data->iValidity->DecodeL( itemList->At( sequenceCounter++ )->Encoding() );

    // decode subject
    data->iSubject = CX500DistinguishedName::NewL(
        itemList->At( sequenceCounter++ )->Encoding() );

    // decode subjectPublicKeyInfo
    data->iSubjectPublicKeyInfo = CCMSX509SubjectPublicKeyInfo::NewL();
    data->iSubjectPublicKeyInfo->DecodeL(
        itemList->At( sequenceCounter++ )->Encoding() );

    // decode issuerUniqueIdentifier, if it exists
    TInt itemCount = itemList->Count();
    TASN1DecBitString bsDecoder;
    if( sequenceCounter < itemCount )
        {
        TASN1DecGeneric* taggedIssuerUniqueIdentifier =
            itemList->At( sequenceCounter );
        if( taggedIssuerUniqueIdentifier->Tag() == KIssuerUniqueIdentifierTag )
            {
            data->iIssuerUniqueIdentifier =
                bsDecoder.ExtractOctetStringL( *taggedIssuerUniqueIdentifier );
            sequenceCounter++;
            }
        }
    
    // decode subjectUniqueIdentifier, if it exists
    if( sequenceCounter < itemCount )
        {
        TASN1DecGeneric* taggedSubjectUniqueIdentifier =
            itemList->At( sequenceCounter );
        if( taggedSubjectUniqueIdentifier->Tag() == KSubjectUniqueIdentifierTag )
            {
            data->iSubjectUniqueIdentifier =
                bsDecoder.ExtractOctetStringL( *taggedSubjectUniqueIdentifier );
            sequenceCounter++;
            }
        }

    // extensions are ignored
    
    // all done, change state
    delete iAlgorithmIdentifier;
    iAlgorithmIdentifier = algId;
    delete iEncrypted;
    iEncrypted = encrypted;
    delete iData;
    iData = data;
    CleanupStack::Pop( data );
    CleanupStack::PopAndDestroy( itemList );
    CleanupStack::Pop( encrypted );
    CleanupStack::Pop( algId );
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509Certificate::EncoderLC() const
	{

    // encode ToBeSigned part
    CASN1EncBase* toBeSigned = ToBeSignedEncoderLC();

    // sign
    CASN1EncSequence* root = SignAndPopLC( toBeSigned );

    return root;
    }

// -----------------------------------------------------------------------------
// CCMSX509Certificate::Version()
// Getter for Version
// -----------------------------------------------------------------------------
EXPORT_C TInt CCMSX509Certificate::Version() const
	{
	return iData->iVersion;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SerialNumber()
// Getter for SerialNumber
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8& CCMSX509Certificate::SerialNumber() const
	{
	return *( iData->iSerialNumber );
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::Signature()
// Getter for signature
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AlgorithmIdentifier& CCMSX509Certificate::Signature() const
	{
	return *( iData->iSignature );
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::Issuer()
// Getter for issuer
// -----------------------------------------------------------------------------
EXPORT_C const CX500DistinguishedName& CCMSX509Certificate::Issuer() const
	{
	return *( iData->iIssuer );
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::Validity()
// Getter for Validity
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509Validity& CCMSX509Certificate::Validity() const
	{
	return *( iData->iValidity );
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::Subject()
// Getter for subject
// -----------------------------------------------------------------------------
EXPORT_C const CX500DistinguishedName& CCMSX509Certificate::Subject() const
	{
	return *( iData->iSubject );
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SubjectPublicKeyInfo()
// Getter for subjectPublicKeyInfo
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509SubjectPublicKeyInfo&
CCMSX509Certificate::SubjectPublicKeyInfo() const
	{
	return *( iData->iSubjectPublicKeyInfo );
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::IssuerUniqueIdentifier()
// Getter for issuerUniqueIdentifier
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8* CCMSX509Certificate::IssuerUniqueIdentifier() const
	{
	return iData->iIssuerUniqueIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SubjectUniqueIdentifier()
// Getter for subjectUniqueIdentifier
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8* CCMSX509Certificate::SubjectUniqueIdentifier() const
	{
	return iData->iSubjectUniqueIdentifier;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetVersion()
// Setter for version
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetVersion( const TInt aVersion )
	{
    iData->iVersion = aVersion;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetSerialNumberL()
// Setter for serialNumber
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetSerialNumberL( const TDesC8& aSerialNumber )
	{
    HBufC8* serialNumber = aSerialNumber.AllocLC();
    delete iData->iSerialNumber;
    iData->iSerialNumber = serialNumber;
    CleanupStack::Pop( serialNumber );
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetSignatureL()
// Setter for signature
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetSignatureL(
    const CCMSX509AlgorithmIdentifier& aSignature )
	{
    CCMSX509AlgorithmIdentifier* signature =
        CCMSX509AlgorithmIdentifier::NewL( aSignature.AlgorithmIdentifier() );
    CleanupStack::PushL( signature );
    const CAlgorithmIdentifier* digestIdentifier =
        aSignature.DigestAlgorithm();
    if( digestIdentifier )
        {
        signature->SetDigestAlgorithmL( digestIdentifier );
        }
    CleanupStack::Pop( signature );
    delete iData->iSignature;
    iData->iSignature = signature;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetIssuerL()
// Setter for issuer
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetIssuerL(
    const CX500DistinguishedName& aIssuer )
	{
    CX500DistinguishedName* issuer = CX500DistinguishedName::NewL( aIssuer );
    delete iData->iIssuer;
    iData->iIssuer = issuer;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetValidityL()
// Setter for validity
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetValidityL(
    const CCMSX509Validity& aValidity )
	{
    CCMSX509Validity* validity =
        CCMSX509Validity::NewL( aValidity.NotBefore(), aValidity.NotAfter() );
    delete iData->iValidity;
    iData->iValidity = validity;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetSubjectL()
// Setter for subject
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetSubjectL(
    const CX500DistinguishedName& aSubject )
	{
    CX500DistinguishedName* subject = CX500DistinguishedName::NewL( aSubject );
    delete iData->iSubject;
    iData->iSubject = subject;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetSubjectPublicKeyInfoL()
// Setter for subjectPublicKeyInfo
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetSubjectPublicKeyInfoL(
    const CCMSX509SubjectPublicKeyInfo& aSubjectPublicKeyInfo )
	{
    CCMSX509SubjectPublicKeyInfo* spkInfo = CCMSX509SubjectPublicKeyInfo::NewL(
        aSubjectPublicKeyInfo.Algorithm(),
        aSubjectPublicKeyInfo.SubjectPublicKey() );
    delete iData->iSubjectPublicKeyInfo;
    iData->iSubjectPublicKeyInfo = spkInfo;
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetIssuerUniqueIdentifierL()
// Setter for issuerUniqueIdentifier, make sure version is v2 or v3
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetIssuerUniqueIdentifierL(
    const TDesC8& aIssuerUniqueIdentifier )
	{
    HBufC8* issuerUniqueIdentifier = aIssuerUniqueIdentifier.AllocL();
    delete iData->iIssuerUniqueIdentifier;
    iData->iIssuerUniqueIdentifier = issuerUniqueIdentifier;
    if( ( iData->iVersion > KVersion3 ) || ( iData->iVersion < KVersion2 ) )
        {
        iData->iVersion = KVersion2;
        }
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetSubjectUniqueIdentifierL()
// Setter for subjectUniqueIdentifier, make sure version is v2 or v3
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509Certificate::SetSubjectUniqueIdentifierL(
    const TDesC8& aSubjectUniqueIdentifier )
	{
    HBufC8* subjectUniqueIdentifier = aSubjectUniqueIdentifier.AllocL();
    delete iData->iSubjectUniqueIdentifier;
    iData->iSubjectUniqueIdentifier = subjectUniqueIdentifier;
    if( ( iData->iVersion > KVersion3 ) || ( iData->iVersion < KVersion2 ) )
        {
        iData->iVersion = KVersion2;
        }
	}

// -----------------------------------------------------------------------------
// CCMSX509Certificate::ToBeSignedEncoderLC
// Returns ASN1 encoder for the the ToBeSigned part
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509Certificate::ToBeSignedEncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode version
    CASN1EncInt* version = CASN1EncInt::NewL( iData->iVersion );
    CASN1EncExplicitTag* taggedVersion =
        CASN1EncExplicitTag::NewLC( version, KVersionTag );
    root->AddAndPopChildL( taggedVersion );
    
    // encode serialNumber
    CASN1EncOctetString* serialNumber =
        CASN1EncOctetString::NewLC( *( iData->iSerialNumber ) );
    serialNumber->SetTag( EASN1Integer, EUniversal );
    root->AddAndPopChildL( serialNumber );

    // encode signature
    CASN1EncBase* signature = iData->iSignature->EncoderLC();
    root->AddAndPopChildL( signature );

    // encode issuer
    CASN1EncSequence* issuer = iData->iIssuer->EncodeASN1LC();
    root->AddAndPopChildL( issuer );
    
    // encode validity
    CASN1EncBase* validity = iData->iValidity->EncoderLC();
    root->AddAndPopChildL( validity );

    // encode subject
    CASN1EncSequence* subject = iData->iSubject->EncodeASN1LC();
    root->AddAndPopChildL( subject );

    // encode subjectPublicKeyInfo
    CASN1EncBase* spkInfo = iData->iSubjectPublicKeyInfo->EncoderLC();
    root->AddAndPopChildL( spkInfo );

    if( iData->iIssuerUniqueIdentifier )
        {
        CASN1EncBitString* iuIdentifier =
            CASN1EncBitString::NewLC( *iData->iIssuerUniqueIdentifier );
        iuIdentifier->SetTag( KIssuerUniqueIdentifierTag );
        root->AddAndPopChildL( iuIdentifier );
        }
    if( iData->iSubjectUniqueIdentifier )
        {
        CASN1EncBitString* suIdentifier =
            CASN1EncBitString::NewLC( *iData->iSubjectUniqueIdentifier );
        suIdentifier->SetTag( KSubjectUniqueIdentifierTag );
        root->AddAndPopChildL( suIdentifier );
        }
    
    return root;
    }

// -----------------------------------------------------------------------------
// CCMSX509Certificate::SetDataL
// Copies the data from the CX509Certificate object
// -----------------------------------------------------------------------------
void CCMSX509Certificate::SetDataL( const CX509Certificate& aCertificate )
    {
    const CSigningAlgorithmIdentifier& signingAlgorithm =
        aCertificate.SigningAlgorithm();
    CCMSX509AlgorithmIdentifier* algId =
        CCMSX509AlgorithmIdentifier::NewL( signingAlgorithm.AsymmetricAlgorithm(),
                                           signingAlgorithm.DigestAlgorithm() );
    CleanupStack::PushL( algId );

    HBufC8* encrypted = aCertificate.Signature().AllocLC();

    CCertificateData* data = new( ELeave ) CCertificateData();
    CleanupStack::PushL( data );

    data->iVersion = aCertificate.Version();
    
    data->iSerialNumber = aCertificate.SerialNumber().AllocL();

    data->iSignature = CCMSX509AlgorithmIdentifier::NewL(
        signingAlgorithm.AsymmetricAlgorithm(),
        signingAlgorithm.DigestAlgorithm() );

    data->iIssuer = CX500DistinguishedName::NewL( aCertificate.IssuerName() );
    
    data->iValidity = CCMSX509Validity::NewL( aCertificate.ValidityPeriod() );

    data->iSubject = CX500DistinguishedName::NewL( aCertificate.SubjectName() );

    data->iSubjectPublicKeyInfo = CCMSX509SubjectPublicKeyInfo::NewL(
        aCertificate.PublicKey() );

    // all done, change state
    delete iData;
    iData = data;
    delete iAlgorithmIdentifier;
    iAlgorithmIdentifier = algId;
    delete iEncrypted;
    iEncrypted = encrypted;
    CleanupStack::Pop( 3 ); // data, encrypted, algId
    }

//  End of File
