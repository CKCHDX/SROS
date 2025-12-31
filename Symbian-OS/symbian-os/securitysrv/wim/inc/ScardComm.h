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
* Description:  Core class for all Smart Card aware applications to use when 
*                communicating with the card.
*
*/



#ifndef CSCARDCOMM_H
#define CSCARDCOMM_H

//  INCLUDES
#include "ScardConnectionRequirement.h"


// FORWARD DECLARATIONS
class RScard;

// CLASS DECLARATION

/**
*  Core class for card communication.
*  This is the core class for all SC aware applications to use when 
*  communicating with the card. All objects of this class are 
*  connected to a reader, and the reader's name must be supplied as a 
*  parameter when constructing objects of this class
*
*  @lib Scard.lib
*  @since Series60 2.1
*/
class CScardComm : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aScard The connection to server. 
        *        The RScard object must not have any other CScardComm objects
        *        attached to it.
        * @param aRequirement Parameters to the connection
        * @param aReaderName This is set to contain the name of the 
        *        reader that was contacted when a connection is established
        * @param aStatus This status is signaled when connection has been made
        * @param aTimeOut The timeout in microseconds
        * @return Pointer to CScardComm object
        */
        IMPORT_C static CScardComm* NewL(
            RScard* aScard, 
            TScardConnectionRequirement& aRequirement,
            TScardReaderName& aReaderName,
            TRequestStatus& aStatus, 
            const TInt32 aTimeOut = 0 );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CScardComm();

    public: // New functions
        
        /**
        * Returns the RScard object contained within this object.
        * @return Pointer to RScard object
        */
        IMPORT_C RScard* Scard() const;

        /**
        * This function fetches the answer-to-reset bytes of the SC 
        * currently in the card reader. If an error occurs during the 
        * operation, the  length of the atr bytes is set to zero. Currently
        * leaves with KErrNotSupported
        * @param aATR ATR-bytes
        * @return void
        */
        IMPORT_C void GetATRL( TScardATR& aATR ) const;

        /**
        * Get proprietary parameters from the reader handler.
        * @param aTag parameter wanted (see scardbase.h)
        * @param aValue value of desired parameter
        * @param aTimeOut timeout in microseconds
        * @return void
        */
        IMPORT_C void GetCapabilitiesL( const TInt32 aTag,
                                        TDes8& aValue, 
                                        const TInt32 aTimeOut = 0 ) const;

        /**
        * This function is the basic transmission function. It takes a
        * 8-bit command data of unspecified length and transmits it to 
        * the reader handler. The reader handler will pass it on to the 
        * card, and place the response in the supplied response buffer 
        * (also of undefined length). The buffer must be sufficiently 
        * big to accomodate the response, otherwise the calling thread 
        * will be panicked.
        * @param aCommand This descriptor holds the command sent to the 
        *        card. The length of the descriptor must match the data length
        *        of the command.
        * @param aResponse The response from the SC is copied here.
        * @param aStatus Signaled when the operation is finished.
        * @param aTimeOut Timeout in microseconds.
        * @param aChannel The logical channel this command will be executed in.
        * @return void
        */
        IMPORT_C void TransmitToCard( const TDesC8& aCommand, 
                                      TDes8& aResponse,
                                      TRequestStatus& aStatus,
                                      const TInt32 aTimeOut = 0, 
                                      const TInt8 aChannel = 0) const;

        /**
        * This function will cancel any transmissions and other 
        * operations this object has currently pending within the 
        * server. If the object has an active transaction reservation, 
        * this will also be cancelled. All operations sent before this 
        * command will terminate with error code KScErrCancelled.
        * @return void
        */
        IMPORT_C void CancelTransmit() const;

        /**
        * Channel management function.
        * @param aCommand Channel operation.
        * @param aArgument
        * @param aResponseBuffer Buffer for response.
        * @param aStatus Signaled when the operation is finished.
        * @param aTimeOut Timeout in microseconds.
        * @return void
        */
        IMPORT_C void ManageChannel( const TScChannelManagement aCommand, 
                                     const TInt8 aArgument,
                                     TDes8& aResponseBuffer,
                                     TRequestStatus& aStatus,
                                     const TInt32 aTimeOut ) const;

    private:

        /**
        * C++ default constructor.
        * @param aScard Pointer to RScard object
        */
        CScardComm( RScard* aScard );


        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TScardConnectionRequirement& aRequirement,
                         TScardReaderName& aReaderName, 
                         TRequestStatus& aStatus, 
                         const TInt32 aTimeOut );

        /**
        * Disconnect from reader
        * @return void
        */
        void DisconnectFromReader() const;
    
    private:    // Data
        // Pointer to Scard object. Not owned. 
        RScard* iScard;
    };

#endif      // CSCARDCOMM_H   
            
// End of File
