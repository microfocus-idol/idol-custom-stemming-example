/*
Simple example IDOL custom stemming library
*/

// IDOL custom stemming library headers 
#include "idol/stemming_dll.h"

// third-party stemming implementation headers
// this is not an endorsement or recommendation of this implementation
#include <stemming/english_stem.h>
#include <stemming/french_stem.h>

#include <memory>
#include <string>

namespace {
/* implementation specific wrapper code */
struct Stemmer
{
    virtual ~Stemmer() {}
    virtual void stem(std::wstring&) = 0;
};

template<typename LangStem>
class LangStemmer : public Stemmer
{
public:
    LangStemmer() {}

    void stem(std::wstring& word) override
    {
        impl(word);
    }
private:
    LangStem impl;
};

}

/* Implementation of required IDOL custom stemming interface functions below */

e_customStemmingError CUSTOMSTEMMING_INITIALIZE_FUNCTION(const t_customStemmingArgs* pArgs, t_customStemmingInfo* pInfo, char** pszErr)
{
    pInfo->nVersion = CUSTOMSTEMMING_API_VERSION;
    pInfo->nMaxStems = 2;   
    /* the third party lib only ever returns one stem - there are example overrides
     * for returning multiple stems in CUSTOMSTEMMING_STEMTERM_FUNCTION */

    std::string lang(pArgs->szLanguage);

    if (lang == "ENG")
    {
        pInfo->pContext = new LangStemmer<stemming::english_stem<>>();
    }
    else if (lang == "FRE")
    {
        pInfo->pContext = new LangStemmer<stemming::french_stem<>>();
    }
    else
    {
        std::string err("Unsupported language");
        *pszErr = (char*) malloc(err.length() + 1);
        memcpy(*pszErr, err.c_str(), err.length());
        (*pszErr)[err.length()] = 0;
        return CUSTOMSTEMMING_ERROR_BADPARAMETER;
    }
    return CUSTOMSTEMMING_ERROR_SUCCESS;
}

void CUSTOMSTEMMING_UNINITIALIZE_FUNCTION(void* pContext)
{
    delete (static_cast<Stemmer*>(pContext));
}

e_customStemmingError CUSTOMSTEMMING_STEMTERM_FUNCTION(void * pContext, const char* szUnstemmed, t_conceptStemmingResult* pResult, char** pszErr)
{
    std::string input(szUnstemmed);
    if (input == "CLASSIFY")
    {
        /* example of multiple stemming */
        strcpy(pResult->aszStems[0], "CLASSIFI"); /* stem for classifier */
        pResult->nStems++;
        if (pResult->nStemsAlloc > 1)
        {
            strcpy(pResult->aszStems[1], "CLASSIF"); /* stem for classification */
            pResult->nStems++;
        }
        return CUSTOMSTEMMING_ERROR_SUCCESS;
    }
    else if (input == "WOUND")
    {
        /* another example of multiple stemming */
        strcpy(pResult->aszStems[0], "WOUND"); /* 'injury' */
        pResult->nStems++;
        if (pResult->nStemsAlloc > 1)
        {
            strcpy(pResult->aszStems[1], "WIND"); /* 'coil' */
            pResult->nStems++;
        }
        return CUSTOMSTEMMING_ERROR_SUCCESS;
    }

    wchar_t workspace[1024];
    size_t len = std::mbstowcs(workspace, szUnstemmed, 1024);
    if (static_cast<std::size_t>(-1) == len)
        return CUSTOMSTEMMING_ERROR_FAILURE;

    workspace[len] = 0;

    std::wstring word(workspace);
    (static_cast<Stemmer*>(pContext))->stem(word);

    std::wcstombs(pResult->aszStems[0], word.data(), pResult->nStemMaxLength);
    pResult->nStems++;
    return CUSTOMSTEMMING_ERROR_SUCCESS;
}

void CUSTOMSTEMMING_FREEBUFFER_FUNCTION(void *pContext, char** pszBuffer)
{
    free(*pszBuffer);
    *pszBuffer = 0;
}
