#include <sophiatx/plugins/custom_tokens/custom_tokens_plugin.hpp>

namespace sophiatx { namespace plugins { namespace custom_tokens {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif

extern "C" BOOST_SYMBOL_EXPORT std::shared_ptr<abstract_plugin> plugin_factory() {
   return std::make_shared<custom_tokens_plugin>();
}

extern "C" BOOST_SYMBOL_EXPORT void options_setter(options_description& cli, options_description& cfg) {
   custom_tokens_plugin::set_program_options(cli, cfg);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif


} } } // sophiatx::plugins::custom_tokens_plugin
