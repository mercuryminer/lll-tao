/*__________________________________________________________________________________________

            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++

            (c) Copyright The Nexus Developers 2014 - 2019

            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.

            "ad vocem populi" - To the Voice of the People

____________________________________________________________________________________________*/

#include <LLD/include/global.h>

#include <TAO/API/types/finance.h>
#include <TAO/API/types/objects.h>
#include <TAO/API/include/global.h>

#include <TAO/API/include/utils.h>
#include <TAO/API/include/json.h>

#include <TAO/Ledger/types/sigchain.h>

#include <TAO/Register/types/object.h>

#include <Util/include/debug.h>


/* Global TAO namespace. */
namespace TAO
{

    /* API Layer namespace. */
    namespace API
    {

        /* Get a list of accounts owned by a signature chain. */
        json::json Tokens::ListAccounts(const json::json& params, bool fHelp)
        {
            /* JSON return value. */
            json::json ret;// = json::json::array();

            /* Get the session to be used for this API call */
            uint256_t nSession = users->GetSession(params);

            /* Get the account. */
            memory::encrypted_ptr<TAO::Ledger::SignatureChain>& user = users->GetAccount(nSession);
            if(!user)
                throw APIException(-10, "Invalid session ID");

            /* Check for paged parameter. */
            uint32_t nPage = 0;
            if(params.find("page") != params.end())
                nPage = std::stoul(params["page"].get<std::string>());

            /* Check for limit parameter. */
            uint32_t nLimit = 100;
            if(params.find("limit") != params.end())
                nLimit = std::stoul(params["limit"].get<std::string>());

            /* Get the list of registers owned by this sig chain */
            std::vector<TAO::Register::Address> vAddresses;
            if(!TAO::API::ListAccounts(user->Genesis(), vAddresses, false, false))
                throw APIException(-74, "No registers found");

            /* Read all the registers to that they are sorted by creation time */
            std::vector<std::pair<TAO::Register::Address, TAO::Register::State>> vAccounts;
            GetRegisters(vAddresses, vAccounts);

            /* Add the register data to the response */
            uint32_t nTotal = 0;
            for(const auto& state : vAccounts)
            {
                /* Double check that it is an object before we cast it */
                if(state.second.nType != TAO::Register::REGISTER::OBJECT)
                    continue;

                /* Cast the state to an Object register */
                TAO::Register::Object object(state.second);

                /* Check that this is a non-standard object type so that we can parse it and check the type*/
                if(object.nType != TAO::Register::REGISTER::OBJECT)
                    continue;

                /* parse object so that the data fields can be accessed */
                if(!object.Parse())
                    throw APIException(-36, "Failed to parse object register");

                /* Check that this is an account */
                uint8_t nStandard = object.Standard();
                if(nStandard != TAO::Register::OBJECTS::ACCOUNT )
                    continue;

                /* Check the account is not a NXS account */
                if(object.get<uint256_t>("token") == 0)
                    continue;

                /* Get the current page. */
                uint32_t nCurrentPage = nTotal / nLimit;

                ++nTotal;

                /* Check the paged data. */
                if(nCurrentPage < nPage)
                    continue;

                if(nCurrentPage > nPage)
                    break;

                if(nTotal - (nPage * nLimit) > nLimit)
                    break;

                /* Convert the object to JSON */
                ret.push_back(TAO::API::ObjectToJSON(params, object, state.first));
            }

            return ret;
        }

        /* Lists all transactions for a given account. */
        json::json Tokens::ListTransactions(const json::json& params, bool fHelp)
        {
            /* The account to list transactions for. */
            TAO::Register::Address hashAccount;

            /* If name is provided then use this to deduce the register address,
             * otherwise try to find the raw hex encoded address. */
            if(params.find("name") != params.end())
                hashAccount = Names::ResolveAddress(params, params["name"].get<std::string>());
            else if(params.find("address") != params.end())
                hashAccount.SetBase58(params["address"].get<std::string>());
            else
                throw APIException(-33, "Missing name or address");

            /* the type to list transactions for - should be "token" or "account" */
            std::string strType = params.find("type") != params.end() ? params["type"].get<std::string>() : "";

            /* Get the account object. */
            TAO::Register::Object object;
            if(!LLD::Register->ReadState(hashAccount, object, TAO::Ledger::FLAGS::MEMPOOL))
            {
                if(strType == "account")
                    throw APIException(-13, "Account not found");
                else if(strType == "token")
                    throw APIException(-125, "Token not found");
                else
                    throw APIException(-104, "Object not found");
            }

            /* Parse the object register. */
            if(!object.Parse())
                throw APIException(-14, "Object failed to parse");

            /* Get the object standard. */
            uint8_t nStandard = object.Standard();

            /* Check the object standard. */
            if(nStandard == TAO::Register::OBJECTS::TOKEN
            || nStandard == TAO::Register::OBJECTS::ACCOUNT
            || nStandard == TAO::Register::OBJECTS::TRUST)
            {
                /* If the user requested a particular object type then check it is that type */
                if(strType == "token" && (nStandard == TAO::Register::OBJECTS::ACCOUNT || nStandard == TAO::Register::OBJECTS::TRUST))
                    throw APIException(-123, "Object is not a token");
                else if(strType == "account" && nStandard == TAO::Register::OBJECTS::TOKEN)
                    throw APIException(-65, "Object is not an account");

                /* Check the account is not a NXS account */
                if(strType == "account" && object.get<uint256_t>("token") == 0)
                    throw APIException(-166, "Account is a NXS account.  Please use the finance API for accessing NXS accounts.");

            }
            else
            {
                throw APIException(-124, "Unknown token / account.");
            }

            

            return Objects::ListTransactions(params, fHelp);
        }
    }
}
