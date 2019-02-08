#pragma once
#include <sophiatx/plugins/account_history/account_history_plugin.hpp>
#include <sophiatx/plugins/json_rpc/json_rpc_plugin.hpp>

#define SOPHIATX_ACCOUNT_HISTORY_API_PLUGIN_NAME "account_history_api"


namespace sophiatx { namespace plugins { namespace account_history {

using namespace appbase;


class account_history_api_plugin : public plugin< account_history_api_plugin >
{
public:
   APPBASE_PLUGIN_REQUIRES(
      (sophiatx::plugins::account_history::account_history_plugin)
      (sophiatx::plugins::json_rpc::json_rpc_plugin)
   )

   account_history_api_plugin();
   virtual ~account_history_api_plugin();

   static const std::string& name() { static std::string name = SOPHIATX_ACCOUNT_HISTORY_API_PLUGIN_NAME; return name; }

   static void set_program_options( options_description& cli, options_description& cfg );

   virtual void plugin_initialize( const variables_map& options ) override;
   virtual void plugin_startup() override;
   virtual void plugin_shutdown() override;

   std::shared_ptr< class account_history_api > api;
};

} } } // sophiatx::plugins::account_history
