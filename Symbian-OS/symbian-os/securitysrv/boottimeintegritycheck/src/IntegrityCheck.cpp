/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of boot time integrity check program.
*
*/


// INCLUDE FILES

#include <e32cons.h>
#include <f32file.h>
#include <cctcertinfo.h>        // TCapabilitySet
#include <s32file.h> 
#include <signed.h>             // VerifySignatureL
#include <x500dn.h>             // ExactMatchL
#include <x509cert.h>           // x509
#include <swi/hashcontainer.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrypackage.h>
#include <hash.h>
#include <etelmm.h>             // For serial number size

#include "IntegrityCheck.h"
#include "InfoServerClient.h"
#include "IntegrityCheckDefs.h"
#include "IntegrityCheckClient.h"
#include "IntegrityCheckDebug.h"


// CONSTANTS

_LIT(KBTICThreadName,"BootIntegrityCheck"); 
_LIT(KBTICCertStoreZ,"z:\\resource\\swicertstore.dat");
_LIT(KBTICCertStoreCName,"updatedswicertstore.dat");
_LIT(KBTICCertStoreCPath,"c:\\resource\\swicertstore\\dat\\");
_LIT(KBTICDriveC,"c:\\");
_LIT(KBTICDriveCWildcard,"c:\\*");
_LIT(KBTICExtensionExe,"*.exe");
_LIT(KBTICSerialNumberFile,"*imei.txt");
_LIT(KBTICStarWildcard,"*\\");
/*
#ifdef __BTIC_BINARIES_CHECK_ENABLED
_LIT(KBTICSysPathC,"c:\\sys\\bin\\");
_LIT(KBTICCertStoreC,
    "c:\\resource\\swicertstore\\dat\\updatedswicertstore.dat"); 
_LIT(KBTICSysPathCWildcard,"c:\\sys\\bin\\*");
_LIT(KBTICExtensionDll,"*.dll");
_LIT(KBTICExtensionDat,"*.dat");
#endif //__BTIC_BINARIES_CHECK_ENABLED 
*/    
const TInt KBTICCapaCount = 2;  //Number of capabilities.
const TUint64 KBTIC_DRM = 64;   //0000 0000 0000 0000 0000 0000 0100 0000
const TUint64 KBTIC_TCB = 1;    //0000 0000 0000 0000 0000 0000 0000 0001
const TInt KBTICCabaWordOffset = 17; //File heder offset for capa. word @ 64bit.  
const TInt KBTICHashSize = 20; 
const TInt KBTICTryToOpen = 240;  //Try to open file. (240 x 0.5sec = 120sec)   
const TInt KBTICTryToCreate = 5; // Try to create process.
const TUint KBTICObserverTime = 1000000; // 1sec.
const TUint KBTICRunCount = 10; 
        
// MACROS

using namespace Swi;

// DATA TYPES

// FUNCTION PROTOTYPES

template <class T>
class CleanupResetAndDestroy
    {
public:
    inline static void PushL(T& aRef);
private:
    static void ResetAndDestroy(TAny *aPtr);
    };
    
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
    {
    CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));
    }

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
    {
    static_cast<T*>(aPtr)->ResetAndDestroy();
    }

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
    {
    CleanupResetAndDestroy<T>::PushL(aRef);
    }


LOCAL_C TInt ThreadStartL();

LOCAL_C TInt ReadCertificatesL( 
    RFs& aFs, 
    const TDesC& aPath,     
    RPointerArray<CX509Certificate>& aX509CertArray,
    RPointerArray<TCapabilitySet>& aCapabilityArray  );
 
LOCAL_C TBool ValidateCertStoreL(
    RFs& aFs,     
    RPointerArray<CX509Certificate>& aX509CertArray,
    RPointerArray<TCapabilitySet>& aCapaArray,
    const TDesC& aCertStoreFileName,
    const TDesC& aCertStoreFilePath ); 
    
LOCAL_C TBool ValidateProvisonerCertStoreL(
    RFs& aFs,                                           
    RPointerArray<CX509Certificate>& aX509CertArray,
    RPointerArray<TCapabilitySet>& aCapaArray,     
    const TDesC& aCertStoreFileName,
    const TDesC& aCertStoreFilePath,
    RSisRegistryEntry& aEntry );
    
LOCAL_C TBool ValidateSymbianCertStoreL(
    RFs& aFs,                                           
    RPointerArray<CX509Certificate>& aX509CertArray,
    RPointerArray<TCapabilitySet>& aCapaArray,              
    RSisRegistryEntry& aEntry,
    RPointerArray<HBufC>& aUpdaterExePaths ); 
    
LOCAL_C TBool RunUpdaterL( RFs& aFs, RPointerArray<HBufC>& aUpdaterExePaths );    
    
LOCAL_C TBool DeleteAllFilesL( RFs& aFs, const TDesC& aFolderPath );                

LOCAL_C TBool SearchFilesFromArrayL( 
    RPointerArray<HBufC>& aSourceArray,
    const TDesC& aDriveAndPath,
    RPointerArray<HBufC>& aStringArray, 
    RPointerArray<HBufC>& aFilesArray );
                  
LOCAL_C TBool SearchValidCertificateL( 
    RSisRegistryEntry& aEntry, 
    RPointerArray<CX509Certificate>& aX509CertArray,
    RPointerArray<TCapabilitySet>& aCertCapaArray,
    TUint64 aCapability );

LOCAL_C TCapability GetCapability( TUint64 aCapability );
    
LOCAL_C TBool VerifyHashL(    
    RSisRegistryEntry& aEntry, 
    RPointerArray<HBufC>& aPathArray,
    RPointerArray<HBufC8>& aHashArray );

LOCAL_C TBool CompareToRegisteryHashL( 
    RSisRegistryEntry& aEntry, 
    TDesC& aEntryFile, 
    TDesC8& aHashBuffer ); 
      
LOCAL_C TBool CheckSerialNumberL( RFs& aFs, const TDesC& aFileName ); 

LOCAL_C void CorruptFileL( RFs& aFs, const TDesC& aFileName );                              

LOCAL_C TBool CheckCapabilityWordAndCalcHashL( 
    RFs& aFs, 
    TBool aCheckCapa,
    TUint64* aCapaVector,    
    RPointerArray<HBufC>& aSourceArray,
    RPointerArray<HBufC>& aFilesArray,
    RPointerArray<HBufC8>& aHashArray,
    RArray<TUint64>& aCapaArray );
/*
#ifdef __BTIC_BINARIES_CHECK_ENABLED

LOCAL_C void RenameFileL( RFs& aFs, const TDesC& aFileName );

LOCAL_C void UniteCertArraysL(     
    RPointerArray<CX509Certificate>& aTargetCertArray,
    RPointerArray<CX509Certificate>& aSourceCertArray );

LOCAL_C void UniteCapaArraysL( 
    RPointerArray<TCapabilitySet>& aTargetCapaArray,
    RPointerArray<TCapabilitySet>& aSourceCapaArray );     
        
LOCAL_C void CheckBinariesL( 
    RFs& aFs, 
    RPointerArray<CX509Certificate>& aX509CertArray,
    RPointerArray<TCapabilitySet>& aCertCapaArray );  
 
LOCAL_C void FindFilesL( 
    RFs& aFs, 
    const TDesC& aPath,
    const TDesC& aSearchString,    
    RPointerArray<HBufC>& aFoundFilesArray );
                 
LOCAL_C TBool FindMatchFromSisPackageL( 
    RFs& aFs,    
    RPointerArray<HBufC>& aFilesArray, 
    RPointerArray<HBufC8>& aHashArray, 
    RArray<TUint64>& aCapaArray,
    RPointerArray<CX509Certificate>& aX509CertArray,
    RPointerArray<TCapabilitySet>& aCertCapaArray ); 

#endif //__BTIC_BINARIES_CHECK_ENABLED
*/     

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Main()
// Main for ARM environment.
// Returns: KErrNone if successful  
//          Othervice some system error code.
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    BTIC_TRACE_PRINT_ALLOC_CELLS("[BOOT INTECRITY CHECK] Main cell count = %d");  
    TInt err = KErrNone;

    CTrapCleanup* cleanup = CTrapCleanup::New();

    if ( !cleanup )
        {
        return KErrNoMemory;
        }

    TRAP( err, ThreadStartL() );

    BTIC_TRACE_PRINT_NUM("[BOOT INTECRITY CHECK] Main TRAP error = %d", err);
    
    delete cleanup;

    BTIC_TRACE_PRINT_ALLOC_CELLS("[BOOT INTECRITY CHECK] Main cell count = %d"); 
    return err;
    }


// -----------------------------------------------------------------------------
// ThreadStart()
// 
// This is programs main.
// -----------------------------------------------------------------------------
//
LOCAL_C TInt ThreadStartL()
    {                                                       
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] START PROGRAM");    
    __UHEAP_MARK;
    BTIC_TRACE_PRINT_ALLOC_CELLS("Start Cell count = %d");  
   
    BTIC_TRACE_PRINT("Rename thread to BootIntegrityCheck");
    // Rename thread as SecEnvInit.
    User::RenameThread( KBTICThreadName );  
      
    // This should be read with feature manager etc. but since we 
    // only support certificate database checking this is ok.
    TBool binariesCheckEnbled = EFalse;   
    
    TBool certFileFound = EFalse; 
    TInt err = KErrNone; 
    TInt pushToStack = 0; 
        
    // GET BOOT REASON   
    
    BTIC_TRACE_PRINT("Get boot reason");     
    TBool bootReasonFormat = EFalse;   
    RIntegrityCheckClient bticServer;
        
    err = bticServer.Connect();    
    BTIC_TRACE_PRINT_NUM("BTIC Server connect error = %d", err );
    
    if ( err == KErrNone )
        {            
        CleanupClosePushL( bticServer );                                
        TInt bootReason = KErrNone;     
                                               
        // Get boot reason from btic server.
        err = bticServer.GetSWBootReason( bootReason );                          
        BTIC_TRACE_PRINT_NUM("GetSWBootReason error = %d", err );
                            
        if ( err == KErrNone )
            {                                  
            BTIC_TRACE_PRINT_NUM("Boot Reason number = %d", bootReason );
            
            if ( bootReason == EBTICRestoreFactorySetDeep )
                {                                                                
                BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] Boot reason: FORMAT");  
                bootReasonFormat = ETrue;
                }                                   
            }             
        CleanupStack::PopAndDestroy(); //bticServer                                  
        }
               
    // OPEN FILE SERVER    
    BTIC_TRACE_PRINT("Open FS session");

    RFs fs;
    // Connect to file server.
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    pushToStack++;

    // Find SWICertStore.dat in C drive.
    TFindFile find( fs ); 
    TBool certStoreOk = EFalse;  
    CDir* entryList = NULL;   
    HBufC* certStoreFilePath = NULL; 
    HBufC* certStoreFileName = NULL; 
           
    BTIC_TRACE_PRINT("Find C SWI certificate store");    
    
    // Get all files from folder. 
    if ( KErrNone == fs.GetDir ( 
        KBTICCertStoreCPath, 
        KEntryAttNormal,
        ESortByName|EDirsFirst, 
        entryList ) )
        {     
        if ( entryList )
            {
            CleanupStack::PushL( entryList );

            TInt count = entryList->Count(); 

            // If file is found.            
            if ( count )
                {                                              
                BTIC_TRACE_PRINT("Store file found");
                
                // Get the highest version certstore file
                const TEntry& entry = (*entryList)[count - 1];                                
                
                // Make temp buffer for file path. 
                TBuf<KMaxFileName> path( KBTICCertStoreCPath );
                path.Append( entry.iName );
                
                certStoreFilePath = path.AllocLC(); 
                pushToStack++;                              
                                                   
                certStoreFileName = entry.iName.AllocLC(); 
                pushToStack++;                                                              
                                                
                certFileFound = ETrue; 
                
                CleanupStack::Pop(); //certStoreFilePath
                CleanupStack::Pop(); //certStoreFileName
                CleanupStack::PopAndDestroy( 1, entryList ); 
                CleanupStack::PushL( certStoreFilePath );                         
                CleanupStack::PushL( certStoreFileName );                           
                }
            else
                {
                BTIC_TRACE_PRINT("No store file present");
                CleanupStack::PopAndDestroy( 1, entryList ); 
                }                                                
            }
        }
    
    BTIC_TRACE_PRINT("Check boot reason");  
    // Check boot reason. If certificate store file is found start active file
    // observer which checks that file is deleted. If not continue checking.        
    if ( bootReasonFormat )
        {
        BTIC_TRACE_PRINT("Boot reason format");    
        if( certFileFound )
            {
            BTIC_TRACE_PRINT("Start active file observer"); 
            TInt status = KErrUnknown;
                        
            // Create active scheduler.
            CActiveScheduler* threadScheduler = new CActiveScheduler;
            CleanupStack::PushL( threadScheduler );
            pushToStack++;

            CActiveScheduler::Install( threadScheduler );

            // Create file observer object.
            CActiveFileObserver* observer =
                CActiveFileObserver::NewL( fs, *certStoreFilePath, status );
            CleanupStack::PushL( observer );
            pushToStack++;
             
            BTIC_TRACE_PRINT("Start active scheduler");            
            CActiveScheduler::Start();            
            
            BTIC_TRACE_PRINT_NUM("File observer status = %d", status );
            
            if ( status == KErrNone )
                {
                // Boot reason is fromat and certificate file is deleted.
                // Stop checking and exit.                    
                BTIC_TRACE_PRINT("Stop checking and exit");
                certFileFound = EFalse;   
                binariesCheckEnbled = EFalse;                 
                }
            }
        else
            {  
            // Boot reason is fromat and no certificate file is found.
            // Stop checking and exit.      
            BTIC_TRACE_PRINT("Stop checking and exit");
            certFileFound = EFalse;   
            binariesCheckEnbled = EFalse;        
            }    
        }
               
 
    BTIC_TRACE_PRINT_NUM("Updated store read enabled(1) = %d",certFileFound);
    BTIC_TRACE_PRINT_NUM("Binaries check enabled(0) = %d",binariesCheckEnbled);           
