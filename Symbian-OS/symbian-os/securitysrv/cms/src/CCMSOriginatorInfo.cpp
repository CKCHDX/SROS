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
#include    "CCMSOriginatorInfo.h"
#include "TCMSTimeUtil.h"
#include "CCMSCertificateChoices.h"
#include "CCMSX509CertificateList.h"
#include <asn1dec.h>
#include <asn1enc.h>

// CONSTANTS
const TInt KMinNumberOfSubModules = 0;
const TInt KMaxNumberOfSubModules = 2;
const TTagType KCertsTag = 0;
const TTagType KCrlsTag = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::CCMSOriginatorInfo
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorInfo::CCMSOriginatorInfo( )
    {
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSOriginatorInfo*
CCMSOriginatorInfo::NewL()
	{
	// creating with empty values
    CCMSOriginatorInfo* self =
        new( ELeave ) CCMSOriginatorInfo();
	return self;
	}

// Destructor
CCMSOriginatorInfo::~CCMSOriginatorInfo()
    {
    if( iCerts )
        {
        iCerts->ResetAndDestroy();
        delete iCerts;
        }
    if( iCrls )
        {
        iCrls->ResetAndDestroy();
        delete iCrls;
        }
    }

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSOriginatorInfo::DecodeL( const TDesC8& aRawData )
	{
    CArrayPtr< TASN1DecGeneric >* itemList = DecodeSequenceLC(
        aRawData, KMinNumberOfSubModules, KMaxNumberOfSubModules );

    DecodeArrayL( itemList );

    CleanupStack::PopAndDestroy( itemList );
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSOriginatorInfo::EncoderLC() const
	{
    CASN1EncSequence* root = CASN1EncSequence::NewLC();

    if( iCerts )
        {
        // encode certs [0] IMPLICIT CertificateSet OPTIONAL
        CASN1EncSequence* certs = CASN1EncSequence::NewLC();
        TInt certCount = iCerts->Count();
        for( TInt i = 0; i < certCount; i++ )
            {
            CASN1EncBase* certEncoder = iCerts->At( i )->EncoderLC();
            certs->AddAndPopChildL( certEncoder );
            }
        certs->SetTag( KCertsTag );
        root->AddAndPopChildL( certs );
        }

    if( iCrls )
        {
        // encode crls [1] IMPLICIT CertificateRevocationLists OPTIONAL
        CASN1EncSequence* crls = CASN1EncSequence::NewLC();
        TInt crlCount = iCrls->Count();
        for( TInt i = 0; i < crlCount; i++ )
            {
            CASN1EncBase* crlEncoder = iCrls->At( i )->EncoderLC();
            crls->AddAndPopChildL( crlEncoder );
            }
        crls->SetTag( KCrlsTag );
        root->AddAndPopChildL( crls );
        }
    
    return root;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::Certs()
// Getter for certs
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSCertificateChoices >*
CCMSOriginatorInfo::Certs() const
	{
	return iCerts;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::Crls()
// Getter for crls
// -----------------------------------------------------------------------------
EXPORT_C const CArrayPtr< CCMSX509CertificateList >*
CCMSOriginatorInfo::Crls() const
	{
	return iCrls;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::SetCertsL()
// Setter for certs
// -----------------------------------------------------------------------------
EXPORT_C void CCMSOriginatorInfo::SetCerts(
    CArrayPtr< CCMSCertificateChoices >* aCerts )
	{
    delete iCerts;
    iCerts = aCerts;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::SetCrlsL()
// Setter for Crls
// -----------------------------------------------------------------------------
EXPORT_C void CCMSOriginatorInfo::SetCrls(
    CArrayPtr< CCMSX509CertificateList >* aCrls )
	{
    delete iCrls;
    iCrls = aCrls;
	}

// -----------------------------------------------------------------------------
// CCMSOriginatorInfo::DecodeImplicitTagL
// Decrypts raw data with implicit tag
// -----------------------------------------------------------------------------
void CCMSOriginatorInfo::DecodeImplicitTagL(
    const TDesC8& aRawData,
    const TTagType aImplicitTag )
    {
    // Check the tag
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    // Accept only given tag
    if( decGen.Tag() != aImplicitTag )
        {
        User::Leave( KErrArgument );
        }
    TASN1DecSequence decSeq;
    CArrayPtr< TASN1DecGeneric >* items = decSeq.DecodeDERLC( decGen );
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
// CCMSOriginatorInfo::DecodeArrayL
// Decodes data from an array of decoders
// -----------------------------------------------------------------------------
void CCMSOriginatorInfo::DecodeArrayL(
    CArrayPtr< TASN1DecGeneric >* aItems )
    {
    TInt itemCount = aItems->Count();
    
    TInt sequenceCounter = 0;

    TASN1DecSequence decSeq;
    
    CArrayPtr< CCMSCertificateChoices >* certs = NULL;
    TInt certCount = 0;
    if( sequenceCounter < itemCount )
        {
        TASN1DecGeneric* certsGenericDecoder = aItems->At( sequenceCounter );
        if( certsGenericDecoder->Tag() == KCertsTag )
            {
            CArrayPtr< TASN1DecGeneric >* certList =
                decSeq.DecodeDERLC( *certsGenericDecoder );

            certCount = certList->Count();
            
            certs = new( ELeave ) CArrayPtrFlat< CCMSCertificateChoices >(
                certCount );

            CleanupStack::PushL( certs );
            
            for( TInt i = 0; i < certCount; i++ )
                {
                CCMSCertificateChoices* choices = CCMSCertificateChoices::NewL();
                CleanupStack::PushL( choices );
                choices->DecodeL( certList->At( i )->Encoding() );
                certs->AppendL( choices );
                }
                
            sequenceCounter++;
            }
        }

    CArrayPtr< CCMSX509CertificateList >* crls = NULL;
    TInt crlCount = 0;
    if( sequenceCounter < itemCount )
        {
        TASN1DecGeneric* crlsGenericDecoder = aItems->At( sequenceCounter );
        if( crlsGenericDecoder->Tag() != KCrlsTag )
            {
            User::Leave( KErrArgument );
            }
        CArrayPtr< TASN1DecGeneric >* crlList =
            decSeq.DecodeDERLC( *crlsGenericDecoder );

        crlCount = crlList->Count();

        crls = new( ELeave ) CArrayPtrFlat< CCMSX509CertificateList >(
            crlCount );

        CleanupStack::PushL( crls );

        for( TInt i = 0; i < crlCount; i++ )
            {
            CCMSX509CertificateList* cList = CCMSX509CertificateList::NewLC();
            cList->DecodeL( crlList->At( i )->Encoding() );
            crls->AppendL( cList );
            }
        }

    if( iCerts )
        {
        iCerts->ResetAndDestroy();
        delete iCerts;
        }
    iCerts = certs;
    if( iCrls )
        {
        iCrls->ResetAndDestroy();
        delete iCrls;
        }
    iCrls = crls;

    if( crls )
        {
        CleanupStack::Pop( crlCount );
        CleanupStack::Pop( crls );
        CleanupStack::PopAndDestroy( ); // crlList
        }
    if( certs )
        {
        CleanupStack::Pop( certCount );
        CleanupStack::Pop( certs );
        CleanupStack::PopAndDestroy( ); // certList
        }
    
    }


//  End of File
