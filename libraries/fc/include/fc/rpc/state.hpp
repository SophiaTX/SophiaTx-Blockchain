#pragma once
#include <fc/variant.hpp>
#include <functional>
#include <fc/thread/future.hpp>

namespace fc { namespace rpc {
   struct request
   {
      std::string         jsonrpc = "2.0";
      std::optional<uint64_t>  id;
      std::string         method;
      variants            params;
   };

   struct error_object
   {
      int64_t           code;
      std::string       message;
      std::optional<variant> data;
   };

   struct response
   {
      response(){}
      response( int64_t i, fc::variant r ):id(i),result(r){}
      response( int64_t i, error_object r ):id(i),error(r){}
      int64_t                id = 0;
      std::optional<fc::variant>  result;
      std::optional<error_object> error;
   };

   class state
   {
      public:
         typedef std::function<variant(const variants&)>       method;
         ~state();

         void add_method( const std::string& name, method m );
         void remove_method( const std::string& name );

         variant local_call( const std::string& method_name, const variants& args );
         void    handle_reply( const response& response );

         request start_remote_call( const std::string& method_name, variants args );
         variant wait_for_response( uint64_t request_id );

         void close();

         void on_unhandled( const std::function<variant(const std::string&,const variants&)>& unhandled );

      private:
         uint64_t                                                   _next_id = 1;
         std::unordered_map<uint64_t, fc::promise<variant>::ptr>    _awaiting;
         std::unordered_map<std::string, method>                    _methods;
         std::function<variant(const std::string&,const variants&)>                    _unhandled;
   };
} }  // namespace  fc::rpc

FC_REFLECT( fc::rpc::request, (jsonrpc)(id)(method)(params) );
FC_REFLECT( fc::rpc::error_object, (code)(message)(data) )
FC_REFLECT( fc::rpc::response, (id)(result)(error) )
