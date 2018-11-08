/*----------------------------------------------------------------------------
    Declarations of functions that should be defined in a custom stemming DLL

    (c) 2018 Micro Focus.
----------------------------------------------------------------------------*/

#ifndef _AUTN_CUSTOM_STEMMING_DLL_H_
#define _AUTN_CUSTOM_STEMMING_DLL_H_

#ifdef __cplusplus
#ifdef _WIN32
#define KBAPI extern "C" __declspec(dllexport)
#else
#define KBAPI extern "C"
#endif //_WIN32
#else
#ifdef _WIN32
#define KBAPI __declspec(dllexport)
#else
#define KBAPI
#endif //_WIN32
#endif //__cplusplus

#include "custom_stemming.h"

/*! Initialize resources within the DLL
 *  \param[in] pArgs Details of the calling application's environment.
 *  \param[out] pInfo Fill in with details about this DLL.
 *  \param[out] pszErr May return an (allocated) error string.
 *  \return CUSTOMSTEMMING_ERROR_SUCCESS if successful, else an error code.
 *  Any resource handles, data, etc. can be placed into pInfo->pContext,
 *  which will be passed into subsequent DLL calls.
 *  Custom stemming libraries MUST define this function.
 */
KBAPI e_customStemmingError CUSTOMSTEMMING_INITIALIZE_FUNCTION(const t_customStemmingArgs* pArgs, t_customStemmingInfo* pInfo, char** pszErr);

/*! Release resources associated with this DLL context
 *  OPTIONAL for custom stemming libraries.
 */
KBAPI void CUSTOMSTEMMING_UNINITIALIZE_FUNCTION(void* pContext);

/*! Stem a term
 *  \param[in] szUnstemmed Unstemmed term, null-terminated, UTF-8 encoded.
 *  \param[in|out] pResult Results structure to populate with stemmed form(s).
 *  \param[out] pszErr May return an (allocated) error string.
 *  \return CUSTOMSTEMMING_ERROR_SUCCESS if successful, else an error code.
 *  The number of allocated results buffers, pResults->nStemsAlloc, may be
 *  less than the value of pInfo->nMaxStems set during initialisation, so
 *  should be checked. It can be expected to be at least 1.
 *  On entry, pResult->nStems can be expected to be zero. If unmodified,
 *  this indicates no stemming was performed by the DLL, and the caller
 *  should fall back on any other configured stemming algorithm.
 *  To explicitly mark the term as not stemming, set nStems to 1 and copy
 *  szUnstemmed into aszStems[0].
 *  Setting aszStems[0] to the empty string ("") and returning nStems>=1
 *  will result in the term being flagged as 'invalid'. (Empty strings in
 *  other positions in aszStems will be ignored.)
 *  All returned stems should be valid UTF-8; do not, for example, remove
 *  trailing bytes from the input without first checking that truncation
 *  occurs at a UTF-8 character boundary (i.e. not midway through a multibyte
 *  character).
 *  Custom stemming libraries MUST define this function.
 */
KBAPI e_customStemmingError CUSTOMSTEMMING_STEMTERM_FUNCTION(void* pContext, const char* szUnstemmed, t_conceptStemmingResult* pResult, char** pszErr);

/*! Free memory allocated by DLL (e.g. an error message)
 *  OPTIONAL for custom stemming libraries, but beware of potential memory
 *  leaks if Initialize or StemTerm ever allocate and return error strings.
 */
KBAPI void CUSTOMSTEMMING_FREEBUFFER_FUNCTION(void* pContext, char** pszBuffer);

#endif /*_AUTN_CUSTOM_STEMMING_DLL_H_*/
