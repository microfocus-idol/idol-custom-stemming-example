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
    char ** aszStems;       //!< Array of buffers to populate with stemmed form(s); DLL should modify buffer content only.
    int     nStems;         //!< Post-stemming, number of stems produced.
    int     nStemsAlloc;    //!< Allocated size of aszStems (i.e. max number of stems); do not modify within DLL.
    int     nStemMaxLength; //!< Allocated size of each aszStems[n] (i.e. max stem length); do not modify within DLL.
} t_conceptStemmingResult;

#endif /*_AUTN_CONCEPT_STEMMING_TYPES_H_*/
