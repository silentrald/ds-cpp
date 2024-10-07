/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_BPTREE_MAP_TPP
#define DS_BPTREE_MAP_TPP

#include "./bptree_map.hpp"
#include "./vector.hpp"
#include "types.hpp"
#include <cstdlib>
#include <type_traits>

namespace ds {

// === Move === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::base_bptree_map(
    base_bptree_map&& rhs
) noexcept
    : root(rhs.root), size(rhs.size), height(height) {
  rhs.root = nullptr;
  rhs.size = rhs.height = 0;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>&
base_bptree_map<Derived, Key, Value, KeyCompare>::operator=(
    base_bptree_map&& rhs
) noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::~base_bptree_map() noexcept {
  this->clear();
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::clear() noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node*
base_bptree_map<Derived, Key, Value, KeyCompare>::find_leaf_node_containing(
    Key key
) const noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
expected<Value*, error_code>
base_bptree_map<Derived, Key, Value, KeyCompare>::at(Key key) const noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
expected<Value*, error_code>
base_bptree_map<Derived, Key, Value, KeyCompare>::at_smaller(Key key
) const noexcept {
  if (this->height == 0) {
    return error_code::CONTAINER_EMPTY;
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_smaller_index(key);
  return index > -1 ? &leaf->at_value(index) : error_code::NOT_FOUND;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
expected<Value*, error_code>
base_bptree_map<Derived, Key, Value, KeyCompare>::at_larger(Key key
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
expected<Value*, error_code>
base_bptree_map<Derived, Key, Value, KeyCompare>::at_not_smaller(Key key
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
expected<Value*, error_code>
base_bptree_map<Derived, Key, Value, KeyCompare>::at_not_larger(Key key
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Value* base_bptree_map<Derived, Key, Value, KeyCompare>::operator[](Key key
) noexcept {
  if (this->height == 0) {
    return nullptr;
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  return leaf->get_value(key);
}

// === Iterators === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::begin() noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::cbegin() const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  if (this->height == 1) {
    return citerator{static_cast<leaf_node*>(this->root)};
  }

  auto* node = static_cast<inner_node*>(this->root);
  for (usize h = this->height; h > 2; --h) {
    node = static_cast<inner_node*>(node->front_child());
  }

  return citerator{static_cast<leaf_node*>(node->front_child())};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::end() noexcept {
  return iterator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::cend() const noexcept {
  return citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::rbegin() noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::crbegin() const noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::rend() noexcept {
  return iterator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::crend() const noexcept {
  return iterator{};
}

// === Capacity === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
bool base_bptree_map<Derived, Key, Value, KeyCompare>::is_empty(
) const noexcept {
  return this->size == 0;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
usize base_bptree_map<Derived, Key, Value, KeyCompare>::get_size(
) const noexcept {
  return this->size;
}

// === Insert Helpers === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node*
base_bptree_map<Derived, Key, Value, KeyCompare>::create_inner_node(void* child
) noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node*
base_bptree_map<Derived, Key, Value, KeyCompare>::create_leaf_node() noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
error_code base_bptree_map<Derived, Key, Value, KeyCompare>::split_inner_node(
    inner_node* left_node
) noexcept {
  usize mid = this->middle();
  inner_node* parent = nullptr;
  inner_node* right_node = nullptr;
  Key key;
  vector<inner_node*> stack{};

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
          stack.pop_back_disc();              // Left node
          std::free(stack.pop_back_unsafe()); // NOLINT
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
      if (error_code error = stack.push_back(right_node, left_node)) {
        std::free(right_node); // NOLINT
        if (parent->get_size() == 1) {
          std::free(parent); // NOLINT
        }

        while (!stack.is_empty()) {
          stack.pop_back_disc();
          std::free(stack.pop_back_unsafe()); // NOLINT
        }

        return error;
      }

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
    left_node = stack.pop_back_unsafe();
    right_node = stack.pop_back_unsafe();

    left_node->redistribute(right_node, mid + 1);
    right_node->reparent_children();
  }

  return error_code::OK;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
error_code base_bptree_map<Derived, Key, Value, KeyCompare>::split_leaf_node(
    leaf_node* left_leaf
) noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
template <typename Value_>
error_code base_bptree_map<Derived, Key, Value, KeyCompare>::insert_impl(
    Key key, Value_ value
) noexcept {
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

    leaf->push_back(key, std::move(value_copy));
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
      leaf->erase(index);
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
    leaf->erase(index);
    return error;
  }

  return error_code::OK;
}

// === Erase === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::borrow_or_merge_inner(
    inner_node* parent, Key key
) noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::borrow_or_merge_leaf(
    leaf_node* leaf
) noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::erase_root_leaf(Key key
) noexcept {
  auto* leaf = static_cast<leaf_node*>(this->root);
  auto* keys = leaf->get_keys();
  isize comparison = 0;
  i32 sz = leaf->get_size();

  for (i32 i = 0; i < sz; ++i) {
    comparison = KeyCompare{}(key, keys[i]);
    if (comparison == 0) {
      leaf->erase(i);

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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::erase(Key key) noexcept {
  if (this->height == 0) {
    return;
  }

  if (this->height == 1) {
    this->erase_root_leaf(key);
    return;
  }

  // Just try to handle leaf deletion
  i32 index = 0;
  leaf_node* leaf = nullptr;

  // Ancestor same key value with the leaf node
  inner_node* ancestor = nullptr;
  i32 ancestor_index = 0;

  this->erase_traversal(key, ancestor, ancestor_index, leaf, index);

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
  leaf->erase(index);
  if (leaf->get_size() < this->min_leaf_children()) {
    this->borrow_or_merge_leaf(leaf);
  }
}

// === Lookup === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::find(Key key) noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_index(key);
  return index > -1 ? iterator{leaf, index} : iterator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::find(Key key) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_index(key);
  return index > -1 ? citerator{leaf, index} : citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_smaller(Key key
) noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_smaller_index(key);
  return index > -1 ? iterator{leaf, index} : iterator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_smaller(Key key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_smaller_index(key);
  return index > -1 ? citerator{leaf, index} : citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_larger(Key key
) noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_larger(Key key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_larger_index(key);
  if (index > -1) {
    return citerator{leaf, index};
  }

  leaf = leaf->get_next();
  return leaf ? citerator{leaf, 0} : citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_not_smaller(Key key
) noexcept {
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

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_not_smaller(Key key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_not_smaller_index(key);
  if (index > -1) {
    return citerator{leaf, index};
  }

  leaf = leaf->get_next();
  return leaf ? citerator{leaf, 0} : citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_not_larger(Key key
) noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_not_larger_index(key);
  return index > -1 ? iterator{leaf, index} : iterator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_not_larger(Key key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  i32 index = leaf->find_not_larger_index(key);
  return index > -1 ? citerator{leaf, index} : citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
bool base_bptree_map<Derived, Key, Value, KeyCompare>::contains(Key key
) const noexcept {
  if (this->height == 0) {
    return false;
  }

  leaf_node* leaf = this->height == 1 ? static_cast<leaf_node*>(this->root)
                                      : this->find_leaf_node_containing(key);
  return leaf->find_index(key) != -1;
}

} // namespace ds

#endif
