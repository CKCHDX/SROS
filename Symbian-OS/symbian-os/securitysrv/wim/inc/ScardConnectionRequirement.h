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
* Description:  For describing required smart card reader connections.
*
*/



#ifndef CSCARDCONNECTIONREQUIREMENT_H
#define CSCARDCONNECTIONREQUIREMENT_H

//  INCLUDES
#include "ScardDefs.h"


// CLASS DECLARATION

/**
*  This class contains requirements for new connections.
*
*  @lib Scard.lib
*  @since Series60 2.1
*/
class TScardConnectionRequirement // : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TScardConnectionRequirement();

    public: // New functions
        
        /**
        * Set Explixit reader name
        * @param aExplicitName Explicit reader name
        * @return void
        */
        IMPORT_C void SetExplicitL( TScardReaderName& aExplicitName );
        
        /**
        * Set excluded reader name
        * @param aExcludedName Excluded Reader name
        * @return void
        */
        IMPORT_C void SetExcludedL( TScardReaderName& aExcludedName );
        
        /**
        * Set ATR bytes
        * @param aATR ATR bytes
        * @return void
        */
        IMPORT_C void SetATRL( TScardATR& aATR );
        
        /**
        * Set AID bytes
        * @param aAIDBytes AID bytes
        * @return void
        */
        IMPORT_C void SetAIDBytesL( TDesC8& aAIDBytes );

        /**
        * Clear excplicit reader name
        * @return void
        */
        IMPORT_C void ClearExplicitL();
        
        /**
        * Clear Excluded reader name
        * @return void
        */
        IMPORT_C void ClearExcludedL();
        
        /**
        * Clear ATR bytes
        * @return void
        */
        IMPORT_C void ClearATRL();
        
        /**
        * Clear AID bytes
        * @return void
        */
        IMPORT_C void ClearAIDBytesL();

        /**
        * Set new readers only flag
        * @param aNewOnly Flag to tell if only new readers are supported
        * @return void
        */
        IMPORT_C void NewReadersOnly( const TBool aNewOnly );
        
        /**
        * Set new card only flag
        * @param aNewOnly Flag to tell if only new cards are supported
        * @return void
        */
        IMPORT_C void NewCardsOnly( const TBool aNewOnly );

    private:
        
        /**
        * Prohibit copy constructor if not deriving from CBase.
        */
        TScardConnectionRequirement( TScardConnectionRequirement& );
        /**
        * Prohibit assigment operator if not deriving from CBase.
        */
        TScardConnectionRequirement& operator=(
            const TScardConnectionRequirement& aRequirement );

    private:    // Data
        // If we want only one singular reader. Owned.
        TScardReaderName* iExplicitReader;
        // This is the reader we do NOT want. Owned.
        TScardReaderName* iExcludedReader;
        // The reader should have a SC with these ATR bytes. Owned.
        TScardATR*      iATRBytes;
        // The reader should have a SC with an appl match these AID bytes.Owned.
        TDesC8*         iAIDBytes;
        // Count of parameters
        TInt            iParameterCount;
        // New card only
        TBool           iNewCards;
        // New readers only
        TBool           iNewReaders;

    public:     // Friend classes
        friend class CScardComm;

    };

#endif      // CSCARDCONNECTIONREQUIREMENT_H   
            
// End of File
