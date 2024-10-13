/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_BPTREE_MAP_HPP
#define DS_BPTREE_MAP_HPP

#include "./bptree_map_iterator.hpp"
#include "./compare.hpp"
#include "./types.hpp"
#include <cstring>
#include <type_traits>

#ifdef DS_TEST
#include "ds/vector.hpp"
#include <cstdio>
#endif

namespace ds {

/**
 * B+ Tree implementation
 * NOTE: No support for Key class type, might need in the future
 * NOTE: No implementation for strings yet, might need in the future
 *
 * Reference: https://en.wikipedia.org/wiki/B%2B_tree
 **/
template <
    typename Derived, typename Key, typename Value,
    typename KeyCompare = compare<Key>>
class base_bptree_map {
public:
  friend Derived;
  class inner_node;

  using key_type = Key;
  using value_type = Value;
  using iterator =
      bptree_map_iterator<base_bptree_map<Derived, Key, Value, KeyCompare>>;
  using citerator = bptree_map_iterator<
      const base_bptree_map<Derived, Key, Value, KeyCompare>>;

private:
  // === Definitions === //

  [[nodiscard]] static constexpr i32 get_degree() noexcept {
    // Minimum degree is 3
    return 64U / sizeof(Key) > 3U ? 64U / sizeof(Key) : 3U;
  }

  [[nodiscard]] constexpr i32 middle() const noexcept {
    return this->get_degree() / 2;
  }

  [[nodiscard]] constexpr i32 min_inner_children() const noexcept {
    // Ceiling function = adding the odd bit
    return this->get_degree() / 2 + (this->get_degree() & 1) - 1;
  }

  [[nodiscard]] constexpr i32 min_leaf_children() const noexcept {
    // Ceiling function = adding the odd bit
    return this->get_degree() / 2 + (this->get_degree() & 1);
  }

public:
  class leaf_node {
  public:
    leaf_node() = default;
    leaf_node(const leaf_node&) = delete;
    leaf_node& operator=(const leaf_node&) = delete;

    // === Move === //

    leaf_node(leaf_node&& other) noexcept;
    leaf_node& operator=(leaf_node&& other) noexcept;

    // === Destructor === //

    ~leaf_node() noexcept;
    void destroy() noexcept;

    // === Setters === //

    void set_parent(inner_node* parent) noexcept;
    void set_next(leaf_node* next) noexcept;
    void set_prev(leaf_node* prev) noexcept;

    // === Getters === //

    [[nodiscard]] Key* get_keys() noexcept {
      return this->keys;
    }

    [[nodiscard]] Value* get_values() noexcept {
      return this->values;
    }

    [[nodiscard]] i32 get_size() const noexcept {
      return this->size;
    }

    [[nodiscard]] inner_node* get_parent() const noexcept {
      return this->parent;
    }

    [[nodiscard]] leaf_node* get_next() const noexcept {
      return this->next;
    }

    [[nodiscard]] leaf_node* get_prev() const noexcept {
      return this->prev;
    }

    // === Element Access === //

    [[nodiscard]] Key at_key(i32 index) const noexcept;
    [[nodiscard]] Key front_key() const noexcept;
    [[nodiscard]] Key back_key() const noexcept;
    [[nodiscard]] Value& at_value(i32 index) const noexcept;

    // === Modifiers === //

    i32 insert(Key key, Value&& value) noexcept;
    void push(Key key, Value&& value) noexcept;
    void erase(i32 index) noexcept;
    void clear() noexcept;

    void redistribute(leaf_node* other, i32 mid) noexcept;
    void borrow_right_sibling() noexcept;
    void borrow_left_sibling() noexcept;
    void merge_right_sibling() noexcept;

    // === Lookup === //

