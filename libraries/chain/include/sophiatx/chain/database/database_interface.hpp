#ifndef SOPHIATX_DATABASE_INTERFACE_HPP
#define SOPHIATX_DATABASE_INTERFACE_HPP

#include <sophiatx/chain/global_property_object.hpp>
#include <sophiatx/chain/hardfork_property_object.hpp>
#include <sophiatx/chain/node_property_object.hpp>
#include <sophiatx/chain/fork_database.hpp>
#include <sophiatx/chain/block_log.hpp>
#include <sophiatx/chain/operation_notification.hpp>
#include <sophiatx/chain/util/signal.hpp>
#include <sophiatx/chain/economics.hpp>
#include <sophiatx/chain/sophiatx_objects.hpp>

#include <sophiatx/chain/util/asset.hpp>

#include <sophiatx/protocol/protocol.hpp>
#include <sophiatx/protocol/hardfork.hpp>
#include <sophiatx/chain/genesis_state.hpp>

#include <boost/signals2/signal.hpp>

#include <fc/log/logger.hpp>

#include <map>

namespace sophiatx {
namespace chain {

using sophiatx::protocol::signed_transaction;
using sophiatx::protocol::operation;
using sophiatx::protocol::authority;
using sophiatx::protocol::asset;
using sophiatx::protocol::asset_symbol_type;
using sophiatx::protocol::price;

class custom_operation_interpreter;

/**
 *   @class database
 *   @brief tracks the blockchain state in an extensible manner
 */
class database_interface : public chainbase::database, public std::enable_shared_from_this<database_interface> {
public:

   database_interface() {}

   virtual ~database_interface() {}

   bool is_producing() const { return _is_producing; }

   bool _is_producing = false;

   bool _log_hardforks = true;

   enum validation_steps {
      skip_nothing = 0,
      skip_witness_signature = 1 << 0,  ///< used while reindexing
      skip_transaction_signatures = 1 << 1,  ///< used by non-witness nodes
      skip_transaction_dupe_check = 1 << 2,  ///< used while reindexing
      skip_fork_db = 1 << 3,  ///< used while reindexing
      skip_block_size_check = 1 << 4,  ///< used when applying locally generated transactions
      skip_tapos_check = 1 << 5,  ///< used while reindexing -- note this skips expiration check as well
      skip_authority_check = 1 << 6,  ///< used while reindexing -- disables any checking of authority on transactions
      skip_merkle_check = 1 << 7,  ///< used while reindexing
      skip_undo_history_check = 1 << 8,  ///< used while reindexing
      skip_witness_schedule_check = 1 << 9,  ///< used while reindexing
      skip_validate = 1 << 10, ///< used prior to checkpoint, skips validate() call on transaction
      skip_validate_invariants = 1 << 11, ///< used to skip database invariant check on block application
      skip_undo_block = 1 << 12, ///< used to skip undo db on reindex
      skip_block_log = 1 << 13  ///< used to skip block logging on reindex
   };

   typedef std::function<void(uint32_t, const abstract_index_cntr_t &)> TBenchmarkMidReport;
   typedef std::pair<uint32_t, TBenchmarkMidReport> TBenchmark;

   struct open_args {
      fc::path shared_mem_dir;
      uint64_t shared_file_size = 0;
      uint16_t shared_file_full_threshold = 0;
      uint16_t shared_file_scale_rate = 0;
      uint32_t chainbase_flags = 0;
      bool do_validate_invariants = false;
      uint64_t app_id = 0;

      // The following fields are only used on reindexing
      uint32_t stop_replay_at = 0;
      TBenchmark benchmark = TBenchmark(0, [](uint32_t, const abstract_index_cntr_t &) {});
   };

   /**
    * @brief Open a database, creating a new one if necessary
    *
    * Opens a database in the specified directory. If no initialized database is found the database
    * will be initialized with the default state.
    *
    * @param data_dir Path to open or create database in
    */
   virtual void open(const open_args &args, const genesis_state_type &genesis) = 0;

