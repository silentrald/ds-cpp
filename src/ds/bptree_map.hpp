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
#include "./vector.hpp"
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
  using const_iterator = bptree_map_iterator<
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
    void remove(i32 index) noexcept;
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
    void remove(i32 index) noexcept;
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

  base_bptree_map(base_bptree_map&& rhs) noexcept
      : root(rhs.root), size(rhs.size), height(height) {
    rhs.root = nullptr;
    rhs.size = rhs.height = 0;
  }

  base_bptree_map& operator=(base_bptree_map&& rhs) noexcept {
    if (&rhs == this) {
      return *this;
    }

    this->root = rhs.root;
    this->size = rhs.size;
    this->height = rhs.height;

    rhs.root = nullptr;
    rhs.size = rhs.height = 0;

    return *this;
  }

  // === Destructor === //

  ~base_bptree_map() noexcept {
    this->clear();
  }

  void destroy() noexcept {
    this->clear();
  }

  /**
   * Removes all elements in the map
   **/
  void clear() noexcept {
    if (this->height == 0) {
      return;
    }

    if (this->height == 1) {
      static_cast<leaf_node*>(this->root)->clear();
      std::free(this->root); // NOLINT

      this->root = nullptr;
      this->size = this->height = 0;
      return;
    }

    auto* node = static_cast<inner_node*>(this->root);
    if (this->height == 2) {
      node->destroy_leaf_children();
      std::free(node); // NOLINT

      this->root = nullptr;
      this->size = this->height = 0;
      return;
    }

    // State based deletion, DFS on the right side
    usize h = this->height;
    while (true) {
      // Inner node containing leaf nodes
      if (h == 2) {
        node->destroy_leaf_children();

        node = node->get_parent();
        node->destroy_last_child();

        ++h;
        continue;
      }

      // If the inner node has children, traverse the last child
      if (node->get_size() > -1) {
        node = static_cast<inner_node*>(node->back_child());
        --h;

        continue;
      }

      // The inner node doesn't have any more children so delete the node
      if (node->get_parent() != nullptr) {
        node = node->get_parent();
        node->destroy_last_child();
        ++h;
        continue;
      }

      // This is the root node
      std::free(this->root); // NOLINT
      break;
    }

    this->root = nullptr;
    this->size = this->height = 0;
  }

  // === Element Access === //

  /**
   * Safe Element Access
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is not found in the tree
   **/
  [[nodiscard]] expected<value_type*, error_code> at(Key key) const noexcept {
    if (this->height == 0) {
      return unexpected{error_code::CONTAINER_EMPTY};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    Value* value = leaf->get_value(key);

    if (value == nullptr) {
      return unexpected{error_code::NOT_FOUND};
    }
    return value;
  }

  /**
   * Get the first element less than the key
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is smaller than the smallest element
   **/
  [[nodiscard]] expected<value_type*, error_code> at_smaller(Key key
  ) const noexcept {
    if (this->height == 0) {
      return error_code::CONTAINER_EMPTY;
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_smaller_index(key);
    return index > -1 ? &leaf->at_value(index) : error_code::NOT_FOUND;
  }

  /**
   * Get the first element greater than the key
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is larger than the largest element
   **/
  [[nodiscard]] expected<value_type*, error_code> at_larger(Key key
  ) const noexcept {
    if (this->height == 0) {
      return error_code::CONTAINER_EMPTY;
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_larger_index(key);
    if (index > -1) {
      return &leaf->at_value(index);
    }

    leaf = leaf->get_next();
    if (leaf == nullptr) {
      return error_code::NOT_FOUND;
    }

    return &leaf->at_value(0);
  }

  /**
   * Get the first element not less than (greater than or equal) the key
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is larger than the greater element
   **/
  [[nodiscard]] expected<value_type*, error_code> at_not_smaller(Key key
  ) const noexcept {
    if (this->height == 0) {
      return error_code::CONTAINER_EMPTY;
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_not_smaller_index(key);
    if (index > -1) {
      return &leaf->at_value(index);
    }

    leaf = leaf->get_next();
    if (leaf == nullptr) {
      return error_code::NOT_FOUND;
    }

    return &leaf->at_value(0);
  }

  /**
   * Get the first element not greater than (less than or equal) the key
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY - tree is empty
   *  - error_code::NOT_FOUND - key is smaller than the smallest element
   **/
  [[nodiscard]] expected<value_type*, error_code> at_not_larger(Key key
  ) const noexcept {
    if (this->height == 0) {
      return error_code::CONTAINER_EMPTY;
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_not_larger_index(key);
    if (index == -1) {
      return error_code::CONTAINER_EMPTY;
    }

    return &leaf->at_value(index);
  }

  /**
   * Unsafe Element Access
   * Returns a nullptr if the data is not found in the tree
   *
   * @return value_ptr
   **/
  [[nodiscard]] Value* operator[](Key key) noexcept {
    if (this->height == 0) {
      return nullptr;
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    return leaf->get_value(key);
  }

  // === Iterators === //

  /**
   * Returns the first element of the bptree map
   **/
  [[nodiscard]] iterator begin() noexcept {
    if (this->height == 0) {
      return iterator{};
    }

    if (this->height == 1) {
      return iterator{static_cast<leaf_node*>(this->root)};
    }

    auto* node = static_cast<inner_node*>(this->root);
    for (usize h = this->height; h > 2; --h) {
      node = static_cast<inner_node*>(node->front_child());
    }

    return iterator{static_cast<leaf_node*>(node->front_child())};
  }

  /**
   * Returns the first element of the bptree map
   **/
  [[nodiscard]] const_iterator cbegin() const noexcept {
    if (this->height == 0) {
      return const_iterator{};
    }

    if (this->height == 1) {
      return const_iterator{static_cast<leaf_node*>(this->root)};
    }

    auto* node = static_cast<inner_node*>(this->root);
    for (usize h = this->height; h > 2; --h) {
      node = static_cast<inner_node*>(node->front_child());
    }

    return const_iterator{static_cast<leaf_node*>(node->front_child())};
  }

  /**
   * Returns the end of the bptree map
   **/
  [[nodiscard]] iterator end() noexcept {
    return iterator{};
  }

  /**
   * Returns the end of the bptree map
   **/
  [[nodiscard]] const_iterator cend() const noexcept {
    return const_iterator{};
  }

  /**
   * Returns the last element of the bptree map
   **/
  [[nodiscard]] iterator rbegin() noexcept {
    if (this->height == 0) {
      return iterator{};
    }

    if (this->height == 1) {
      return iterator{static_cast<leaf_node*>(this->root), this->size - 1};
    }

    auto* node = static_cast<inner_node*>(this->root);
    for (usize h = this->height; h > 2; --h) {
      node = static_cast<inner_node*>(node->back_child());
    }

    auto* leaf = static_cast<leaf_node*>(node->back_child());
    return iterator{leaf, leaf->get_size() - 1};
  }

  /**
   * Returns the last element of the bptree map
   **/
  [[nodiscard]] const_iterator crbegin() const noexcept {
    if (this->height == 0) {
      return iterator{};
    }

    if (this->height == 1) {
      return iterator{static_cast<leaf_node*>(this->root), this->size - 1};
    }

    auto* node = static_cast<inner_node*>(this->root);
    for (usize h = this->height; h > 2; --h) {
      node = static_cast<inner_node*>(node->back_child());
    }

    auto* leaf = static_cast<leaf_node*>(node->back_child());
    return iterator{leaf, leaf->get_size() - 1};
  }

  /**
   * Returns the end of the bptree map
   **/
  [[nodiscard]] iterator rend() noexcept {
    return iterator{};
  }

  /**
   * Returns the end of the bptree map
   **/
  [[nodiscard]] const_iterator crend() const noexcept {
    return iterator{};
  }

  // === Capacity === //

  /**
   * Checks if the bptree is empty
   *
   * @return bool
   **/
  [[nodiscard]] bool is_empty() const noexcept {
    return this->size == 0U;
  }

  /**
   * Returns the number elements in the bptree
   **/
  [[nodiscard]] usize get_size() const noexcept {
    return this->size;
  }

  // === Modifiers === //

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
  void remove(Key key) noexcept {
    if (this->height == 0) {
      return;
    }

    if (this->height == 1) {
      this->remove_root_leaf(key);
      return;
    }

    // Just try to handle leaf deletion
    i32 index = 0;
    leaf_node* leaf = nullptr;

    // Ancestor same key value with the leaf node
    inner_node* ancestor = nullptr;
    i32 ancestor_index = 0;

    this->remove_traversal(key, ancestor, ancestor_index, leaf, index);

    if (ancestor != nullptr) {
      if constexpr (this->get_degree() > 3) {
        ancestor->get_keys()[ancestor_index] = leaf->get_keys()[1];
      }

      index = 0U;
    } else {
      index = leaf->find_index(key);
      if (index == -1) {
        // Nothing is deleted
        return;
      }
    }

    --this->size;
    leaf->remove(index);
    if (leaf->get_size() < this->min_leaf_children()) {
      this->borrow_or_merge_leaf(leaf);
    }
  }

  // === Lookup === //

  /**
   * Returns the iterator with the corresponding key.
   * If it does not find an exact match, it will return
   * the end of the iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find(Key key) noexcept {
    if (this->height == 0) {
      return iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_index(key);
    return index > -1 ? iterator{leaf, index} : iterator{};
  }

  /**
   * Returns the iterator with the corresponding key.
   * If it does not find an exact match, it will return
   * the end of the iterator.
   **/
  [[nodiscard]] const_iterator find(Key key) const noexcept {
    if (this->height == 0) {
      return const_iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_index(key);
    return index > -1 ? const_iterator{leaf, index} : const_iterator{};
  }

  /**
   * Returns an iterator to the first element less than the key.
   * If the key is smaller than the smallest key, then this returns
   * the end iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_smaller(Key key) noexcept {
    if (this->height == 0) {
      return iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_smaller_index(key);
    return index > -1 ? iterator{leaf, index} : iterator{};
  }

  /**
   * Returns an iterator to the first element less than the key.
   * If the key is smaller than the smallest key, then this returns
   * the end iterator.
   **/
  [[nodiscard]] const_iterator find_smaller(Key key) const noexcept {
    if (this->height == 0) {
      return const_iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_smaller_index(key);
    return index > -1 ? const_iterator{leaf, index} : const_iterator{};
  }

  /**
   * Returns an iterator to the first element greater than the key.
   * If the key is larger than the largest key, then this returns
   * the end iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_larger(Key key) noexcept {
    if (this->height == 0) {
      return iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_larger_index(key);
    if (index > -1) {
      return iterator{leaf, index};
    }

    leaf = leaf->get_next();
    return leaf ? iterator{leaf, 0} : iterator{};
  }

  /**
   * Returns an iterator to the first element greater than the key.
   * If the key is larger than the largest key, then this returns
   * the end iterator.
   **/
  [[nodiscard]] const_iterator find_larger(Key key) const noexcept {
    if (this->height == 0) {
      return const_iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_larger_index(key);
    if (index > -1) {
      return const_iterator{leaf, index};
    }

    leaf = leaf->get_next();
    return leaf ? const_iterator{leaf, 0} : const_iterator{};
  }

  /**
   * Returns an iterator to the first element not less than (greater than or
   * equal) the key. If the key is larger than the largest key, then this
   *returns the end iterator
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_not_smaller(Key key) noexcept {
    if (this->height == 0) {
      return iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_not_smaller_index(key);
    if (index > -1) {
      return iterator{leaf, index};
    }

    leaf = leaf->get_next();
    return leaf ? iterator{leaf, 0} : iterator{};
  }

  /**
   * Returns an iterator to the first element not less than (greater than or
   * equal) the key. If the key is larger than the largest key, then this
   *returns the end iterator
   **/
  [[nodiscard]] const_iterator find_not_smaller(Key key) const noexcept {
    if (this->height == 0) {
      return const_iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_not_smaller_index(key);
    if (index > -1) {
      return const_iterator{leaf, index};
    }

    leaf = leaf->get_next();
    return leaf ? const_iterator{leaf, 0} : const_iterator{};
  }

  /**
   * Returns an iterator to the first element not greater than (less than or
   * equal) the key. If the key is smaller than the smallest key, then this
   * returns the end iterator
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_not_larger(Key key) noexcept {
    if (this->height == 0) {
      return iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_not_larger_index(key);
    return index > -1 ? iterator{leaf, index} : iterator{};
  }

  /**
   * Returns an iterator to the first element not greater than (less than or
   * equal) the key. If the key is smaller than the smallest key, then this
   * returns the end iterator
   **/
  [[nodiscard]] const_iterator find_not_larger(Key key) const noexcept {
    if (this->height == 0) {
      return const_iterator{};
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    i32 index = leaf->find_not_larger_index(key);
    return index > -1 ? const_iterator{leaf, index} : const_iterator{};
  }

  /**
   * Checks if the key exists in the bptree_map
   *
   * @return bool
   **/
  [[nodiscard]] bool contains(Key key) const noexcept {
    if (this->height == 0) {
      return false;
    }

    leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                        : this->find_leaf_node_containing(key);
    return leaf->find_index(key) != -1;
  }

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
  [[nodiscard]] leaf_node* find_leaf_node_containing(Key key) const noexcept {
    // NOTE: In the future, can test if fast traversal for ancestor can be fast
    // here
    auto* node = static_cast<inner_node*>(this->root);

    // Traverse the internal nodes until the parent containing the leaf nodes
    for (usize h = this->height; h > 2; --h) {
      node = static_cast<inner_node*>(node->find_smaller_child(key));
    }

    // Traverse the parent containing leaf nodes
    return static_cast<leaf_node*>(node->find_smaller_child(key));
  }

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
  [[nodiscard]] inner_node* create_inner_node(void* child) noexcept {
    // NOLINTNEXTLINE
    auto* node = (inner_node*)std::malloc(sizeof(inner_node));
    if (node == nullptr) {
      return nullptr;
    }

    new (node) inner_node{};
    if constexpr (std::is_class<Key>::value) {
      new (node->get_keys())
          Key{base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree()};
    }
    node->get_children()[0] = child;

    return node;
  }

  /**
   * Creates a leaf node, if an error happens, this will return nullptr
   *
   * @errors
   *  - bad allocation in creating the node
   *  - bad allocation in copying the key
   *
   * @return leaf_node*
   **/
  [[nodiscard]] leaf_node* create_leaf_node() noexcept {
    // NOLINTNEXTLINE
    auto* node = (leaf_node*)std::malloc(sizeof(leaf_node));
    if (node == nullptr) {
      return nullptr;
    }

    new (node) leaf_node{};
    if constexpr (std::is_class<Key>::value) {
      new (node->get_keys())
          Key{base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree()};
    }

    if constexpr (std::is_class<Value>::value) {
      new (node->get_values())
          Value{base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree()};
    }

    return node;
  }

  /**
   * Splits the internal node
   *
   * @errors
   *  error_code::BAD_ALLOCATION
   *    - bad allocation in creating the new containers
   *    - bad allocation in copying the key
   **/
  [[nodiscard]] error_code split_inner_node(inner_node* left_node) noexcept {
    usize mid = this->middle();
    inner_node* parent = nullptr;
    inner_node* right_node = nullptr;
    Key key;
    // NOTE: Check if stack can be replaced, might check chaining since the
    // nodes can be pointed to one another
    vector<inner_node*> stack{};
    TRY(stack.reserve((this->height - 1) * 2));

    // NOTE: Only the right node only the new element being malloc'ed
    while (true) {
      // Create the right and parent internal node
      right_node = this->create_inner_node(left_node->get_children()[mid + 1]);
      if (right_node == nullptr) {
        return error_code::BAD_ALLOCATION;
      }

      parent = left_node->get_parent();
      if (parent == nullptr) {
        parent = this->create_inner_node(left_node);
        if (parent == nullptr) {
          std::free(right_node); // NOLINT
          while (!stack.is_empty()) {
            stack.pop();            // Left node
            std::free(stack.pop()); // NOLINT
          }
          return error_code::BAD_ALLOCATION;
        }
      }

      // Get the middle node(left bias) and move it to the parent
      key = left_node->get_keys()[mid];

      // Set the nodes parents
      left_node->set_parent(parent);
      right_node->set_parent(parent);

      parent->insert(key, right_node);
      if (parent->get_size() == this->get_degree()) { // parent will split
        static_cast<void>(stack.push(right_node));
        static_cast<void>(stack.push(left_node));
        left_node = parent;
        continue;
      }

      // Move and reparent starting with mid+1
      left_node->redistribute(right_node, mid + 1);
      right_node->reparent_children();

      // If the root node was split, reparent and add to the height by 1
      if (left_node == this->root) {
        this->root = parent;
        ++this->height;
      }
      break;
    }

    // Empty the stack
    while (!stack.is_empty()) {
      left_node = stack.pop();
      right_node = stack.pop();

      left_node->redistribute(right_node, mid + 1);
      right_node->reparent_children();
    }

    return error_code::OK;
  }

  /**
   * Splits the leaf node
   *
   * @errors
   *  - bad allocation in creating the new containers
   *  - bad allocation in copying the key
   **/
  [[nodiscard]] error_code split_leaf_node(leaf_node* left_leaf) noexcept {
    // Create the left leaf and the parent node
    leaf_node* right_leaf = this->create_leaf_node();
    if (right_leaf == nullptr) {
      return error_code::BAD_ALLOCATION;
    }

    inner_node* parent = left_leaf->get_parent();
    if (parent == nullptr) {
      parent = this->create_inner_node(left_leaf);
      if (parent == nullptr) {
        std::free(right_leaf); // NOLINT
        return error_code::BAD_ALLOCATION;
      }
    }

    // Get the middle node(right bias)
    usize mid = this->middle();
    auto key = left_leaf->at_key(mid);

    // Set the parents of the leaf nodes
    left_leaf->set_parent(parent);
    right_leaf->set_parent(parent);

    // Try to insert the middle node to the parent
    parent->insert(key, right_leaf);
    if (parent->get_size() == this->get_degree()) {
      if (auto error = this->split_inner_node(parent)) {
        std::free(right_leaf); // NOLINT
        if (parent->get_size() == 1) {
          std::free(parent); // NOLINT
          left_leaf->set_parent(nullptr);
        }

        return std::move(error);
      }
    }

    // Setup the pointers
    if (auto* next = left_leaf->get_next()) {
      next->set_prev(right_leaf);
      right_leaf->set_next(next);
    }

    left_leaf->set_next(right_leaf);
    right_leaf->set_prev(left_leaf);

    // Move the half the data from the left leaf to the right leaf
    left_leaf->redistribute(right_leaf, mid);
    return error_code::OK;
  }

  template <typename Value_>
  [[nodiscard]] error_code insert_impl(Key key, Value_ value) noexcept {
    Value value_copy{};
    if constexpr (std::is_rvalue_reference<Value_>::value) {
      value_copy = std::move(value);
    } else if constexpr (std::is_copy_assignable<Value>::value) {
      value_copy = value;
    } else {
      TRY(value_copy.copy(value));
    }

    // Separate this to a function call
    if (this->height == 0) { // Create the root as a leaf node
      auto* leaf = this->root ? static_cast<leaf_node*>(this->root)
                              : this->create_leaf_node();
      if (leaf == nullptr) {
        return error_code::BAD_ALLOCATION;
      }

      leaf->push(key, std::move(value_copy));
      this->root = leaf;
      ++this->size;
      this->height = 1;

      return error_code::OK;
    }

    // Separate this to a function call
    if (this->height == 1) { // Root is the leaf node
      auto* leaf = static_cast<leaf_node*>(this->root);
      i32 index = leaf->insert(key, std::move(value_copy));
      if (index == -1) {
        return error_code::OK;
      }

      // Check if the leaf node exceeds the max degree rule
      ++this->size;
      if (this->size < this->get_degree()) {
        return error_code::OK;
      }

      // Split here
      if (auto error = this->split_leaf_node(leaf)) {
        leaf->remove(index);
        return error;
      }

      this->root = leaf->get_parent();
      this->height = 2;
      return error_code::OK;
    }

    leaf_node* leaf = this->find_leaf_node_containing(key);
    i32 index = leaf->insert(key, std::move(value_copy));
    if (index == -1) {
      return error_code::OK;
    }

    ++this->size;
    if (leaf->get_size() < this->get_degree()) {
      return error_code::OK;
    }

    // Split the leaf node
    if (auto error = this->split_leaf_node(leaf)) {
      leaf->remove(index);
      return error;
    }

    return error_code::OK;
  }

  // === Erase Helpers === //

  inline void remove_traversal(
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
  void borrow_or_merge_inner(inner_node* parent, Key key) noexcept {
    i32 sz = 0;
    i32 index = 0;
    inner_node* grandparent = nullptr;
    inner_node* uncle = nullptr;
    void** children = nullptr;

    while (true) {
      // Get the current index of the parent relative to its parent
      grandparent = parent->get_parent();
      sz = grandparent->get_size();
      index = grandparent->find_smaller_index(key);

      // Try to borrow
      children = grandparent->get_children();
      if (index < sz) { // Borrow to the right uncle
        uncle = static_cast<inner_node*>(children[index + 1]);
        if (uncle->get_size() > this->min_inner_children()) {
          parent->borrow_from_right_uncle(uncle, index);
          return;
        }
      }

      if (index > 0) { // Borrow to the left uncle
        uncle = static_cast<inner_node*>(children[index - 1]);
        if (uncle->get_size() > this->min_inner_children()) {
          parent->borrow_from_left_uncle(uncle, index);
          return;
        }
      }

      // Try to merge
      grandparent->merge_children(index < sz ? index : sz - 1);

      // Check if the sz suffices the rule, else merge again from the parent's
      // parent
      if (sz >= this->get_degree() / 2) {
        return;
      }

      if (this->root == grandparent) { // This is already the root node
        if (sz < 2) {                  // 0 children
          this->root = grandparent->get_children()[0];
          // NOTE: inner_node & leaf_node parent are aligned
          static_cast<inner_node*>(this->root)->set_parent(nullptr);

          --this->height;
          std::free(grandparent); // NOLINT
        }
        break;
      }

      parent = grandparent;
    }
  }

  /**
   * Tries to borrows from siblings or merge leaf nodes,
   * this will also propagate to the parent nodes
   **/
  void borrow_or_merge_leaf(leaf_node* leaf) noexcept {
    // Check for right sibling
    leaf_node* sibling = leaf->get_next();
    bool has_right_sibling =
        sibling != nullptr && sibling->get_parent() == leaf->get_parent();
    if (has_right_sibling) {
      // Try to borrow to the right
      if (sibling->get_size() > this->min_leaf_children()) {
        leaf->borrow_right_sibling();
        return;
      }
    }

    // Check for left sibling
    sibling = leaf->get_prev();
    if (sibling && sibling->get_parent() == leaf->get_parent()) {
      // Try to borrow to the left
      if (sibling->get_size() > this->min_leaf_children()) {
        leaf->borrow_left_sibling();
        return;
      }
    }

    // Merge nodes
    if (!has_right_sibling) {
      leaf = leaf->get_prev();
    }
    leaf->merge_right_sibling();

    inner_node* parent = leaf->get_parent();
    if (parent->get_size() >= this->min_inner_children()) {
      return;
    }

    if (parent == this->root) {
      if (parent->get_size() == 0) {
        this->root = parent->front_child();
        std::free(parent); // NOLINT

        static_cast<leaf_node*>(this->root)->set_parent(nullptr);
        static_cast<leaf_node*>(this->root)->set_next(nullptr);

        this->height = 1;
      }
      return;
    }

    this->borrow_or_merge_inner(parent, leaf->front_key());
  }

  /**
   * Erases the root node being a leaf node
   **/
  void remove_root_leaf(Key key) noexcept {
    auto* leaf = static_cast<leaf_node*>(this->root);
    auto* keys = leaf->get_keys();
    isize comparison = 0;
    i32 sz = leaf->get_size();

    for (i32 i = 0; i < sz; ++i) {
      comparison = KeyCompare{}(key, keys[i]);
      if (comparison == 0) {
        leaf->remove(i);

        --this->size;
        if (this->size == 0) {
          this->height = 0;
          std::free(this->root); // NOLINT
        }

        return;
      }

      if (comparison < 0) {
        return;
      }
    }
  }
};

} // namespace ds

#ifndef DS_BPTREE_MAP_NODE_HPP
#include "./bptree_map_node.hpp"
#endif

namespace ds {

template <typename Key, typename Value, typename KeyCompare = compare<Key>>
class bptree_map
    : public base_bptree_map<
          bptree_map<Key, Value, KeyCompare>, Key, Value, KeyCompare> {};

} // namespace ds

#endif
