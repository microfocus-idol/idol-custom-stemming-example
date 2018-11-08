# IDOL Custom Stemming Libraries

You can configure IDOL to call out to an external shared library to perform stemming.
When neither IDOL's built-in stemming nor bespoke stemming files offer sufficient flexibility,
you can implement your own stemming library to give you complete control over the stemming process.

## Configuring a custom stemming library

Custom stemming is configured on a per-language basis. For example:

    [English]
    Encodings=UTF8:englishUTF8
    Stoplist=english.dat
    CustomStemming=mycustomstemmer

In this example, IDOL attempts to load an external library called `mycustomstemmer.dll` 
_(Windows)_ or `libmycustomstemmer.so` _(non-Windows)_ from the configured language directory, 
which IDOL will use to provide stems for terms in English text.

The custom stemming interface allows the stemming library to indicate that it cannot stem a term, 
in which case IDOL falls back on the built-in stemming for that language (unless the language also 
has _Stemming=False_ configured). It is possible to configure both a custom stemming library and a 
bespoke stemming file. In that case, stems in the file take precedence. 

## Terms with multiple stems

It is possible for a custom stemming library to return multiple stemmed forms for a single term, something 
which was not possible with either built-in stemming algorithms or bespoke stemming. As far as possible, 
multiple-stemming works transparently:
* Documents that contain a term with multiple stems match either stem in query text;
* Query terms with multiple stems match documents that contain either stem.

For example, consider the English word _wound_, which might stem to both _WOUND_ and _WIND_.
* Documents that contain _wound_ match queries for _wounded_ or _winds_
* Queries for _wound_ match documents that contain either _wounded_ or _winds_

NOTE: this example assumes there are also 'normal' English stemming rules to strip suffixes such as _-ed_ or _-s_.

Multiple stems work as expected with Boolean, phrase and proximity search.

Some term actions are limited: 
* TermGetInfo returns only the 'primary' stem for a term (the first stem that the library returns).
* TermExpand with expansion=stem only considers primary stems.

## Writing a custom stemming library

A complete example custom stemming library is provided on the Micro Focus public github site:
> https://github.com/microfocus-idol/idol-custom-stemming-example

This example includes the necessary headers to implement your own stemming library in C or C++.
You can include the header `stemming_dll.h` in your own code and implement the functions declared there.
This file includes the other headers, which define the structures used by the interface.

#### Note on error handling

A number of functions have return type `e_customStemmingError`. This enum has members to describe success, 
and a number of common failure modes. The library must return `CUSTOMSTEMMING_ERROR_SUCCESS` if the call 
succeeds, or otherwise it must return the most applicable error code. If none of the other codes are 
appropriate, it can return the generic `CUSTOMSTEMMING_ERROR_FAILURE`.

These functions also take a parameter `char **pszErr`. You can use this parameter on failure to point to a 
more specific error string to use in logging. Any non-NULL pointer returned is passed to the library's 
`customStemmingFreeBuffer` after it is used. If the error message was an allocated rather than static string, 
this must call the appropriate memory-management function (for example `free` or `delete`).

### customStemmingInitialize

    e_customStemmingError customStemmingInitialize(const t_customStemmingArgs* pArgs, t_customStemmingInfo* pInfo, char** pszErr);

This function is called each time the library is loaded (that is, once per language configured to use the 
library for stemming) to initialize resources in the stemming library.

Custom stemming libraries **MUST** define this function.

#### Arguments

* `pArgs` Details of the calling application's environment:
    * `->nVersion` The version of the interface API used by the calling application.
        If this version is less than the value of `CUSTOMSTEMMING_API_VERSION` the library was compiled 
        with, you might want to return an error (for example if features the library relies on are absent 
        in the earlier version of the API).
    * `->szLangDirectory` The configured location of language resource files.
    * `->szLanguage` The 3-letter ISO 639-2 code for the language that is loading the stemming library.
* `pInfo` Fill in with details about this library
    * `->pContext` An opaque pointer that is passed into subsequent calls to the library.
    * `->nVersion` The version of the interface API used by the DLL (that is, set this to `CUSTOMSTEMMING_API_VERSION`).
    * `->nMaxStems` The maximum number of stems that might be returned (0 indicates unknown at initialization time).
* `pszErr` Can hold a pointer to an error string on exit.

