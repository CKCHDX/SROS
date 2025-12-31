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
#include    "CCMSAuthenticatedData.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include "CCMSOriginatorInfo.h"
#include "CCMSRecipientInfo.h"
#include "CCMSKeyTransRecipientInfo.h"
#include "CCMSKeyAgreeRecipientInfo.h"
#include "CCMSKEKRecipientInfo.h"
#include "CCMSAttribute.h"
#include "CCMSX509Certificate.h"
#include "CCMSCertificateChoices.h"
#include "CCMSX509CertificateList.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KDefaultGranularity = 1;
const TInt KMinNumberOfSubModules = 5;
const TInt KMaxNumberOfSubModules = 9;
const TTagType KOriginatorInfoTag = 0;
const TTagType KDigestAlgorithmTag = 1;
const TTagType KAuthenticatedAttributesTag = 2;
const TTagType KUnauthenticatedAttributesTag = 3;

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CCMSAuthenticatedData::CAuthenticatedDataData::~CAuthenticatedDataData()
    {
    delete iOriginatorInfo;
    if( iRecipientInfos )
        {
        iRecipientInfos->ResetAndDestroy();
        delete iRecipientInfos;
        }
    delete iMacAlgorithm;
    delete iDigestAlgorithm;
    delete iEncapContentInfo;
    if( iAuthenticatedAttributes )
        {
        iAuthenticatedAttributes->ResetAndDestroy();
        delete iAuthenticatedAttributes;
        }
    delete iMac;
    if( iUnauthenticatedAttributes )
        {
        iUnauthenticatedAttributes->ResetAndDestroy();
        delete iUnauthenticatedAttributes;
        }
    }

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::CCMSAuthenticatedData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAuthenticatedData::CCMSAuthenticatedData( )
    {
    }

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSAuthenticatedData::ConstructL(
    CArrayPtr< CCMSRecipientInfo >* aRecipientInfos,
    CCMSX509AlgorithmIdentifier* aMacAlgorithm,
    CCMSEncapsulatedContentInfo* aEncapContentInfo,
    const TDesC8& aMac )
    {
    iData = new( ELeave ) CAuthenticatedDataData;
    iData->iMac = aMac.AllocL();
    
    // all memory allocations have been done, we can take ownerships
    iData->iRecipientInfos = aRecipientInfos;
    iData->iMacAlgorithm = aMacAlgorithm;
    iData->iEncapContentInfo = aEncapContentInfo;
    }

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSAuthenticatedData::ConstructL( )
    {
    // creating empty/default values
    iData = new( ELeave ) CAuthenticatedDataData;
    iData->iRecipientInfos =
        new( ELeave ) CArrayPtrFlat< CCMSRecipientInfo >( KDefaultGranularity );
    iData->iMacAlgorithm = CCMSX509AlgorithmIdentifier::NewL();
    iData->iEncapContentInfo = CCMSEncapsulatedContentInfo::NewL();
    iData->iMac = KNullDesC8().AllocL();
    }
    

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAuthenticatedData*
CCMSAuthenticatedData::NewLC()
	{
	// creating with empty values
    CCMSAuthenticatedData* self =
        new( ELeave ) CCMSAuthenticatedData();
    CleanupStack::PushL( self );
    self->ConstructL( );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSAuthenticatedData*
CCMSAuthenticatedData::NewLC(
    CArrayPtr< CCMSRecipientInfo >* aRecipientInfos,
    CCMSX509AlgorithmIdentifier* aMacAlgorithm,
    CCMSEncapsulatedContentInfo* aEncapContentInfo,
    const TDesC8& aMac )
    {
    CCMSAuthenticatedData* self =
        new( ELeave ) CCMSAuthenticatedData();
    CleanupStack::PushL( self );
    self->ConstructL( aRecipientInfos, aMacAlgorithm, aEncapContentInfo, aMac );

    return self;
    }

// Destructor
CCMSAuthenticatedData::~CCMSAuthenticatedData()
    {
	delete iData;
    }

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSAuthenticatedData::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* items = DecodeSequenceLC(
        aRawData, KMinNumberOfSubModules, KMaxNumberOfSubModules );

    // now we know that there are at least 5 items
    TInt itemCount = items->Count();
    __ASSERT_DEBUG( itemCount >= KMinNumberOfSubModules, User::Invariant() );
    
    TInt sequenceCounter = 0;

    CAuthenticatedDataData* data = new( ELeave ) CAuthenticatedDataData;
    CleanupStack::PushL( data );
    
    // decode version
    TASN1DecInteger intDecoder;
    data->iVersion =
        intDecoder.DecodeDERShortL( *items->At( sequenceCounter++ ) );

    // decode originatorInfo, if exist
    TASN1DecGeneric* originatorInfoDecoder = items->At( sequenceCounter );
    if( originatorInfoDecoder->Tag() == KOriginatorInfoTag )
        {
        data->iOriginatorInfo = CCMSOriginatorInfo::NewL();
        data->iOriginatorInfo->DecodeImplicitTagL(
            originatorInfoDecoder->Encoding(), KOriginatorInfoTag );
        sequenceCounter++;
        }

    // decode recipientInfos
    CArrayPtr< TASN1DecGeneric >* recipientInfos = DecodeSequenceLC(
        items->At( sequenceCounter++)->Encoding() );
    TInt recipientInfoCount = recipientInfos->Count();
    data->iRecipientInfos =
        new( ELeave ) CArrayPtrFlat< CCMSRecipientInfo >( recipientInfoCount );
    for( TInt i = 0; i < recipientInfoCount; i++ )
        {
        TASN1DecGeneric* recipientInfoDecoder = recipientInfos->At( i );
        CCMSRecipientInfo* recipientInfo = NULL;
        switch( recipientInfoDecoder->Tag() )
            {
            case EASN1Sequence:
                {
                recipientInfo = CCMSKeyTransRecipientInfo::NewLC();
                recipientInfo->DecodeL( recipientInfoDecoder->Encoding() );
                break;
                }
            case KCMSKeyAgreeRecipientInfoTag:
                {
                recipientInfo = CCMSKeyAgreeRecipientInfo::NewLC();
                recipientInfo->DecodeL( recipientInfoDecoder->Encoding() );
                break;
                }
            case KCMSKEKRecipientInfoTag:
                {
                recipientInfo = CCMSKEKRecipientInfo::NewLC();
                recipientInfo->DecodeL( recipientInfoDecoder->Encoding() );
                break;
                }
            default:
                {
                User::Leave( KErrArgument );
                }
            }
        data->iRecipientInfos->AppendL( recipientInfo );
        CleanupStack::Pop( recipientInfo );
        }
    CleanupStack::PopAndDestroy( recipientInfos );
    
    // decode macAlgorithm
    data->iMacAlgorithm = CCMSX509AlgorithmIdentifier::NewL();
    data->iMacAlgorithm->DecodeL( items->At( sequenceCounter++)->Encoding() );

    // decode digestAlgorithm, if exist
    TASN1DecGeneric* digestAlgorithmDec = items->At( sequenceCounter );
    if( digestAlgorithmDec->Tag() == KDigestAlgorithmTag )
        {
        data->iDigestAlgorithm = CCMSX509AlgorithmIdentifier::NewL();
        data->iDigestAlgorithm->DecodeL( digestAlgorithmDec->GetContentDER() );
        sequenceCounter++;
        }

    // decode encapContentInfo
    if( sequenceCounter == itemCount )
        {
        User::Leave( KErrArgument );
        }
    data->iEncapContentInfo = CCMSEncapsulatedContentInfo::NewL();
    data->iEncapContentInfo->DecodeL(
        items->At( sequenceCounter++)->Encoding() );

    // decode authenticatedAttributes, if exist
    if( sequenceCounter == itemCount )
        {
        User::Leave( KErrArgument );
        }
    TASN1DecGeneric* authAttributesDec = items->At( sequenceCounter );
    if( authAttributesDec->Tag() == KAuthenticatedAttributesTag )
        {
        data->iAuthenticatedAttributes = DecodeAttributesL( authAttributesDec );
        sequenceCounter++;
        }

    // decode mac
    if( sequenceCounter == itemCount )
        {
        User::Leave( KErrArgument );
        }
    TASN1DecOctetString octetStrDec;
    data->iMac = octetStrDec.DecodeDERL( *( items->At( sequenceCounter++ ) ) );

    // decode unauthenticatedAttributes, if exist
    if( itemCount > sequenceCounter )
        {
        TASN1DecGeneric* unauthAttributesDec = items->At( sequenceCounter );
        if( unauthAttributesDec->Tag() != KUnauthenticatedAttributesTag )
            {
            User::Leave( KErrArgument );
            }
        data->iUnauthenticatedAttributes =
            DecodeAttributesL( unauthAttributesDec );
        }

    // all done, change state
    CleanupStack::Pop( data );
    CleanupStack::PopAndDestroy( items );
    delete iData;
    iData = data;
	}




// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------
CASN1EncBase* CCMSAuthenticatedData::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    // encode version
    CASN1EncInt* version = CASN1EncInt::NewLC( iData->iVersion );
    root->AddAndPopChildL( version );

    // encode originatorInfo, if exist
    if( iData->iOriginatorInfo )
        {
        CASN1EncBase* originatorInfo = iData->iOriginatorInfo->EncoderLC();
        originatorInfo->SetTag( KOriginatorInfoTag );
        root->AddAndPopChildL( originatorInfo );
        }

    // encode recipientInfos ::= SET OF RecipientInfo
    CASN1EncSequence* recipientInfos = CASN1EncSequence::NewLC();
    TInt recipientInfoCount = iData->iRecipientInfos->Count();
    TInt i = 0;
    for( ; i < recipientInfoCount; i++ )
        {
        CASN1EncBase* recipientInfo =
            iData->iRecipientInfos->At( i )->TaggedEncoderLC();
        recipientInfos->AddAndPopChildL( recipientInfo );
        }
    recipientInfos->SetTag( EASN1Set, EUniversal );
    root->AddAndPopChildL( recipientInfos );
    
    // encode macAlgorithm
    CASN1EncBase* macAlgorithm = iData->iMacAlgorithm->EncoderLC();
    root->AddAndPopChildL( macAlgorithm );

    // encode digestAlgorithm, if exist
    if( iData->iDigestAlgorithm )
        {
        CASN1EncBase* digestAlgorithm = iData->iDigestAlgorithm->EncoderLC();
        CleanupStack::Pop( digestAlgorithm );
        
        // CASN1EncExplicitTag takes ownership of the parameter, even
        // if the method leaves.
        CASN1EncExplicitTag* taggedDigestAlgorithm =
            CASN1EncExplicitTag::NewLC( digestAlgorithm, KDigestAlgorithmTag );
        root->AddAndPopChildL( taggedDigestAlgorithm );
        }

    // encode encapContentInfo
    CASN1EncBase* encapContentInfo = iData->iEncapContentInfo->EncoderLC();
    root->AddAndPopChildL( encapContentInfo );

    // encode authenticatedAttributes, if exist
    if( iData->iAuthenticatedAttributes )
        {
        CASN1EncSequence* authAttributes = CASN1EncSequence::NewLC();
        TInt authAttributeCount = iData->iAuthenticatedAttributes->Count();
        for( i = 0; i < authAttributeCount; i++ )
            {
            CASN1EncBase* attribute =
                iData->iAuthenticatedAttributes->At( i )->EncoderLC();
            authAttributes->AddAndPopChildL( attribute );
            }
        authAttributes->SetTag( KAuthenticatedAttributesTag );
        root->AddAndPopChildL( authAttributes );
        }

    // encode mac
    CASN1EncOctetString* mac = CASN1EncOctetString::NewLC( *iData->iMac );
    root->AddAndPopChildL( mac );
    
    // encode unauthenticatedAttributes, if exist
    if( iData->iUnauthenticatedAttributes )
        {
        CASN1EncSequence* unauthAttributes = CASN1EncSequence::NewLC();
        TInt unauthAttributeCount = iData->iUnauthenticatedAttributes->Count();
        for( i = 0; i < unauthAttributeCount; i++ )
            {
            CASN1EncBase* attribute =
                iData->iUnauthenticatedAttributes->At( i )->EncoderLC();
            unauthAttributes->AddAndPopChildL( attribute );
            }
        unauthAttributes->SetTag( KUnauthenticatedAttributesTag );
        root->AddAndPopChildL( unauthAttributes );
        }
    
    return root;
    }

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::Version()
// Getter for Version
// -----------------------------------------------------------------------------
EXPORT_C TInt CCMSAuthenticatedData::Version() const
	{
	return iData->iVersion;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::OriginatorInfo()
// Getter for OriginatorInfo
// -----------------------------------------------------------------------------
EXPORT_C const CCMSOriginatorInfo* CCMSAuthenticatedData::OriginatorInfo() const
	{
	return iData->iOriginatorInfo;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::RecipientInfos()
// Getter for recipientInfos
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSRecipientInfo >&
CCMSAuthenticatedData::RecipientInfos() const
	{
	return *( iData->iRecipientInfos );
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::MacAlgorithm()
// Getter for macAlgorithm
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AlgorithmIdentifier&
CCMSAuthenticatedData::MacAlgorithm() const
	{
	return *( iData->iMacAlgorithm );
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::DigestAlgorithm()
// Getter for digestAlgorithm
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AlgorithmIdentifier*
CCMSAuthenticatedData::DigestAlgorithm() const
	{
	return iData->iDigestAlgorithm;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::EncapContentInfo()
// Getter for encapContentInfo
// -----------------------------------------------------------------------------
EXPORT_C const CCMSEncapsulatedContentInfo&
CCMSAuthenticatedData::EncapContentInfo() const
	{
	return *( iData->iEncapContentInfo );
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::AuthenticatedAttributes()
// Getter for authenticatedAttributes
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSAttribute >*
CCMSAuthenticatedData::AuthenticatedAttributes() const
	{
	return iData->iAuthenticatedAttributes;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::Mac()
// Getter for mac
// -----------------------------------------------------------------------------
EXPORT_C const TDesC8& CCMSAuthenticatedData::Mac() const
	{
	return *( iData->iMac );
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::UnauthenticatedAttributes()
// Getter for unauthenticatedAttributes
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSAttribute >*
CCMSAuthenticatedData::UnauthenticatedAttributes() const
	{
	return iData->iUnauthenticatedAttributes;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetVersion()
// Setter for version
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetVersion( TInt aVersion )
	{
    iData->iVersion = aVersion;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetOriginatorInfoL()
// Setter for OriginatorInfo
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetOriginatorInfoL(
    CCMSOriginatorInfo* aOriginatorInfo )
	{
    delete iData->iOriginatorInfo;
    iData->iOriginatorInfo = aOriginatorInfo;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetRecipientInfosL()
// Setter for recipientInfos
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetRecipientInfosL(
    CArrayPtr< CCMSRecipientInfo >* aRecipientInfos )
	{
    if( ( !aRecipientInfos ) || ( aRecipientInfos->Count() == 0 ) )
        {
        User::Leave( KErrArgument );
        }
    
    if( iData->iRecipientInfos )
        {
        iData->iRecipientInfos->ResetAndDestroy();
        delete iData->iRecipientInfos;
        }
    iData->iRecipientInfos = aRecipientInfos;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetMacAlgorithmL()
// Setter for macAlgorithm
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetMacAlgorithmL(
    CCMSX509AlgorithmIdentifier* aMacAlgorithm )
	{
    if( !aMacAlgorithm )
        {
        User::Leave( KErrArgument );
        }
    delete iData->iMacAlgorithm;
    iData->iMacAlgorithm = aMacAlgorithm;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetDigestAlgorithmL()
// Setter for digestAlgorithm
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetDigestAlgorithmL(
    CCMSX509AlgorithmIdentifier* aDigestAlgorithm )
	{
    delete iData->iDigestAlgorithm;
    iData->iDigestAlgorithm = aDigestAlgorithm;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetEncapContentInfoL()
// Setter for encapContentInfo
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetEncapContentInfoL(
    CCMSEncapsulatedContentInfo* aEncapContentInfo )
	{
    if( !aEncapContentInfo )
        {
        User::Leave( KErrArgument );
        }
    delete iData->iEncapContentInfo;
    iData->iEncapContentInfo = aEncapContentInfo;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetAuthenticatedAttributesL()
// Setter for authenticatedAttributes
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetAuthenticatedAttributesL(
    CArrayPtr< CCMSAttribute >* aAuthenticatedAttributes )
	{
    if( iData->iAuthenticatedAttributes )
        {
        iData->iAuthenticatedAttributes->ResetAndDestroy();
        delete iData->iAuthenticatedAttributes;
        }
    iData->iAuthenticatedAttributes = aAuthenticatedAttributes;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetMacL
// Setter for mac
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetMacL(
    const TDesC8& aMac)
	{
    HBufC8* mac = aMac.AllocL();
    delete iData->iMac;
    iData->iMac = mac;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::SetUnauthenticatedAttributesL()
// Setter for unauthenticatedAttributes
// -----------------------------------------------------------------------------
EXPORT_C void CCMSAuthenticatedData::SetUnauthenticatedAttributesL(
    CArrayPtr< CCMSAttribute >* aUnauthenticatedAttributes )
	{
    if( iData->iUnauthenticatedAttributes )
        {
        iData->iUnauthenticatedAttributes->ResetAndDestroy();
        delete iData->iUnauthenticatedAttributes;
        }
    iData->iUnauthenticatedAttributes = aUnauthenticatedAttributes;
	}

// -----------------------------------------------------------------------------
// CCMSAuthenticatedData::DecodeAttributesL
// Decodes an array of attributes
// -----------------------------------------------------------------------------
CArrayPtrFlat< CCMSAttribute >* CCMSAuthenticatedData::DecodeAttributesL(
    TASN1DecGeneric* aAttributesDec ) // generic decoder for the sequence
    {
    TASN1DecSequence sequenceDecoder;
    CArrayPtr< TASN1DecGeneric >* attributes =
        sequenceDecoder.DecodeDERLC( *aAttributesDec );
    TInt attributeCount = attributes->Count();
    if( attributeCount <  1 )
        {
        User::Leave( KErrArgument );
        }
    CArrayPtrFlat< CCMSAttribute >* retVal =
        new( ELeave ) CArrayPtrFlat< CCMSAttribute >( attributeCount );
    CleanupStack::PushL( retVal );
    for( TInt i = 0; i < attributeCount; i++ )
        {
        CCMSAttribute* attribute = CCMSAttribute::NewLC();
        attribute->DecodeL( attributes->At( i )->Encoding() );
        retVal->AppendL( attribute );
        // attribute is left in cleanup stack, as retVal has not been pushed
        // with ResetAndDestroyPushL
        }
    CleanupStack::Pop( attributeCount ); // all attributes
    CleanupStack::Pop( retVal );
    CleanupStack::PopAndDestroy( attributes );
    return retVal;
    }

//  End of File