//FIX   
    // Create pointer array for root certificates.  
    RPointerArray<CX509Certificate> x509CertArray; 
    CleanupResetAndDestroyPushL( x509CertArray );
    pushToStack++;
    RPointerArray<TCapabilitySet> certCapaArray;
    CleanupResetAndDestroyPushL( certCapaArray );
    pushToStack++;
    TInt fileError = 0;
     
    // Read root certificates if needed.      
    if ( certFileFound || binariesCheckEnbled )
        {                                    
        // Get root certificates to x509 format.  
        fileError = 
          ReadCertificatesL( 
              fs, 
              KBTICCertStoreZ, 
              x509CertArray, 
              certCapaArray );

        // Can't read root certificates or array is empty. 
        // Let's return and try this in next boot.    
        if ( fileError || x509CertArray.Count() == 0 )
            {
            BTIC_TRACE_PRINT("ERROR Can't read root certificates ! ! !");
            CleanupStack::PopAndDestroy( pushToStack );

            User::Leave( fileError );
            }               
        }
            
    // Create temporary pointer array for C-drive updated certificates. 
    RPointerArray<CX509Certificate> tempCertArray; 
    CleanupResetAndDestroyPushL( tempCertArray );
    pushToStack++;
    RPointerArray<TCapabilitySet> tempCapaArray; 
    CleanupResetAndDestroyPushL( tempCapaArray );
    pushToStack++;
    
    // If candidate for certstore file is found read it and validate file.
    if ( certFileFound )
        {        
        BTIC_TRACE_PRINT("Read certificates from C");       
                                        
        // Read certificates from C drive and set them to x509 format         
        TRAP( err ,
            fileError = ReadCertificatesL( 
                fs, 
                *certStoreFilePath, 
                tempCertArray,                                          
                tempCapaArray ) );
                
        BTIC_TRACE_PRINT_NUM("C read file error (0) = %d", fileError );
        BTIC_TRACE_PRINT_NUM("C read Trap err (0) = %d", err );                
               
        // If ReadCertificatesL function leaves, we assume that something is
        // tampered and the file is corrupted. However if function returns
        // an error code, file is not corrupted.           
        if ( err )
            {
            fileError = err;
            TRAP_IGNORE( CorruptFileL( fs, *certStoreFilePath ) );
            }                     
                
        // If there is no error when constructing a permanent file store object
        // let's continue.                      
        if ( fileError == KErrNone )
            {
            BTIC_TRACE_PRINT("Validate cert store");
             
            // Validate the certstore in C if certs are found. 
            TRAP( err, 
                  certStoreOk = ValidateCertStoreL( 
                        fs, 
                        x509CertArray, 
                        certCapaArray, 
                        *certStoreFileName,
                        *certStoreFilePath ) );
          
            BTIC_TRACE_PRINT_NUM("Trap err (0) = %d", err );
            BTIC_TRACE_PRINT_NUM("Cert storage valid (1) = %d", certStoreOk ); 
         
            // If validation fails or function leaves we will assume
            // that database is tempered and it will be corrupted. 
            // In practice this mean that all binaries signed with 
            // certificates in C database will be corrupted.                                                                            
            if ( ! certStoreOk || err != KErrNone )
                {                                                    
                TRAP_IGNORE( CorruptFileL( fs, *certStoreFilePath ) );
                } 
                
            BTIC_TRACE_PRINT("\nStore validation complete"); 
            }                                                
        }  
/*                               
#ifdef __BTIC_BINARIES_CHECK_ENABLED
          
    // If found cert store is valid unite arrays.
    if ( certStoreOk )
        {
        UniteCertArraysL( x509CertArray, tempCertArray ); 
        // Reset tempCertArray because x509CertArray owns entrys now.
        tempCertArray.Reset();
         
        UniteCapaArraysL( certCapaArray, tempCapaArray );
        // Reset tempCapaArray because certCapaArray owns entrys now.               
        tempCapaArray.Reset();
        }

    // Check that certificates are found before checking binaries.
    if ( x509CertArray.Count() )
        {
        // Check that cert array and capa array has same amount of entrys.
        // If amount is diffrent do not check binaries. Something is wrong.
        if ( x509CertArray.Count() == certCapaArray.Count() )
            {
            // Check that binaries found in C: are istalled from a valid SIS 
            // pagaces and binary's hash is valid.        
            CheckBinariesL( fs, x509CertArray, certCapaArray );              
            
            BTIC_TRACE_PRINT("\nBinaries check complete\n");
            }                     
        }
                                  
#endif //__BTIC_BINARIES_CHECK_ENABLED
*/      
                            
    BTIC_TRACE_PRINT("\nProgram complete\n");
    // DELETE ALL ARRAYS AND ARRAY CONTENT  
    // x509CertArray, tempCertArray, certCapaArray, tempCapaArray, fs,  buffers
    CleanupStack::PopAndDestroy( pushToStack ); 
      
    BTIC_TRACE_PRINT("UHEAP MARK END");
    __UHEAP_MARKEND;
    BTIC_TRACE_PRINT_ALLOC_CELLS("End Cell count = %d");
     
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] PROGRAM END");   
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// ReadCertificatesL
// 
// This function reads certificates from certificate storage and adds them in to 
// given cert array if certificate can grant TCB or/and DRM. Also certificates 
// capability set is added to given capability array. 
// Given path must be valid one, othervice function returns error code.
//
// Returns: KErrNone if successful 
//          Othervice some system-wide error code.
// -----------------------------------------------------------------------------
//
LOCAL_C TInt ReadCertificatesL( 
    RFs& aFs,                                        // File server handle 
    const TDesC& aPath,                              // Path to storage file. 
    RPointerArray<CX509Certificate>& aX509CertArray, // Cert array.
    RPointerArray<TCapabilitySet>& aCapabilityArray )// Capability array.
    {            
    TInt err = KErrNone;      
    RFile file;
    
    err = file.Open( aFs, aPath, EFileRead );
    
    if ( err == KErrNone )
        {                             
        CleanupClosePushL( file );
      
        CPermanentFileStore* certStore = NULL;
      
        // Trap FromL if file is not in correct form it may leave.
        TRAP( err, certStore = CPermanentFileStore::FromL( file ) );    
          
        if ( certStore && err == KErrNone )
            {
            CleanupStack::Pop( &file ); // Now owned by store.
            CleanupStack::PushL( certStore );

            // Read id of cert list stream
            TStreamId streamId;
            RStoreReadStream stream;  
            stream.OpenLC( *certStore, certStore->Root() );     
            stream >> streamId; 
            CleanupStack::PopAndDestroy( &stream ); 

            // Read the certificate list
            RStoreReadStream certEntryStream;
            certEntryStream.OpenLC( *certStore, streamId );

            // Count entrys from stream.
            TInt count = certEntryStream.ReadInt32L();    
            BTIC_TRACE_PRINT_NUM("CertEntryStream count = %d", count );
                       
            HBufC8* certDataBuf = NULL;
                               
            for ( TInt index = 0 ; index < count ; index++ )
                {
                // Read data from stream.
                CRootCertificateEntry* entry = 
                    CRootCertificateEntry::NewLC( certEntryStream );
                        
                // Get certificate's capabilties from entry.
                TCapabilitySet* capaSet = 
                new( ELeave ) TCapabilitySet( entry->Capabilities() );        
                CleanupStack::PushL( capaSet );
              
                // Check that certificate can grant TCB or DRM capabiltiy.                                 
                if ( capaSet->HasCapability( GetCapability( KBTIC_TCB ) ) || 
                    capaSet->HasCapability( GetCapability( KBTIC_DRM ) ) )
                    {         
                    // Append certificates capability set to array.         
                    User::LeaveIfError( aCapabilityArray.Append( capaSet ) );

                    CleanupStack::Pop( capaSet );                 
                        
                    // Open certificate data stream. This is actual x509 certificate.    
                    RStoreReadStream stream;                  
                    stream.OpenLC( *certStore, entry->DataStreamId() );
                                                      
                    // Get x509 size.
                    TInt size = entry->Size();                                  

                    // Since we do not have so meny certs in device let's make alloc
                    // inside of the loop.                 
                    certDataBuf = HBufC8::NewMaxLC( size );

                    TPtr8 dataBufPtr = certDataBuf->Des();                       
                                
                    // Read x509 binary data to buffer.
                    stream.ReadL( dataBufPtr, size );                   

                    // Create x509 certificate object from binary data.   
                    CX509Certificate* x509Cert = 
                        CX509Certificate::NewLC( *certDataBuf ); 

                    // Append x509 certificate object to pointer array. 
                    User::LeaveIfError( aX509CertArray.Append( x509Cert ) );
                                                  
                    CleanupStack::Pop( x509Cert );
                    x509Cert = NULL;
                    CleanupStack::PopAndDestroy( certDataBuf );
                    certDataBuf = NULL; 
                    CleanupStack::PopAndDestroy( &stream );                     
                    }
                else
                    {
                    // Delete capaSet if certificate has not TCB or DRM.
                    CleanupStack::PopAndDestroy( capaSet );
                    capaSet = NULL;
                    }    
                        
                CleanupStack::PopAndDestroy( entry );
                entry = NULL;       
                }   
              
            CleanupStack::PopAndDestroy( 2 );//certStore, certEntryStream               
            }
        else
            {
            // Clean up file is FromL leaves.
            CleanupStack::PopAndDestroy( &file );
            BTIC_TRACE_PRINT("ERROR CPermanentFileStore - Can not read certs.");
            }           
        }
      
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ReadCertificatesL <---"); 
  
    return err;      
    }                


