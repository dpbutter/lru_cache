# lru_cache

A simple STL-like LRU cache for C++ using `std::list` and `std::map`. 

Unlike other LRU implementations, this one uses `std::map` rather than `std::unordered_map` for storing iterators into the linked list. This is to enable the use of weak pointers.

Declaration:

```cpp
LRUcache<key_t, value_t, compare>  cache(max_size);
```

`compare` is a comparison function object of the type, which is used for the underlying `std::map`.
