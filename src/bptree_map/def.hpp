/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 * Updated: 2023-01-04
 *===============================*/

#ifndef DS_BPTREE_MAP_DEF_HPP
#define DS_BPTREE_MAP_DEF_HPP

#include "../compare/def.hpp"
#include "../error.hpp"
#include "../hash/def.hpp"
#include "../macro.hpp"
#include "../types.hpp"
#include "./iterator.hpp"
#include <utility>

namespace ds {

const char* const BPTREE_MAP_BAD_ALLOC = "bptree_map:bad_alloc";
const char* const BPTREE_MAP_EMPTY = "bptree_map:empty";
const char* const BPTREE_MAP_NOT_FOUND = "bptree_map:not_found";
const char* const BPTREE_MAP_NODE_BAD_ALLOC = "bptree_map:bad_alloc";

/**
 * NOTE: This should only handle non class type keys
 * Key class type will produce an error if copied,
 * and it will pose a problem in erase since its hard
 * to revert it to the original form. As much as possible,
 * key type should be a primitive
 **/
template <
    typename Derived, typename Key, typename Value,
    typename KeyCompare = compare<Key>>
class base_bptree_map {
public:
  friend Derived;
  class inner_node;

  using key_type = Key;
  using key_ptr = Key*;
  using key_ref = Key&;
  using key_cref = const Key&;
  using key_rref = Key&&;

  using value_type = Value;
  using value_ptr = Value*;
  using value_ref = Value&;
  using value_cref = const Value&;
  using value_rref = Value&&;

  using iterator =
      bptree_map_iterator<base_bptree_map<Derived, Key, Value, KeyCompare>>;
  using citerator = bptree_map_iterator<
      const base_bptree_map<Derived, Key, Value, KeyCompare>>;

  class leaf_node {
  private:
    key_ptr keys = nullptr;     // size
    value_ptr values = nullptr; // size
    i32 size = 0;

    inner_node* parent = nullptr;
    leaf_node* next = nullptr;
    leaf_node* prev = nullptr;

    void insert_indexed(i32 index, key_type key, value_rref value) noexcept;

  public:
    leaf_node() = default;
    leaf_node(const leaf_node&) = delete;
    leaf_node& operator=(const leaf_node&) = delete;

    // === Initializers === //

    /**
     * Call this once on initialization
     *
     * This will create the arrays for the keys and values
     *
     * @errors
     *  - bad allocation in creating the arrays
     **/
    opt_err init(i32 capacity) noexcept;

    // === Move === //

    leaf_node(leaf_node&& other) noexcept;
    leaf_node& operator=(leaf_node&& other) noexcept;

    // === Destructor === //

    ~leaf_node() noexcept;

    // === Setters === //

    void set_parent(inner_node* parent) noexcept;
    void set_next(leaf_node* next) noexcept;
    void set_prev(leaf_node* prev) noexcept;

    // === Getters === //

    [[nodiscard]] key_ptr get_keys() const noexcept;
    [[nodiscard]] value_ptr get_values() const noexcept;
    [[nodiscard]] i32 get_size() const noexcept;
    [[nodiscard]] inner_node* get_parent() const noexcept;
    [[nodiscard]] leaf_node* get_next() const noexcept;
    [[nodiscard]] leaf_node* get_prev() const noexcept;

    // === Element Access === //

    [[nodiscard]] key_type at_key(i32 index) const noexcept;
    [[nodiscard]] key_type front_key() const noexcept;
    [[nodiscard]] key_type back_key() const noexcept;
    [[nodiscard]] Value& at_value(i32 index) const noexcept;

    // === Modifiers === //

    i32 insert(key_type key, value_rref value) noexcept;
    void push_back(key_type key, value_rref value) noexcept;
    void erase(i32 index) noexcept;
    void clear() noexcept;

    void redistribute(leaf_node* other, i32 mid) noexcept;
    void borrow_right_sibling() noexcept;
    void borrow_left_sibling() noexcept;
    void merge_right_sibling() noexcept;

    // === Lookup === //

    [[nodiscard]] value_ptr get_value(key_type key) const noexcept;
    [[nodiscard]] i32 find_index(key_type key) const noexcept;
    [[nodiscard]] i32 find_smaller_index(key_type key) const noexcept;
    [[nodiscard]] i32 find_larger_index(key_type key) const noexcept;
    [[nodiscard]] i32 find_not_smaller_index(key_type key) const noexcept;
    [[nodiscard]] i32 find_not_larger_index(key_type key) const noexcept;
  };

  class inner_node {
  private:
    Key* keys = nullptr;       // size
    void** children = nullptr; // size + 1
    i32 size = 0;

    inner_node* parent = nullptr;

    void insert_indexed(i32 index, key_type key, void* child) noexcept;

  public:
    inner_node() = default;
    inner_node(const inner_node&) = delete;
    inner_node& operator=(const inner_node&) = delete;

    // === Initializers === //

    [[nodiscard]] opt_err init(i32 capacity, void* child) noexcept;