// -----------------------------------------------------------------------------
// ValidateCertStoreL()
//
// This function checks that updated SWI certstore in C drive has been installed 
// from valid SIS pacage. 
// SIS pacage must have valid certificate and exe with a TCB capabilities. 
//
// NOTE this function assumes that Certprovisioner tool has imei.txt file. 
//
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
//
LOCAL_C TBool ValidateCertStoreL( 
    RFs& aFs,                                       // File server handle
    RPointerArray<CX509Certificate>& aX509CertArray,// Certificate array
    RPointerArray<TCapabilitySet>& aCapaArray,      // Capability array
    const TDesC& aCertStoreFileName,
    const TDesC& aCertStoreFilePath  )     
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ValidateCertStoreL v2->");
    __UHEAP_MARK;
                          
    TBool storeOK = EFalse;                  
 
    RPointerArray<HBufC> updaterExePaths;
    CleanupResetAndDestroyPushL( updaterExePaths );
    
    RPointerArray<CSisRegistryPackage> sisPackages;
    CleanupResetAndDestroyPushL( sisPackages );
         
    Swi::RSisRegistrySession sisRegSession;  

    // Leave if we can not connect to sis register. Othervice we have a risk 
    // that valid files will be renamed if updated certstore is left out.             
    User::LeaveIfError( sisRegSession.Connect() );
        
    CleanupClosePushL( sisRegSession );
                
    // Find installed packages.
    sisRegSession.InstalledPackagesL( sisPackages );            
            
    Swi::RSisRegistryEntry entry;        
        
    TInt packageUID;   
        
    BTIC_TRACE_PRINT("Find matching UID");
    
    // If CertProvisioner tool has install updatedswicertstore file.
    if ( aCertStoreFileName.Compare( KBTICCertStoreCName ) == KErrNone )
        {
        BTIC_TRACE_PRINT("UID -> KBTICCertProvUID");
        packageUID = KBTICCertProvUID;
        }
    else
        {
        BTIC_TRACE_PRINT("UID -> KBTICSymbianSWIUpdaterUID");
        packageUID = KBTICSymbianSWIUpdaterUID;
        }
                
    TBool entryOpen = EFalse;       
    
    // Search correct sis pagace and open entry to it.
    for( TInt index = 0; index < sisPackages.Count(); index++)
        {  
        BTIC_TRACE_PRINT_NUM("Search entry from sis reg. index = %d", index );
             
        if ( sisPackages[index]->Uid() == TUid::Uid( packageUID ) )
            { 
            BTIC_TRACE_PRINT("Open entry");
                                                                    
            User::LeaveIfError( entry.Open( 
                sisRegSession, TUid::Uid( packageUID ) ) );
 
            CleanupClosePushL( entry );

            entryOpen = ETrue;                                                          
            break;                                       
            }             
        } 
                        
    // Validate cert storage.     
    if ( entryOpen )
        {        
        TBool controllerOk = EFalse;   

#ifdef __BTIC_VERIFY_CONTROLLER        
        // Check that sis controller is valid.       
        controllerOk = entry.VerifyControllerSignatureL( aX509CertArray );
                                     
        BTIC_TRACE_PRINT("Controller verify ENABLED"); 
        BTIC_TRACE_PRINT_NUM("Controller verify result (1) = %d", controllerOk );                                             
#else   
        // If controller verify is disabled set boolen to true.     
        controllerOk = ETrue;          
        BTIC_TRACE_PRINT("Controller verify DISABLED" );      
#endif
    
        if ( controllerOk )
            {            
            if ( packageUID == KBTICCertProvUID )
                {                   
                storeOK = ValidateProvisonerCertStoreL(
                    aFs,                                       
                    aX509CertArray,
                    aCapaArray,      
                    aCertStoreFileName,
                    aCertStoreFilePath,
                    entry );
                }
            else
                {
                storeOK = ValidateSymbianCertStoreL(
                    aFs,                                       
                    aX509CertArray,
                    aCapaArray,                                      
                    entry,
                    updaterExePaths );
                }                
            }                                                        
        }                       
                                         
    if ( entryOpen )
        {        
        CleanupStack::PopAndDestroy(); //RSisRegistryEntry               
        }
                       
    CleanupStack::PopAndDestroy(); //RSisRegistrySession    
    
    // Ok Symbian store is valid, let's run Symbian updater exe.
    // NOTE SisRegistry server etc. has file handle open, so we should
    // close SisRegistryEntry before running updater. 
    if ( storeOK && packageUID == KBTICSymbianSWIUpdaterUID )
        {              
        storeOK = RunUpdaterL( aFs, updaterExePaths );                        
        }
    
    CleanupStack::PopAndDestroy( 1, &sisPackages );    
    CleanupStack::PopAndDestroy( 1, &updaterExePaths ); 
                  
    BTIC_TRACE_PRINT("UHEAP MARK END");     
    __UHEAP_MARKEND;   
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ValidateCertStoreL v2 <-");        
    
    return storeOK;        
    }
    
    
// -----------------------------------------------------------------------------
// ValidateProvisonerCertStoreL()
//
// 
//
//
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
//
LOCAL_C TBool ValidateProvisonerCertStoreL(
    RFs& aFs,                                       // File server handle
    RPointerArray<CX509Certificate>& aX509CertArray,// Certificate array
    RPointerArray<TCapabilitySet>& aCapaArray,      // Capability array
    const TDesC& aCertStoreFileName,
    const TDesC& aCertStoreFilePath,
    RSisRegistryEntry& aEntry )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ValidateProvisonerStoreL --->");
    __UHEAP_MARK;
       
    TBool retOK = EFalse;                      
    // installedFiles array owns entry's files.
    RPointerArray<HBufC> installedFilesArray;
    RPointerArray<HBufC> foundFilesArray;  
    RPointerArray<HBufC> fileArray;  // Files that need to be cheked.
    RPointerArray<HBufC8> hashArray; // Hash for each fileArray's file.
    RArray<TUint64> capaArray;              
    RPointerArray<HBufC> stringArray;
    HBufC* string = NULL;
    
    CleanupResetAndDestroyPushL( installedFilesArray );
    CleanupResetAndDestroyPushL( foundFilesArray );
    CleanupResetAndDestroyPushL( fileArray );
    CleanupResetAndDestroyPushL( hashArray );
    CleanupResetAndDestroyPushL( stringArray );
    
    TUint64* capaVector = new( ELeave ) TUint64[KBTICCapaCount];
    CleanupArrayDeletePushL( capaVector );
    capaVector[0] = KBTIC_TCB;
                                                                                    
    // Get all installed files from this sis register entry 
    // to installedFilesArray.                                                            
    aEntry.FilesL( installedFilesArray );  
                                                                  
    // Search all exes from array.   
    if ( installedFilesArray.Count() > 0 )
        { 
        string = KBTICExtensionExe().AllocLC();
        User::LeaveIfError( stringArray.Append( string ) );
        CleanupStack::Pop();
        
        retOK = SearchFilesFromArrayL( 
            installedFilesArray,
            KBTICDriveCWildcard, 
            stringArray, 
            foundFilesArray );                                  
        } 
                                                                                                               
    // Find a exe that has TCB capapility and calculate hash over the file. 
    if ( retOK )                       
        {                                     
        retOK = CheckCapabilityWordAndCalcHashL( 
            aFs, 
            ETrue, 
            capaVector, 
            foundFilesArray,
            fileArray,
            hashArray,
            capaArray );                        
        }                           
                                                  
    // Validate certificate from this entry and check that 
    // certificate can grant the capability which is neede for the exe (TCB).     
    if ( retOK )
        {                                   
        retOK = SearchValidCertificateL( 
            aEntry, 
            aX509CertArray, 
            aCapaArray, 
            KBTIC_TCB );     
        }         
                 
    // Calculate hash over each exe file found in this entry.     
    // Let's be confident that server is not tempered.    
    if ( retOK )
        {
        retOK = EFalse;
        
        // Ok so now we should have one TCB file in fileArray and 2 other 
        // files in foundFilesArray. We do not need to calculate hash over 
        // TCB files again, so let's remove TCB files from array.              
        TInt countExe = foundFilesArray.Count();        
        TInt countTCB =  fileArray.Count();        
        TBool removeFile = EFalse;                
        
        for ( TInt i = countExe-1; i >= 0 ; i-- )
            {
            removeFile = EFalse;            
            
            for ( TInt k = countTCB-1; k >= 0; k-- )
                {  
                // If fileArray has this path remove it.                               
                if ( (fileArray[k]->Compare( *foundFilesArray[i] )) 
                    == KErrNone )
                    {
                    removeFile = ETrue;                 
                    }
                }
            
            if ( removeFile )
                {                
                foundFilesArray.Remove( i );                      
                }                          
            }
            
        // Compress array so we don not have null pointers on it.        
        foundFilesArray.Compress(); 
        
        // If all files are removed something is wrong. Return False.
        if ( foundFilesArray.Count() )
            {
            // Ok let's calculate hashes for the other exes.
            retOK = CheckCapabilityWordAndCalcHashL( 
                aFs, 
                EFalse, 
                0, 
                foundFilesArray,
                fileArray,
                hashArray,
                capaArray );                             
            }                   
        }          
           
    // Remove previous strings from array.
    stringArray.ResetAndDestroy();
    // Remove pointers from array. Let's not calc. hash again.
    foundFilesArray.Reset();                                    
     
    // Search imei.txt file from array and check that device's serial number
    // is found in the file. NOTE file name must be imei.txt. 
    if ( retOK )
        {          
        retOK = EFalse;
                                 
        string = KBTICSerialNumberFile().AllocLC();
        User::LeaveIfError( stringArray.Append( string ) );
        CleanupStack::Pop();
               
        TBool foundFile = SearchFilesFromArrayL( 
            installedFilesArray,
            KBTICDriveCWildcard, 
            stringArray, 
            foundFilesArray );               
                                   
        if ( foundFile && foundFilesArray.Count() )
            {
            retOK = CheckSerialNumberL( aFs, *foundFilesArray[0] ); 
            }                            
        }        
    
    // Calculate hash over each file (*.dat and imei.txt) and check that 
    // sisregistery has same hash (*.exe,*.dat,imei.txt).             
    if ( retOK )
        {        
        retOK = EFalse;
                        
        // Note that certstore path from sis reg. points to private 
        // directory not file's current logation. So we have to switch 
        // file paths to get correct hash.

        // Remove previous strings from array.
        stringArray.ResetAndDestroy();
                        
        string = HBufC::NewLC( 
            aCertStoreFileName.Length() + 
            sizeof( KBTICStarWildcard ) );            
        TPtr stringPtr = string->Des();  
        
        // Append certstorage name and wildcard (*\\) to string for search. 
        // String should be = "*\\filename.dat"  Let's add "\\" to string
        // so search do not accept eg. Öfilename.dat etc.
        stringPtr.Copy( KBTICStarWildcard );     
        stringPtr.Append( aCertStoreFileName );
         
        User::LeaveIfError( stringArray.Append( string ) );
        CleanupStack::Pop(); //String                
                                        
        // Search correct certstorage file from installedFilesArray.(sis reg.)                           
        TBool foundFile = SearchFilesFromArrayL( 
            installedFilesArray,
            KBTICDriveCWildcard, 
            stringArray, 
            foundFilesArray );                   
                
        if ( foundFile )
            { 
            BTIC_TRACE_PRINT("Correct CertStore file found");
                                   
            // Get original certstorage path from array. 
            HBufC* sisRegPath = foundFilesArray[foundFilesArray.Count() - 1];
                
            // Remove original path from array.     
            foundFilesArray.Remove( foundFilesArray.Count() - 1 );
            CleanupStack::PushL( sisRegPath ); //sisRegPath
            
            // Get current certstorage path.
            HBufC* currentPath = aCertStoreFilePath.AllocLC();

            // Set current path to array. So now we get correct hash. 
            foundFilesArray.Append( currentPath );
            CleanupStack::Pop(); //currentPath

            BTIC_TRACE_PRINT("Calculate HASH for files");

            // Calculate hash over files in foundFilesArray and add result to
            // filesArray and hashArray                                 
            CheckCapabilityWordAndCalcHashL( 
                aFs, 
                EFalse, 
                0, 
                foundFilesArray,
                fileArray,
                hashArray,
                capaArray );   
                
             // Ok now we have correct hash but certstore path is not 
             // correct for sis register. So we have to copy original 
             // certstorage file path from sis reg. to filesArray.
             
             // Remove currentPath entry from array.             
             fileArray.Remove( fileArray.Count() - 1 );
             
             // Delete currentPath buffer. We need to delete buffer because 
             // we call only reset for foundFilesArray. Reset do not delete 
             // buffer.  
             delete currentPath;
             
             // Append orginal path from sis reg. for hash checking. 
             fileArray.Append( sisRegPath ); 
             CleanupStack::Pop(); //sisRegPath                                                                
                                         
            // Check that sis registery entry has same hash as we have.                                                                                                                                              
            retOK = VerifyHashL( aEntry, fileArray, hashArray );   
                         
            }                                                                       
        }
    
    CleanupStack::PopAndDestroy(); //capaVector   

    CleanupStack::Pop( &stringArray );
    CleanupStack::Pop( &hashArray );
    CleanupStack::Pop( &fileArray );
    CleanupStack::Pop( &foundFilesArray );
    CleanupStack::Pop( &installedFilesArray );
                 
    // Reset only. InstalledFiles array owns buffers.
    foundFilesArray.Reset();
    fileArray.Reset(); 
    hashArray.ResetAndDestroy();                                    
    installedFilesArray.ResetAndDestroy();         
    stringArray.ResetAndDestroy(); 
    capaArray.Close();                 
    
    BTIC_TRACE_PRINT("UHEAP MARK END");     
    __UHEAP_MARKEND;
    
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ValidateProvisonerStoreL <---");
    
    return retOK;    
    }
  
            
