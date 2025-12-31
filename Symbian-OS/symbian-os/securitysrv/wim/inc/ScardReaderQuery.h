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
* Description:  This class returns all supported readers and reader groups.
*
*/



#ifndef CSCARDREADERQUERY_H
#define CSCARDREADERQUERY_H

//  INCLUDES
#include "ScardDefs.h"


// CLASS DECLARATION

/**
*  Utility class to make queries for readers and reader groups.
*  This class is intended for use by all Smart card aware
*  applications that need to query the resources available on
*  the framework.
*
*  @lib Scard.lib
*  @since Series60 2.1
*/
class CScardReaderQuery : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CScardReaderQuery* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CScardReaderQuery();

    public: // New functions
        
        /**
        * Create a list of all the readers belonging to the group.
        * @param aReaderList Dynamic array that will be filled with the 
        * names of the readers that belong to the group. The array must 
        * be already allocated and it must be empty. There is only one 
        * reader in Series 60. If given group is not what expected, leaves
        * with KScErrNotSupported
        * @param aGroupName Name of the reader group to be queried.
        */
        IMPORT_C void ListReadersL(
            CArrayFixFlat<TScardReaderName>* aReaderList,
            const TScardReaderName& aGroupName );

        /**
        * List all groups currently installed to the framework.
        * @param aGroupList dynamic array that will be filled with the 
        *        names of the groups. The array must be already allocated and
        *        it must be empty.
        */
        IMPORT_C void ListGroupsL(
            CArrayFixFlat<TScardReaderName>* aGroupList );


    private:

        /**
        * C++ default constructor.
        */
        CScardReaderQuery();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
    
    };

#endif      // CSCARDREADERQUERY_H   
            
// End of File
