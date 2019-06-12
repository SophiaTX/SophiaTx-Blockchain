#include <appbase/application.hpp>
#include <sophiatx/manifest/plugins.hpp>

#include <sophiatx/protocol/types.hpp>
#include <sophiatx/protocol/version.hpp>

#include <sophiatx/utilities/key_conversion.hpp>
#include <sophiatx/utilities/git_revision.hpp>

#include <sophiatx/plugins/chain/chain_plugin_full.hpp>
#include <sophiatx/plugins/p2p/p2p_plugin.hpp>
#include <sophiatx/plugins/webserver/webserver_plugin.hpp>

#include <sophiatx/chain/database/database.hpp>

#include <fc/exception/exception.hpp>
#include <fc/thread/thread.hpp>
#include <fc/interprocess/signals.hpp>
#include <fc/git_revision.hpp>
#include <fc/stacktrace.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <csignal>
#include <vector>

#include <fc/crypto/rand.hpp>

namespace bpo = boost::program_options;
using sophiatx::protocol::version;
using std::string;
using std::vector;

string& version_string()
{
   static string v_str =
      "sophiatx_blockchain_version: " + std::string( SOPHIATX_BLOCKCHAIN_VERSION ) + "\n" +
      "sophiatx_git_revision:       " + std::string( sophiatx::utilities::git_revision_sha ) + "\n" +
      "fc_git_revision:          " + std::string( fc::git_revision_sha ) + "\n";
   return v_str;
}

void info()
{
#if IS_TEST_NET
      std::cerr << "------------------------------------------------------\n\n";
      std::cerr << "            STARTING TEST NETWORK\n\n";
      std::cerr << "------------------------------------------------------\n";
      auto initminer_private_key = sophiatx::utilities::key_to_wif( SOPHIATX_INIT_PRIVATE_KEY );
      std::cerr << "initminer public key: " << SOPHIATX_INIT_PUBLIC_KEY_STR << "\n";
      std::cerr << "initminer private key: " << initminer_private_key << "\n";
      std::cerr << "blockchain version: " << std::string( SOPHIATX_BLOCKCHAIN_VERSION ) << "\n";
      std::cerr << "------------------------------------------------------\n";
#else
    const auto& genesis = appbase::app().get_plugin< sophiatx::plugins::chain::chain_plugin_full >().get_genesis();
    std::cerr << "------------------------------------------------------\n\n";
    if(genesis.is_private_net) {
      std::cerr << "        STARTING SOPHIATX PRIVATE NETWORK\n\n";
    } else {
      std::cerr << "            STARTING SOPHIATX NETWORK\n\n";
    }
    std::cerr << "------------------------------------------------------\n";
    std::cerr << "initminer public key: " << std::string(genesis.initial_public_key) << "\n";
    std::cerr << "chain id: " << std::string( genesis.initial_chain_id) << "\n";
    std::cerr << "blockchain version: " << std::string( SOPHIATX_BLOCKCHAIN_VERSION ) << "\n";
    std::cerr << "------------------------------------------------------\n";

#endif
}

int main( int argc, char** argv )
{
   try
   {
      appbase::app().register_plugin<sophiatx::plugins::chain::chain_plugin_full>();
      sophiatx::plugins::register_plugins();

      // Reads main application config file
      if( !appbase::app().load_config(argc, argv)) {
          return 0;
      }

      auto& args = appbase::app().get_args();

      // Initializes logger
      fc::Logger::init("sophiatx"/* Do not change this parameter as syslog config depends on it !!! */, args.at("log-level").as< std::string >());

      appbase::app().set_version_string( version_string() );

      bool initialized = appbase::app().initialize<
            sophiatx::plugins::chain::chain_plugin_full,
            sophiatx::plugins::p2p::p2p_plugin,
            sophiatx::plugins::webserver::webserver_plugin >
            ( argc, argv );

      info();

      if( !initialized ) {
         return 0;
      }

       fc::ecc::public_key::init_cache(static_cast<uint32_t>(sophiatx::chain::sophiatx_config::get<uint32_t>("SOPHIATX_MAX_BLOCK_SIZE") / SOPHIATX_MIN_TRANSACTION_SIZE_LIMIT), std::chrono::milliseconds(2000));

      if( args.at( "backtrace" ).as< string >() == "yes" )
      {
         fc::print_stacktrace_on_segfault();
         ilog( "Backtrace on segfault is enabled." );
      }

      appbase::app().startup();
      appbase::app().exec();
      ilog("exited cleanly");

      return 0;
   }
   catch ( const boost::exception& e )
   {
      std::cerr << boost::diagnostic_information(e) << "\n";
   }
   catch ( const fc::exception& e )
   {
      std::cerr << e.to_detail_string() << "\n";
   }
   catch ( const std::exception& e )
   {
      std::cerr << e.what() << "\n";
   }
   catch ( ... )
   {
      std::cerr << "unknown exception\n";
   }

   return -1;
}
