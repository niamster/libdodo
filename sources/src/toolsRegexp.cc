/***************************************************************************
 *            toolsRegexp.cc
 *
 *  Thu Sep 20 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef PCRE_EXT
#include <pcre.h>
#else
#include <sys/types.h>
#include <regex.h>
#endif

namespace dodo {
    namespace tools {
        /**
         * @struct __regexp__
         * @brief defines regular expression internal handles
         */
        struct __regexp__ {
#ifdef PCRE_EXT
            pcre    *code;                          ///< compiled pattern
#else
            regex_t code;                           ///< compiled pattern
            bool    notCompiled;                    ///< true if not compiled
#endif
        };
    };
};

#include <libdodo/toolsRegexp.h>
#include <libdodo/types.h>
#include <libdodo/toolsRegexpEx.h>

using namespace dodo::tools;

regexp::regexp(regexp &)
{
}

//-------------------------------------------------------------------

regexp::regexp() : extended(true),
                   icase(false),
                   greedy(true),
                   multiline(false),
                   regex(new __regexp__)
{
#ifdef PCRE_EXT
#else
    regex->notCompiled = true;
#endif
}

//-------------------------------------------------------------------

regexp::regexp(const dodo::string &pattern) : extended(true),
                                            icase(false),
                                            greedy(true),
                                            multiline(false),
                                            regex(new __regexp__)
{
#ifdef PCRE_EXT
#else
    regex->notCompiled = true;
#endif

    compile(pattern);
}

//-------------------------------------------------------------------

regexp::~regexp()
{
#ifdef PCRE_EXT
#else
    if (!regex->notCompiled)
        regfree(&regex->code);

#endif

    delete regex;
}

//-------------------------------------------------------------------

bool
regexp::match(const dodo::string &pattern,
              const dodo::string &sample,
              dodoStringArray  &pockets)
{
    dodo_try {
        compile(pattern);
    } dodo_catch (exception::basic *e UNUSED) {
        if (e->function == REGEXPEX_COMPILE)
            return false;
        else
            dodo_rethrow;
    }

    return match(sample, pockets);
}

//-------------------------------------------------------------------

bool
regexp::match(const dodo::string &sample,
              dodoStringArray  &pockets)
{
    pockets.clear();
    if (!boundMatch(sample))
        return false;

#ifndef USE_DEQUE
    pockets.reserve(boundaries.size());
#endif

    dodoArray<__regex_match__>::const_iterator i(boundaries.begin()), j(boundaries.end());
    for (; i != j; ++i)
        pockets.push_back(dodo::string(sample.data() + i->begin, i->end - i->begin));

    return true;
}

//-------------------------------------------------------------------

bool
regexp::boundMatch(const dodo::string &sample)
{
    boundaries.clear();

#ifdef PCRE_EXT
    int subs;

    if (pcre_fullinfo(regex->code, NULL, PCRE_INFO_CAPTURECOUNT, &subs) != 0)
        return false;

    subs *= 3;
    subs += 3;

    int *oVector = new int[subs];
    int rc = pcre_exec(regex->code, NULL, sample.data(), sample.size(), 0, 0, oVector, subs);
    if (rc <= 0) {
        delete [] oVector;

        return false;
    }

    __regex_match__ bound;

    for (int j = 1; j < rc; ++j) {
        subs = j * 2;
        bound.begin = oVector[subs];
        bound.end = oVector[subs + 1];
        boundaries.push_back(bound);
    }

    delete [] oVector;

    return true;
#else
    int subs = regex->code.re_nsub + 1;
    regmatch_t *pmatch = new regmatch_t[subs];

    int res = regexec(&regex->code, sample.data(), subs, pmatch, 0);
    if (res != 0) {
        delete [] pmatch;
        return false;
    }

    __regex_match__ bound;

    for (int i(1); i < subs; ++i) {
        bound.begin = pmatch[i].rm_so;
        bound.end = pmatch[i].rm_eo;
        boundaries.push_back(bound);
    }

    delete [] pmatch;

    return true;
#endif
}

//-------------------------------------------------------------------

void
regexp::compile(const dodo::string &pattern)
{
    int bits(0);

#ifdef PCRE_EXT
    if (icase)
        bits |= PCRE_CASELESS;
    if (!greedy)
        bits |= PCRE_UNGREEDY;
    if (multiline)
        bits |= PCRE_MULTILINE;
    bits |= PCRE_DOTALL;

    int errOffset(0), errn(0);
    const char *error;
    regex->code = pcre_compile2(pattern.data(), bits, &errn, &error, &errOffset, NULL);
    if (regex->code == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSREGEXP, REGEXPEX_COMPILE, exception::ERRNO_PCRE, errn, error, __LINE__, __FILE__, pattern);

#else
    if (extended)
        bits |= REG_EXTENDED;
    if (icase)
        bits |= REG_ICASE;

    if (regex->notCompiled)
        regex->notCompiled = false;
    else
        regfree(&regex->code);

    int errn = regcomp(&regex->code, pattern.data(), bits);
    if (errn != 0) {
#define ERROR_LEN 256
        char error[ERROR_LEN];
        regerror(errn, &regex->code, error, ERROR_LEN);
        dodo_throw exception::basic(exception::MODULE_TOOLSREGEXP, REGEXPEX_COMPILE, exception::ERRNO_POSIXREGEX, errn, error, __LINE__, __FILE__, pattern);
    }
#endif
}

//-------------------------------------------------------------------

dodo::string
regexp::replace(const dodo::string      &pattern,
                const dodo::string      &sample,
                const dodoStringArray &replacements)
{
    dodo_try {
        compile(pattern);
    } dodo_catch (exception::basic *e UNUSED) {
        if (e->function == REGEXPEX_COMPILE)
            return sample;
        else
            dodo_rethrow;
    }

    return replace(sample, replacements);
}

//-------------------------------------------------------------------

dodo::string
regexp::replace(const dodo::string      &sample,
                const dodoStringArray &replacements)
{
    if (!boundMatch(sample))
        return sample;

    dodoArray<__regex_match__>::const_iterator i(boundaries.begin()), j(boundaries.end()), o;

    dodoStringArray::const_iterator k(replacements.begin());
    int subs = replacements.size();

    dodo::string temp = sample;

    long shift = 0;
    unsigned long begin(0), end(0);

    for (int res = 0; res < subs && i != j; ++i, ++res, ++k) {
        if (res > 0) {
            o = i - 1;
            shift = o->end - o->begin - (k - 1)->size();
        }
        begin = i->begin - shift;
        end = i->end - shift;

        temp.replace(begin, end - begin, *k);
    }

    return temp;
}

//-------------------------------------------------------------------