// -----------------------------------------------------------------------------
// ValidateSymbianCertStoreL()
//
// 
//
//
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
//
LOCAL_C TBool ValidateSymbianCertStoreL(
    RFs& aFs,                                       // File server handle
    RPointerArray<CX509Certificate>& aX509CertArray,// Certificate array
    RPointerArray<TCapabilitySet>& aCapaArray,      // Capability array        
    RSisRegistryEntry& aEntry,
    RPointerArray<HBufC>& aUpdaterExePaths )
    {  
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ValidateSymbianStoreL v2 --->");   
      
    TBool retOK  = EFalse;                       
    // installedFiles array owns entry's files.
    RPointerArray<HBufC> installedFilesArray;
    RPointerArray<HBufC> foundFilesArray;  
    RPointerArray<HBufC> fileArray;  // Files that need to be cheked.
    RPointerArray<HBufC8> hashArray; // Hash for each fileArray's file.
    RArray<TUint64> capaArray;              
    RPointerArray<HBufC> stringArray;
    HBufC* string = NULL;
   
    CleanupResetAndDestroyPushL( installedFilesArray );
    CleanupResetAndDestroyPushL( foundFilesArray );
    CleanupResetAndDestroyPushL( fileArray );
    CleanupResetAndDestroyPushL( hashArray );
    CleanupResetAndDestroyPushL( stringArray );
    
    TUint64* capaVector = new( ELeave ) TUint64[KBTICCapaCount];
    CleanupArrayDeletePushL( capaVector );
    capaVector[0] = KBTIC_TCB;
                                                                                   
    // Get all installed files from this sis register entry 
    // to installedFilesArray.                                                            
    aEntry.FilesL( installedFilesArray );                                          
                     
    // Search all exes from array.   
    if ( installedFilesArray.Count() > 0 )
        { 
        string = KBTICExtensionExe().AllocLC();
        User::LeaveIfError( stringArray.Append( string ) );
        CleanupStack::Pop();
        
        retOK = SearchFilesFromArrayL( 
            installedFilesArray,
            KBTICDriveCWildcard, 
            stringArray, 
            foundFilesArray );                                  
        } 
                                                                                             
    // Find a exe that has TCB capapility and calculate hash over the file. 
    if ( retOK )                       
        {                                     
        retOK = CheckCapabilityWordAndCalcHashL( 
            aFs, 
            ETrue, 
            capaVector, 
            foundFilesArray,
            fileArray,
            hashArray,
            capaArray );                        
        }    
                                                          
    // Validate certificate from this entry and check that 
    // certificate can grant the capability which is neede for the exe (TCB).     
    if ( retOK )
        {                                   
        retOK = SearchValidCertificateL( 
            aEntry, 
            aX509CertArray, 
            aCapaArray, 
            KBTIC_TCB );     
        } 
    
    // Check that sis registery has same hash as we have.              
    if ( retOK )
        {                                                  
        retOK = VerifyHashL( aEntry, fileArray, hashArray );             
        } 
    
    // Return exe's path to caller. NOTE Updater exe can not be executed
    // before SisRegistryEntry is closed.    
    if ( retOK )
        {
        // We will assume that updater has only one exe with TCB.
        HBufC* path = fileArray[0]->AllocLC();          
        // Append path to pointer array. 
        User::LeaveIfError( aUpdaterExePaths.Append( path ) );         
        CleanupStack::Pop(); //path          
        }
   
    CleanupStack::PopAndDestroy(); //capaVector   
    
    CleanupStack::Pop( &stringArray ); 
    CleanupStack::Pop( &hashArray );
    CleanupStack::Pop( &fileArray );
    CleanupStack::Pop( &foundFilesArray );
    CleanupStack::Pop( &installedFilesArray );
                                                               
    // Reset only. InstalledFiles array owns buffers.
    foundFilesArray.Reset();
    fileArray.Reset(); 
    hashArray.ResetAndDestroy();                                    
    installedFilesArray.ResetAndDestroy();         
    stringArray.ResetAndDestroy();  
    capaArray.Close();              
        
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ValidateSymbianStoreL v2 <---"); 
                                     
    return retOK;
    }


// -----------------------------------------------------------------------------
// RunUpdaterL()
//
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
// 
LOCAL_C TBool RunUpdaterL( RFs& aFs, RPointerArray<HBufC>& aUpdaterExePaths )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] RunUpdaterL ->");
    // SIS register do not have hash values for storage file if it is provided 
    // by Symbian SWI updater exe. We have to remove current storage file 
    // and run exe again to get a verified swicertstorage in folder.
            
    TBool retOK = EFalse; 
    
    BTIC_TRACE_PRINT("Delete old store file(s)");          
    // Delete all files from dat folder. If delete fails storage file is 
    // corrupted.
    // NOTE that SisRegistry Server has some closing time so we must try this 
    // in loop.                
    for ( TInt loop = 0; loop < KBTICTryToOpen; loop++ )
        {                
        if ( DeleteAllFilesL( aFs, KBTICCertStoreCPath ) )
            {            
            // We will assume that updater has only on exe with TCB.
            HBufC* updaterExePath = aUpdaterExePaths[0];
                                                           
            RProcess process;
            
            BTIC_TRACE_PRINT("Run updater exe to get new store file"); 
            
            for ( TInt i = 0; i < KBTICTryToCreate; i++ )
                {  
                BTIC_TRACE_PRINT("Try to create Updater process");
                              
                if ( process.Create( *updaterExePath, KNullDesC ) == KErrNone )
                    {
                    // Run updater exe again.
                    process.Resume(); 
                    // Get rid of our handle. 
                    process.Close();
                                        
                    retOK = ETrue;  

                    BTIC_TRACE_PRINT("Updater process started OK"); 
                    break;                                                 
                    }                
                }
                                        
            // Break the loop if delete is succesful.    
            break;    			                                      
            }
        else
            {
            BTIC_TRACE_PRINT_NUM("Wait 0.5 sec. Attempt nr.%d", loop );
            User::After( 500000 );
            }                                                                   
        }
        
    return retOK;                                                     
    }

// -----------------------------------------------------------------------------
// DeleteAllFiles()
//
// Delete all files from given folder.
//
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
// 
LOCAL_C TBool DeleteAllFilesL( RFs& aFs, const TDesC& aFolderPath )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] DeleteAllFilesL ");
    
    TBool folderEmpty = EFalse;                      
    CDir* entryList = NULL; 
    TBuf<KMaxFileName> path;        
    
    // Get all files from folder.    
    if ( KErrNone == 
            aFs.GetDir ( 
                aFolderPath, 
                KEntryAttNormal, 
                ESortByName|EDirsFirst, 
                entryList ) )
        {
        if ( entryList )
            {
            CleanupStack::PushL( entryList );
            
            TInt delCount = KErrNone;
            TInt count = entryList->Count();            
            
            for ( TInt index = 0; index < count; index++ )
                {
                // Get file name.
                const TEntry& entry = (*entryList)[index];
                
                // Set path to file. 
                path.Copy( aFolderPath );
                path.Append( entry.iName ); 
                
                // Remove Read Only attribute.
                TInt err = aFs.SetAtt( path, 0, KEntryAttReadOnly );
                BTIC_TRACE_PRINT_NUM("Remove Read Only attrib. = %d", err );                                 
                       
                if ( KErrNone == aFs.Delete( path ) )
                    {                    
                    delCount++;
                    BTIC_TRACE_PRINT_NUM("Delete file with index = %d", index);
                    }
                }
            
            // If all files are removed.           
            if ( delCount == count )
                {
                folderEmpty = ETrue; 
                BTIC_TRACE_PRINT("All files removed");
                }

            CleanupStack::PopAndDestroy( 1, entryList );                             
            }                                     
        }
                                    
    return folderEmpty;
    }


