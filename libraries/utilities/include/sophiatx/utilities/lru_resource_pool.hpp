#ifndef LRU_RESOURCE_POOL_HPP
#define LRU_RESOURCE_POOL_HPP

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/optional.hpp>
#include <chrono>
#include <iostream>

namespace sophiatx { namespace utilities {


/**
 * @brief Non-thread-safe resource pool. It stores maximum <max_resources_> resources of type "ValueType", which are mapped to the keys of type "KeyType".
 *        In case max. num of resources is reached, the least used one is deleted and replaced with the one, which is needed at the moment.
 *
 * @tparam KeyType
 * @tparam ValueType
 */
template<typename KeyType, typename ValueType>
class LruResourcePool {
public:
   LruResourcePool(uint32_t max_resources_count) :
      max_resources_(max_resources_count),
      cache_resources(),
      cache_resources_by_key(cache_resources.template get<by_key>()),
      cache_resources_by_last_access(cache_resources.template get<by_last_access>())
   {}


   /**
    * @brief Creates new resource mapped to the key
    *
    * @tparam ArgsType
    * @param key that new resource is going to be mapped to
    * @param args arguments forwarded to the constructor of newly created resource
    * @return ValueType& - reference to the newly created resource of type "ValueType"
    */
   template <typename... ArgsType>
   ValueType& createResource(const KeyType& key, ArgsType... args) {
      auto resource = cache_resources_by_key.find(key);
      if (resource != cache_resources_by_key.end()) {
         // Adjusts last_access of the found resource
         if (cache_resources_by_key.modify(resource, [](Resource& resource) { resource.updateAccessTime(); }) == false) {
            throw std::runtime_error("Unable to modify last access time of resource");
         }

         return resource->getValue();
      }

      if (cache_resources_by_key.size() >= max_resources_) {
         popResource();
      }

      // Creates new database handle inside pool
      auto emplace_result = cache_resources_by_key.emplace(key, std::forward<ArgsType>(args)...);
      if (emplace_result.second == false) {
         throw std::runtime_error("Unable to create resource");
      }

      return emplace_result.first->getValue();
   }

   /**
    * @brief Returns resource(boost::optional<ValueType&>) mapped to the provided key. In case no such resource exist, empty boost::optional is returned. Otherwise
    *        it returnes optional with reference to the resource
    *
    * @param key
    * @return boost::optional<ValueType&>
    */
   boost::optional<ValueType&> getResource(const KeyType& key) {
      boost::optional<ValueType&> ret;

      auto resource = cache_resources_by_key.find(key);
      if (resource == cache_resources_by_key.end()) {
         return ret;
      }

      // Adjusts last_access of the found resource
      if (cache_resources_by_key.modify(resource, [](Resource& resource) { resource.updateAccessTime(); }) == false) {
         throw std::runtime_error("Unable to modify last access time of resource");
      }
      

      ret = resource->getValue();
      return ret;
   }

   /**
    * @brief Returns resource(ValueType&) mapped to the provided key. In case no such resource exist, it creates new one.
    *
    * @tparam ArgsType
    * @param key
    * @param args arguments forwarded to the constructor of newly created resource, if it needs to be created.
    * @return ValueType& - reference to the resource of type "ValueType"
    */
   template <typename... ArgsType>
   ValueType& getResourceAut(const KeyType& key, ArgsType... args) {
      return createResource(key, std::forward<ArgsType>(args)...);
   }

   /**
    * @return actual number of resources in the pool
    */
   const uint32_t size() const {
      return cache_resources.size();
   }

   /**
    * @brief Pops the least used resource
    */
   void popResource() {
      if (cache_resources_by_key.empty() == false) {
         cache_resources_by_key.erase(cache_resources_by_key.begin());
      }
   }

   /**
    * @brief Erases resource mapped to the key
    *
    * @param key
    */
   void eraseResource(const KeyType& key) {
      auto resource = cache_resources_by_key.find(key);
      if (resource != cache_resources_by_key.end()) {
         cache_resources_by_key.erase(resource);
      }
   }

private:
   class Resource {
   public:
      /**
       * Class members are public because it is necessary for usage in boost::multi_index_container.
       */
      KeyType                                key_;
      mutable ValueType                      value_;
      std::chrono::system_clock::time_point  last_access_;

      template <typename... ArgsType>
      Resource(const KeyType& key, ArgsType... args) :
            key_(key),
            value_(std::forward<ArgsType>(args)...),
            last_access_(std::chrono::system_clock::now())
      {}

      ~Resource() = default;

      // Disables default/copy/move ctors - resources should not be copied
      Resource()                             = delete;
      Resource(const Resource &)            = delete;
      Resource &operator=(const Resource &) = delete;
      Resource(Resource &&)                 = delete;
      Resource &operator=(Resource &&)      = delete;

      void updateAccessTime() { last_access_ = std::chrono::system_clock::now(); }

      /*
       * getValue() always returns ValueType& and not const ValueType&. This value can be changed. Only values that are multiindex container
       * keys should never be changed
       */
      ValueType& getValue()         { return value_; }
      ValueType& getValue() const   { return value_; }
   };



   /**
    * @brief Defines boost multiindex container that stores resource objects
    */
   struct by_key;
   struct by_last_access;

   using cache_resources_index =
   boost::multi_index::multi_index_container<
         Resource,
         boost::multi_index::indexed_by<
               boost::multi_index::ordered_unique<boost::multi_index::tag<by_key>, boost::multi_index::member<Resource, KeyType, &Resource::key_> >,
               boost::multi_index::ordered_non_unique<boost::multi_index::tag<by_last_access>, boost::multi_index::member<Resource, std::chrono::system_clock::time_point, &Resource::last_access_> >
         >
   >;
   using cache_resources_by_key_index           = typename cache_resources_index::template index<by_key>::type;
   using cache_resources_by_last_access_index   = typename cache_resources_index::template index<by_last_access>::type;

   // maximum number of resources
   uint32_t                               max_resources_;
   cache_resources_index                  cache_resources;
   cache_resources_by_key_index&          cache_resources_by_key;
   cache_resources_by_last_access_index&  cache_resources_by_last_access;
};

}}
#endif //LRU_RESOURCE_POOL_HPP