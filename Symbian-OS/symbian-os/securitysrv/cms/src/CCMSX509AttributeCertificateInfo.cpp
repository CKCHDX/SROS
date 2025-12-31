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
* Description:  X.509 AttributeCertificateInfo type
*
*/


// INCLUDE FILES
#include    "CCMSX509AttributeCertificateInfo.h"
#include "CCMSX509IssuerSerial.h"
#include "CCMSX509GeneralNames.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include "CCMSAttribute.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KMinNumberOfSubModules = 6;
const TInt KMaxNumberOfSubModules = 9;
const TInt KValidityPeriodModuleCount = 2;
const TInt KValidityNotBeforeIndex = 0;
const TInt KValidityNotAfterIndex = 1;
const TInt KDefaultGranularity = 1;
const TInt KDefaultVersion = 0; // v1 == 0
const TTagType KBaseCertificateIDTag = 0;
const TTagType KSubjectNameTag = 1;

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CCMSX509AttributeCertificateInfo::CAttributeCertificateData::
~CAttributeCertificateData()
    {
	delete iBaseCertificateID;
	delete iSubjectName;
	delete iIssuer;
	delete iSignature;

 	if( iAttributes )
 		{
 		iAttributes->ResetAndDestroy();
		delete iAttributes;
		}

    delete iIssuerUniqueID;
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::CCMSX509AttributeCertificateInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AttributeCertificateInfo::CCMSX509AttributeCertificateInfo()
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509AttributeCertificateInfo::ConstructL(
    const CCMSX509IssuerSerial& aBaseCertificateID,
    const CCMSX509GeneralNames& aIssuer,
    const CCMSX509AlgorithmIdentifier& aSignature,
    const TInt aSerialNumber,
    const TTime& aNotBeforeTime,
    const TTime& aNotAfterTime,
    const CArrayPtr< CCMSAttribute >& aAttributes )
    {
    iData = new( ELeave ) CAttributeCertificateData();
    iData->iVersion = KDefaultVersion;
    SetBaseCertificateIDL( aBaseCertificateID );
    SetIssuerL( aIssuer );
    SetSignatureL( aSignature );
    SetSerialNumber( aSerialNumber );
    SetNotBeforeTimeL( aNotBeforeTime );
    SetNotAfterTimeL( aNotAfterTime );
    SetAttributesL( aAttributes );
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509AttributeCertificateInfo::ConstructL(
    const CCMSX509GeneralNames& aSubjectName,
    const CCMSX509GeneralNames& aIssuer,
    const CCMSX509AlgorithmIdentifier& aSignature,
    const TInt aSerialNumber,
    const TTime& aNotBeforeTime,
    const TTime& aNotAfterTime,
    const CArrayPtr< CCMSAttribute >& aAttributes )
    {
    iData = new( ELeave ) CAttributeCertificateData();
    iData->iVersion = KDefaultVersion;
    SetSubjectNameL( aSubjectName );
    SetIssuerL( aIssuer );
    SetSignatureL( aSignature );
    SetSerialNumber( aSerialNumber );
    SetNotBeforeTimeL( aNotBeforeTime );
    SetNotAfterTimeL( aNotAfterTime );
    SetAttributesL( aAttributes );
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509AttributeCertificateInfo::ConstructL( )
    {
    // creating empty/default values
    CAttributeCertificateData* data = new( ELeave ) CAttributeCertificateData();
    CleanupStack::PushL( data );
    data->iVersion = KDefaultVersion;
    data->iBaseCertificateID = CCMSX509IssuerSerial::NewL();
    data->iIssuer = CCMSX509GeneralNames::NewL();
    data->iSignature = CCMSX509AlgorithmIdentifier::NewL();
    data->iNotBeforeTime.UniversalTime();
    data->iNotAfterTime.UniversalTime();
    data->iNotAfterTime+=TTimeIntervalYears( 1 );
    data->iAttributes = new( ELeave ) CArrayPtrFlat< CCMSAttribute >( 1 );
    CleanupStack::Pop( data );
    iData = data;
    }
    

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AttributeCertificateInfo*
CCMSX509AttributeCertificateInfo::NewL()
	{
	// creating with empty values
    CCMSX509AttributeCertificateInfo* self =
        new( ELeave ) CCMSX509AttributeCertificateInfo();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AttributeCertificateInfo*
CCMSX509AttributeCertificateInfo::NewL(
    const CCMSX509IssuerSerial& aBaseCertificateID,
    const CCMSX509GeneralNames& aIssuer,
    const CCMSX509AlgorithmIdentifier& aSignature,
    const TInt aSerialNumber,
    const TTime& aNotBeforeTime,
    const TTime& aNotAfterTime,
    const CArrayPtr< CCMSAttribute >& aAttributes )
    {
    CCMSX509AttributeCertificateInfo* self =
        new( ELeave ) CCMSX509AttributeCertificateInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aBaseCertificateID, aIssuer, aSignature, aSerialNumber,
                      aNotBeforeTime, aNotAfterTime,
                      aAttributes );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AttributeCertificateInfo*
CCMSX509AttributeCertificateInfo::NewL(
    const CCMSX509GeneralNames& aSubjectName,
    const CCMSX509GeneralNames& aIssuer,
    const CCMSX509AlgorithmIdentifier& aSignature,
    const TInt aSerialNumber,
    const TTime& aNotBeforeTime,
    const TTime& aNotAfterTime,
    const CArrayPtr< CCMSAttribute >& aAttributes )
    {
    CCMSX509AttributeCertificateInfo* self =
        new( ELeave ) CCMSX509AttributeCertificateInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aSubjectName, aIssuer, aSignature, aSerialNumber,
                      aNotBeforeTime, aNotAfterTime,
                      aAttributes );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CCMSX509AttributeCertificateInfo::~CCMSX509AttributeCertificateInfo()
    {
	delete iData;
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509AttributeCertificateInfo::DecodeL( const TDesC8& aRawData )
	{
    CAttributeCertificateData* data = new( ELeave ) CAttributeCertificateData();
    CleanupStack::PushL( data );
    
    CArrayPtr< TASN1DecGeneric >* itemList = DecodeSequenceLC(
        aRawData, KMinNumberOfSubModules, KMaxNumberOfSubModules );

    TInt sequenceCounter = 0;
    
    // decode version
    TASN1DecGeneric* version = itemList->At( sequenceCounter );
    TASN1DecInteger intDecoder;
    if( ( version->Tag() == EASN1Integer ) &&
        ( version->Class() == EUniversal ) )
        {
        data->iVersion =
            intDecoder.DecodeDERShortL( *version );
        sequenceCounter++;
        }
    else
        {
        data->iVersion = KDefaultVersion;
        }

    // decode subject
    TASN1DecGeneric* subject = itemList->At( sequenceCounter++ );
    subject->InitL( );
    TTagType subjectTag( subject->Tag() );
    switch( subjectTag )
        {
        case KBaseCertificateIDTag:
            {
            data->iBaseCertificateID = CCMSX509IssuerSerial::NewL();
            data->iBaseCertificateID->DecodeL( subject->GetContentDER() );
            break;
            }
        case KSubjectNameTag:
            {
            data->iSubjectName = CCMSX509GeneralNames::NewL();
            data->iSubjectName->DecodeL( subject->GetContentDER() );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }

    // decode issuer
    TASN1DecGeneric* issuer = itemList->At( sequenceCounter++ );
    issuer->InitL();
    data->iIssuer = CCMSX509GeneralNames::NewL();
    data->iIssuer->DecodeL( issuer->Encoding() );

    // decode signature
    TASN1DecGeneric* signature = itemList->At( sequenceCounter++ );
    signature->InitL();
    data->iSignature = CCMSX509AlgorithmIdentifier::NewL();
    data->iSignature->DecodeL( signature->Encoding() );

    // decode serialNumber
    data->iSerialNumber =
        intDecoder.DecodeDERShortL( *( itemList->At( sequenceCounter++ ) ) );

    // decode attCertValidityPeriod
    TASN1DecGeneric* validityPeriodDecoder = itemList->At( sequenceCounter++ );
    validityPeriodDecoder->InitL();
    CArrayPtr< TASN1DecGeneric >* validityPeriod = DecodeSequenceLC(
        validityPeriodDecoder->Encoding(), KValidityPeriodModuleCount,
        KValidityPeriodModuleCount );
    TASN1DecGeneralizedTime timeDecoder;
    data->iNotBeforeTime = timeDecoder.DecodeDERL(
        *( validityPeriod->At( KValidityNotBeforeIndex ) ) );
    data->iNotAfterTime = timeDecoder.DecodeDERL(
        *( validityPeriod->At( KValidityNotAfterIndex ) ) );
    CleanupStack::PopAndDestroy( validityPeriod );

    // decode attributes
    TASN1DecGeneric* attributesDecoder = itemList->At( sequenceCounter++ );
    attributesDecoder->InitL();
    CArrayPtr< TASN1DecGeneric >* attributes = DecodeSequenceLC(
        attributesDecoder->Encoding() );
    TInt attributeCount = attributes->Count();
    data->iAttributes = new( ELeave ) CArrayPtrFlat< CCMSAttribute >(
        attributeCount ? attributeCount : KDefaultGranularity );
    for( TInt i = 0; i < attributeCount; i++ )
        {
        TASN1DecGeneric* attributeDecoder = attributes->At( i );
        attributeDecoder->InitL();
        CCMSAttribute* attribute = CCMSAttribute::NewLC();
        attribute->DecodeL( attributeDecoder->Encoding() );
        data->iAttributes->AppendL( attribute );
        CleanupStack::Pop( attribute );
        }
    CleanupStack::PopAndDestroy( attributes );
    
    // decode issuerUniqueID
    if( itemList->Count() > sequenceCounter )
        {
        TASN1DecBitString bsDecoder;
        data->iIssuerUniqueID = bsDecoder.ExtractOctetStringL(
            *( itemList->At( sequenceCounter ) ) );
        }

    // all decoded, change state
    CleanupStack::PopAndDestroy( itemList );
    
    delete iData;
    iData = data;

    CleanupStack::Pop( data );
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509AttributeCertificateInfo::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();
    
    // encode version
    CASN1EncInt* version = CASN1EncInt::NewLC( iData->iVersion );
    root->AddAndPopChildL( version );
    
    // encode subject
    CASN1EncBase* subject = NULL;
    if( iData->iBaseCertificateID )
        {
        // encode baseCertificateID  [0]  IssuerSerial
        CASN1EncBase* baseCertificateID =
            iData->iBaseCertificateID->EncoderLC( );
        CleanupStack::Pop( baseCertificateID );
        subject = CASN1EncExplicitTag::NewLC(
            baseCertificateID, KBaseCertificateIDTag );
        }
    else
        {
        // encode subjectName        [1]  GeneralNames
        CASN1EncBase* subjectName = iData->iSubjectName->EncoderLC( );
        CleanupStack::Pop( subjectName );
        subject = CASN1EncExplicitTag::NewLC( subjectName, KSubjectNameTag );
        }
    root->AddAndPopChildL( subject );
    
    // encode issuer
    CASN1EncBase* issuer = iData->iIssuer->EncoderLC();
    root->AddAndPopChildL( issuer );
    
    // encode signature
    CASN1EncBase* signature = iData->iSignature->EncoderLC();
    root->AddAndPopChildL( signature );
    
    // encode serialNumber
    CASN1EncInt* serialNumber = CASN1EncInt::NewLC( iData->iSerialNumber );
    root->AddAndPopChildL( serialNumber );
    
    // encode attCertValidityPeriod
    CASN1EncSequence* validityPeriod = CASN1EncSequence::NewLC( );
    // encode notBeforeTime
    CASN1EncGeneralizedTime* notBeforeTime =
        CASN1EncGeneralizedTime::NewLC( iData->iNotBeforeTime );
    validityPeriod->AddAndPopChildL( notBeforeTime );
    //encode notAfterTime
    CASN1EncGeneralizedTime* notAfterTime =
        CASN1EncGeneralizedTime::NewLC( iData->iNotAfterTime );
    validityPeriod->AddAndPopChildL( notAfterTime );
    root->AddAndPopChildL( validityPeriod );
    
    // encode attributes             SEQUENCE OF Attribute
    CASN1EncSequence* attributes = CASN1EncSequence::NewLC();
    TInt attributeCount = iData->iAttributes->Count();
    for( TInt i = 0; i < attributeCount; i++ )
        {
        CCMSAttribute* attribute = iData->iAttributes->At( i );
        CASN1EncBase* attributeEncoder = attribute->EncoderLC( );
        attributes->AddAndPopChildL( attributeEncoder );
        }
    root->AddAndPopChildL( attributes );
    
    // encode issuerUniqueID         UniqueIdentifier OPTIONAL
    if( iData->iIssuerUniqueID )
        {
        CASN1EncBitString* uniqueID =
            CASN1EncBitString::NewLC( *( iData->iIssuerUniqueID ) );
        root->AddAndPopChildL( uniqueID );
        }
    
    return root;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::Version()
// Getter for Version
// -----------------------------------------------------------------------------
EXPORT_C TInt CCMSX509AttributeCertificateInfo::Version() const
	{
	return iData->iVersion;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::BaseCertificateID()
// Getter for baseCertificateID
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509IssuerSerial*
CCMSX509AttributeCertificateInfo::BaseCertificateID() const
	{
	return iData->iBaseCertificateID;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SubjectName()
// Getter for subjectName
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509GeneralNames*
CCMSX509AttributeCertificateInfo::SubjectName() const
	{
	return iData->iSubjectName;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::Issuer()
// Getter for Issuer
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509GeneralNames&
CCMSX509AttributeCertificateInfo::Issuer() const
	{
	return *iData->iIssuer;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::Signature()
// Getter for signature
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AlgorithmIdentifier&
CCMSX509AttributeCertificateInfo::Signature() const
	{
	return *iData->iSignature;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SerialNumber()
// Getter for serialNumber
// -----------------------------------------------------------------------------
EXPORT_C TInt
CCMSX509AttributeCertificateInfo::SerialNumber() const
	{
	return iData->iSerialNumber;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::NotBeforeTime()
// Getter for notBeforeTime
// -----------------------------------------------------------------------------
EXPORT_C const TTime& CCMSX509AttributeCertificateInfo::NotBeforeTime()	 const
	{
	return iData->iNotBeforeTime;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::NotAfterTime()
// Getter for notAfterTime
// -----------------------------------------------------------------------------
EXPORT_C const TTime& CCMSX509AttributeCertificateInfo::NotAfterTime()	 const
	{
	return iData->iNotAfterTime;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::Attributes()
// Getter for Attributes
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr<CCMSAttribute>&
CCMSX509AttributeCertificateInfo::Attributes() const
	{
	return *iData->iAttributes;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::IssuerUniqueID()
// Getter for subjectName
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8*
CCMSX509AttributeCertificateInfo::IssuerUniqueID() const
	{
	return iData->iIssuerUniqueID;
	}


// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetVersion()
// Setter for version
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetVersion(
    const TInt aVersion )
	{
	iData->iVersion = aVersion;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetBaseCertificateIDL()
// Setter for baseCertificateID, deletes also subjectName
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetBaseCertificateIDL(
    const CCMSX509IssuerSerial& aBaseCertificateID )
	{
    CCMSX509IssuerSerial* id = CCMSX509IssuerSerial::NewL(
        aBaseCertificateID.Issuer(), aBaseCertificateID.Serial() );
    CleanupStack::PushL( id );
    const TDesC8* uid = aBaseCertificateID.IssuerUID();
    if( uid )
        {
        id->SetIssuerUIDL( *uid );
        }
    CleanupStack::Pop( id );
    delete iData->iBaseCertificateID;
    iData->iBaseCertificateID = id;
    delete iData->iSubjectName;
    iData->iSubjectName = NULL;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetSubjectNameL()
// Setter for subjectName, deletes also baseCertificateID
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetSubjectNameL(
    const CCMSX509GeneralNames& aSubjectName )
	{
    CCMSX509GeneralNames* subjectName =
        CCMSX509GeneralNames::NewL( aSubjectName );
    delete iData->iSubjectName;
    iData->iSubjectName = subjectName;
    delete iData->iBaseCertificateID;
    iData->iBaseCertificateID = NULL;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetIssuerL()
// Setter for Issuer
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetIssuerL(
    const CCMSX509GeneralNames& aIssuer )
	{
    CCMSX509GeneralNames* issuer = CCMSX509GeneralNames::NewL( aIssuer );
    delete iData->iIssuer;
    iData->iIssuer = issuer;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetSignatureL()
// Setter for Signature
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetSignatureL(
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
// CCMSX509AttributeCertificateInfo::SetSerialNumber()
// Setter for serialNumber
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetSerialNumber(
    const TInt aSerialNumber )
	{
    iData->iSerialNumber = aSerialNumber;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetNotBeforeTimeL()
// Setter for notBeforeTime
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetNotBeforeTimeL(
	const TTime& aNotBeforeTime )
	{
    iData->iNotBeforeTime = aNotBeforeTime;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetNotAfterTimeL()
// Setter for notAfterTime
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetNotAfterTimeL(
	const TTime& aNotAfterTime )
	{
    iData->iNotAfterTime = aNotAfterTime;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetAttributesL()
// Setter for attributes
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetAttributesL(
	const CArrayPtr< CCMSAttribute >& aAttributes )
	{
    TInt attributeCount = aAttributes.Count();
    CArrayPtr< CCMSAttribute >* attributes = NULL;
    if( attributeCount > 0 )
        {
        attributes =
            new( ELeave ) CArrayPtrFlat< CCMSAttribute >( attributeCount );
        CleanupStack::PushL( attributes );
        for( TInt i = 0; i < attributeCount; i++ )
            {
            const CCMSAttribute* origAttribute = aAttributes[ i ];
            CCMSAttribute* attribute =
                CCMSAttribute::NewLC( origAttribute->AttributeType(),
                                      origAttribute->AttributeValues() );
            attributes->AppendL( attribute );
            }
    
        CleanupStack::Pop( attributeCount ); // all attributes
        CleanupStack::Pop( attributes );
        }
    else
        {
        attributes =
            new( ELeave ) CArrayPtrFlat< CCMSAttribute >( KDefaultGranularity );
        }


    if( iData->iAttributes )
        {
        iData->iAttributes->ResetAndDestroy();
        delete iData->iAttributes;
        }
    iData->iAttributes = attributes;
    
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificateInfo::SetIssuerUniqueIDL()
// Setter for issuerUniqueID
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificateInfo::SetIssuerUniqueIDL(
	const TDesC8& aIssuerUniqueID )
	{
    HBufC8* uniqueId = aIssuerUniqueID.AllocL();
    delete iData->iIssuerUniqueID;
    iData->iIssuerUniqueID = uniqueId;
	}

//  End of File