// -----------------------------------------------------------------------------
// SearchFilesFromArrayL
// 
// This function search file names containing given string from aSourceArray and 
// add found file (path) to an aFilesArray. Search is limited to specified drive 
// and path.
// Note this function only adds source arrays pointers to files array. It dos not
// create new heap descriptor.    
// 
//
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
// 
LOCAL_C TBool SearchFilesFromArrayL( 
    RPointerArray<HBufC>& aSourceArray, // Array for searching.
    const TDesC& aDriveAndPath,         // Accepted drive or/and path. 
    RPointerArray<HBufC>& aStringArray, // String to be searched from path.
    RPointerArray<HBufC>& aFilesArray ) // Result array.
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] SearchFilesFromArrayL --->");

    TBool retVal = EFalse; 
                
    TInt count = aSourceArray.Count();
    TInt stringCount = aStringArray.Count();
    
    // This loop gos through all paths from aSourceArray.                                 
    for ( TInt index = 0; index < count; index++ )
        {                
        HBufC* filePath = aSourceArray[index];
        
        // Convert descriptor to lower case so we don't get problems with 
        // Match or Compare functions. 
        TPtr filePathPtr = filePath->Des();
        filePathPtr.LowerCase(); 
        
        // Check that file is in correct directory.
        TInt offsetA = filePath->Match( aDriveAndPath );
        
        if ( offsetA >= 0 )
            {                                                       
            // This loop gos through all strings what we are looking for.
            for ( TInt indexB = 0; indexB < stringCount; indexB++ )
                {                                
                // Search string from file path.            
                TInt offset = filePath->Match( *aStringArray[indexB] );
                                                    
                if ( offset >= 0 )
                    {            
                    User::LeaveIfError( aFilesArray.Append( filePath ) );  
                                        
                    retVal = ETrue;                                    
                    }
                }  
            }
        }                              
     
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] SearchFilesFromArrayL <---");         
        
    return retVal;                    
    }


// -----------------------------------------------------------------------------
// SearchValidCertificateL
// 
// This function validates entry's certificate against to certificate found in 
// given array. It check also that certificate has rigth to grant given 
// capability. 
//
// Note This function only validates first chain.    
// 
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
//              
LOCAL_C TBool SearchValidCertificateL( 
    RSisRegistryEntry& aEntry,                      // SIS Registery entry
    RPointerArray<CX509Certificate>& aX509CertArray,// Array for certificates
    RPointerArray<TCapabilitySet>& aCertCapaArray,  // Array for certs capability
    TUint64 aCapability )                           // Needed capabiltiy
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] SearchValidCertificate v2.1 --->");      

    RPointerArray<HBufC8> binaryCertChainArray;
    CleanupResetAndDestroyPushL( binaryCertChainArray );
    
    // Get certificates in binary format.
    aEntry.CertificateChainsL( binaryCertChainArray );
                            
    TInt chainCount = binaryCertChainArray.Count();
    BTIC_TRACE_PRINT_NUM("Entry chain count (1) = %d", chainCount );
        
    TBool retVal = EFalse;                                                                  

    RPointerArray<CX509Certificate> chainCertArray;
    CleanupResetAndDestroyPushL( chainCertArray );
    
    // Check all entry's chains    
    for ( TInt chain = 0; chain < chainCount; chain++ )
        {  
        BTIC_TRACE_PRINT_NUM("Check chain nr = %d", chain);
                                   
        TPtr8 chainData = binaryCertChainArray[chain]->Des();

        TInt pos = 0;
        TInt end = chainData.Length();                                                    

        // Get all certificates from chain.
        while ( pos < end )
            {                                   
            CX509Certificate* decoded = 
                CX509Certificate::NewLC( chainData, pos ); 
                                    
            // Add x509 certificate to pointter array. 
            User::LeaveIfError( chainCertArray.Append( decoded ) );  
                                                                     
            CleanupStack::Pop( decoded );
            }
                       
        TInt chainCertCount = chainCertArray.Count();    
        BTIC_TRACE_PRINT_NUM("Chain's certificate count = %d", chainCertCount);

        // Search certificate issued to root and verify signature against the root.
        if ( chainCertCount > 0 )
            {        
            BTIC_TRACE_PRINT("Validate last cert against root");                  
            // Last cert in chain should be issued to root.
            const CX509Certificate* x509LastChainCert = chainCertArray[chainCertCount-1];    
                
            TInt rootCertCount = aX509CertArray.Count();                   
            BTIC_TRACE_PRINT_NUM("Root cert array count = %d", rootCertCount);
            TBool rootFound = EFalse;
            TInt rootIndex = 0;         
                
            // Find matching root certificate and validate certificates.
            for ( rootIndex = 0; rootIndex < rootCertCount; rootIndex++ )
                {
                BTIC_TRACE_PRINT_NUM("Root cert index = %d", rootIndex );
                            
                const CX509Certificate* x509RootCert = aX509CertArray[rootIndex];

                // Compare entry cert's issuer to root cert's subject. 
                if ( x509LastChainCert->IssuerName().ExactMatchL(
                     x509RootCert->SubjectName() ) )
                    {
                    BTIC_TRACE_PRINT("Root found for this chain"); 
                    BTIC_TRACE_PRINT("Verify signature");                                
                    
                    // Verify signature 
                    if ( x509LastChainCert->VerifySignatureL( 
                        x509RootCert->PublicKey().KeyData() ) )
                        {                                                    
                        BTIC_TRACE_PRINT("Signature OK");                    
                        rootFound = ETrue;
                        break; // Break loop.
                        }
                    }
                }            
              
              // If root is found we have correct chain.
              if ( rootFound )
                  {              
                  // If Chain has more then one certificate, verify chain.
                  if ( chainCertCount > 1 )
                    {
                    BTIC_TRACE_PRINT("Verify certificate chain"); 
                    TBool chainOK = ETrue;
                    TInt certindex;
                    
                    for ( certindex = 0; certindex < chainCertCount-1; certindex++ )
                        {    
                        BTIC_TRACE_PRINT_NUM("Cert index = %d", certindex);
                        BTIC_TRACE_PRINT_NUM("Signature Cert index = %d", certindex+1);                    
                                        
                        const CX509Certificate* cert = chainCertArray[certindex];
                        const CX509Certificate* signatureCert = 
                            chainCertArray[certindex+1];                    
                                            
                        // Compare issuer name to subject name.
                        if ( cert->IssuerName().ExactMatchL(
                             signatureCert->SubjectName() ) )
                            {  
                            BTIC_TRACE_PRINT("Issuer match found."); 
                            BTIC_TRACE_PRINT("Verify signature against next cert");                                               
                            // Validate certificate 
                            if ( ! cert->VerifySignatureL( 
                                signatureCert->PublicKey().KeyData() ) )
                                {                                                    
                                // Chain is broken. Validation failed. 
                                BTIC_TRACE_PRINT("ERROR Chain broken, validation failed"); 
                                chainOK = EFalse;
                                break;                       
                                } 
                            }
                         else
                            {
                            // Issuer do not match. Validation failed.
                            BTIC_TRACE_PRINT("ERROR Chain broken, issuer error"); 
                            chainOK = EFalse;
                            break;
                            }   
                        }
                    
                    if ( chainOK )
                        {
                        BTIC_TRACE_PRINT("Entry's chain valid"); 
                        retVal = ETrue;
                        }
                                   
                    }
                  // Chain has only one certificate, root.  
                  else
                    {
                    BTIC_TRACE_PRINT("Entry's chain has only one certificate"); 
                    retVal = ETrue;
                    }  
                    
                  }
              else
                {
                BTIC_TRACE_PRINT("No root found for this chain");
                retVal = EFalse; 
                }

            // Check that matching root certificate has same capablitiy 
            // as aCapability parameter.
            if ( aCapability && retVal )
                {            
                TCapabilitySet* rootCapaSet = aCertCapaArray[rootIndex];  
                
                retVal = rootCapaSet->HasCapability( GetCapability( aCapability ) );
                }
            
            // Reset array for next chain.                                                             
            chainCertArray.ResetAndDestroy();  
            }
        
        // If this is a valid certificate chain, break to loop.   
        if ( retVal )
            {                
            break;
            }           
        } // FOR LOOP

    CleanupStack::PopAndDestroy( &chainCertArray );
    CleanupStack::PopAndDestroy( &binaryCertChainArray );
                                            
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] SearchValidCertificate v2.1 <---");         
    
    return retVal;
    }


// -----------------------------------------------------------------------------
// GetCapability
// 
// This function converts 64 bit capability integer to Symbian capability 
// enumeration. 
//
// Returns: ECapabilityTCB if capabilty is TCB 
//          ECapabilityDRM if capabilty is DRM
//          ECapability_Denied if capability not found 
// -----------------------------------------------------------------------------
//  
LOCAL_C TCapability GetCapability( TUint64 aCapability )
    {            
    if ( aCapability == KBTIC_TCB )
        {        
        return ECapabilityTCB;
        }
        
    else if ( aCapability == KBTIC_DRM )    
        {        
        return ECapabilityDRM;
        }
        
    else
        {
        BTIC_TRACE_PRINT("! ! ERROR GetCapability -> ECapability_Denied ! !");
        return ECapability_Denied;
        }            
    }


// -----------------------------------------------------------------------------
// VerifyHashL
// 
// This function compare file's hash to hash found in SIS Register. 
//
// Returns: ETure if all compare operations was successful.  
//          EFalse other vice
// -----------------------------------------------------------------------------
//  
LOCAL_C TBool VerifyHashL(    
    RSisRegistryEntry& aEntry, 
    RPointerArray<HBufC>& aPathArray,
    RPointerArray<HBufC8>& aHashArray )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] VerifyHashL --->");
    __UHEAP_MARK;
           
    TBool retVal = EFalse; 
    TInt hashOk = 0;                        
    TInt count = aPathArray.Count();                    
    
    for ( TInt index = 0; index < count; index++ )
        {        
        // Take first file        
        HBufC* path = aPathArray[index];
        HBufC8* hash = aHashArray[index];
                                            
        if ( CompareToRegisteryHashL( 
            aEntry, 
            *path, 
            *hash ) )
            {
            hashOk++;                    
            }                                                               
        }
        
    // If all hashes are ok return ETrue.
    if ( hashOk == count )
        {
        BTIC_TRACE_PRINT("\nAll hashes are ok\n");
        retVal = ETrue;
        }
         
     __UHEAP_MARKEND;     
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] VerifyHashL <---");
    
    return retVal;
    }


// -----------------------------------------------------------------------------
// CompareToRegisteryHashL
// 
// Note this function returns EFalse is HashL function leaves.   
//
// Returns: ETure if hash is valid.
//          EFalse if hash was not valid.
// -----------------------------------------------------------------------------
// 
LOCAL_C TBool CompareToRegisteryHashL( 
    RSisRegistryEntry& aEntry, 
    TDesC& aEntryFile, 
    TDesC8& aHashBuffer )
    {    
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CompareToRegisteryHashL --->");    
       
    TInt err = 0; 
    TBool hashOk = EFalse;    
    CHashContainer* hashContainer = NULL; 

    BTIC_TRACE_PRINT("Get hash for file: ");        
    BTIC_TRACE_PRINT_RAW( aEntryFile );     
    
    BTIC_TRACE_PRINT("RSisRegistryEntry.HashL()");                       
    // Get hashConteiner for given file. 
    TRAP( err, hashContainer = aEntry.HashL( aEntryFile ) ); 
                
    if ( err == KErrNone && hashContainer )    
        {                            
        CleanupStack::PushL( hashContainer );
        
        // Get hash data from hashConteiner.                                                                                                                                       
        TBufC8<KBTICHashSize> hashFromReg( hashContainer->Data() );
                                                
        if ( hashFromReg.Compare( aHashBuffer ) == 0 )
            {
            hashOk = ETrue;
            
            BTIC_TRACE_PRINT("Hash ok"); 
            }
                                               
        CleanupStack::PopAndDestroy(); //hashContainer       
        }
    else
        {
        // If HashL function leaves, return false.          
        hashOk = EFalse;
        BTIC_TRACE_PRINT_NUM("ERROR RSisRegistryEntry HashL = %d", err ); 
        }    
              
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CompareToRegisteryHashL <---");         
           
    return hashOk;
    }
    
    
