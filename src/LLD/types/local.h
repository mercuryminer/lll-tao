/*__________________________________________________________________________________________

            (c) Hash(BEGIN(Satoshi[2010]), END(Sunny[2012])) == Videlicet[2014] ++

            (c) Copyright The Nexus Developers 2014 - 2019

            Distributed under the MIT software license, see the accompanying
            file COPYING or http://www.opensource.org/licenses/mit-license.php.

            "ad vocem populi" - To the Voice of the People

____________________________________________________________________________________________*/

#pragma once
#ifndef NEXUS_LLD_INCLUDE_LOCAL_H
#define NEXUS_LLD_INCLUDE_LOCAL_H

#include <LLC/types/uint1024.h>

#include <LLD/templates/sector.h>
#include <LLD/cache/binary_lru.h>
#include <LLD/keychain/hashmap.h>

#include <TAO/Ledger/include/stake_change.h>


/** Forward declarations **/
namespace TAO
{
    namespace Ledger
    {
        class Transaction;
    }
}


namespace LLD
{

  /** LocalDB
   *
   *  Database class for storing local wallet transactions.
   *
   **/
    class LocalDB : public SectorDatabase<BinaryHashMap, BinaryLRU>
    {

    public:

        /** The Database Constructor. To determine file location and the Bytes per Record. **/
        LocalDB(const uint8_t nFlagsIn = FLAGS::CREATE | FLAGS::WRITE,
            const uint32_t nBucketsIn = 77773, const uint32_t nCacheIn = 1024 * 1024);


        /** Default Destructor **/
        virtual ~LocalDB();


        /** WriteGenesis
         *
         *  Writes a genesis transaction-id to disk.
         *
         *  @param[in] hashGenesis The genesis ID to write for.
         *  @param[in] tx The transaction to write.
         *
         *  @return True if the genesis is written, false otherwise.
         *
         **/
        bool WriteGenesis(const uint256_t& hashGenesis, const TAO::Ledger::Transaction& tx);


        /** ReadGenesis
         *
         *  Reads a genesis transaction-id from disk.
         *
         *  @param[in] hashGenesis The genesis ID to read for.
         *  @param[out] tx The transaction to read.
         *
         *  @return True if the genesis was read, false otherwise.
         *
         **/
        bool ReadGenesis(const uint256_t& hashGenesis, TAO::Ledger::Transaction &tx);


        /** HasGenesis
         *
         *  Checks if a genesis transaction exists.
         *
         *  @param[in] hashGenesis The genesis ID to check for.
         *
         *  @return True if the genesis exists, false otherwise.
         *
         **/
        bool HasGenesis(const uint256_t& hashGenesis);


        /** WriteLast
         *
         *  Writes the last txid of sigchain to disk indexed by genesis.
         *
         *  @param[in] hashGenesis The genesis hash to write.
         *  @param[in] hashLast The last hash (txid) to write.
         *
         *  @return True if the last was successfully written, false otherwise.
         *
         **/
        bool WriteLast(const uint256_t& hashGenesis, const uint512_t& hashLast);


        /** ReadLast
         *
         *  Reads the last txid of sigchain to disk indexed by genesis.
         *
         *  @param[in] hashGenesis The genesis hash to read.
         *  @param[in] hashLast The last hash (txid) to read.
         *
         *  @return True if the last was successfully read, false otherwise.
         *
         **/
        bool ReadLast(const uint256_t& hashGenesis, uint512_t &hashLast);


        /** WriteStakeChange
         *
         *  Writes a stake change request indexed by genesis.
         *
         *  @param[in] hashGenesis The user genesis requesting the stake change.
         *  @param[in] stakeChange Stake change request data to record.
         *
         *  @return True if the change was successfully written, false otherwise.
         *
         **/
        bool WriteStakeChange(const uint256_t& hashGenesis, const TAO::Ledger::StakeChange& stakeChange);


        /** ReadStakeChange
         *
         *  Reads a stake change request for a sig chain genesis.
         *
         *  @param[in] hashGenesis The user genesis to read.
         *
         *  @return True if the change was successfully read, false otherwise.
         *
         **/
        bool ReadStakeChange(const uint256_t& hashGenesis, TAO::Ledger::StakeChange& stakeChange);


        /** EraseStakeChange
         *
         *  Removes a recorded stake change request.
         *
         *  @param[in] hashGenesis The user genesis of the request to remove.
         *
         *  @return True if the change was successfully erased, false otherwise.
         *
         **/
        bool EraseStakeChange(const uint256_t& hashGenesis);

    };
}

#endif
