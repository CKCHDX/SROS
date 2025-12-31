/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  WIM specific trusted settings information
*
*/



#ifndef CWIMCERTINFO_H
#define CWIMCERTINFO_H

//  INCLUDES

#include <e32base.h>
#include <cctcertinfo.h>
#include <ct.h>

// CLASS DECLARATION

/**
*  This class is for sending data between WimClient and WimServer.
*  The data is packed into this form with ExternalizeL and read with
*  InternalizeL methods.
*
*  @lib WimUtil.lib
*  @since Series60 3.0
*/

class TWimCertInfoPckg
    {
    public:

        /**
        * Constructor.
        */
        TWimCertInfoPckg(){};

        /**
        * Destructor.
        */
        virtual ~TWimCertInfoPckg(){};
          
        TBuf<KMaxCertLabelLength>   iCertLabel;
        TSHA1Hash                   iHash;
        TBool                       iTrusted;
        TCertificateFormat          iFormat;
        TCertificateOwnerType       iCertificateOwnerType;
        TInt                        iSize;
        TUint8                      iCDFRefs;
        TKeyIdentifier              iSubjectKeyId;
        TKeyIdentifier              iIssuerKeyId;
        TBuf<20>                    iTokenSerialNbr;
        TInt                        iIndex;
    };

// CLASS DECLARATION

/**
*  This class contains data for trust settings of certificate.
*  Class includes functions for creating DB, fetching, updating and 
*  deleting trust settings.
*
*  @lib WimUtil.lib
*  @since Series60 2.1
*/
class CWimCertInfo : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param  aCCTCertInfo  A pointer to CCTCertInfo object. This is owned
        *         and released by CWimCertInfo  
        * @param  aHash  A hash of certificate
        * @param  aTrustedUsages  Array containing oids
        * @param  aCDFRefs Location information of a certificate
        * @return Pointer to CWimCertInfo object
        */
        IMPORT_C static CWimCertInfo* NewL( CCTCertInfo* aCCTCertInfo,
                                      const TBuf8<KSHA1HashLengthBytes> aHash,
                                      RArray<HBufC*> aTrustedUsages,
                                      TUint8 aCDFRefs );
        
        /**
        * Destructor.
        */
        virtual ~CWimCertInfo();

    public: // New functions

        /**
        * Add ExtendedKeyUsage OID to list
        * @param   aExtendedKeyUsage  A pointer to ExtendedKeyUsage OID
        * @return  TInt -Error code
        */
        IMPORT_C TInt AddExtendedKeyUsage( HBufC* aExtendedKeyUsage );
        
        /**
        * Add TrustedUsage OID to list
        * @param   aTrustedUsage  Pointer to trusted usage info
        * @return  TInt -Error code
        */
        IMPORT_C TInt AddTrustedUsage( HBufC* aTrustedUsage );

        /**
        * Get hash of certificate
        * @param   aHash  Hash of certificate
        * @return  void
        */
        IMPORT_C void GetCertHash( TBuf8<KSHA1HashLengthBytes>& aHash ) const;

        /**
        * Return pointer to CCTCertInfo
        * @return Pointer to CCTCertInfo
        */
        IMPORT_C const CCTCertInfo* CctCert() const;

        /**
        * Return array of TrustedUsage pointers
        * @return  TrustedUsage buffer
        */
        IMPORT_C RArray<HBufC*> TrustedUsage() const;

        /*
        * Return location information of this certificate
        * @return TUint8 -location
        */
        IMPORT_C TUint8 CDFRefs() const;

        /*
        * Set location information of this certificate
        * @param aCDFRefs -location to be set.
        * @return void
        */
        IMPORT_C void SetCDFRefs( TUint8 aCDFRefs );

        /**
        * Return array of ExtendedKeyUsage pointers
        * @return  ExtendedKeyUsage buffer
        */
        IMPORT_C RArray<HBufC*> ExtendedKeyUsage() const;

        /**
        * Externalize CWimCertInfo data to flat buffer
        * @return pointer to buffer
        */
        IMPORT_C TWimCertInfoPckg* ExternalizeL() const;

        /**
        * Instantiates CWimCertInfo of externalized data
        * @param flat buffer created with ExternalizeL
        * @return pointer to CWimCertInfo
        */
        IMPORT_C static CWimCertInfo* InternalizeL(
                               TWimCertInfoPckg& aExternalizedInfo );
        
    private: // Constructors

        /**
        * Default constructor.
        * @param  aCCTCertInfo  A pointer to CCTCertInfo object. This is owned
        *         and released by CWimCertInfo  
        * @param  aHash  A hash of certificate
        * @param  aTrustedUsages Oids of a certificate
        * @parma  aCDFRefs Location of a certificate
        */
        CWimCertInfo( CCTCertInfo* aCCTCertInfo,
                      const TBuf8<KSHA1HashLengthBytes> aHash,
                      RArray<HBufC*> aTrustedUsages,
                      TUint8  aCDFRefs );
                    

        /**
        * 2nd phase constructor
        */
        void ConstructL();

    private:    // Data

        // Pointer to corresponding CCTCertInfo object
        // This class owns pointed object
        CCTCertInfo*                iCCTCertInfo;

        //Certificate Hash
        TBuf8<KSHA1HashLengthBytes> iCertHash;

        // Array of pointers that point to trusted usage OIDs
        // This class owns the pointed objects
       // RArray<HBufC*> iTrustedUsage;

        // Array of pointer that point to extended key usage OIDs
        // This class owns the pointed objects
        RArray<HBufC*>              iExtendedKeyUsage;

        // Array of pointers that point to trusted usage OIDs
        // This class owns the pointed objects
        RArray<HBufC*>              iTrustedUsage;
        
        //Location information of a Certificate
        TUint8                      iCDFRefs; 


    };

#endif      // CWIMCERTINFO_H
            
// End of File
