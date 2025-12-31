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
* Description: 
*
*/


// INCLUDE FILES
#include    "CCMSCertificateChoices.h"
#include "CCMSX509Certificate.h"
#include "CCMSX509AttributeCertificate.h"
#include <asn1dec.h>
#include <asn1enc.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::CCMSCertificateChoices
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSCertificateChoices::CCMSCertificateChoices()
    {
    }

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSCertificateChoices::ConstructL(
    const CCMSX509Certificate& aCertificate )
    {
    SetCertificateL( aCertificate );
    }

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCMSCertificateChoices::ConstructL(
    const CCMSX509AttributeCertificate& aAttrCert )
    {
    SetAttrCertL( aAttrCert );
    }

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSCertificateChoices* CCMSCertificateChoices::NewL()
	{
	// creating with empty/default values
    CCMSX509Certificate* certificate = CCMSX509Certificate::NewL( );
	CleanupStack::PushL( certificate );
	CCMSCertificateChoices* self = NewL( *certificate );
	CleanupStack::PopAndDestroy( certificate );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSCertificateChoices* CCMSCertificateChoices::NewLC()
	{
	// creating with empty/default values
    CCMSX509Certificate* certificate = CCMSX509Certificate::NewL( );
	CleanupStack::PushL( certificate );
	CCMSCertificateChoices* self = NewL( *certificate );
	CleanupStack::PopAndDestroy( certificate );
    CleanupStack::PushL( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSCertificateChoices* CCMSCertificateChoices::NewL(
    const CCMSX509Certificate& aCertificate )
    {
    CCMSCertificateChoices* self = NewLC( aCertificate );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSCertificateChoices* CCMSCertificateChoices::NewLC(
    const CCMSX509Certificate& aCertificate )
    {
    CCMSCertificateChoices* self =
    	new( ELeave ) CCMSCertificateChoices();

    CleanupStack::PushL( self );
    self->ConstructL( aCertificate );

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSCertificateChoices* CCMSCertificateChoices::NewL(
    const CCMSX509AttributeCertificate& aAttrCert )
    {
    CCMSCertificateChoices* self = NewLC( aAttrCert );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCMSCertificateChoices* CCMSCertificateChoices::NewLC(
    const CCMSX509AttributeCertificate& aAttrCert )
    {
    CCMSCertificateChoices* self =
    	new( ELeave ) CCMSCertificateChoices();

    CleanupStack::PushL( self );
    self->ConstructL( aAttrCert );

    return self;
    }

// Destructor
CCMSCertificateChoices::~CCMSCertificateChoices()
    {
    delete iCertificate;
    delete iAttrCert;
    }

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::DecodeL
// Decrypts raw data to this instance
// -----------------------------------------------------------------------------
void CCMSCertificateChoices::DecodeL( const TDesC8& aRawData )
	{
    
    TASN1DecGeneric decGen( aRawData );
    decGen.InitL();
    switch( decGen.Tag() )
        {
        case EASN1Sequence:
            {
            // check class and decode certificate
            if( decGen.Class() != EUniversal )
                {
                User::Leave( KErrArgument );
                }
            CCMSX509Certificate* certificate = CCMSX509Certificate::NewL();
            CleanupStack::PushL( certificate );
            certificate->DecodeL( aRawData );
            delete iAttrCert;
            iAttrCert = NULL;
            delete iCertificate;
            iCertificate = certificate;
            CleanupStack::Pop( certificate );
            break;
            }
        case KCMSAttrCertTag:
            {
            // check class and decode attrCert
            if( decGen.Class() != EContextSpecific )
                {
                User::Leave( KErrArgument );
                }
            CCMSX509AttributeCertificate* attrCert =
                CCMSX509AttributeCertificate::NewL();
            CleanupStack::PushL( attrCert );
            attrCert->DecodeImplicitTagL( aRawData, KCMSAttrCertTag );
            delete iCertificate;
            iCertificate = NULL;
            delete iAttrCert;
            iAttrCert = attrCert;
            CleanupStack::Pop( attrCert );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }
	}

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::EncodeL
// Encrypts this instance to descriptor
// -----------------------------------------------------------------------------

void CCMSCertificateChoices::EncodeL( HBufC8*& aResult ) const
	{
	CASN1EncBase* root = EncoderLC();

	// encode the object in a DER encoding
    HBufC8* der = HBufC8::NewMaxLC( root->LengthDER() );
    TPtr8 pder( der->Des() );
    TUint pos = 0;
    root->WriteDERL( pder, pos );
    CleanupStack::Pop( der );
    aResult = der;

	CleanupStack::PopAndDestroy( root );
	}

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::EncoderLC
// Returns ASN1 encoder for this instance
// -----------------------------------------------------------------------------

CASN1EncBase* CCMSCertificateChoices::EncoderLC() const
	{
    if( iCertificate )
        {
        return iCertificate->EncoderLC();
        }
    else
        {
        CASN1EncBase* encoder = iAttrCert->EncoderLC();
        encoder->SetTag( KCMSAttrCertTag );
        return encoder;
        }
	}

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::Certificate
// certificate getter.
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509Certificate*
CCMSCertificateChoices::Certificate() const
	{
	return iCertificate;
	}

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::AttrCert
// attrCert getter.
// -----------------------------------------------------------------------------
EXPORT_C const CCMSX509AttributeCertificate*
CCMSCertificateChoices::AttrCert() const
	{
	return iAttrCert;
	}

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::SetCertificateL
// Certificate setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSCertificateChoices::SetCertificateL(
	const CCMSX509Certificate& aCertificate )
	{
    CCMSX509Certificate* certificate = CCMSX509Certificate::NewL(
        aCertificate.SerialNumber(),
        aCertificate.Signature(),
        aCertificate.Issuer(),
        aCertificate.Validity(),
        aCertificate.Subject(),
        aCertificate.SubjectPublicKeyInfo(),
        aCertificate.AlgorithmIdentifier(),
        aCertificate.Encrypted() );
    CleanupStack::PushL( certificate );
    certificate->SetVersion( aCertificate.Version() );
    const TDesC8* issuerUniqueIdentifier = aCertificate.IssuerUniqueIdentifier();
    if( issuerUniqueIdentifier )
        {
        certificate->SetIssuerUniqueIdentifierL( *issuerUniqueIdentifier );
        }
    const TDesC8* subjectUniqueIdentifier =
        aCertificate.SubjectUniqueIdentifier();
    if( subjectUniqueIdentifier )
        {
        certificate->SetSubjectUniqueIdentifierL( *subjectUniqueIdentifier );
        }
    delete iAttrCert;
    iAttrCert = NULL;
    delete iCertificate;
    iCertificate = certificate;
    CleanupStack::Pop( certificate );
	}

// -----------------------------------------------------------------------------
// CCMSCertificateChoices::SetAttrCertL
// AttrCert setter
// -----------------------------------------------------------------------------
EXPORT_C void CCMSCertificateChoices::SetAttrCertL(
	const CCMSX509AttributeCertificate& aAttrCert )
	{
    CCMSX509AttributeCertificate* attrCert = CCMSX509AttributeCertificate::NewL(
        aAttrCert.Info(),
        aAttrCert.AlgorithmIdentifier(),
        aAttrCert.Encrypted() );
    delete iCertificate;
    iCertificate = NULL;
    delete iAttrCert;
    iAttrCert = attrCert;
	}

//  End of File
