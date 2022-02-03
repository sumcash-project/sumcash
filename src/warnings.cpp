// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <warnings.h>

#include <sync.h>
#include <util/system.h>
#include <util/translation.h>

static RecursiveMutex cs_warnings;
static std::string strMiscWarning GUARDED_BY(cs_warnings);
static bool fLargeWorkForkFound GUARDED_BY(cs_warnings) = false;
static bool fLargeWorkInvalidChainFound GUARDED_BY(cs_warnings) = false;
std::string strMintWarning;

void SetMiscWarning(const std::string& strWarning)
{
    LOCK(cs_warnings);
    strMiscWarning = strWarning;
}

void SetfLargeWorkForkFound(bool flag)
{
    LOCK(cs_warnings);
    fLargeWorkForkFound = flag;
}

bool GetfLargeWorkForkFound()
{
    LOCK(cs_warnings);
    return fLargeWorkForkFound;
}

void SetfLargeWorkInvalidChainFound(bool flag)
{
    LOCK(cs_warnings);
    fLargeWorkInvalidChainFound = flag;
}

std::string GetWarnings(bool verbose)
{
    int nPriority = 0;
    std::string warnings_concise;
    std::string warnings_verbose;
    const std::string warning_separator = "<hr />";

    LOCK(cs_warnings);

    // Pre-release build warning
    if (!CLIENT_VERSION_IS_RELEASE) {
        warnings_concise = "This is a pre-release test build - use at your own risk - do not use for mining or merchant applications";
        warnings_verbose = _("This is a pre-release test build - use at your own risk - do not use for mining or merchant applications").translated;
    }

    // sumcash: wallet lock warning for minting
    if (strMintWarning != "")
    {
        nPriority = 0;
        warnings_concise = strMintWarning;
        warnings_verbose += (warnings_verbose.empty() ? "" : warning_separator) + _(strMintWarning.c_str()).translated;
    }

    // Misc warnings like out of disk space and clock is wrong
    if (strMiscWarning != "") {
        nPriority = 1000;
        warnings_concise = strMiscWarning;
        warnings_verbose += (warnings_verbose.empty() ? "" : warning_separator) + strMiscWarning;
    }

    if (fLargeWorkForkFound) {
        nPriority = 2000;
        warnings_concise = "Warning: The network does not appear to fully agree! Some miners appear to be experiencing issues.";
        warnings_verbose += (warnings_verbose.empty() ? "" : warning_separator) + _("Warning: The network does not appear to fully agree! Some miners appear to be experiencing issues.").translated;
    } else if (fLargeWorkInvalidChainFound) {
        nPriority = 2000;
        warnings_concise = "Warning: We do not appear to fully agree with our peers! You may need to upgrade, or other nodes may need to upgrade.";
        warnings_verbose += (warnings_verbose.empty() ? "" : warning_separator) + _("Warning: We do not appear to fully agree with our peers! You may need to upgrade, or other nodes may need to upgrade.").translated;
    }

    if (verbose) return warnings_verbose;
    else return warnings_concise;
}