    [[nodiscard]] Value* get_value(Key key) noexcept;
    [[nodiscard]] i32 find_index(Key key) const noexcept;
    [[nodiscard]] i32 find_smaller_index(Key key) const noexcept;
    [[nodiscard]] i32 find_larger_index(Key key) const noexcept;
    [[nodiscard]] i32 find_not_smaller_index(Key key) const noexcept;
    [[nodiscard]] i32 find_not_larger_index(Key key) const noexcept;

#ifdef DS_TEST
    void print() const noexcept {
      printf(
          "Leaf %p | parent %p ; %p %p ; %d[", this, this->parent, this->prev,
          this->next, this->size
      );
      for (i32 i = 0; i < this->size; ++i) {
        printf(" %x", this->values[i]);
      }
      printf(" ]\n");
    }
#endif

  private:
    Key keys[base_bptree_map::get_degree()]; // NOLINT
    i32 size = 0;

    inner_node* parent = nullptr;
    leaf_node* next = nullptr;
    leaf_node* prev = nullptr;

    Value values[base_bptree_map::get_degree()]; // NOLINT

    void insert_indexed(i32 index, Key key, Value&& value) noexcept;
  };

  class inner_node {
  public:
    inner_node() = default;
    inner_node(const inner_node&) = delete;
    inner_node& operator=(const inner_node&) = delete;

    // === Move === //

    inner_node(inner_node&&) = delete;
    inner_node& operator=(inner_node&&) = delete;

    // === Destructor === //

    ~inner_node() noexcept;
    void destroy() noexcept;
    void destroy_last_child() noexcept;
    void destroy_leaf_children() noexcept;

    // === Setters === //

    void set_parent(inner_node* parent) noexcept;

    // === Getters === //

    [[nodiscard]] Key* get_keys() noexcept {
      return this->keys;
    }

    /**
     * Can either be inner_node** or leaf_node**
     **/
    [[nodiscard]] void** get_children() noexcept {
      return this->children;
    }

    [[nodiscard]] i32 get_size() const noexcept {
      return this->size;
    }

    [[nodiscard]] inner_node* get_parent() const noexcept {
      return this->parent;
    }

    // === Element Access === //

    /**
     * First child that is less than the key, if all keys are smaller than the
     * key passed, this returns the last child
     **/
    [[nodiscard]] void* find_smaller_child(Key key) const noexcept;
    [[nodiscard]] i32 find_smaller_index(Key key) const noexcept;

    [[nodiscard]] Key at_key(i32 index) const noexcept;
    [[nodiscard]] Key front_key() const noexcept;
    [[nodiscard]] Key back_key() const noexcept;
    [[nodiscard]] void* front_child() const noexcept;
    [[nodiscard]] void* back_child() const noexcept;

    // === Modifiers === //

    void insert(Key key, void* child) noexcept;
    void push_front(Key key, void* child) noexcept;
    void pop_front() noexcept;
    void push(Key key, void* child) noexcept;
    void pop_back() noexcept;
    void erase(i32 index) noexcept;
    void set_key(i32 index, Key key) noexcept;

    // === Reparenting === //

    void reparent_children() noexcept;

    // === Inserting === //

    void redistribute(inner_node* other, i32 mid) noexcept;

    // === Erasing === //

    void
    borrow_from_right_uncle(inner_node* right_uncle, i32 parent_index) noexcept;
    void
    borrow_from_left_uncle(inner_node* left_uncle, i32 parent_index) noexcept;

    // Merges index and index + 1
    void merge_children(i32 index) noexcept;

#ifdef DS_TEST
    void print() const noexcept {
      printf(
          "Inner %p | parent: %p ; children %d[", this, this->parent,
          this->size + 1
      );
      for (i32 i = 0; i <= this->size; ++i) {
        printf(
            " (%x:%p)", i == this->size ? 0U : this->keys[i], this->children[i]
        );
      }
      printf(" ]\n");
    }
#endif

  private:
    Key keys[base_bptree_map::get_degree()]; // NOLINT
    i32 size = 0;

    inner_node* parent = nullptr;

    void* children[base_bptree_map::get_degree() + 1]{}; // NOLINT

    void insert_indexed(i32 index, Key key, void* child) noexcept;
  };

  friend leaf_node;
  friend inner_node;

  base_bptree_map() noexcept = default;
  base_bptree_map(const base_bptree_map&) = delete;
  base_bptree_map& operator=(const base_bptree_map&) = delete;

