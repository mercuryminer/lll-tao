/*__________________________________________________________________________________________

            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++

            (c) Copyright The Nexus Developers 2014 - 2019

            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.

            "ad vocem populi" - To the Voice of the People

____________________________________________________________________________________________*/

#include <TAO/Ledger/include/constants.h>

#include <TAO/Ledger/include/timelocks.h>

#include <Util/include/args.h>
#include <Util/include/runtime.h>


/* Global TAO namespace. */
namespace TAO
{

    /* Ledger Layer namespace. */
    namespace Ledger
    {
        /* Retrieve the number of blocks (confirmations) required for coinbase maturity. */
        uint32_t MaturityCoinBase()
        {
            if(config::fTestNet)
                return TESTNET_MATURITY_BLOCKS;

            int32_t nCurrent = CurrentVersion();

            /* Apply legacy maturity for all versions prior to version 7 */
            if(nCurrent < 7)
                return NEXUS_MATURITY_LEGACY;

            /* Legacy maturity as long as version 6 is active (includes one hour after v7 timelock)  */
            if(nCurrent == 7 && VersionActive(runtime::unifiedtimestamp(), (nCurrent - 1)))
                return NEXUS_MATURITY_LEGACY;

            return NEXUS_MATURITY_COINBASE;
        }


        /* Retrieve the number of blocks (confirmations) required for coinstake maturity. */
        uint32_t MaturityCoinStake()
        {
            if(config::fTestNet)
                return TESTNET_MATURITY_BLOCKS;

            int32_t nCurrent = CurrentVersion();

            /* Apply legacy maturity for all versions prior to version 7 */
            if(nCurrent < 7)
                return NEXUS_MATURITY_LEGACY;

            /* Legacy maturity as long as version 6 is active (includes one hour after v7 timelock)  */
            if(nCurrent == 7 && VersionActive(runtime::unifiedtimestamp(), (nCurrent - 1)))
                return NEXUS_MATURITY_LEGACY;

            return NEXUS_MATURITY_COINSTAKE;
        }

        /* Retrieve the number of blocks (confirmations) required for sig chain maturity after a coinbase/coinstake transaction. */
        uint32_t MaturitySigChain()
        {
            if(config::fTestNet)
                return TESTNET_MATURITY_SIGCHAIN;
            else
                return NEXUS_MATURITY_SIGCHAIN;
        }

    }
}