// -----------------------------------------------------------------------------
// CheckSerialNumberL
//
// This function opens IMEI file and checks that device's serial number is found 
// in the file.
//
// Returns: ETure if serial number is found or some error with server. 
//          EFalse if serial number not found
// -----------------------------------------------------------------------------
//     
LOCAL_C TBool CheckSerialNumberL( RFs& aFs, const TDesC& aFileName )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CheckSerialNumberL --->");
    __UHEAP_MARK;      
    
    TBool matchFound = EFalse;        
           
    RInfoServerClient server;
              
    // Connect to server.    
    TInt err = server.Connect();
    
    BTIC_TRACE_PRINT_NUM("Server connect error = %d", err );
    
    if ( err == KErrNone )
        {            
        CleanupClosePushL( server );
        
        TBuf<RMobilePhone::KPhoneSerialNumberSize> deviceSerialNumber;
                            
        // Get device serial number from server.
        err = server.GetSerialNumber( deviceSerialNumber );          
        
        RFile file;  
            
        if ( err == KErrNone )
            {                        
            // Open text file.
            User::LeaveIfError( file.Open( aFs, aFileName, EFileRead ) );         
            CleanupClosePushL( file );
          
            TInt size; 
            file.Size( size );
          
            HBufC8* content = HBufC8::NewLC( size );      
            TPtr8 ptr(content->Des());

            // Read content
            User::LeaveIfError( file.Read( ptr, size ) );                                
            
            TLex8 input( ptr );
            TBool lineEnd( EFalse );
            TChar ch;

            TBuf<RMobilePhone::KPhoneSerialNumberSize> serialNumber;                
            
            while ( ! input.Eos() )
                {                         
                ch = input.Get();
                                                          
                if ( ( ch != '\n' ) && ( ch != '\r' ) )
                    {
                    // Check that we have space for next number. 
                    if ( serialNumber.Size() < serialNumber.MaxSize() )
                        {
                        serialNumber.Append( ch ); 
                        } 
                    else
                        {
                        // Buffer size is 50 and it should be enoug, if not 
                        // then something is wrong. Let's break to loop and
                        // return false.      
                        break;
                        }                                        
                   }                                  
                else
                   {
                   lineEnd = ETrue;
                   }

                if ( ( lineEnd ) || ( input.Eos() ) )
                   {                   
                   if ( deviceSerialNumber == serialNumber )
                        {
                        BTIC_TRACE_PRINT("Correct serial number found");
                        matchFound = ETrue;
                        break;
                        }                              
                   
                   serialNumber.FillZ(0);
                   serialNumber.SetLength(0);
                   lineEnd = EFalse;
                   }               
                }                
            CleanupStack::PopAndDestroy( 2 ); //file, content                   
            }                       
        CleanupStack::PopAndDestroy(); //client                                                           
        }  
                
    __UHEAP_MARKEND;                                  
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CheckSerialNumberL <---");
                         
    return matchFound;     
    }
 
           
// -----------------------------------------------------------------------------
// CorruptFileL
//
// This function corrupts certificate file so that certs can not be read.
// -----------------------------------------------------------------------------
//
LOCAL_C void CorruptFileL( RFs& aFs, const TDesC& aFileName )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ! ! CORRUPT STORE FILE ! !");           
              
    TFindFile find( aFs );
    
    TUint8 byteVector[8] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };    
    
    // Find given file in C drive.
    if ( find.FindByDir( aFileName, KBTICDriveC ) == KErrNone )
        {             
        RFile file;
        TInt err;
                     
        for ( TInt loop = 0; loop < KBTICTryToOpen; loop++ )
            {
            err = file.Open( aFs, aFileName, EFileRead | EFileWrite );
            BTIC_TRACE_PRINT_NUM("File Open error = %d", err );            
            
            // If file is open or it has ReadOnly attribute wait and try again. 
            if ( err )
                {
                BTIC_TRACE_PRINT_NUM("Wait 0.5 sec. Attempt nr.%d", loop );
                User::After( 500000 );
                
                TInt seterr = aFs.SetAtt( aFileName, 0, KEntryAttReadOnly );
                BTIC_TRACE_PRINT_NUM("Remove Read Only attrib. = %d", seterr );                    
                }
            else
                {
                break;
                }    
            }
            
        // Leave if error.
        User::LeaveIfError( err );          
        
        CleanupClosePushL( file );
           
        TInt size = 0;
        file.Size( size );

        HBufC8* fileData = HBufC8::NewLC( size );
        TPtr8 dataPtr = fileData->Des();                        

        HBufC8* newFileData = HBufC8::NewLC( size + sizeof( byteVector ) );
        TPtr8 newDataPtr = newFileData->Des();

        file.Read( dataPtr ); 
        // Copy corrupted bytes to buffer.
        newDataPtr.Copy( &byteVector[0], sizeof( byteVector ) ); 
        // Copy original file data to buffer.
        newDataPtr.Append( dataPtr );            
        // Write data at start of file.  
        file.Write( 0, *newFileData ); 

        file.Flush();  

        CleanupStack::PopAndDestroy( 3 ); 
        
        BTIC_TRACE_PRINT("File corrupted" );        
        }                        
    }


// -----------------------------------------------------------------------------
// CheckCapabilityWordAndCalcHashL
//
// This function reads capability word from file heder and compares that to given
// capability (aCapaVector). If there is more then one whanted capability the 
// first match is selected. If match is found file path is added to aFilesArray
// and hash is calculated over the binary and found capa is added to array. 
// If aCheckCapa is set as EFalse, capability word is not checked and all files 
// from aSourceArray is added to aFilesArray.
// Note aCapaArray conteins only matched capability. Not all file's capabilities.
//
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
//        
LOCAL_C TBool CheckCapabilityWordAndCalcHashL( 
    RFs& aFs,                           // File server handle
    TBool aCheckCapa,                   // Enables capability check.
    TUint64* aCapaVector,               // Vector con. capa. that are checked.
    RPointerArray<HBufC>& aSourceArray, // Files to be checked.
    RPointerArray<HBufC>& aFilesArray,  // Passed files
    RPointerArray<HBufC8>& aHashArray,  // File's hash
    RArray<TUint64>& aCapaArray )       // File's capability.
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CheckCapaWordAndCalcHashL v3.1 ->");
       
    TFindFile find( aFs );    
    
    TInt pathCount = aSourceArray.Count();    
    
    TBool retVal = EFalse;     
    TInt index;
    TInt err;    
    
    CSHA1* sha = CSHA1::NewL();
    CleanupStack::PushL( sha );
    
    RFile file; 
    
    for ( index = 0; index < pathCount; index++ )
        {
        BTIC_TRACE_PRINT_NUM("File array index = %d", index );
        
        HBufC* path = aSourceArray[index]; 
        
        BTIC_TRACE_PRINT("File path: ");
        BTIC_TRACE_PRINT_RAW( *path );        
                                                            
        err = file.Open( aFs, *path, EFileShareReadersOnly );              
      
        if ( err == KErrNone )
            {                               
            CleanupClosePushL( file );                

            TInt size = 0; 
            file.Size( size );                                                               

            // Alloc buffer for file.
            HBufC8* buffer = HBufC8::NewLC( size );

            if ( buffer )
                {
                TPtr8 bufferPtr = buffer->Des();                            

                // Read file binary to buffer.          
                err = file.Read( bufferPtr );               
                                 
                if ( err == KErrNone || buffer->Length() != 0 )
                    { 
                    TUint64 capa = 0;
                    TBool capabilityOk = EFalse;   
                     
                    // Check capability if needed.                    
                    if ( aCheckCapa )
                        {
                        // Get pointer to heap buffer.
                        const TUint8* ptr = buffer->Ptr();   
                                                     
                        // Capapility word (64bit) is found in 0x88 bytes after 
                        // file heder offset. 0x88 = 136 bytes/8=17 words@64.                                                     
                        TUint64 word = ((TUint64*)ptr)[KBTICCabaWordOffset]; 
                                                                            
                        for ( TInt iCapa = 0; iCapa < KBTICCapaCount; iCapa++ ) 
                            {
                            BTIC_TRACE_PRINT_NUM("Capa index = %d", iCapa );
                            
                            capa = aCapaVector[iCapa];
                            
                            if ( (word & capa) == capa )
                                {  
                                BTIC_TRACE_PRINT("Capability match found");                      
                                capabilityOk = ETrue;
                                break;
                                }                                     
                            }                                                              
                        }
                        
                    // If capa check is not needed calculate hash over the file. 
                    // Capability is set as zero.   
                    else
                        {
                        BTIC_TRACE_PRINT("Capability NOT REGUIRED");
                        capabilityOk = ETrue;    
                        }
                                           
                    if ( capabilityOk )
                        {              
                        BTIC_TRACE_PRINT("Append file information to array"); 
                        
                        // Add found capability to array
                        User::LeaveIfError( aCapaArray.Append( capa ) );                    
                                                                         
                        // Append pointer to files path.
                        User::LeaveIfError( aFilesArray.Append( path ) ); 
                                               
                        HBufC8* hash = HBufC8::NewLC( KBTICHashSize );                        
                        TPtr8 hashPtr = hash->Des();
                                                                                
                        sha->Reset();                        
                                                
                        // Calucalte hash over file's binary.
                        hashPtr.Copy( sha->Hash( bufferPtr ) );
                        
                        // Check that digest has correct length.
                        if ( hash->Length() != KBTICHashSize )
                            {
                            BTIC_TRACE_PRINT("\nERROR HASH SIZE"); 
                            User::Leave( KErrGeneral );
                            }                                                        
                        
                        // Add file's hash to array
                        User::LeaveIfError( aHashArray.Append( hash ) );                                                  
                        
                        CleanupStack::Pop(); // hash; 
                        
                        retVal = ETrue;                          
                        }
                    }
                else
                    {
                    BTIC_TRACE_PRINT_NUM("\nERROR File read = %d", err );            
                    User::Leave( KErrGeneral );  
                    }                                                   
                }
            else
                {
                BTIC_TRACE_PRINT("\nERROR HBufC8 buffer alloc failed");              
                User::Leave( KErrGeneral );  
                }      
                                                                                
            CleanupStack::PopAndDestroy( 2 ); // file,  fileBuffer         
            }
        else
            {
            BTIC_TRACE_PRINT_NUM("\nERROR File open = %d", err );              
            User::Leave( KErrGeneral );  
            }                                    
        } 
            
    CleanupStack::PopAndDestroy(); // sha                 
                    
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CheckCapaWordAndCalcHashL v3 <---");     
    
    return retVal;            
    }    

