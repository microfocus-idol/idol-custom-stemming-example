/*----------------------------------------------------------------------------
    Types used in custom stemming DLL interface.

    Update CUSTOMSTEMMING_API_VERSION in custom_stemming.h if modified.

    (c) 2018 Micro Focus.
----------------------------------------------------------------------------*/

#ifndef _AUTN_CONCEPT_STEMMING_TYPES_H_
#define _AUTN_CONCEPT_STEMMING_TYPES_H_

/*! Structure to be populated with stemming results */
typedef struct s_conceptStemmingResult
{
    char ** aszStems;           //!< Array of buffers to populate with stemmed form(s); DLL should modify buffer content only.
    int     nStems;             //!< Post-stemming, number of stems produced.
    int     nStemsAlloc;        //!< Allocated size of aszStems (i.e. max number of stems); do not modify within DLL.
    int     nStemMaxLength;     //!< Allocated size of each aszStems[n] (i.e. max stem length); do not modify within DLL.

    /*! Members below here only present for CUSTOMSTEMMING_API_VERSION >= 2 */

    unsigned int * anStemFlags; //!< Flag words to set bits signalling information about each stem; same length as aszStems.

} t_conceptStemmingResult;

/*! Flag bits that may be set on anStemFlags members */
#define CONCEPTSTEMMINGRESULT_FLAG_NODEFERREDTRANSLITERATION    0x01    //!< When deferring transliteration, don't apply to this stem

#endif /*_AUTN_CONCEPT_STEMMING_TYPES_H_*/
