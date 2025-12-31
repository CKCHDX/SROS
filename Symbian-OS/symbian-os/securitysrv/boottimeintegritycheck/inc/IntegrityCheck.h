/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for integrity check program.
*
*/


#ifndef INTEGRITYCHECK_H
#define INTEGRITYCHECK_H

// INCLUDES 
#include    <e32base.h>
#include    <s32std.h>          // TStreamId
#include    <cctcertinfo.h>     // TCertLabel
#include    <securitydefs.h>    // TKeyIdentifier

// CONSTANTS 

// MACROS 

// Enable C:\sys\bin binaries check.
//#define __BTIC_BINARIES_CHECK_ENABLED

// Enable sis controller verify.
#define __BTIC_VERIFY_CONTROLLER 

// DATA TYPES 

// FUNCTION PROTOTYPES 

// FORWARD DECLARATIONS

class CCTCertInfo; // TCapabilitySet

// LOCAL FUNCTION PROTOTYPES 

// CLASS DECLARATION 

/**
* Class CRootCertificateEntry
*
* Class for reading root certificates from certificate store file. 
*/
NONSHARABLE_CLASS( CRootCertificateEntry ) : public CBase
    {
    public:
        
        /**
         * Symbian constructor
         *
         * @since 
         * @param aStream Certificate stream           
         */
        static CRootCertificateEntry* NewLC( RReadStream& aStream );
        
        /**
         * Destructor
         *
         * @since                   
         */        
        virtual ~CRootCertificateEntry();

        /**
         * Returns certificates data stream. This is actual 
         * x509 certificate.
         *
         * @since        
         * @return TStreamId         
         */                
        TStreamId DataStreamId() const;
        
        /**
         * Returns certificates capability set.
         *
         * @since 
         * @return TCapabilitySet        
         */        
        const TCapabilitySet& Capabilities() const;               

        /**
         * Returns certificate's size. This is actual size of
         * x509 certificate.
         *
         * @since 
         * @return TInt Size of certificate in bytes.        
         */        
        TInt Size() const;

        /**
         * Returns certificate's store ID number.
         *
         * @since 
         * @return TInt Store ID number.         
         */        
        TInt CertID() const;
        
    private:

        /**
         * C++ default constructor. 
         *
         * @since         
         */            
        CRootCertificateEntry();
 
         /**
         * Reads data from stream.
         *
         * @since 
         * @param aStream Certificate stream         
         */               
        void InternalizeL( RReadStream& aStream );

    private:
    
        TUint8          iX509CertificateType; // Defines type eg. X509
        TInt            iSize;         // Size of certificate data  
        TCertLabel      iLabel; 
        TInt            iCertId;       // Certificate's store id
        TUint8          iCACertificateType;
        TKeyIdentifier  iSubjectKeyId;
        TKeyIdentifier  iIssuerKeyId;
        RArray<TUid>    iApplications;
        TBool           iTrusted;      // Certs are always trusted
        TStreamId       iDataStreamId; // Certificate data stream
        TCapabilitySet  iCapabilities; // Certificates capability set
        TBool           iMandatory;         
    };


/**
* Class 
*
* Class for . 
*/
NONSHARABLE_CLASS( CActiveFileObserver ) : public CActive
    {
    public:

        /**
        * Destructor.
        */
        virtual ~CActiveFileObserver();

        /**
        * Two-phased constructor.       
        */    
        static CActiveFileObserver* NewL(    
            RFs&  aFs,
            const TDesC& aPath,
            TInt& aResult );

        /**
        * Function íssues request to 
        */        
        void IssueRequest();

    protected:

        /**
        * Function                
        */
        void RunL();

        /**
        * Cancel request.
        */
        void DoCancel();  
        
        /**
        * Handles a leave occurring in the request completion 
        * event handler RunL.
        */             
        TInt RunError( TInt aError );         

    private:

        /**
        * C++ default constructor.
        */
        CActiveFileObserver( RFs&  aFs, TInt& aResult );

        /**
        * 2nd phase constructor.         
        */        
        void ConstructL( const TDesC& aPath );

    private:
                       
        RFs&    iFs;
        TInt&   iResult; 
        TInt    iRunCount;   
        RTimer  iRTimer;    
        HBufC*  iPath;                                                                                  
    };


#endif //INTEGRITYCHECK_H

//EOF 