  // === Move === //

  base_bptree_map(base_bptree_map&& rhs) noexcept;
  base_bptree_map& operator=(base_bptree_map&& rhs) noexcept;

  // === Destructor === //

  ~base_bptree_map() noexcept;

  // === Element Access === //

  /**
   * Safe Element Access
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is not found in the tree
   **/
  [[nodiscard]] expected<value_type*, error_code> at(Key key) const noexcept;

  /**
   * Get the first element less than the key
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is smaller than the smallest element
   **/
  [[nodiscard]] expected<value_type*, error_code> at_smaller(Key key
  ) const noexcept;

  /**
   * Get the first element greater than the key
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is larger than the largest element
   **/
  [[nodiscard]] expected<value_type*, error_code> at_larger(Key key
  ) const noexcept;

  /**
   * Get the first element not less than (greater than or equal) the key
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is larger than the greater element
   **/
  [[nodiscard]] expected<value_type*, error_code> at_not_smaller(Key key
  ) const noexcept;

  /**
   * Get the first element not greater than (less than or equal) the key
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is smaller than the smallest element
   **/
  [[nodiscard]] expected<value_type*, error_code> at_not_larger(Key key
  ) const noexcept;

  /**
   * Unsafe Element Access
   * Returns a nullptr if the data is not found in the tree
   *
   * @return value_ptr
   **/
  [[nodiscard]] Value* operator[](Key key) noexcept;

  // === Iterators === //

  /**
   * Returns the first element of the bptree map
   **/
  [[nodiscard]] iterator begin() noexcept;

  /**
   * Returns the first element of the bptree map
   **/
  [[nodiscard]] citerator cbegin() const noexcept;

  /**
   * Returns the end of the bptree map
   **/
  [[nodiscard]] iterator end() noexcept;

  /**
   * Returns the end of the bptree map
   **/
  [[nodiscard]] citerator cend() const noexcept;

  /**
   * Returns the last element of the bptree map
   **/
  [[nodiscard]] iterator rbegin() noexcept;

  /**
   * Returns the last element of the bptree map
   **/
  [[nodiscard]] citerator crbegin() const noexcept;

  /**
   * Returns the end of the bptree map
   **/
  [[nodiscard]] iterator rend() noexcept;

  /**
   * Returns the end of the bptree map
   **/
  [[nodiscard]] citerator crend() const noexcept;

  // === Capacity === //

  /**
   * Checks if the bptree is empty
   *
   * @return bool
   **/
  [[nodiscard]] bool is_empty() const noexcept;

  /**
   * Returns the number elements in the bptree
   **/
  [[nodiscard]] usize get_size() const noexcept;

  // === Modifiers === //

  /**
   * Removes all elements in the map
   **/
  void clear() noexcept;

  /**
   * Inserts a value into the bptree
   *
   * @errors
   *  - bad allocation in creating the containers
   *  - bad allocation in copying the key or value
   **/
  [[nodiscard]] error_code insert(Key key, const Value& value) noexcept {
    return this->insert_impl<const Value&>(key, value);
  }

  /**
   * Inserts a value into the bptree
   *
   * @errors
   *  - bad allocation in creating the containers
   *  - bad allocation in copying the key
   **/
  [[nodiscard]] error_code insert(Key key, Value&& value) noexcept {
    return this->insert_impl<Value&&>(key, std::move(value));
  }

  /*
   * Erases an element in the bptree
   * If the element does not exist in the bptree, then this will not change
   * anything in the bptree and no error is thrown
   **/
  void erase(Key key) noexcept;

  // === Lookup === //

