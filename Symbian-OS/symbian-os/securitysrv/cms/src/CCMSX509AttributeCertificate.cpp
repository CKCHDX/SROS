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
#include    "CCMSX509AttributeCertificate.h"
#include "CCMSX509AttributeCertificateInfo.h"
#include "CCMSX509AlgorithmIdentifier.h"
#include <asn1dec.h>
#include <asn1enc.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::CCMSX509AttributeCertificate
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AttributeCertificate::CCMSX509AttributeCertificate()
    {
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509AttributeCertificate::ConstructL(
    const CCMSX509AttributeCertificateInfo& aInfo,
    const CCMSX509AlgorithmIdentifier& aAlgorithmIdentifier,
    const TDesC8& aEncrypted )
    {
    BaseConstructL( aAlgorithmIdentifier, aEncrypted );
    SetInfoL( aInfo );
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSX509AttributeCertificate::ConstructL( )
    {
    // creating empty/default values
    iInfo = CCMSX509AttributeCertificateInfo::NewL();
    iAlgorithmIdentifier = CCMSX509AlgorithmIdentifier::NewL();
    iEncrypted = KNullDesC8().AllocL();
    }
    

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AttributeCertificate*
CCMSX509AttributeCertificate::NewL()
	{
	// creating with empty values
    CCMSX509AttributeCertificate* self =
        new( ELeave ) CCMSX509AttributeCertificate();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSX509AttributeCertificate*
CCMSX509AttributeCertificate::NewL(
    const CCMSX509AttributeCertificateInfo& aInfo,
    const CCMSX509AlgorithmIdentifier& aAlgorithmIdentifier,
    const TDesC8& aEncrypted )
    {
    CCMSX509AttributeCertificate* self =
        new( ELeave ) CCMSX509AttributeCertificate();
    CleanupStack::PushL( self );
    self->ConstructL( aInfo, aAlgorithmIdentifier, aEncrypted );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CCMSX509AttributeCertificate::~CCMSX509AttributeCertificate()
    {
	delete iInfo;
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSX509AttributeCertificate::DecodeL( const TDesC8& aRawData )
	{
    CCMSX509AlgorithmIdentifier* algId = NULL;
    HBufC8* encrypted = NULL;
    TASN1DecGeneric infoDecoder =
        DecodeSignatureL( aRawData, algId, encrypted );

    CleanupStack::PushL( algId );
    CleanupStack::PushL( encrypted );
    
    // decode info
    CCMSX509AttributeCertificateInfo* info =
        CCMSX509AttributeCertificateInfo::NewL( );
    CleanupStack::PushL( info );
    info->DecodeL( infoDecoder.Encoding() );

    // all done, change state
    delete iInfo;
    iInfo = info;
    delete iAlgorithmIdentifier;
    iAlgorithmIdentifier = algId;
    delete iEncrypted;
    iEncrypted = encrypted;
    CleanupStack::Pop( 3 ); // info, encrypted, algId
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSX509AttributeCertificate::EncoderLC() const
	{
    // encode AttributeCertificateInfo
    CASN1EncBase* info = iInfo->EncoderLC( );

    // sign
    CASN1EncSequence* root = SignAndPopLC( info );

    return root;
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::Info()
// Getter for AttributeCertificateInfo
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AttributeCertificateInfo&
CCMSX509AttributeCertificate::Info() const
	{
	return *iInfo;
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::SetInfoL()
// Setter for AttributeCertificateInfo
// -----------------------------------------------------------------------------
EXPORT_C void CCMSX509AttributeCertificate::SetInfoL(
    const CCMSX509AttributeCertificateInfo& aInfo )
	{
    CCMSX509AttributeCertificateInfo* info = NULL;
    const CCMSX509IssuerSerial* baseCertificateID = aInfo.BaseCertificateID();
    if( baseCertificateID )
        {
        info = CCMSX509AttributeCertificateInfo::NewL(
            *baseCertificateID,
            aInfo.Issuer(),
            aInfo.Signature(),
            aInfo.SerialNumber(),
            aInfo.NotBeforeTime(),
            aInfo.NotAfterTime(),
            aInfo.Attributes() );
        }
    else
        {
        info = CCMSX509AttributeCertificateInfo::NewL(
            *aInfo.SubjectName(),
            aInfo.Issuer(),
            aInfo.Signature(),
            aInfo.SerialNumber(),
            aInfo.NotBeforeTime(),
            aInfo.NotAfterTime(),
            aInfo.Attributes() );
        }
    CleanupStack::PushL( info );

    info->SetVersion( aInfo.Version() );

    const TDesC8* issuerUniqueID = aInfo.IssuerUniqueID();
    if( issuerUniqueID )
        {
        info->SetIssuerUniqueIDL( *issuerUniqueID );
        }

    delete iInfo;
    iInfo = info;
    CleanupStack::Pop( info );
    
	}

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::DecodeImplicitTagL
// Decrypts raw data with implicit tag
// -----------------------------------------------------------------------------
void CCMSX509AttributeCertificate::DecodeImplicitTagL(
    const TDesC8& aRawData,
    const TTagType aImplicitTag )
    {
    CArrayPtr< TASN1DecGeneric >* items = NULL;

    // Check the tag
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    // Accept only given tag
    if( decGen.Tag() != aImplicitTag )
        {
        User::Leave( KErrArgument );
        }
    TASN1DecSequence decSeq;
    items = decSeq.DecodeDERLC( decGen );
    DecodeArrayL( items );
    CleanupStack::PopAndDestroy( items );
    }

// -----------------------------------------------------------------------------
// CCMSX509AttributeCertificate::DecodeArrayL
// Decodes data from an array of decoders
// -----------------------------------------------------------------------------
void CCMSX509AttributeCertificate::DecodeArrayL(
    CArrayPtr< TASN1DecGeneric >* aItems )
    {
    CCMSX509AlgorithmIdentifier* algId = NULL;
    HBufC8* encrypted = NULL;
    TASN1DecGeneric infoDecoder =
        DecodeSignatureArrayL( *aItems, algId, encrypted );

    CleanupStack::PushL( algId );
    CleanupStack::PushL( encrypted );
    
    // decode info
    CCMSX509AttributeCertificateInfo* info =
        CCMSX509AttributeCertificateInfo::NewL( );
    CleanupStack::PushL( info );
    info->DecodeL( infoDecoder.Encoding() );

    // all done, change state
    delete iInfo;
    iInfo = info;
    delete iAlgorithmIdentifier;
    iAlgorithmIdentifier = algId;
    delete iEncrypted;
    iEncrypted = encrypted;
    CleanupStack::Pop( 3 ); // info, encrypted, algId
    }

//  End of File