#### On exit

Return `CUSTOMSTEMMING_ERROR_SUCCESS` to indicate successful initialization. Otherwise, return an error code and, 
optionally, an error string.

The struct `*pInfo` should be filled in by the library. In particular, the pointer placed into 
`pInfo->pContext` is passed back into subsequent library calls, so it can be used to hold references to any 
data structures, and so on, that were created during initialization.

Because the calling application is responsible for allocating the memory into which the library places the 
results, it must know how many stems the library might return, to allow sufficient space. The library can 
indicate this by setting `pInfo->nMaxStems`. If it returns a value of zero, the application sets a sensible 
upper limit. When stemming is requested, the library must always check the size of the results space 
provided, as it might be smaller than the value returned here.

### customStemmingUninitialize

    void customStemmingUninitialize(void* pContext);

This function is called when the application has finished using the stemming library (for example, on 
shutdown or reset).

Custom stemming libraries **SHOULD** define this function if they need to free resources allocated during 
initialization (associated with `pContext`).

### customStemmingStemTerm

    e_customStemmingError customStemmingStemTerm(void* pContext, const char* szUnstemmed, t_conceptStemmingResult* pResult, char** pszErr);

This function is called whenever the application requires the library to stem a term.
This might occur in multiple threads simultaneously, so the library must be able to handle concurrent 
calls.

Custom stemming libraries **MUST** define this function.

#### Arguments

* `pContext` As returned from initialization.
* `szUnstemmed` The input to stem; NULL-terminated string, UTF-8 encoded.
* `pResult` The results structure to populate with stemmed forms.
    * `->aszStems` The array of buffers to populate with stemmed form(s). The library must modify buffer contents 
        only, _not_ the pointers in the array.
    * `->nStems` Set this value to the number of stems produced.
    * `->nStemsAlloc` The allocated size of aszStems (that is, the maximum number of stems).
        _Do not modify this value in the library_.
    * `->nStemMaxLength` The allocated size of each aszStems[n] (that is, the maximum stem length).
        _Do not modify this value in the library_.
* `pszErr` Can hold a pointer to an error string on exit.

The input string `szUnstemmed` is uppercased by the application. Terms that contain any numeric or 
wildcard characters are not stemmed, so these characters should not be expected in the input. Stemming is 
called after any transliteration has been performed, so `szUnstemmed` will have been transliterated, if 
transliteration is configured for that language. If you need greater control over transliteration, you can 
disable it in the IDOL configuration and apply your own rules in the stemming library.

#### On exit

Return `CUSTOMSTEMMING_ERROR_SUCCESS` to indicate success. Otherwise return an error code and, optionally, 
an error string.

In the success case, the buffers in `pResult->aszStems` must be populated with the stemmed forms of 
the input string, `szUnstemmed`. To avoid repeated allocations, and any issues of memory ownership, these 
buffers are allocated by the application and passed into the library, which must write results directly 
into them. 

On entry, `pResult->nStems` can be expected to be zero. If left unmodified, this value indicates that no 
stemming was performed by the library, and the calling application must fall back on any other stemming 
algorithm configured for the language. To explicitly mark the term as not stemming, set `nStems` to 1 and 
copy `szUnstemmed` into `aszStems[0]`. Setting `aszStems[0]` to the empty string (`""`) and returning 
`nStems`>=1 results in the term being flagged as 'invalid'. (Empty strings in other positions in `aszStems` 
are ignored.)

Note that the number of allocated results buffers, `pResults->nStemsAlloc`, might be less than the value of 
`pInfo->nMaxStems` set during initialization, so must be checked. It can be expected to be at least 1. 
You must also take care not to overrun the length of any individual buffer (`nStemMaxLength`).

All returned stems must be valid UTF-8. Do not, for example, remove trailing bytes from the input without 
first checking that truncation occurs at a UTF-8 character boundary (that is, it does not happen midway 
through a multibyte Unicode character). It is recommended that the returned stems contain only uppercase 
alphabetic characters.

### customStemmingFreeBuffer

    void customStemmingFreeBuffer(void* pContext, char** pszBuffer);
    
This function is called to release any memory passed out of the library (for example, returned error strings).

Custom stemming libraries **MAY** define this function. See notes on error strings above, and be aware of 
the potential for memory leaks if these are ever returned.
