/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 * Updated: 2023-01-04
 *===============================*/

#ifndef DS_BPTREE_MAP_NODE_HPP
#define DS_BPTREE_MAP_NODE_HPP

#include "../vector/def.hpp"
#include "./def.hpp"
#include <cstdlib>
#include <new>
#include <type_traits>

// * === Leaf Node === * //
namespace ds {

// === Initializers === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
opt_err
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::init(i32 capacity
) noexcept {
  this->keys = new (std::nothrow) Key[capacity]; // NOLINT
  if (this->keys == nullptr) {
    return error{BPTREE_MAP_NODE_BAD_ALLOC, def_err_vals};
  }

  // NOLINTNEXTLINE
  this->values = static_cast<Value*>(std::malloc(sizeof(Value) * capacity));
  if (this->values == nullptr) {
    delete this->keys; // NOLINT
    this->keys = nullptr;

    return error{BPTREE_MAP_NODE_BAD_ALLOC, def_err_vals};
  }

  new (this->values) Value[capacity];
  return null;
}

// === Move === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::leaf_node(
    leaf_node&& other
) noexcept
    : keys(other.keys),
      values(other.values),
      size(other.size),
      parent(other.parent),
      next(other.next),
      prev(other.prev) {
  other.keys = nullptr;
  other.values = nullptr;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node&
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::operator=(
    leaf_node&& other
) noexcept {
  if (&other == this) {
    return *this;
  }
  delete this->keys;   // NOLINT
  delete this->values; // NOLINT

  this->keys = other.keys;
  this->values = other.values;
  this->size = other.size;

  this->parent = other.parent;
  this->next = other.next;
  this->prev = other.prev;

  other.keys = nullptr;
  other.values = nullptr;

  return *this;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::~leaf_node(
) noexcept {
  delete this->keys; // NOLINT
  this->keys = nullptr;

  delete this->values; // NOLINT
  this->values = nullptr;
}

// === Setters === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::set_parent(
    inner_node* parent
) noexcept {
  this->parent = parent;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::set_next(
    leaf_node* next
) noexcept {
  this->next = next;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::set_prev(
    leaf_node* prev
) noexcept {
  this->prev = prev;
}

// === Getters === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Key* base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::get_keys(
) const noexcept {
  return this->keys;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Value* base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::get_values(
) const noexcept {
  return this->values;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::get_size(
) const noexcept {
  return this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node*
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::get_parent(
) const noexcept {
  return this->parent;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node*
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::get_next(
) const noexcept {
  return this->next;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node*
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::get_prev(
) const noexcept {
  return this->prev;
}

// === Element Access === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Key base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::at_key(
    i32 index
) const noexcept {
  return this->keys[index];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Key base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::front_key(
) const noexcept {
  return this->keys[0];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Key base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::back_key(
) const noexcept {
  return this->keys[this->size - 1];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Value&
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::at_value(i32 index
) const noexcept {
  return this->values[index];
}

// === Modifiers === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::
    insert_indexed(i32 index, key_type key, Value&& value) noexcept {
  if (index == this->size) {
    this->push_back(key, std::move(value));
    return;
  }

  // Move the elements
  for (i32 i = this->size; i > index; --i) {
    this->keys[i] = this->keys[i - 1];
    this->values[i] = std::move(this->values[i - 1]);
  }

  this->keys[index] = key;
  this->values[index] = std::move(value);
  ++this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::insert(
    key_type key, Value&& value
) noexcept {
  i32 comparison = 0;

  for (i32 i = 0; i < this->size; ++i) {
    comparison = KeyCompare{}(key, this->keys[i]);

    if (comparison == 0) {
      this->values[i] = std::move(value);
      return -1;
    }

    if (comparison < 0) {
      this->insert_indexed(i, key, std::move(value));
      return i;
    }
  }

  this->push_back(key, std::move(value));
  return this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::push_back(
    key_type key, Value&& value
) noexcept {
  this->keys[this->size] = key;
  this->values[this->size] = std::move(value);
  ++this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::erase(
    i32 index
) noexcept {

  for (i32 i = index + 1; i < this->size; ++i) {
    this->keys[i - 1] = this->keys[i];
    this->values[i - 1] = std::move(this->values[i]);
  }

  --this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::clear(
) noexcept {
  if constexpr (std::is_class<Value>::value) {
    for (i32 i = 0; i < this->size; ++i) {
      this->values[i].~Value();
    }
  }

  this->size = 0;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::redistribute(
    leaf_node* other, i32 mid
) noexcept {
  for (i32 i = mid; i < this->size; ++i) {
    other->push_back(this->keys[i], std::move(this->values[i]));
  }
  this->size = mid;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::
    borrow_right_sibling() noexcept {
  auto key = this->next->get_keys()[0];
  this->push_back(key, std::move(this->next->get_values()[0]));
  this->next->erase(0);

  auto* keys = this->parent->get_keys();
  i32 sz = this->parent->get_size() - 1;

  for (i32 i = 0; i < sz; ++i) {
    if (KeyCompare{}(key, keys[i]) == 0) {
      keys[i] = this->next->get_keys()[0];
      return;
    }
  }

  keys[sz] = this->next->get_keys()[0];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::
    borrow_left_sibling() noexcept {
  i32 sz = this->prev->get_size() - 1;

  auto key = this->prev->get_keys()[sz];
  this->insert_indexed(0, key, std::move(this->prev->get_values()[sz]));
  this->prev->erase(sz);

  auto* keys = this->parent->get_keys();
  sz = this->parent->get_size() - 1;

  for (i32 i = 0; i < sz; ++i) {
    if (KeyCompare{}(key, keys[i]) < 0) {
      keys[i] = key;
      return;
    }
  }

  keys[sz] = key;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::
    merge_right_sibling() noexcept {
  auto* keys = this->next->get_keys();
  auto* values = this->next->get_values();
  i32 sz = this->next->get_size();

  for (i32 i = 0; i < sz; ++i) {
    this->push_back(keys[i], std::move(values[i]));
  }

  auto* tmp = this->next;
  this->next = this->next->get_next();
  if (this->next) {
    this->next->set_prev(this);
  }

  delete tmp; // NOLINT
}

// === Lookup === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
Value* base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::get_value(
    key_type key
) const noexcept {
  for (i32 i = 0; this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) == 0) {
      return this->values + i;
    }
  }
  return nullptr;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::find_index(
    key_type key
) const noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) == 0) {
      return i;
    }
  }

  return -1;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<
    Derived, Key, Value, KeyCompare>::leaf_node::find_smaller_index(key_type key
) const noexcept {
  for (i32 i = this->size - 1; i > -1; --i) {
    if (KeyCompare{}(key, this->keys[i]) > 0) {
      return i;
    }
  }

  return -1;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<
    Derived, Key, Value, KeyCompare>::leaf_node::find_larger_index(key_type key
) const noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) < 0) {
      return i;
    }
  }

  return -1;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::
    find_not_smaller_index(key_type key) const noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) <= 0) {
      return i;
    }
  }

  return -1;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::
    find_not_larger_index(key_type key) const noexcept {
  for (i32 i = this->size - 1; i > -1; --i) {
    if (KeyCompare{}(key, this->keys[i]) >= 0) {
      return i;
    }
  }

  return -1;
}

} // namespace ds

// * === Inner Node === * //
namespace ds {

// === Initializers === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
opt_err base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::init(
    i32 capacity, void* child
) noexcept {

  this->keys = new (std::nothrow) Key[capacity]; // NOLINT
  if (this->keys == nullptr) {
    return error{BPTREE_MAP_NODE_BAD_ALLOC, def_err_vals};
  }

  // NOLINTNEXTLINE
  this->children = new (std::nothrow) void*[capacity + 1];
  if (this->children == nullptr) {
    delete this->keys; // NOLINT
    return error{BPTREE_MAP_NODE_BAD_ALLOC, def_err_vals};
  }

  this->children[0] = child;
  return null;
}

// === Destructor === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::~inner_node(
) noexcept {
  delete this->keys; // NOLINT
  this->keys = nullptr;

  delete this->children; // NOLINT
  this->children = nullptr;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    destroy_last_child() noexcept {
  delete static_cast<leaf_ptr>(this->children[this->size]);
  --this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    destroy_leaf_children() noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    delete static_cast<leaf_ptr>(this->children[i]); // NOLINT
  }
  delete static_cast<leaf_ptr>(this->children[this->size]); // NOLINT
}

// === Setters === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::set_parent(
    inner_node* parent
) noexcept {
  this->parent = parent;
}

// === Getters === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
Key* base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::get_keys(
) const noexcept {
  return this->keys;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void**
base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::get_children(
) const noexcept {
  return this->children;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::get_size(
) const noexcept {
  return this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::inner_ptr
base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::get_parent(
) const noexcept {
  return this->parent;
}

// === Element Access === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void* base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    find_smaller_child(key_type key) const noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) < 0) {
      return this->children[i];
    }
  }

  return this->children[this->size];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    find_smaller_index(key_type key) const noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) < 0) {
      return i;
    }
  }
  return this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Key base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::at_key(
    i32 index
) const noexcept {
  return this->keys[index];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Key base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::front_key(
) const noexcept {
  return this->keys[0];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Key base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::back_key(
) const noexcept {
  return this->keys[this->size - 1];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void* base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::front_child(
) const noexcept {
  return this->children[0];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void* base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::back_child(
) const noexcept {
  return this->children[this->size];
}

// === Modifiers === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    insert_indexed(i32 index, key_type key, void* child) noexcept {
  for (i32 i = this->size; i > index; --i) {
    this->keys[i] = this->keys[i - 1];
    this->children[i + 1] = this->children[i];
  }

  this->keys[index] = key;
  this->children[index + 1] = child;

  ++this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::insert(
    key_type key, void* child
) noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) < 0) {
      this->insert_indexed(i, key, child);
      return;
    }
  }

  this->push_back(key, child);
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::push_front(
    key_type key, void* child
) noexcept {
  this->children[this->size + 1] = this->children[this->size];
  for (i32 i = this->size; i > 0; --i) {
    this->keys[i] = this->keys[i - 1];
    this->children[i] = this->children[i - 1];
  }

  this->keys[0] = key;
  this->children[0] = child;
  ++this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::pop_front(
) noexcept {
  for (i32 i = 1; i < this->size; ++i) {
    this->keys[i - 1] = this->keys[i];
    this->children[i - 1] = this->children[i];
  }
  this->children[this->size - 1] = this->children[this->size];

  --this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::push_back(
    key_type key, void* child
) noexcept {
  this->keys[this->size] = key;
  this->children[++this->size] = child;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::pop_back(
) noexcept {
  --this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::erase(
    i32 index
) noexcept {
  // Deletes key[i] and child[i + 1]
  for (i32 i = index + 1; i < this->size; ++i) {
    this->keys[i - 1] = this->keys[i];
    this->children[i] = this->children[i + 1];
  }

  --this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::set_key(
    i32 index, key_type key
) noexcept {
  this->keys[index] = key;
}

// === Reparenting === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    reparent_leaf_children() noexcept {
  for (i32 i = 0; i <= this->size; ++i) {
    static_cast<leaf_node*>(this->children[i])->set_parent(this);
  }
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<
    Derived, Key, Value, KeyCompare>::inner_node::reparent_children() noexcept {
  for (i32 i = 0; i <= this->size; ++i) {
    static_cast<inner_node*>(this->children[i])->set_parent(this);
  }
}

// === Inserting === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::redistribute(
    inner_node* other, i32 mid
) noexcept {
  for (i32 i = mid; i < size; ++i) {
    other->push_back(this->keys[i], this->children[i + 1]);
  }
  // The mid should have the node that is transferred to the parent
  this->size = mid - 1;
}

// === Erasing === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    borrow_from_right_uncle(
        inner_node* right_uncle, i32 parent_index, bool leaf_child
    ) noexcept {
  // Get the grandparent key and the first child pointer from the right uncle
  auto* child = right_uncle->front_child();

  if (leaf_child) { // Reparent the child to its new parent
    static_cast<leaf_ptr>(child)->set_parent(parent);
  } else {
    static_cast<inner_ptr>(child)->set_parent(parent);
  }
  parent->push_back(this->parent->at_key(parent_index), child);

  // Grandparents key change to the right uncle's key
  this->parent->set_key(parent_index, right_uncle->front_key());

  // Erase the first element from the uncle
  right_uncle->pop_front();
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    borrow_from_left_uncle(
        inner_node* left_uncle, i32 parent_index, bool leaf_child
    ) noexcept {
  // Get the grandparent key and the last child pointer from the left uncle
  auto* child = left_uncle->back_child();
  if (leaf_child) {
    static_cast<leaf_ptr>(child)->set_parent(parent);
  } else {
    static_cast<inner_ptr>(child)->set_parent(parent);
  }
  --parent_index;
  parent->push_front(this->parent->at_key(parent_index), child);

  // Grandparents key change to the left uncle's key
  this->parent->set_key(parent_index, left_uncle->back_key());

  // Erase the last element from the uncle
  left_uncle->pop_back();
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<
    Derived, Key, Value, KeyCompare>::inner_node::merge_children(i32 index
) noexcept {
  auto* child1 = static_cast<inner_node*>(this->children[index]);
  auto* child2 = static_cast<inner_node*>(this->children[index + 1]);

  // child1 <- child2
  auto* _keys = child2->get_keys();
  auto* _children = child2->get_children();
  auto _size = child2->get_size();

  auto* grandchild = static_cast<inner_node*>(_children[0]);
  grandchild->parent = child1;
  child1->push_back(this->keys[index], grandchild);

  for (i32 i = 0; i < _size; ++i) {
    grandchild = static_cast<inner_node*>(_children[i + 1]);
    grandchild->parent = child1;
    child1->push_back(_keys[i], grandchild);
  }
  delete child2; // NOLINT

  this->erase(index);
}

} // namespace ds

#endif