/*
#ifdef __BTIC_BINARIES_CHECK_ENABLED


// -----------------------------------------------------------------------------
// RenameFileL
//
// This function renames given file by replacing existing file extension to .bak
// -----------------------------------------------------------------------------
//    
LOCAL_C void RenameFileL( RFs& aFs, const TDesC& aFileName )
    { 
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ! ! ! RenameFileL --->");
       
    _LIT( KBTICFileExtensionBak,".bak" );
              
    TFindFile find( aFs );
      
    // Find given file in C drive.
    if ( find.FindByDir( aFileName, KBTICSysPathC ) == KErrNone )
        {          
        TInt length = aFileName.Length();
        HBufC* newName = HBufC::NewLC( length + sizeof( KBTICFileExtensionBak ) );
        TPtr newNamePtr = newName->Des();                        
        
        newNamePtr.Copy( aFileName ); 
        // Add ".bak" extension to file name.
        newNamePtr.Append( KBTICFileExtensionBak );                              
        
        // Rename file.
        TInt err = aFs.Rename( aFileName, *newName );
        
        // If error try ones more. 
        if ( err != KErrNone )
            {
            aFs.Rename( aFileName, *newName );
            }
        }

    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ! ! ! RenameFileL <---");        
    }
    

// -----------------------------------------------------------------------------
// UniteCertArraysL
//
// Function append source array's pointers to target array. 
// -----------------------------------------------------------------------------
//
LOCAL_C void UniteCertArraysL(     
    RPointerArray<CX509Certificate>& aTargetCertArray,
    RPointerArray<CX509Certificate>& aSourceCertArray )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] UniteCertArraysL --->");    
        
    TInt count = aSourceCertArray.Count();
        
    if ( count > 0 )
        {
        for ( TInt index = 0; index < count; index++ )
            {
            CX509Certificate* x509Cert = aSourceCertArray[index];
            // Note if append fails -> some binaries will be corrupted.          
            aTargetCertArray.Append( x509Cert );                        
            }                
        }
            
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] UniteCertArraysL <---");            
    }


// -----------------------------------------------------------------------------
// UniteCapaArraysL
//
// Function append source array's pointers to target array. 
// -----------------------------------------------------------------------------
//
LOCAL_C void UniteCapaArraysL( 
    RPointerArray<TCapabilitySet>& aTargetCapaArray,
    RPointerArray<TCapabilitySet>& aSourceCapaArray )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] UniteCapaArraysL --->");    
   
    TInt count = aSourceCapaArray.Count();            
        
    if ( count > 0 )
        {
        for ( TInt index = 0; index < count; index++ )
            {
            TCapabilitySet* capaSet = aSourceCapaArray[index];
            // Note if append fails -> some binaries will be corrupted.           
            aTargetCapaArray.Append( capaSet );                        
            }                
        }       
        
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] UniteCapaArraysL <---");    
    }


// -----------------------------------------------------------------------------
// CheckBinariesL
//
// This function checks all exe or dll binarys conteining TCB and DRM capability 
// in C:\sys\bin directory. It calculates hash over the binary and compares that 
// to one found in SIS Registery. It check also that SIS pacage's certificate 
// has rigth to grant TCB or DRM capabiltity.
// If some file in C:\sys\bin directory is not found in SIS Registery it is 
// renamed.
// 
// -----------------------------------------------------------------------------
//
LOCAL_C void CheckBinariesL( 
    RFs& aFs, 
    RPointerArray<CX509Certificate>& aX509CertArray,
    RPointerArray<TCapabilitySet>& aCertCapaArray )    
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CheckBinariesL --->");
    __UHEAP_MARK;          
    
    TInt pushToStack = 0;
    TInt err = 0;            
    
    RPointerArray<HBufC> allFilesArray;
    
    // Find all exe files from c:\sys\bin.
    FindFilesL( aFs, KBTICSysPathC, KBTICExtensionExe, allFilesArray );    
    // Find all dll files from c:\sys\bin.
    FindFilesL( aFs, KBTICSysPathC, KBTICExtensionDll, allFilesArray );
     
    TInt count = allFilesArray.Count();
    
    if ( count > 0 )
        {            
        RPointerArray<HBufC> filesArray;
        RPointerArray<HBufC8> hashArray; 
        RArray<TUint64> capaArray;  
                
        TBool foundFiles = EFalse;

        // Copy whanted capabilities to array.                   
        TUint64* capaVector = new( ELeave ) TUint64[KBTICCapaCount];
        CleanupArrayDeletePushL( capaVector );
        capaVector[0] = KBTIC_TCB;
        capaVector[1] = KBTIC_DRM;
        
        // Find files conteining whanted capability and calculate hash over file.  
        
        // NOTE NOTE currently this function addends only one capability to 
        // capaArray. This must be changed if all capabilities should be 
        // checked from certificate.        
                         
        foundFiles = CheckCapabilityWordAndCalcHashL( 
            aFs,
            ETrue, 
            capaVector,                        
            allFilesArray, 
            filesArray, 
            hashArray,
            capaArray );                             
                                                 
        CleanupStack::PopAndDestroy();  //capaVector 
                    
        if ( foundFiles && err == KErrNone )
            {
            // Find match for fileArray's files from SIS registery
            // Check that file's (binary) hash matches to SIS registery's hash 
            // Check that SIS registery cert. is valid and has file's capability.              
            FindMatchFromSisPackageL( 
                aFs,                 
                filesArray, 
                hashArray,
                capaArray, 
                aX509CertArray,
                aCertCapaArray );                           
                            
            count = filesArray.Count();
            
            BTIC_TRACE_PRINT_NUM("Files array count = %d (should be 0 )", count );
            
            // Rename all files which was not found in sis registery.
            if ( count )  
                {                               
                for ( TInt index = 0; index < count; index++ )
                    {
                    HBufC* filePath = filesArray[index];
                                        
                    RenameFileL( aFs, *filePath );                                                                 
                    }
                }                
            }    
                                           
        filesArray.Reset(); // Reset only. allFilesArray owns heap memory.                               
        hashArray.ResetAndDestroy();                                       
        }
    
    allFilesArray.ResetAndDestroy(); 
               
    CleanupStack::PopAndDestroy( pushToStack );  //searchPath    
       
    BTIC_TRACE_PRINT("UHEAP MARK END");  
    __UHEAP_MARKEND;    
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CheckBinariesL <---");  
    }


// -----------------------------------------------------------------------------
// FindFilesL
//
// This function searches one or more files that conteins given string in file 
// name. File names are added to array with complete path.  
// -----------------------------------------------------------------------------
//       
LOCAL_C void FindFilesL( 
    RFs& aFs,                               // File server handle.
    const TDesC& aPath,                     // Search directory and drive.
    const TDesC& aSearchString,             // String to be found in file name.
    RPointerArray<HBufC>& aFoundFilesArray )// Result files.
    {  
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] FindFilesL --->"); 
     
    TInt err = 0;
    TFindFile find( aFs ); 
    CDir* dirList = NULL;
    HBufC* path;
          
    TFileName searchExtension;
    searchExtension.Copy( aSearchString );
    
    // Find files from searchPath directory. Note that dirlist
    // contains only file name not path to it.                  
    err = find.FindWildByPath( searchExtension, &aPath, dirList );
                    
    TInt pathLength = aPath.Length(); 
    
    if ( err == KErrNone && dirList )
        {
        CleanupStack::PushL( dirList );
        
        // Get the count of find files.
        TInt count = dirList->Count();
        
        BTIC_TRACE_PRINT_NUM("Dir list count = %d", count ); 
                                   
        for ( TInt index = 0; index < count; index++ )
            {                                    
            path = HBufC::NewLC( pathLength + (*dirList)[index].iName.Length() );              
            TPtr ptrPath = path->Des();
            
            // Copy system path to buffer.
            ptrPath.Copy( KBTICSysPathC );
            // Append files name to path buffer.
            ptrPath.Append( (*dirList)[index].iName ); 
            
            ptrPath.LowerCase();                                   
            
            aFoundFilesArray.Append( path );
            
            CleanupStack::Pop(); //path
            }
            
        CleanupStack::Pop(); //dirList 
        delete dirList;  
        }
                       
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] FindFilesL <---");                 
    }



// -----------------------------------------------------------------------------  
// FindMatchFromSisPackageL
//
// This function search given files from SIS Registery. Function removes file 
// from aFilesArray alway when it is found in Registery. If aFilesArray contains 
// files after function return those files are not found in SIS Registery.
// 
// Returns: ETure if successful  
//          EFalse othervice
// -----------------------------------------------------------------------------
//  
LOCAL_C TBool FindMatchFromSisPackageL( 
    RFs& aFs,                           // File server handle
    RPointerArray<HBufC>& aFilesArray,  // Files to be checked
    RPointerArray<HBufC8>& aHashArray,  // File's hash
    RArray<TUint64>& aCapaArray,        // File's capability
    RPointerArray<CX509Certificate>& aX509CertArray, // Cert array
    RPointerArray<TCapabilitySet>& aCertCapaArray )  // Cert's capability
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] FindMatchFromSisPackageL --->");
    __UHEAP_MARK; 
    
    TInt err = KErrNone;        
    TBool allOK = EFalse; 
               
    Swi::RSisRegistrySession sisRegSession;  
            
    err = sisRegSession.Connect();
    
    if ( err == KErrNone )
        {
        CleanupClosePushL( sisRegSession );
        
        RPointerArray<CSisRegistryPackage> sisPackages;
                       
        // Find installed packages.
        sisRegSession.InstalledPackagesL( sisPackages );                
                      
        TBool entryOpen = EFalse;    
        TInt entryFilesCount = 0;
        TInt fileCount = 0;        
        Swi::RSisRegistryEntry entry; 
        // Array for entrys files. This owns heap memory.                       
        RPointerArray<HBufC> installedFiles; 
        // Array for dll and exe files found in sis registery.
        RPointerArray<HBufC> entryFiles;
        // Array for wild card string to be search from file list.
        RPointerArray<HBufC> stringArray;
        HBufC* string;
        
        // Add wild card strings to array.                    
        string = KBTICExtensionExe().AllocLC();
        User::LeaveIfError( stringArray.Append( string ) );
        CleanupStack::Pop();
        
        string = KBTICExtensionDll().AllocLC();
        User::LeaveIfError( stringArray.Append( string ) );
        CleanupStack::Pop();                     
                
        // Open sis registery entry and search paths to installed exes and dlls.
        for( TInt index = 0; index < sisPackages.Count(); index++)
            {  
            BTIC_TRACE_PRINT_NUM("entry loop Index = %d", index );

            // If error happens make leave. If some pacage is left out we can 
            // not continue, because all files found in C must be found also 
            // in sis registery.                                           
            User::LeaveIfError( 
                entry.OpenL( sisRegSession, *sisPackages[index] ) );                         
                                                 
            CleanupClosePushL( entry );
            entryOpen = ETrue; 
            
            // Get this entrys installed files.                                                                          
            entry.FilesL( installedFiles );                                          
                        
            if ( installedFiles.Count() > 0 )
                {                     
                // Search match for string array conten from entrys files.
                // Note that heap memory is owned by installeFiles array.
                SearchFilesFromArrayL( 
                    installedFiles, 
                    KBTICSysPathCWildcard,
                    stringArray, 
                    entryFiles );                                                                                             
                }                                                                                                                                                                              
                                                                            
            entryFilesCount = entryFiles.Count();
            
            BTIC_TRACE_PRINT_NUM("Entry files count = %d", entryFilesCount );
                    
            if ( entryFilesCount > 0 )
                {
                TBool pagaceCertValid = EFalse;                                
                
                // Go torug all file paths from entryFiles aray.
                for ( TInt indexB = 0; indexB < entryFilesCount; indexB++ )
                    {
                    BTIC_TRACE_PRINT_NUM("file path loop IndexB = %d", indexB );
                    
                    // Get file's path from sis pagace entry.
                    HBufC* entryFile = entryFiles[indexB];                                         
                    
                    fileCount = aFilesArray.Count();
                                                                     
                    // Compare registery file path to files found in C:\sys\bin.            
                    for ( TInt indexC = fileCount - 1; indexC >= 0; indexC-- )
                        {                                                
                        // Get file's path from array conteining dlls and exes.
                        HBufC* file = aFilesArray[indexC];                                                                                                
                        
                        TInt result = entryFile->CompareC( *file );
                                               
                        BTIC_TRACE_PRINT_NUM("\nCompare result = %d", result );                        
                        
                        // If result is zero match is found.
                        if ( result == 0 )                        
                            {
                            BTIC_TRACE_PRINT("File path Match found" ); 
                            
                            // Get file's hash
                            HBufC8* hash = aHashArray[indexC];                                                                                                                                                                       
                                                    
                            BTIC_TRACE_PRINT("CompareToRegisteryHashL -->" );                            
                           
                            TBool hashOk = CompareToRegisteryHashL( 
                                entry, 
                                *entryFile, 
                                *hash );
                                
                            BTIC_TRACE_PRINT("CompareToRegisteryHashL <--" );     
                            
                                                      
                            if ( hashOk )
                                {
                                BTIC_TRACE_PRINT("HASH OK");
                                
                                // Check if entry cert is already valid.
                                if ( ! pagaceCertValid )
                                    {                                          
                                    BTIC_TRACE_PRINT("Validate pagace cert");
                                     
                                    // Validate entry's certificate and 
                                    // check that certificate has correct 
                                    // capablities.                                                                                                                  
                                    TBool certOk = SearchValidCertificateL( 
                                        entry, 
                                        aX509CertArray,                                             
                                        aCertCapaArray,
                                        aCapaArray[indexC] );
                                    
                                    if ( certOk )
                                        {
                                        BTIC_TRACE_PRINT("Pagace CERT VALID - OK");
                                        
                                        // Validate pagace cert. only once.
                                        pagaceCertValid = ETrue;
                                        }
                                    else
                                        {
                                        // Rename file if cert is not ok.
                                        
                                        RenameFileL( aFs, *entryFile );                                            
                                        // Should we rename all files in this
                                        // entry or only this one.
                                        }                                            
                                    }                                                                   
                                } 
                            else
                                {
                                BTIC_TRACE_PRINT("HASH Error -> Rename file");
                                // If hash is false rename file.
                                RenameFileL( aFs, *entryFile );
                                }      
                                                                                                                                                                                                                     
                            // Set arrays entry to zero so we will not 
                            // compear it twice. We do not have to delete 
                            // entry because aFilesArray do not own heap 
                            // memory.                                
                            aFilesArray.Remove( indexC );
                            
                            //Remove also hash and capa entry from array.
                            HBufC8* hashBuffer = aHashArray[indexC];
                            delete hashBuffer;
                            aHashArray.Remove( indexC);
                            
                            aCapaArray.Remove( indexC );                                                        
                                                                                                                                                         
                            break;                  
                            }                                                                                                         
                        }
                    }
                } 
                
            if ( entryOpen )
                {    
                BTIC_TRACE_PRINT("Clear ENTRY and installedFiles array");
                    
                CleanupStack::PopAndDestroy(); //entry                   
                entryFiles.Reset(); //Reset only.
                installedFiles.ResetAndDestroy();               
                }
                
           // Break entry loop if all files are removed.     
           if ( aFilesArray.Count() == 0 )
                {
                BTIC_TRACE_PRINT("NO MORE EXE/DLL FILES -> BREAK LOOP");
                break;
                }                                                                                                                   
            }                       
        
        stringArray.ResetAndDestroy();
        CleanupStack::PopAndDestroy(); // sisRegSession 
        
        allOK = ETrue;    
        }
     
     
    BTIC_TRACE_PRINT("UHEAP MARK END");  
    __UHEAP_MARKEND;          
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] FindMatchFromSisPackageL <---"); 
    
    return allOK;
    }


#endif //__BTIC_BINARIES_CHECK_ENABLED
*/
      