  /**
   * Returns the iterator with the corresponding key.
   * If it does not find an exact match, it will return
   * the end of the iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find(Key key) noexcept;

  /**
   * Returns the iterator with the corresponding key.
   * If it does not find an exact match, it will return
   * the end of the iterator.
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find(Key key) const noexcept;

  /**
   * Returns an iterator to the first element less than the key.
   * If the key is smaller than the smallest key, then this returns
   * the end iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_smaller(Key key) noexcept;

  /**
   * Returns an iterator to the first element less than the key.
   * If the key is smaller than the smallest key, then this returns
   * the end iterator.
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find_smaller(Key key) const noexcept;

  /**
   * Returns an iterator to the first element greater than the key.
   * If the key is larger than the largest key, then this returns
   * the end iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_larger(Key key) noexcept;

  /**
   * Returns an iterator to the first element greater than the key.
   * If the key is larger than the largest key, then this returns
   * the end iterator.
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find_larger(Key key) const noexcept;

  /**
   * Returns an iterator to the first element not less than (greater than or
   * equal) the key. If the key is larger than the largest key, then this
   *returns the end iterator
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_not_smaller(Key key) noexcept;

  /**
   * Returns an iterator to the first element not less than (greater than or
   * equal) the key. If the key is larger than the largest key, then this
   *returns the end iterator
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find_not_smaller(Key key) const noexcept;

  /**
   * Returns an iterator to the first element not greater than (less than or
   * equal) the key. If the key is smaller than the smallest key, then this
   * returns the end iterator
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_not_larger(Key key) noexcept;

  /**
   * Returns an iterator to the first element not greater than (less than or
   * equal) the key. If the key is smaller than the smallest key, then this
   * returns the end iterator
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find_not_larger(Key key) const noexcept;

  /**
   * Checks if the key exists in the bptree_map
   *
   * @return bool
   **/
  [[nodiscard]] bool contains(Key key) const noexcept;

#if DS_TEST
  void print_inner_nodes() const noexcept {
    if (this->height <= 1) {
      printf("No inner nodes\n");
      return;
    }

    ds::vector<inner_node*> stack{};
    ds::vector<inner_node*> next{};
    ds::error_code error = stack.push((inner_node*)this->root);

    for (usize i = this->height; i > 1; --i) {
      printf("=== Map Height " USIZE_FORMAT " ===\n", i);

      for (auto* node : stack) {
        node->print();
        if (i > 2) {
          for (i32 j = 0; j <= node->get_size(); ++j) {
            error = next.push((inner_node*)node->get_children()[j]);
          }
        }
      }

      stack = std::move(next);
    }
  }

  void print_leaf_nodes() const noexcept {
    if (this->height == 0) {
      printf("No leaf nodes\n");
      return;
    }

    leaf_node* leaf = nullptr;
    if (this->height == 1) {
      leaf = (leaf_node*)this->root;
    } else {
      auto* node = (inner_node*)this->root;
      for (usize i = 2; i < this->height; ++i) {
        node = (inner_node*)node->front_child();
      }

      leaf = (leaf_node*)node->front_child();
    }

    while (leaf != nullptr) {
      leaf->print();
      leaf = leaf->get_next();
    }
  }
#endif

private:
  void* root = nullptr;
  usize size = 0U;
  usize height = 0U;

  constexpr void move(leaf_node&& other) noexcept {
    if constexpr (std::is_class<Key>::value || std::is_class<Value>::value) {
      if constexpr (std::is_class<Key>::value) {
        for (u32 i = 0; i < this->size; ++i) {
          this->get_keys()[i] = std::move(other.get_keys()[i]);
        }
      } else {
        std::memcpy(
            this->get_keys(), other.get_keys(),
            sizeof(Key) *
                base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree()
        );
      }

      if constexpr (std::is_class<Value>::value) {
        for (u32 i = 0; i < this->size; ++i) {
          this->get_values()[i] = std::move(other.get_values()[i]);
        }
      } else {
        std::memcpy(
            this->get_values(), other.get_values(),
            sizeof(Value) *
                base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree()
        );
      }
    } else {
      std::memcpy(
          this->get_keys(), other.get_keys(),
          (sizeof(Key) + sizeof(Value)) *
              base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree()
      );
    }
  }

  // === Lookup === //

  /**
   * Finds the leaf node containing the key we want to find
   * This requires the tree to be height >= 1
   **/
  [[nodiscard]] leaf_node* find_leaf_node_containing(Key key) const noexcept;

