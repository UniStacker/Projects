#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <initializer_list>
#include <iostream>
#include <algorithm>
#include <stdexcept>

template<typename Key, typename Value>
class OrderedMap {
  std::unordered_map<Key, Value> map;
  std::vector<Key> order;

  template<typename IterKey, typename IterMap>
  class iterator_base {
    using order_iter_t = IterKey;
    order_iter_t it;
    IterMap map_ptr;

  public:
    using map_type = typename std::remove_pointer<IterMap>::type;
    using value_type = typename map_type::value_type;

    using reference = typename std::conditional<
    std::is_const<map_type>::value,
    const value_type&,
    value_type&
    >::type;

    using pointer = typename std::conditional<
    std::is_const<map_type>::value,
    const value_type*,
    value_type*
    >::type;

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    iterator_base(order_iter_t i, IterMap m) : it(i), map_ptr(m) {}

    iterator_base& operator++() { ++it; return *this; }
    iterator_base operator++(int) { auto tmp = *this; ++(*this); return tmp; }

    bool operator==(const iterator_base& other) const { return it == other.it; }
    bool operator!=(const iterator_base& other) const { return it != other.it; }

    reference operator*() const {
      auto map_it = map_ptr->find(*it);
      return *map_it; // returns const or non-const pair& correctly
    }

    pointer operator->() const {
      auto map_it = map_ptr->find(*it);
      return &(*map_it);
    }

    order_iter_t raw_iterator() const { return it; }
  };

public:
  using iterator = iterator_base<typename std::vector<Key>::iterator, std::unordered_map<Key, Value>*>;
  using const_iterator = iterator_base<typename std::vector<Key>::const_iterator, const std::unordered_map<Key, Value>*>;

  OrderedMap() = default;

  OrderedMap(std::initializer_list<std::pair<const Key, Value>> init) {
    for (const auto &p : init) insert(p.first, p.second);
  }

  bool contains(const Key &k) const { return map.find(k) != map.end(); }

  void insert(const Key &k, const Value &v) {
    if (!contains(k)) order.push_back(k);
    map[k] = v;
  }

  void emplace_at(const Key &k, const Value &v, const size_t &pos) {
    if (pos < 0 || pos >= order.size())
      throw std::out_of_range("Invalid push idx");
    if (contains(k)) erase(k);
    order.insert(order.begin() + pos, k);
    map.emplace(k, v);
  }

  template<typename... Args>
  void emplace(const Key &k, Args&&... args) {
    if (!contains(k)) {
      order.push_back(k);
      map.emplace(k, Value(std::forward<Args>(args)...));
    }
  }

  void reserve(size_t n) {
    map.reserve(n);
    order.reserve(n);
  }

  Value& operator[](const Key &k) {
    if (!contains(k)) order.push_back(k);
    return map[k];
  }

  const Value& at(const Key &k) const { return map.at(k); }
  Value& at(const Key &k) { return map.at(k); }

  iterator begin() { return iterator(order.begin(), &map); }
  iterator end() { return iterator(order.end(), &map); }

  const_iterator begin() const { return const_iterator(order.begin(), &map); }
  const_iterator end() const { return const_iterator(order.end(), &map); }

  void erase(const Key &k) {
    auto it = map.find(k);
    if (it == map.end()) return;
    map.erase(it);
    order.erase(std::remove(order.begin(), order.end(), k), order.end());
  }

  iterator erase(iterator pos) {
    auto vec_it = pos.raw_iterator();
    if (vec_it == order.end()) return end();

    Key k = *vec_it;
    map.erase(k);
    auto next_it = order.erase(vec_it); // erase from vector and get next iterator
    return iterator(next_it, &map);
  }

  OrderedMap slice(size_t begin_idx, size_t end_idx) const {
    if (begin_idx > end_idx || end_idx > order.size())
      throw std::out_of_range("Invalid slice range");

    OrderedMap<Key, Value> result;
    result.reserve(end_idx - begin_idx);

    for (size_t i = begin_idx; i < end_idx; ++i) {
      const Key& k = order[i];
      result.insert(k, map.at(k));
    }
    return result;
  }

  void clear() {
    map.clear();
    order.clear();
  }

  size_t size() const { return map.size(); }
  bool empty() const { return map.empty(); }
  std::vector<Key> keys() { return order; }

  void print() const {
    for (const auto &k : order)
    std::cout << k << " => '" << map.at(k) << "'\n";
  }
};