    // === Move === //

    inner_node(inner_node&&) = delete;
    inner_node& operator=(inner_node&&) = delete;

    // === Destructor === //

    ~inner_node() noexcept;
    void destroy_last_child() noexcept;
    void destroy_leaf_children() noexcept;

    // === Setters === //

    void set_parent(inner_node* parent) noexcept;

    // === Getters === //

    [[nodiscard]] key_ptr get_keys() const noexcept;
    [[nodiscard]] void** get_children() const noexcept;
    [[nodiscard]] i32 get_size() const noexcept;
    [[nodiscard]] inner_node* get_parent() const noexcept;

    // === Element Access === //

    /**
     * First child that is less than the key, if all keys are smaller than the
     * key passed, this returns the last child
     **/
    [[nodiscard]] void* find_smaller_child(key_type key) const noexcept;
    [[nodiscard]] i32 find_smaller_index(key_type key) const noexcept;

    [[nodiscard]] key_type at_key(i32 index) const noexcept;
    [[nodiscard]] key_type front_key() const noexcept;
    [[nodiscard]] key_type back_key() const noexcept;
    [[nodiscard]] void* front_child() const noexcept;
    [[nodiscard]] void* back_child() const noexcept;

    // === Modifiers === //

    void insert(key_type key, void* child) noexcept;
    void push_front(key_type key, void* child) noexcept;
    void pop_front() noexcept;
    void push_back(key_type key, void* child) noexcept;
    void pop_back() noexcept;
    void erase(i32 index) noexcept;
    void set_key(i32 index, key_type key) noexcept;

    // === Reparenting === //

    void reparent_leaf_children() noexcept;
    void reparent_children() noexcept;

    // === Inserting === //

    void redistribute(inner_node* other, i32 mid) noexcept;

    // === Erasing === //

    void borrow_from_right_uncle(
        inner_node* right_uncle, i32 parent_index, bool leaf_child
    ) noexcept;
    void borrow_from_left_uncle(
        inner_node* left_uncle, i32 parent_index, bool leaf_child
    ) noexcept;

    // Merges index and index + 1
    void merge_children(i32 index) noexcept;
  };

  using inner_ptr = inner_node*;
  using leaf_ptr = leaf_node*;

protected:
  void* root = nullptr;
  i32 _size = 0;
  i32 height = 0;
  const i32 degree = 6; // TODO: Connect this to CMake?

  // * Definitions * //

  [[nodiscard]] constexpr i32 middle() const noexcept {
    return this->degree / 2;
  }

  [[nodiscard]] constexpr i32 min_inner_children() const noexcept {
    // Ceiling function = adding the odd bit
    return this->degree / 2 + (this->degree & 1) - 1;
  }

  [[nodiscard]] constexpr i32 min_leaf_children() const noexcept {
    // Ceiling function = adding the odd bit
    return this->degree / 2 + (this->degree & 1);
  }

  // * Lookup * //

  /**
   * Finds the leaf node containing the key we want to find
   * This requires the tree to be height >= 1
   **/
  [[nodiscard]] leaf_ptr find_leaf_node_containing(key_type key) const noexcept;

  // * Insert Helpers * //

  /**
   * Creates a internal node, if an error happens, this will return nullptr
   *
   * @errors
   *  - bad allocation in creating the node
   *  - bad allocation in copying the key
   *
   * @return inner_ptr
   **/
  [[nodiscard]] inner_ptr create_inner_node(void* child) noexcept;

  /**
   * Creates a leaf node, if an error happens, this will return nullptr
   *
   * @errors
   *  - bad allocation in creating the node
   *  - bad allocation in copying the key
   *
   * @return leaf_ptr
   **/
  [[nodiscard]] leaf_ptr create_leaf_node() noexcept;

  /**
   * Splits the internal node
   *
   * @errors
   *  - bad allocation in creating the new containers
   *  - bad allocation in copying the key
   *
   * @return exp_err<inner_ptr> the pointer to the right parent
   **/
  [[nodiscard]] opt_err split_inner_node(inner_ptr left_node) noexcept;

  /**
   * Splits the leaf node
   *
   * @errors
   *  - bad allocation in creating the new containers
   *  - bad allocation in copying the key
   **/
  [[nodiscard]] opt_err split_leaf_node(leaf_ptr left_leaf) noexcept;

  template <typename Value_>
  [[nodiscard]] opt_err insert_impl(key_type key, Value_ value) noexcept;

  // * Erase Helpers * //

  /**
   * Tries to borrow from siblings or merge inner nodes,
   * this will also propagate to the parent nodes
   **/
  void borrow_or_merge_inner(inner_ptr parent, key_type key) noexcept;

  /**
   * Tries to borrows from siblings or merge leaf nodes,
   * this will also propagate to the parent nodes
   **/
  void borrow_or_merge_leaf(leaf_ptr leaf) noexcept;

