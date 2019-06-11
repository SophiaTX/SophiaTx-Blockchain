#pragma once

#include <sophiatx/protocol/config.hpp>
#include <sophiatx/protocol/asset.hpp>
#include <sophiatx/protocol/types.hpp>
#include <sophiatx/protocol/version.hpp>
#include <sophiatx/chain/genesis_state.hpp>

namespace sophiatx { namespace protocol {

class sophiatx_config {

public:
    inline static void init(const chain::genesis_state_type &genesis)
    {
        instance().config_loaded_ = true;
#ifdef IS_TEST_NET
        instance().config_["IS_TEST_NET"] = true;
#else
        instance().config_["IS_TEST_NET"] = false;
#endif
        instance().config_["SOPHIATX_BLOCKCHAIN_VERSION"] = SOPHIATX_BLOCKCHAIN_VERSION;
        instance().config_["SOPHIATX_INIT_PUBLIC_KEY_STR"] = SOPHIATX_INIT_PUBLIC_KEY_STR;
        instance().config_["SOPHIATX_MIN_ACCOUNT_CREATION_FEE"] = SOPHIATX_MIN_ACCOUNT_CREATION_FEE;
        instance().config_["SOPHIATX_OWNER_AUTH_RECOVERY_PERIOD"] = SOPHIATX_OWNER_AUTH_RECOVERY_PERIOD;
        instance().config_["SOPHIATX_ACCOUNT_RECOVERY_REQUEST_EXPIRATION_PERIOD"] = SOPHIATX_ACCOUNT_RECOVERY_REQUEST_EXPIRATION_PERIOD;
        instance().config_["SOPHIATX_OWNER_UPDATE_LIMIT"] = SOPHIATX_OWNER_UPDATE_LIMIT;
        instance().config_["SOPHIATX_INIT_SUPPLY"] = SOPHIATX_INIT_SUPPLY;
        instance().config_["SOPHIATX_TOTAL_SUPPLY"] = SOPHIATX_TOTAL_SUPPLY;
        instance().config_["SOPHIATX_MIN_FEEDS"] = SOPHIATX_MIN_FEEDS;
        instance().config_["SOPHIATX_GENESIS_TIME"] = SOPHIATX_GENESIS_TIME;
        instance().config_["SOPHIATX_BLOCK_INTERVAL"] = SOPHIATX_BLOCK_INTERVAL;
        instance().config_["SOPHIATX_BLOCKS_PER_YEAR"] = SOPHIATX_BLOCKS_PER_YEAR;
        instance().config_["SOPHIATX_BLOCKS_PER_DAY"] = SOPHIATX_BLOCKS_PER_DAY;
        instance().config_["SOPHIATX_START_MINER_VOTING_BLOCK"] = SOPHIATX_START_MINER_VOTING_BLOCK;
        instance().config_["SOPHIATX_DECIMALS"] = SOPHIATX_DECIMALS;
        instance().config_["SOPHIATX_SATOSHIS"] = SOPHIATX_SATOSHIS;
        instance().config_["SOPHIATX_PROMOTION_POOL_PERCENTAGE"] = genesis.promotion_pool_percentage;
        instance().config_["SOPHIATX_MINING_POOL_PERCENTAGE"] = genesis.mining_pool_percentage;
        instance().config_["SOPHIATX_INTEREST_POOL_PERCENTAGE"] = genesis.interest_pool_percentage;
        instance().config_["SOPHIATX_BURN_FEE_PERCENTAGE"] = genesis.burn_fee_percentage;
        instance().config_["SOPHIATX_INTEREST_BLOCKS"] = SOPHIATX_INTEREST_BLOCKS;
        instance().config_["SOPHIATX_INTEREST_FEES_TIME"] = SOPHIATX_INTEREST_FEES_TIME;
        instance().config_["SOPHIATX_INTEREST_DELAY"] = SOPHIATX_INTEREST_DELAY;
        instance().config_["SOPHIATX_COINBASE_YEARS"] = genesis.coinbase_years;
        instance().config_["SOPHIATX_COINBASE_BLOCKS"] = SOPHIATX_BLOCKS_PER_YEAR * genesis.coinbase_years;

        instance().config_["SOPHIATX_FINAL_WITNESS_REQUIRED_VESTING_BALANCE"] = SOPHIATX_FINAL_WITNESS_REQUIRED_VESTING_BALANCE;
        instance().config_["SOPHIATX_WITNESS_VESTING_INCREASE_DAYS"] = SOPHIATX_WITNESS_VESTING_INCREASE_DAYS;
        instance().config_["SOPHIATX_INITIAL_WITNESS_REQUIRED_VESTING_BALANCE"] = SOPHIATX_INITIAL_WITNESS_REQUIRED_VESTING_BALANCE;

        instance().config_["VESTS_SYMBOL"] = VESTS_SYMBOL;
        instance().config_["SOPHIATX_SYMBOL"] = SOPHIATX_SYMBOL;
        instance().config_["SBD1_SYMBOL"] = SBD1_SYMBOL;
        instance().config_["SBD2_SYMBOL"] = SBD2_SYMBOL;
        instance().config_["SBD3_SYMBOL"] = SBD3_SYMBOL;
        instance().config_["SBD4_SYMBOL"] = SBD4_SYMBOL;
        instance().config_["SBD5_SYMBOL"] = SBD5_SYMBOL;
        instance().config_["BASE_FEE"] = BASE_FEE;
        instance().config_["BASE_FEE_SBD1"] = BASE_FEE_SBD1;
        instance().config_["BASE_FEE_SBD2"] = BASE_FEE_SBD2;
        instance().config_["BASE_FEE_SBD3"] = BASE_FEE_SBD3;
        instance().config_["BASE_FEE_SBD4"] = BASE_FEE_SBD4;
        instance().config_["BASE_FEE_SBD5"] = BASE_FEE_SBD5;
        instance().config_["SIZE_COVERED_IN_BASE_FEE"] = SIZE_COVERED_IN_BASE_FEE;
        instance().config_["SIZE_INCREASE_PER_FEE"] = SIZE_INCREASE_PER_FEE;
        instance().config_["SOPHIATX_BLOCKCHAIN_HARDFORK_VERSION"] = SOPHIATX_BLOCKCHAIN_HARDFORK_VERSION;
        instance().config_["SOPHIATX_INIT_MINER_NAME"] = SOPHIATX_INIT_MINER_NAME;
        instance().config_["SOPHIATX_NUM_INIT_MINERS"] = SOPHIATX_NUM_INIT_MINERS;
        instance().config_["SOPHIATX_MAX_WITNESSES"] = SOPHIATX_MAX_WITNESSES;
        instance().config_["SOPHIATX_MAX_VOTED_WITNESSES_HF0"] = SOPHIATX_MAX_VOTED_WITNESSES_HF0;
        instance().config_["SOPHIATX_MAX_RUNNER_WITNESSES_HF0"] = SOPHIATX_MAX_RUNNER_WITNESSES_HF0;
        instance().config_["SOPHIATX_HARDFORK_REQUIRED_WITNESSES"] = SOPHIATX_HARDFORK_REQUIRED_WITNESSES;
        instance().config_["SOPHIATX_MAX_TIME_UNTIL_EXPIRATION"] = SOPHIATX_MAX_TIME_UNTIL_EXPIRATION;
        instance().config_["SOPHIATX_MAX_MEMO_SIZE"] = SOPHIATX_MAX_MEMO_SIZE;
        instance().config_["SOPHIATX_MAX_NAME_SEED_SIZE"] = SOPHIATX_MAX_NAME_SEED_SIZE;
        instance().config_["SOPHIATX_MAX_PROXY_RECURSION_DEPTH"] = SOPHIATX_MAX_PROXY_RECURSION_DEPTH;
        instance().config_["SOPHIATX_VESTING_WITHDRAW_INTERVALS"] = SOPHIATX_VESTING_WITHDRAW_INTERVALS;
        instance().config_["SOPHIATX_VESTING_WITHDRAW_INTERVAL_SECONDS"] = SOPHIATX_VESTING_WITHDRAW_INTERVAL_SECONDS;
        instance().config_["SOPHIATX_MAX_ACCOUNT_WITNESS_VOTES"] = SOPHIATX_MAX_ACCOUNT_WITNESS_VOTES;
        instance().config_["SOPHIATX_100_PERCENT"] = SOPHIATX_100_PERCENT;
        instance().config_["SOPHIATX_1_PERCENT"] = SOPHIATX_1_PERCENT;
        instance().config_["SOPHIATX_LIMIT_BANDWIDTH_BLOCKS"] = SOPHIATX_LIMIT_BANDWIDTH_BLOCKS;
        instance().config_["SOPHIATX_MAX_ALLOWED_BANDWIDTH"] = SOPHIATX_MAX_ALLOWED_BANDWIDTH;
        instance().config_["SOPHIATX_MAX_ALLOWED_OPS_COUNT"] = SOPHIATX_MAX_ALLOWED_OPS_COUNT;
        instance().config_["SOPHIATX_MIN_ACCOUNT_NAME_LENGTH"] = SOPHIATX_MIN_ACCOUNT_NAME_LENGTH;
        instance().config_["SOPHIATX_MAX_ACCOUNT_NAME_LENGTH"] = SOPHIATX_MAX_ACCOUNT_NAME_LENGTH;
        instance().config_["SOPHIATX_MAX_PERMLINK_LENGTH"] = SOPHIATX_MAX_PERMLINK_LENGTH;
        instance().config_["SOPHIATX_MAX_WITNESS_URL_LENGTH"] = SOPHIATX_MAX_WITNESS_URL_LENGTH;
        instance().config_["SOPHIATX_MAX_SHARE_SUPPLY"] = SOPHIATX_MAX_SHARE_SUPPLY;
        instance().config_["SOPHIATX_MAX_SATOSHIS"] = SOPHIATX_MAX_SATOSHIS;
        instance().config_["SOPHIATX_MAX_SIG_CHECK_DEPTH"] = SOPHIATX_MAX_SIG_CHECK_DEPTH;
        instance().config_["SOPHIATX_MIN_TRANSACTION_SIZE_LIMIT"] = SOPHIATX_MIN_TRANSACTION_SIZE_LIMIT;
        instance().config_["SOPHIATX_MAX_TRANSACTION_SIZE"] = SOPHIATX_MAX_TRANSACTION_SIZE;
        instance().config_["SOPHIATX_MIN_BLOCK_SIZE_LIMIT"] = SOPHIATX_MIN_BLOCK_SIZE_LIMIT;
        instance().config_["SOPHIATX_MAX_BLOCK_SIZE"] = SOPHIATX_MAX_BLOCK_SIZE;
        instance().config_["SOPHIATX_MIN_BLOCK_SIZE"] = SOPHIATX_MIN_BLOCK_SIZE;
        instance().config_["SOPHIATX_BLOCKS_PER_HOUR"] = SOPHIATX_BLOCKS_PER_HOUR;
        instance().config_["SOPHIATX_FEED_INTERVAL_BLOCKS"] = SOPHIATX_FEED_INTERVAL_BLOCKS;
        instance().config_["SOPHIATX_FEED_HISTORY_WINDOW"] = SOPHIATX_FEED_HISTORY_WINDOW;
        instance().config_["SOPHIATX_MAX_FEED_AGE_SECONDS"] = SOPHIATX_MAX_FEED_AGE_SECONDS;
        instance().config_["SOPHIATX_MAX_UNDO_HISTORY"] = SOPHIATX_MAX_UNDO_HISTORY;
        instance().config_["SOPHIATX_IRREVERSIBLE_THRESHOLD"] = SOPHIATX_IRREVERSIBLE_THRESHOLD;
        instance().config_["SOPHIATX_VIRTUAL_SCHEDULE_LAP_LENGTH"] = SOPHIATX_VIRTUAL_SCHEDULE_LAP_LENGTH;
        instance().config_["SOPHIATX_VIRTUAL_SCHEDULE_LAP_LENGTH2"] = SOPHIATX_VIRTUAL_SCHEDULE_LAP_LENGTH2;
        instance().config_["SOPHIATX_MINER_ACCOUNT"] = SOPHIATX_MINER_ACCOUNT;
        instance().config_["SOPHIATX_NULL_ACCOUNT"] = SOPHIATX_NULL_ACCOUNT;
        instance().config_["SOPHIATX_TEMP_ACCOUNT"] = SOPHIATX_TEMP_ACCOUNT;
        instance().config_["SOPHIATX_PROXY_TO_SELF_ACCOUNT"] = SOPHIATX_PROXY_TO_SELF_ACCOUNT;
    }

    inline static const fc::mutable_variant_object& get_config() {
        FC_ASSERT(instance().config_loaded_, "sophiatx_config is not initialized!");
        return instance().config_;
    }

    template<typename T>
    inline static T get( std::string_view index ) {
        FC_ASSERT(instance().config_loaded_, "sophiatx_config is not initialized!");
        T type;
        fc::from_variant(instance().config_[index], type);
        return type;
    }

private:
    sophiatx_config() : config_loaded_(false) {}
    ~sophiatx_config() {}

    inline static sophiatx_config &instance() {
        static sophiatx_config instance;
        return instance;
    }

    bool config_loaded_;
    fc::mutable_variant_object config_;
};

} } // sophiatx::protocol
