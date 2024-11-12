//	STL-like templated LRU cache.
//
// Copyright (C) 2024 Daniel Butter <dbutter@gmail.com>
// Distributed under the MIT license.
//

/** \mainpage lru_cache.hh
    \author   Daniel Butter
    \version  1.00
    \date     2024-11-12
    \see      http://github.com/dpbutter/lru_cache.hh/

*/


#ifndef lru_cache_hh_
#define lru_cache_hh_

#include <map>
#include <list>
#include <stdexcept>
#include <iterator>

template <typename Key, typename Value, typename Compare>
class LRUcache {
public:
    using value_type = std::pair<Key, Value>;
    using List = std::list<value_type>;
    using iterator = typename List::iterator;
    using const_iterator = typename List::const_iterator;
    using size_type = size_t;


    LRUcache(size_t capacity) : max_size(capacity) {}

    // Access an item by key
    Value& at(const Key& key) {
        auto lookup_it = cache_lookup.find(key);
        if (lookup_it == cache_lookup.end()) {
            throw std::out_of_range("Key not found");
        }
        // Move accessed node to the front
        cache_list.splice(cache_list.begin(), cache_list, lookup_it->second);
        return lookup_it->second->second;
    }

    // Insert or update an item
    void insert(const Key& key, const Value& value) {
        auto lookup_it = cache_lookup.find(key);
        if (lookup_it != cache_lookup.end()) {
            lookup_it->second->second = value;
            cache_list.splice(cache_list.begin(), cache_list, lookup_it->second);
        } else {
            while (cache_list.size() >= max_size) {
                evict();
            }
            cache_list.emplace_front(key, value);
            cache_lookup[key] = cache_list.begin();
        }
    }

    // Implementing operator[] to access or insert into properties
    Value& operator[](const Key& key) {
        auto lookup_it = cache_lookup.find(key);
        if (lookup_it != cache_lookup.end()) {
            // If found, move it to the front of the list (LRU behavior)
            cache_list.splice(cache_list.begin(), cache_list, lookup_it->second);
            return lookup_it->second->second;
        } else {
            while (cache_list.size() >= max_size) {
                evict();
            }
            // Create new entry, inserting at the front of the list
            cache_list.emplace_front(key, Value());
            iterator new_iter = cache_list.begin();
            cache_lookup[key] = new_iter;
            return new_iter->second;
        }
    }

    // Check if a key exists (and moves the key to the front)
    bool contains(const Key& key) {
        // return cache_lookup.find(key) != cache_lookup.end();
        return find(key) != end();
    }

    // Find a key 
    iterator find(const Key& key) {
        auto lookup_it = cache_lookup.find(key);
        if (lookup_it == cache_lookup.end()) {
            return cache_list.end();  // Key not found
        }
        // Move accessed element to the front (most recently used)
        /** Make this conditional in case rule already at front? **/
        cache_list.splice(cache_list.begin(), cache_list, lookup_it->second);
        return cache_list.begin();
    }

    // Remove an item by key
    size_t erase(const Key& key) {
        auto lookup_it = cache_lookup.find(key);
        if (lookup_it != cache_lookup.end()) {
            cache_list.erase(lookup_it->second);
            cache_lookup.erase(lookup_it);
            return 1;
        }
        return 0;
    }

    // Remove an item by iterator
    iterator erase(const_iterator& it) {
        cache_lookup.erase(it->first);
        return cache_list.erase(it);
    }
    iterator erase(iterator& it) {
        cache_lookup.erase(it->first);
        return cache_list.erase(it);
    }


    // Clear the cache
    void clear() {
        cache_list.clear();
        cache_lookup.clear();
    }

    // Get the current size of the cache
    size_t size() const {
        assert(cache_list.size() == cache_lookup.size());
        return cache_list.size();
    }

    // Check if the cache is empty
    bool empty() const {
        return cache_list.empty();
    }

    // Iterators
    iterator begin() { return cache_list.begin(); }
    iterator end() { return cache_list.end(); }
    const_iterator begin() const { return cache_list.begin(); }
    const_iterator end() const { return cache_list.end(); }
    const_iterator cbegin() const { return cache_list.cbegin(); }
    const_iterator cend() const { return cache_list.cend(); }



private:

    List cache_list;  								 	// Doubly linked list of key-value pairs
    std::map<Key, iterator, Compare> cache_lookup;  	// Map to locate list nodes
    size_t max_size;

    void evict() {
        auto lru = cache_list.back().first;
        cache_lookup.erase(lru);
        cache_list.pop_back();
    }
};


#endif