  // === Insert Helpers === //

  /**
   * Creates a internal node, if an error happens, this will return nullptr
   *
   * @errors
   *  - bad allocation in creating the node
   *  - bad allocation in copying the key
   *
   * @return inner_ptr
   **/
  [[nodiscard]] inner_node* create_inner_node(void* child) noexcept;

  /**
   * Creates a leaf node, if an error happens, this will return nullptr
   *
   * @errors
   *  - bad allocation in creating the node
   *  - bad allocation in copying the key
   *
   * @return leaf_node*
   **/
  [[nodiscard]] leaf_node* create_leaf_node() noexcept;

  /**
   * Splits the internal node
   *
   * @errors
   *  error_code::BAD_ALLOCATION
   *    - bad allocation in creating the new containers
   *    - bad allocation in copying the key
   **/
  [[nodiscard]] error_code split_inner_node(inner_node* left_node) noexcept;

  /**
   * Splits the leaf node
   *
   * @errors
   *  - bad allocation in creating the new containers
   *  - bad allocation in copying the key
   **/
  [[nodiscard]] error_code split_leaf_node(leaf_node* left_leaf) noexcept;

  template <typename Value_>
  [[nodiscard]] error_code insert_impl(Key key, Value_ value) noexcept;

  // === Erase Helpers === //

  inline void erase_traversal(
      Key key, inner_node*& ancestor, i32& ancestor_index, leaf_node*& leaf,
      i32& index
  ) noexcept {
    auto* node = static_cast<inner_node*>(this->root);

    i32 sz = 0;
    Key* keys = nullptr;
    isize comparison = 0;

    // Traverse the internal nodes until the parent containing the leaf nodes
    for (usize h = this->height; h > 2; --h) {
      sz = node->get_size();
      keys = node->get_keys();

      for (index = 0; index < sz; ++index) {
        comparison = KeyCompare{}(key, keys[index]);
        if (comparison == 0) {
          ancestor = node;
          ancestor_index = index;

          // Fast traversal since ancestors have equivalent leaf nodes
          // No need for any comparison
          node = static_cast<inner_node*>(node->get_children()[index + 1]);
          index = 0U;
          for (; h > 3; --h) {
            node = static_cast<inner_node*>(node->get_children()[0]);
          }
          leaf = static_cast<leaf_node*>(node->get_children()[0]);
          break;
        }

        if (comparison < 0) {
          break;
        }
      }

      node = static_cast<inner_node*>(node->get_children()[index]);
    }

    if (ancestor == nullptr) {
      // Traverse the parent containing leaf nodes
      sz = node->get_size();
      keys = node->get_keys();
      for (index = 0; index < sz; ++index) {
        comparison = KeyCompare{}(key, keys[index]);
        if (comparison == 0) {
          ancestor = node;
          ancestor_index = index;
          ++index;
          break;
        }

        if (comparison < 0) {
          break;
        }
      }
      leaf = static_cast<leaf_node*>(node->get_children()[index]);
    }
  }

  /**
   * Tries to borrow from siblings or merge inner nodes,
   * this will also propagate to the parent nodes
   **/
  void borrow_or_merge_inner(inner_node* parent, Key key) noexcept;

  /**
   * Tries to borrows from siblings or merge leaf nodes,
   * this will also propagate to the parent nodes
   **/
  void borrow_or_merge_leaf(leaf_node* leaf) noexcept;

  /**
   * Erases the root node being a leaf node
   **/
  void erase_root_leaf(Key key) noexcept;
};

} // namespace ds

#ifndef DS_BPTREE_MAP_NODE_HPP
#include "./bptree_map_node.hpp"
#endif

#ifndef DS_BPTREE_MAP_TPP
#include "./bptree_map.tpp"
#endif

namespace ds {

template <typename Key, typename Value, typename KeyCompare = compare<Key>>
class bptree_map
    : public base_bptree_map<
          bptree_map<Key, Value, KeyCompare>, Key, Value, KeyCompare> {};

} // namespace ds

#endif
