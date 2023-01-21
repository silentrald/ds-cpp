/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 * Updated: 2023-01-04
 *===============================*/

#ifndef DS_BPTREE_MAP_IMPL_HPP
#define DS_BPTREE_MAP_IMPL_HPP

#include "./def.hpp"
#include <cstdlib>
#include <new>
#include <type_traits>

namespace ds {

// === Move === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::base_bptree_map(
    base_bptree_map&& rhs
) noexcept
    : root(rhs.root), _size(rhs._size), height(height) {
  rhs.root = nullptr;
  rhs._size = rhs.height = 0;
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
  this->_size = rhs._size;
  this->height = rhs.height;

  rhs.root = nullptr;
  rhs._size = rhs.height = 0;

  return *this;
}

// === Destructor === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::~base_bptree_map() noexcept {
  this->clear();

  if (this->root) {
    delete static_cast<leaf_ptr>(this->root);
    this->root = nullptr;
  }
}

// === Element Access === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_ptr
base_bptree_map<Derived, Key, Value, KeyCompare>::find_leaf_node_containing(
    key_type key
) const noexcept {
  auto* node = static_cast<inner_ptr>(this->root);

  // Traverse the internal nodes until the parent containing the leaf nodes
  for (i32 h = this->height; h > 2; --h) {
    node = static_cast<inner_ptr>(node->find_smaller_child(key));
  }

  // Traverse the parent containing leaf nodes
  return static_cast<leaf_ptr>(node->find_smaller_child(key));
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
exp_ptr_err<
    typename base_bptree_map<Derived, Key, Value, KeyCompare>::value_type>
base_bptree_map<Derived, Key, Value, KeyCompare>::at(key_type key
) const noexcept {
  if (this->height == 0) {
    return unexpected{error{BPTREE_MAP_EMPTY, def_err_vals}};
  }

  auto* leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                 : this->find_leaf_node_containing(key);
  auto* value = leaf->get_value(key);

  if (value == nullptr) {
    return unexpected{error{BPTREE_MAP_NOT_FOUND}};
  }
  return value;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
exp_ptr_err<
    typename base_bptree_map<Derived, Key, Value, KeyCompare>::value_type>
base_bptree_map<Derived, Key, Value, KeyCompare>::at_smaller(key_type key
) const noexcept {
  if (this->height == 0) {
    return unexpected{error{BPTREE_MAP_EMPTY, def_err_vals}};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_smaller_index(key);
  if (index == -1) {
    return unexpected{error{BPTREE_MAP_NOT_FOUND}};
  }

  return &leaf->at_value(index);
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
exp_ptr_err<
    typename base_bptree_map<Derived, Key, Value, KeyCompare>::value_type>
base_bptree_map<Derived, Key, Value, KeyCompare>::at_larger(key_type key
) const noexcept {
  if (this->height == 0) {
    return unexpected{error{BPTREE_MAP_EMPTY, def_err_vals}};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_larger_index(key);
  if (index > -1) {
    return &leaf->at_value(index);
  }

  leaf = leaf->get_next();
  if (leaf == nullptr) {
    return unexpected{error{BPTREE_MAP_NOT_FOUND}};
  }

  return &leaf->at_value(0);
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
exp_ptr_err<
    typename base_bptree_map<Derived, Key, Value, KeyCompare>::value_type>
base_bptree_map<Derived, Key, Value, KeyCompare>::at_not_smaller(key_type key
) const noexcept {
  if (this->height == 0) {
    return unexpected{error{BPTREE_MAP_EMPTY, def_err_vals}};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_not_smaller_index(key);
  if (index > -1) {
    return &leaf->at_value(index);
  }

  leaf = leaf->get_next();
  if (leaf == nullptr) {
    return unexpected{error{BPTREE_MAP_NOT_FOUND}};
  }

  return &leaf->at_value(0);
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
exp_ptr_err<
    typename base_bptree_map<Derived, Key, Value, KeyCompare>::value_type>
base_bptree_map<Derived, Key, Value, KeyCompare>::at_not_larger(key_type key
) const noexcept {
  if (this->height == 0) {
    return unexpected{error{BPTREE_MAP_EMPTY, def_err_vals}};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_not_larger_index(key);
  if (index == -1) {
    return unexpected{error{BPTREE_MAP_NOT_FOUND}};
  }

  return &leaf->at_value(index);
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::value_ptr
base_bptree_map<Derived, Key, Value, KeyCompare>::operator[](key_type key
) noexcept {
  if (this->height == 0) {
    return nullptr;
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
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
    return iterator{static_cast<leaf_ptr>(this->root)};
  }

  auto* node = static_cast<inner_ptr>(this->root);
  for (i32 h = this->height; h > 2; --h) {
    node = static_cast<inner_ptr>(node->front_child());
  }

  return iterator{static_cast<leaf_ptr>(node->front_child())};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::cbegin() const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  if (this->height == 1) {
    return citerator{static_cast<leaf_ptr>(this->root)};
  }

  auto* node = static_cast<inner_ptr>(this->root);
  for (i32 h = this->height; h > 2; --h) {
    node = static_cast<inner_ptr>(node->front_child());
  }

  return citerator{static_cast<leaf_ptr>(node->front_child())};
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
    return iterator{static_cast<leaf_ptr>(this->root), this->_size - 1};
  }

  auto* node = static_cast<inner_ptr>(this->root);
  for (i32 h = this->height; h > 2; --h) {
    node = static_cast<inner_ptr>(node->back_child());
  }

  auto* leaf = static_cast<leaf_ptr>(node->back_child());
  return iterator{leaf, leaf->get_size() - 1};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::crbegin() const noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  if (this->height == 1) {
    return iterator{static_cast<leaf_ptr>(this->root), this->_size - 1};
  }

  auto* node = static_cast<inner_ptr>(this->root);
  for (i32 h = this->height; h > 2; --h) {
    node = static_cast<inner_ptr>(node->back_child());
  }

  auto* leaf = static_cast<leaf_ptr>(node->back_child());
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
bool base_bptree_map<Derived, Key, Value, KeyCompare>::empty() const noexcept {
  return this->_size == 0;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::size() const noexcept {
  return this->_size;
}

// === Modifiers === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::clear() noexcept {
  if (this->height == 0) {
    return;
  }

  if (this->height == 1) {
    static_cast<leaf_ptr>(this->root)->clear();
    this->_size = this->height = 0;
    return;
  }

  auto* node = static_cast<inner_ptr>(this->root);
  if (this->height == 2) {
    node->destroy_leaf_children();
    delete node; // NOLINT

    this->root = nullptr;
    this->_size = this->height = 0;
    return;
  }

  // State based deletion
  i32 h = this->height;
  while (true) {
    // Inner node containing leaf nodes
    if (h == 2) {
      node->destroy_leaf_children();

      node = node->get_parent();
      node->destroy_last_child();

      ++h;
    }

    // If the inner node have children, tranverse the last child
    if (node->get_size() > -1) {
      node = static_cast<inner_ptr>(node->back_child());
      --h;

      continue;
    }

    // This is the root node
    if (node->get_parent() == nullptr) {
      delete node; // NOLINT
      break;
    }

    // Inner node doesn't have any more children so delete the node
    node = node->get_parent();
    node->destroy_last_child();
    ++h;
  }

  this->root = nullptr;
  this->_size = this->height = 0;
}

// * Insert Helpers * //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::inner_ptr
base_bptree_map<Derived, Key, Value, KeyCompare>::create_inner_node(void* child
) noexcept {
  auto* node = new (std::nothrow) inner_node{}; // NOLINT
  if (node == nullptr) {
    return nullptr;
  }

  if (node->init(this->degree, child)) {
    delete node; // NOLINT
    return nullptr;
  }

  return node;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_ptr
base_bptree_map<Derived, Key, Value, KeyCompare>::create_leaf_node() noexcept {
  auto* leaf = new (std::nothrow) leaf_node{}; // NOLINT
  if (leaf == nullptr) {
    return nullptr;
  }

  if (leaf->init(this->degree)) {
    delete leaf; // NOLINT
    return nullptr;
  }

  return leaf;
}

// TODO: Turn this into an iterative call to not stackoverflow
template <typename Derived, typename Key, typename Value, typename KeyCompare>
exp_err<typename base_bptree_map<Derived, Key, Value, KeyCompare>::inner_ptr>
base_bptree_map<Derived, Key, Value, KeyCompare>::split_inner_node(
    inner_ptr left_node
) noexcept {
  i32 mid = this->middle();

  // Create the right and parent internal node
  auto* right_node =
      this->create_inner_node(left_node->get_children()[mid + 1]);
  if (right_node == nullptr) {
    return unexpected{error{BPTREE_MAP_BAD_ALLOC, def_err_vals}};
  }

  auto* parent = left_node->get_parent();
  bool created = false;
  if (parent == nullptr) {
    created = true;
    parent = this->create_inner_node(left_node);
    if (parent) {
      delete right_node; // NOLINT
      return unexpected{error{BPTREE_MAP_BAD_ALLOC, def_err_vals}};
    }
  }

  // Get the middle node(right bias) and move it to the parent
  auto key = left_node->get_keys()[mid];

  // Set the nodes parents
  left_node->set_parent(parent);
  right_node->set_parent(parent);

  parent->insert(key, right_node);
  if (parent->get_size() == this->degree) {
    auto exp = this->split_inner_node(parent);
    if (!exp) {
      delete right_node; // NOLINT
      if (created) {
        delete parent; // NOLINT
        left_node->set_parent(nullptr);
      }

      return exp;
    }

    // Reparents the right parent's child to its own
    (*exp)->reparent_children();
  }

  // Move the data from m + 1 to end
  left_node->redistribute(right_node, mid + 1);

  // If the root node was split, reparent and add to the height by 1
  if (left_node == this->root) {
    this->root = parent;
    ++this->height;
  }

  // Needs to return the right parent to be parented by the leaf/internal nodes
  return right_node;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
opt_err base_bptree_map<Derived, Key, Value, KeyCompare>::split_leaf_node(
    leaf_ptr left_leaf
) noexcept {
  // Create the left leaf and the parent node
  auto right_leaf = this->create_leaf_node();
  if (right_leaf == nullptr) {
    return error{BPTREE_MAP_BAD_ALLOC, def_err_vals};
  }

  auto* parent = left_leaf->get_parent();
  bool created = false;
  if (parent == nullptr) {
    created = true;
    parent = this->create_inner_node(left_leaf);
    if (parent == nullptr) {
      delete right_leaf; // NOLINT
      return error{BPTREE_MAP_BAD_ALLOC, def_err_vals};
    }
  }

  // Get the middle node(right bias)
  i32 mid = this->middle();
  auto key = left_leaf->at_key(mid);

  // Set the parents of the leaf nodes
  left_leaf->set_parent(parent);
  right_leaf->set_parent(parent);

  // Try to insert the middle node to the parent
  parent->insert(key, right_leaf);
  if (parent->get_size() == this->degree) {
    auto exp = this->split_inner_node(parent);
    if (!exp) {
      delete right_leaf; // NOLINT
      if (created) {
        delete parent; // NOLINT
        left_leaf->set_parent(nullptr);
      }

      return std::move(exp.error());
    }

    // Reparents the right parent's children to itself
    (*exp)->reparent_leaf_children();
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
  return null;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
template <typename Value_>
opt_err base_bptree_map<Derived, Key, Value, KeyCompare>::insert_impl(
    key_type key, Value_ value
) noexcept {
  Value value_copy{};
  if constexpr (std::is_rvalue_reference<Value_>::value) {
    value_copy = std::move(value);
  } else if constexpr (std::is_class<value_type>::value) {
    try_opt(value_copy.copy(value));
  } else {
    value_copy = value;
  }

  // Separate this to a function call
  if (this->height == 0) { // Create the root as a leaf node
    auto* leaf = this->root ? static_cast<leaf_ptr>(this->root)
                            : this->create_leaf_node();
    if (leaf == nullptr) {
      return error{BPTREE_MAP_BAD_ALLOC, def_err_vals};
    }

    leaf->push_back(key, std::move(value_copy));
    this->root = leaf;
    ++this->_size;
    this->height = 1;

    return null;
  }

  // Separate this to a function call
  if (this->height == 1) { // Root is the leaf node
    auto* leaf = static_cast<leaf_ptr>(this->root);
    i32 index = leaf->insert(key, std::move(value_copy));
    if (index == -1) {
      return null;
    }

    // Check if the leaf node exceeds the max degree rule
    ++this->_size;
    if (this->_size < this->degree) {
      return null;
    }

    // Split here
    if (auto error = this->split_leaf_node(leaf)) {
      leaf->erase(index);
      return error;
    }

    this->root = leaf->get_parent();
    this->height = 2;
    return null;
  }

  leaf_ptr leaf = this->find_leaf_node_containing(key);
  i32 index = leaf->insert(key, std::move(value_copy));

  if (index == -1) {
    return null;
  }

  ++this->_size;
  if (leaf->get_size() < this->degree) {
    return null;
  }

  // Split the leaf node
  if (auto error = this->split_leaf_node(leaf)) {
    leaf->erase(index);
    return error;
  }

  return null;
}

// * Erase * //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::borrow_or_merge_inner(
    inner_ptr parent, key_type key
) noexcept {
  i32 sz = 0;
  i32 index = 0;
  inner_ptr grandparent = nullptr;
  inner_ptr uncle = nullptr;
  void** children = nullptr;
  bool leaf_child = true;

  while (true) {
    // Get the current index of the parent relative to its parent
    grandparent = parent->get_parent();
    sz = grandparent->get_size();
    index = grandparent->find_smaller_index(key);

    // Try to borrow
    children = grandparent->get_children();
    if (index < sz) { // Borrow to the right uncle
      uncle = static_cast<inner_ptr>(children[index + 1]);
      if (uncle->get_size() > this->min_inner_children()) {
        parent->borrow_from_right_uncle(uncle, index, leaf_child);
        return;
      }
    }

    if (index > 0) { // Borrow to the left uncle
      uncle = static_cast<inner_ptr>(children[index - 1]);
      if (uncle->get_size() > this->min_inner_children()) {
        parent->borrow_from_left_uncle(uncle, index, leaf_child);
        return;
      }
    }

    // Try to merge
    grandparent->merge_children(index < sz ? index : sz - 1);

    // Check if the sz suffices the rule, else merge again from the parent's
    // parent
    if (sz >= this->degree / 2) {
      return;
    }

    if (this->root == grandparent) { // This is already the root node
      if (sz < 2) {                  // 0 children
        this->root = grandparent->get_children()[0];

        if (this->height == 2) {
          static_cast<leaf_ptr>(this->root)->set_parent(nullptr);
        } else {
          static_cast<inner_ptr>(this->root)->set_parent(nullptr);
        }

        --this->height;
        delete grandparent; // NOLINT
      }
      break;
    }

    parent = grandparent;
    leaf_child = false;
  }
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::borrow_or_merge_leaf(
    leaf_ptr leaf
) noexcept {
  // Check for right sibling
  auto* sibling = leaf->get_next();
  bool has_right_sibling =
      sibling && sibling->get_parent() == leaf->get_parent();
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

  // Fix internal nodes
  auto key = leaf->front_key();
  auto* parent = leaf->get_parent();
  i32 index = parent->find_smaller_index(key);

  parent->erase(index);
  i32 sz = parent->get_size();

  if (sz >= this->min_inner_children()) {
    return;
  }

  if (parent == this->root && sz == 0) {
    this->root = parent->front_child();
    static_cast<leaf_ptr>(this->root)->set_parent(nullptr);
    this->height = 1;
    return;
  }

  this->borrow_or_merge_inner(parent, key);
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::erase_root_leaf(
    key_type key
) noexcept {
  auto* leaf = static_cast<leaf_ptr>(this->root);
  auto* keys = leaf->get_keys();
  i32 sz = leaf->get_size();
  i32 comparison = 0;

  for (i32 i = 0; i < sz; ++i) {
    comparison = KeyCompare{}(key, keys[i]);
    if (comparison == 0) {
      leaf->erase(i);

      if (sz == 1) {
        this->height = 0;
      }

      return;
    }

    if (comparison < 0) {
      return;
    }
  }
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::erase(key_type key
) noexcept {
  if (this->height == 0) {
    return;
  }

  if (this->height == 1) {
    this->erase_root_leaf(key);
    return;
  }

  // Just try to handle leaf deletion
  auto* node = static_cast<inner_ptr>(this->root);
  inner_ptr grandparent = nullptr;
  i32 grandparent_index = 0;

  i32 index = 0;
  i32 sz = 0;
  key_ptr keys = nullptr;
  i32 comparison = 0;

  // Traverse the internal nodes until the parent containing the leaf nodes
  for (i32 h = this->height; h > 2; --h) {
    sz = node->get_size();
    keys = node->get_keys();

    for (index = 0; index < sz; ++index) {
      comparison = KeyCompare{}(key, keys[index]);
      if (comparison == 0) {
        grandparent = node;
        grandparent_index = index;
        ++index;
        break;
      }

      if (comparison < 0) {
        break;
      }
    }
    node = static_cast<inner_ptr>(node->get_children()[index]);
  }

  // Traverse the parent containing leaf nodes
  sz = node->get_size();
  keys = node->get_keys();
  for (index = 0; index < sz; ++index) {
    comparison = KeyCompare{}(key, keys[index]);
    if (comparison == 0) {
      grandparent = node;
      grandparent_index = index;
      ++index;
      break;
    }

    if (comparison < 0) {
      break;
    }
  }
  auto* leaf = static_cast<leaf_ptr>(node->get_children()[index]);

  if (grandparent) {
    if (this->degree > 3) {
      grandparent->get_keys()[grandparent_index] = keys[1];
    }

    --this->_size;

    leaf->erase(0);
    if (leaf->get_size() < this->min_leaf_children()) {
      this->borrow_or_merge_leaf(leaf);
    }
    return;
  }

  index = leaf->find_index(key);
  if (index == -1) {
    // Nothing is deleted
    return;
  }

  --this->_size;
  leaf->erase(index);
  if (leaf->get_size() < this->min_leaf_children()) {
    this->borrow_or_merge_leaf(leaf);
  }
}

// === Lookup === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::find(key_type key) noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_index(key);
  return index > -1 ? iterator{leaf, index} : iterator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::find(key_type key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_index(key);
  return index > -1 ? citerator{leaf, index} : citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_smaller(key_type key
) noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_smaller_index(key);
  return index > -1 ? iterator{leaf, index} : iterator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_smaller(key_type key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_smaller_index(key);
  return index > -1 ? citerator{leaf, index} : citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::iterator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_larger(key_type key
) noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
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
base_bptree_map<Derived, Key, Value, KeyCompare>::find_larger(key_type key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
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
base_bptree_map<Derived, Key, Value, KeyCompare>::find_not_smaller(key_type key
) noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
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
base_bptree_map<Derived, Key, Value, KeyCompare>::find_not_smaller(key_type key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
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
base_bptree_map<Derived, Key, Value, KeyCompare>::find_not_larger(key_type key
) noexcept {
  if (this->height == 0) {
    return iterator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_not_larger_index(key);
  return index > -1 ? iterator{leaf, index} : iterator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::citerator
base_bptree_map<Derived, Key, Value, KeyCompare>::find_not_larger(key_type key
) const noexcept {
  if (this->height == 0) {
    return citerator{};
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  i32 index = leaf->find_not_larger_index(key);
  return index > -1 ? citerator{leaf, index} : citerator{};
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
bool base_bptree_map<Derived, Key, Value, KeyCompare>::contains(key_type key
) const noexcept {
  if (this->height == 0) {
    return false;
  }

  leaf_ptr leaf = this->height == 1 ? static_cast<leaf_ptr>(this->root)
                                    : this->find_leaf_node_containing(key);
  return leaf->find_index(key) > -1;
}

} // namespace ds

#endif