   /**
    * @brief Rebuild object graph from block history and open detabase
    *
    * This method may be called after or instead of @ref database::open, and will rebuild the object graph by
    * replaying blockchain history. When this method exits successfully, the database will be open.
    *
    * @return the last replayed block number.
    */
   virtual uint32_t reindex(const open_args &args, const genesis_state_type &genesis) = 0;

   /**
    * @brief wipe Delete database from disk, and potentially the raw chain as well.
    * @param include_blocks If true, delete the raw chain as well as the database.
    *
    * Will close the database before wiping. Database will be closed when this function returns.
    */
   virtual void wipe(const fc::path &shared_mem_dir, bool include_blocks);

   virtual void close(bool rewind = true) = 0;

   //////////////////// db_block.cpp ////////////////////

   /**
    *  @return true if the block is in our fork DB or saved to disk as
    *  part of the official chain, otherwise return false
    */
   virtual bool is_known_block(const block_id_type &id) const = 0;

   virtual bool is_known_transaction(const transaction_id_type &id) const = 0;

   virtual block_id_type get_block_id_for_num(uint32_t block_num) const = 0;

   virtual optional<signed_block> fetch_block_by_id(const block_id_type &id) const = 0;

   virtual optional<signed_block> fetch_block_by_number(uint32_t num) const = 0;

   virtual const signed_transaction get_recent_transaction(const transaction_id_type &trx_id) const = 0;

   virtual std::vector<block_id_type> get_block_ids_on_fork(block_id_type head_of_fork) const = 0;

   virtual const dynamic_global_property_object &get_dynamic_global_properties() const = 0;

   virtual const hardfork_property_object &get_hardfork_property_object() const = 0;

   virtual void add_checkpoints(const flat_map<uint32_t, block_id_type> &checkpts) = 0;

   virtual bool push_block(const signed_block &b, uint32_t skip) = 0;

   virtual void push_transaction(const signed_transaction &trx, uint32_t skip) = 0;

   virtual void _maybe_warn_multiple_production(uint32_t height) const = 0;

   virtual bool _push_block(const signed_block &b) = 0;

   virtual void _push_transaction(const signed_transaction &trx) = 0;

   /**
    *  This method is used to track applied operations during the evaluation of a block, these
    *  operations should include any operation actually included in a transaction as well
    *  as any implied/virtual operations that resulted, such as filling an order.
    *  The applied operations are cleared after post_apply_operation.
    */
   void notify_pre_apply_operation(operation_notification &note);

   void notify_post_apply_operation(const operation_notification &note);

   inline const void push_virtual_operation(const operation &op,
                                            bool force = false) // vops are not needed for low mem. Force will push them on low mem.
   {
      /*
      if( !force )
      {
         #if defined( IS_LOW_MEM ) && ! defined( IS_TEST_NET )
         return;
         #endif
      }
      */

      FC_ASSERT(is_virtual_operation(op));
      operation_notification note(op);
      ++_current_virtual_op;
      _current_trx_id = transaction_id_type();
      note.virtual_op = _current_virtual_op;
      notify_pre_apply_operation(note);
      notify_post_apply_operation(note);
   }

   void notify_applied_block(const signed_block &block);

   void notify_on_pending_transaction(const signed_transaction &tx);

   void notify_on_pre_apply_transaction(const signed_transaction &tx);

   void notify_on_applied_transaction(const signed_transaction &tx);

   /**
    *  This signal is emitted for plugins to process every operation after it has been fully applied.
    */
   boost::signals2::signal<void(const operation_notification &)> pre_apply_operation;
   boost::signals2::signal<void(const operation_notification &)> post_apply_operation;

   /**
    *  This signal is emitted after all operations and virtual operation for a
    *  block have been applied but before the get_applied_operations() are cleared.
    *
    *  You may not yield from this callback because the blockchain is holding
    *  the write lock and may be in an "inconstant state" until after it is
    *  released.
    */
   boost::signals2::signal<void(const signed_block &)> applied_block;