  /**
   * Erases the root node being a leaf node
   **/
  void erase_root_leaf(key_type key) noexcept;

public:
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
   *  - tree is empty
   *  - key is not found in the tree
   *
   * @return exp_ptr_err<value_type>
   **/
  [[nodiscard]] exp_ptr_err<value_type> at(key_type key) const noexcept;

  /**
   * Get the first element less than the key
   *
   * @errors
   *  - tree is empty
   *  - key is smaller than the smallest element
   *
   * @return exp_ptr_err<value_type>
   **/
  [[nodiscard]] exp_ptr_err<value_type> at_smaller(key_type key) const noexcept;

  /**
   * Get the first element greater than the key
   *
   * @errors
   *  - tree is empty
   *  - key is larger than the largest element
   *
   * @return exp_ptr_err<value_type>
   **/
  [[nodiscard]] exp_ptr_err<value_type> at_larger(key_type key) const noexcept;

  /**
   * Get the first element not less than (greater than or equal) the key
   *
   * @errors
   *  - tree is empty
   *  - key is larger than the greater element
   *
   * @return exp_ptr_err<value_type>
   **/
  [[nodiscard]] exp_ptr_err<value_type> at_not_smaller(key_type key
  ) const noexcept;

  /**
   * Get the first element not greater than (less than or equal) the key
   *
   * @errors
   *  - tree is empty
   *  - key is smaller than the smallest element
   *
   * @return exp_ptr_err<value_type>
   **/
  [[nodiscard]] exp_ptr_err<value_type> at_not_larger(key_type key
  ) const noexcept;

  /**
   * Unsafe Element Access
   * Returns a nullptr if the data is not found in the tree
   *
   * @return value_ptr
   **/
  [[nodiscard]] value_ptr operator[](key_type key) noexcept;

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
  [[nodiscard]] bool empty() const noexcept;

  /**
   * Returns the number elements in the bptree
   *
   * @return i32
   **/
  [[nodiscard]] i32 size() const noexcept;

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
  [[nodiscard]] opt_err insert(key_type key, value_cref value) noexcept {
    return this->insert_impl<value_cref>(key, value);
  }

  /**
   * Inserts a value into the bptree
   *
   * @errors
   *  - bad allocation in creating the containers
   *  - bad allocation in copying the key
   **/
  [[nodiscard]] opt_err insert(key_type key, value_rref value) noexcept {
    return this->insert_impl<value_rref>(key, std::move(value));
  }

  /**
   * Erases an element in the bptree
   * If the element does not exist in the bptree, then this will not change
   * anything in the bptree and no error is thrown
   **/
  void erase(key_type key) noexcept;

  // === Lookup === //

  /**
   * Returns the iterator with the corresponding key.
   * If it does not find an exact match, it will return
   * the end of the iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find(key_type key) noexcept;

  /**
   * Returns the iterator with the corresponding key.
   * If it does not find an exact match, it will return
   * the end of the iterator.
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find(key_type key) const noexcept;

  /**
   * Returns an iterator to the first element less than the key.
   * If the key is smaller than the smallest key, then this returns
   * the end iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_smaller(key_type key) noexcept;

  /**
   * Returns an iterator to the first element less than the key.
   * If the key is smaller than the smallest key, then this returns
   * the end iterator.
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find_smaller(key_type key) const noexcept;

  /**
   * Returns an iterator to the first element greater than the key.
   * If the key is larger than the largest key, then this returns
   * the end iterator.
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_larger(key_type key) noexcept;

  /**
   * Returns an iterator to the first element greater than the key.
   * If the key is larger than the largest key, then this returns
   * the end iterator.
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find_larger(key_type key) const noexcept;

  /**
   * Returns an iterator to the first element not less than (greater than or
   * equal) the key. If the key is larger than the largest key, then this
   *returns the end iterator
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_not_smaller(key_type key) noexcept;

  /**
   * Returns an iterator to the first element not less than (greater than or
   * equal) the key. If the key is larger than the largest key, then this
   *returns the end iterator
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find_not_smaller(key_type key) const noexcept;

  /**
   * Returns an iterator to the first element not greater than (less than or
   * equal) the key. If the key is smaller than the smallest key, then this
   * returns the end iterator
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find_not_larger(key_type key) noexcept;

  /**
   * Returns an iterator to the first element not greater than (less than or
   * equal) the key. If the key is smaller than the smallest key, then this
   * returns the end iterator
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find_not_larger(key_type key) const noexcept;

  /**
   * Checks if the key exists in the bptree_map
   *
   * @return bool
   **/
  [[nodiscard]] bool contains(key_type key) const noexcept;
};

} // namespace ds

#ifndef DS_BPTREE_MAP_NODE_HPP
#include "./node.hpp"
#endif

#ifndef DS_BPTREE_MAP_IMPL_HPP
#include "./impl.hpp"
#endif

namespace ds {

template <typename Key, typename Value, typename KeyCompare = compare<Key>>
class bptree_map
    : public base_bptree_map<
          bptree_map<Key, Value, KeyCompare>, Key, Value, KeyCompare> {};

} // namespace ds

#endif

