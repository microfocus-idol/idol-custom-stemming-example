/*----------------------------------------------------------------------------
    Types used in custom stemming DLL interface.

    (c) 2018 Micro Focus.
----------------------------------------------------------------------------*/

#ifndef _AUTN_CONCEPT_CUSTOM_STEMMING_H_
#define _AUTN_CONCEPT_CUSTOM_STEMMING_H_

#include "stemming_types.h"

#define CUSTOMSTEMMING_API_VERSION 1

#define CUSTOMSTEMMING_INITIALIZE_FUNCTION   customStemmingInitialize
#define CUSTOMSTEMMING_UNINITIALIZE_FUNCTION customStemmingUninitialize
#define CUSTOMSTEMMING_STEMTERM_FUNCTION     customStemmingStemTerm
#define CUSTOMSTEMMING_FREEBUFFER_FUNCTION   customStemmingFreeBuffer

/*! Arguments to custom stemming initialization */
typedef struct s_customStemmingArgs
{
    int         nVersion;           //!< Version of the custom stemming ABI used by the calling application
    const char* szLangDirectory;    //!< Location of resource files
    const char* szLanguage;         //!< ISO 639 3-letter language code
} t_customStemmingArgs;

/*! Information struct to fill in on initialisation */
typedef struct s_customStemmingInfo
{
    void*       pContext;           //!< May be set by the DLL to hold context data to be passed in on each subsequent call
    int         nVersion;           //!< Version of the custom stemming ABI used by the DLL (i.e. CUSTOMSTEMMING_API_VERSION)
    int         nMaxStems;          //!< Maximum number of stems that may be returned (0 indicates unknown at init time)
} t_customStemmingInfo;

/*! Return value from custom stemming calls to indicate success/failure */
typedef enum
{
    CUSTOMSTEMMING_ERROR_SUCCESS = 0,       //!< Call succeeded
    CUSTOMSTEMMING_ERROR_FAILURE = -1,      //!< Call failed, no more specific error code applied
    CUSTOMSTEMMING_ERROR_MEMORY  = -2,      //!< Unable to allocate memory
    CUSTOMSTEMMING_ERROR_UNINITIALIZED = -3,//!< Library has not been successfully initialized
    CUSTOMSTEMMING_ERROR_BADAPIVERSION = -4,//!< This version of the API not supported
    CUSTOMSTEMMING_ERROR_BADPARAMETER = -5, //!< Required parameter not supplied or in invalid state
    CUSTOMSTEMMING_ERROR_BADENCODING = -6,  //!< Input text was badly encoded
    CUSTOMSTEMMING_ERROR_FILEIO = -7        //!< A required file was missing or could not be loaded
} e_customStemmingError;

#endif /*_AUTN_CONCEPT_CUSTOM_STEMMING_H_*/