   /**
    * This signal is emitted any time a new transaction is added to the pending
    * block state.
    */
   boost::signals2::signal<void(const signed_transaction &)> on_pending_transaction;

   /**
    * This signla is emitted any time a new transaction is about to be applied
    * to the chain state.
    */
   boost::signals2::signal<void(const signed_transaction &)> on_pre_apply_transaction;

   /**
    * This signal is emitted any time a new transaction has been applied to the
    * chain state.
    */
   boost::signals2::signal<void(const signed_transaction &)> on_applied_transaction;

   /**
    *  Emitted After a block has been applied and committed.  The callback
    *  should not yield and should execute quickly.
    */
   //boost::signals2::signal<void(const vector< object_id_type >&)> changed_objects;

   /** this signal is emitted any time an object is removed and contains a
    * pointer to the last value of every object that was removed.
    */
   //boost::signals2::signal<void(const vector<const object*>&)>  removed_objects;

   //////////////////// db_init.cpp ////////////////////

   void set_custom_operation_interpreter(const uint32_t id, std::shared_ptr<custom_operation_interpreter> registry);

   std::shared_ptr<custom_operation_interpreter> get_custom_json_evaluator(const uint64_t id);

   time_point_sec head_block_time() const {
      return get_dynamic_global_properties().time;
   }

   uint32_t head_block_num() const {
      return get_dynamic_global_properties().head_block_number;
   }

   block_id_type head_block_id() const {
      return get_dynamic_global_properties().head_block_id;
   }
   uint32_t last_non_undoable_block_num() const {
      return get_dynamic_global_properties().last_irreversible_block_num;
   }

   bool has_hardfork(uint32_t hardfork) const {
      return get_hardfork_property_object().processed_hardforks.size() > hardfork;
   }

   /** when popping a block, the transactions that were removed get cached here so they
    * can be reapplied at the proper time */
   std::deque<signed_transaction> _popped_tx;
   vector<signed_transaction> _pending_tx;

   virtual void validate_invariants() const = 0;

   const std::string &get_json_schema() const {
      return _json_schema;
   }

   void set_flush_interval(uint32_t flush_blocks) {
      _flush_blocks = flush_blocks;
      _next_flush_block = 0;
   }

#ifdef IS_TEST_NET
   bool disable_low_mem_warning = true;
#endif

   typedef void on_reindex_start_t();

   typedef void on_reindex_done_t(bool, uint32_t);

   void on_reindex_start_connect(on_reindex_start_t functor) { _on_reindex_start.connect(functor); }

   void on_reindex_done_connect(on_reindex_done_t functor) { _on_reindex_done.connect(functor); }

   chain_id_type get_chain_id() const {
      return get_dynamic_global_properties().chain_id;
   }

   time_point_sec get_genesis_time() const {
      return get_dynamic_global_properties().genesis_time;
   }

protected:

   // this function needs access to _plugin_index_signal
   template<typename MultiIndexType>
   friend void add_plugin_index(const std::shared_ptr<database_interface> &db);

   boost::signals2::signal<void()> _plugin_index_signal;

   transaction_id_type _current_trx_id;
   uint32_t _current_block_num = 0;
   int32_t _current_trx_in_block = 0;
   uint16_t _current_op_in_trx = 0;
   uint16_t _current_virtual_op = 0;

   uint32_t _flush_blocks = 0;
   uint32_t _next_flush_block = 0;

   uint32_t _last_free_gb_printed = 0;

   uint16_t _shared_file_full_threshold = 0;
   uint16_t _shared_file_scale_rate = 0;

   flat_map<uint64_t, std::shared_ptr<custom_operation_interpreter> > _custom_operation_interpreters;
   std::string _json_schema;

   boost::signals2::signal<on_reindex_start_t> _on_reindex_start;
   boost::signals2::signal<on_reindex_done_t> _on_reindex_done;

};

}
}

#endif //SOPHIATX_DATABASE_INTERFACE_HPP