// =========================== CRootCertificateEntry ===========================

// -----------------------------------------------------------------------------
// NewLC()
// 
// -----------------------------------------------------------------------------
//
CRootCertificateEntry* CRootCertificateEntry::NewLC( RReadStream& aStream )
    {
    CRootCertificateEntry* self = new(ELeave) CRootCertificateEntry();
    CleanupStack::PushL(self);

    self->InternalizeL( aStream );

    return self;
    }


// -----------------------------------------------------------------------------
// CRootCertificateEntry()
// 
// -----------------------------------------------------------------------------
//
CRootCertificateEntry::CRootCertificateEntry()
    { 
    }


// -----------------------------------------------------------------------------
// ~CRootCertificateEntry()
// 
// -----------------------------------------------------------------------------
//
CRootCertificateEntry::~CRootCertificateEntry()
    {
    iApplications.Close();    
    }


// -----------------------------------------------------------------------------
// InternalizeL()
// 
// -----------------------------------------------------------------------------
//
void CRootCertificateEntry::InternalizeL( RReadStream& aStream )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] InternalizeL");

    iX509CertificateType = aStream.ReadUint8L();

    iSize = aStream.ReadInt32L();

    aStream >> iLabel;

    iCertId = aStream.ReadInt32L();

    iCACertificateType = aStream.ReadUint8L();

    aStream >> iSubjectKeyId;

    aStream >> iIssuerKeyId; 

    TInt count = aStream.ReadInt32L();

    for ( TInt i = 0 ; i < count ; ++i )
        {
        TUid id;
        aStream >> id;
        User::LeaveIfError( iApplications.Append( id ) );
        }

    iTrusted = !!aStream.ReadUint8L();  // converts TUint8 to TBool

    aStream >> iDataStreamId;

    TPckg<TCapabilitySet> capsPckg( iCapabilities );  
    aStream >> capsPckg;  

    iMandatory = !!aStream.ReadUint8L();    
    }

     
// -----------------------------------------------------------------------------
// DataStreamId()
// 
// -----------------------------------------------------------------------------
//
TStreamId CRootCertificateEntry::DataStreamId() const
    {
    return iDataStreamId;
    }


// -----------------------------------------------------------------------------
// Capabilities()
// 
// -----------------------------------------------------------------------------
//
const TCapabilitySet& CRootCertificateEntry::Capabilities() const
    {
    return iCapabilities;
    }

// -----------------------------------------------------------------------------
// Size()
// 
// -----------------------------------------------------------------------------
//
TInt CRootCertificateEntry::Size() const
    {
    return iSize;
    }

  
// -----------------------------------------------------------------------------
// CertID()
// 
// -----------------------------------------------------------------------------
//
TInt CRootCertificateEntry::CertID() const
    {
    return iCertId;
    }



// =========================== CActiveFileObserver ============================

// -----------------------------------------------------------------------------
// CActiveFileObserver::CActiveFileObserver()
// -----------------------------------------------------------------------------
//
CActiveFileObserver::CActiveFileObserver( RFs&  aFs, TInt& aResult) 
    : CActive( EPriorityNormal ),
    iFs( aFs ), 
    iResult( aResult )    
    {
    }


// -----------------------------------------------------------------------------
// CActiveFileObserver::~CActiveFileObserver()
// Destructor
// -----------------------------------------------------------------------------
//
CActiveFileObserver::~CActiveFileObserver()
    {  
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] ~CActiveFileObserver");   
    // Cancel the outstanding request. This calls DoCancel().
    Cancel();  
    // Close timer.    
    iRTimer.Close();
    
    if ( iPath )
        {
        delete iPath;
        iPath = NULL;
        }        
    }


// -----------------------------------------------------------------------------
// CActiveFileObserver* CActiveFileObserver::NewL()
// Factory method.
// -----------------------------------------------------------------------------
//
CActiveFileObserver* CActiveFileObserver::NewL( 
    RFs&  aFs,
    const TDesC& aPath,      
    TInt& aResult )
    {
    CActiveFileObserver* observer = 
        new(ELeave) CActiveFileObserver( aFs, aResult );
        
    CleanupStack::PushL( observer );

    observer->ConstructL( aPath );

    CleanupStack::Pop();

    return observer;
    }


// -----------------------------------------------------------------------------
// CActiveFileObserver::ConstructL()
// Second-phase constructor.
// -----------------------------------------------------------------------------
//
void CActiveFileObserver::ConstructL( const TDesC& aPath )
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CActiveFileObserver::ConstructL"); 
    
    iRunCount = 0;
       
    iPath = HBufC::NewL( aPath.Length() );
    TPtr iPathPtr = iPath->Des();
    iPathPtr.Copy( aPath );
    
    iRTimer.CreateLocal();
    
    // Add to scheduler.    
    CActiveScheduler::Add( this );
    
    // Issue first timer request.
    IssueRequest();
    }


// -----------------------------------------------------------------------------
// CActiveFileObserver::DoCancel()
// Cancel function for active object.
// -----------------------------------------------------------------------------
//
void CActiveFileObserver::DoCancel()
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CActiveFileObserver::DoCancel"); 
    // Cancel outstanding request.    
    iRTimer.Cancel();
    }


// -----------------------------------------------------------------------------
// CActiveFileObserver::RunError()
// Handles a leave occurring in the request completion event handler RunL.
// -----------------------------------------------------------------------------
//
TInt CActiveFileObserver::RunError( TInt aError )
    { 
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CActiveFileObserver::RunError"); 
    iResult = aError;             
    BTIC_TRACE_PRINT_NUM("[BOOT INTE.] RunError: err = %d", aError );
    return aError;
    }


// -----------------------------------------------------------------------------
// CActiveFileObserver::IssueRequest()
// This function issues new 
// -----------------------------------------------------------------------------
void CActiveFileObserver::IssueRequest()
    {     
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CActiveFileObserver::IssueRequest");            
    //iFs.NotifyChange( ENotifyEntry, iStatus, *iPath );
    iRunCount++;

    iRTimer.After( iStatus, KBTICObserverTime );
       
    // Wait request to complete.
    SetActive();
    }


// -----------------------------------------------------------------------------
// CActiveFileObserver::RunL()
// This function handles active object’s request completion event
// -----------------------------------------------------------------------------
//
void CActiveFileObserver::RunL()
    {
    BTIC_TRACE_PRINT("[BOOT INTECRITY CHECK] CActiveFileObserver::RunL"); 
                                      
    TInt err = KErrNone; 
    RFile file;
    // Try to open the file.
    err = file.Open( iFs, *iPath, EFileShareReadersOrWriters | EFileRead );               
    
    BTIC_TRACE_PRINT_NUM("[BOOT INTE.] RunL: file open err = %d", err );
    
    if ( err == KErrNone )
        {
        // Close the file.
        file.Close();
        
        if ( iRunCount < KBTICRunCount )        
            {
            // Make new request.
            IssueRequest();      
            }
        else
            {
            BTIC_TRACE_PRINT("[BOOT INTE.] RunL: File not deleted -> CANCEL"); 
            // File is not deleted. Stop active observer 
            // and continue data base checking.
            iResult = KErrCancel;
            Deque();
            CActiveScheduler::Stop();             
            }    
        }        
    else if ( err == KErrNotFound || err == KErrPathNotFound )
        {
        // Ok file is deleted. Stop active observer and return.
        BTIC_TRACE_PRINT("[BOOT INTE.] RunL: File deleted -> OK");         
        iResult = KErrNone;   
        Deque();
        CActiveScheduler::Stop();       
        } 
    else
        {
        // Some other file open error etc. KErrInUse. Let's continue.
                         
        if ( iRunCount < KBTICRunCount )        
            {
            // Make new request.
            IssueRequest();      
            }
        else
            {
            BTIC_TRACE_PRINT("[BOOT INTE.] RunL: File not deleted -> CANCEL"); 
            // File is not deleted. Stop active observer 
            // and continue data base checking.
            iResult = KErrCancel;
            Deque();
            CActiveScheduler::Stop();             
            }            
        }                                               
    }

//EOF
