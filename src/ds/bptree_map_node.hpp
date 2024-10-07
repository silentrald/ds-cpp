/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_BPTREE_MAP_NODE_HPP
#define DS_BPTREE_MAP_NODE_HPP

#include "./bptree_map.hpp"
#include "types.hpp"
#include <cstdio>
#include <cstdlib>
#include <type_traits>

// * === Leaf Node === * //

namespace ds {

// === Move === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::leaf_node(
    leaf_node&& other
) noexcept
    : parent(other.parent),
      next(other.next),
      prev(other.prev),
      size(other.size) {
  this->move(std::move(other));
  other.size = 0;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
typename base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node&
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::operator=(
    leaf_node&& other
) noexcept {
  if (&other == this) {
    return *this;
  }

  this->parent = other.parent;
  this->next = other.next;
  this->prev = other.prev;

  this->size = other.size;
  this->move(std::move(other));
  other.size = 0;

  return *this;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::~leaf_node(
) noexcept {
  this->destroy();
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::destroy(
) noexcept {
  if (this->size == 0) {
    return;
  }

  if constexpr (std::is_class<Key>::value) {
    for (u32 i = 0U;
         i < base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree();
         ++i) {
      this->keys[i].~Key();
    }
  }

  if constexpr (std::is_class<Value>::value) {
    for (u32 i = 0U;
         i < base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree();
         ++i) {
      this->values[i].~Value();
    }
  }

  this->size = 0;
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
    insert_indexed(i32 index, Key key, Value&& value) noexcept {
  if (index == this->size) {
    this->push_back(key, std::move(value));
    return;
  }

  // Move the elements
  for (i32 i = this->size; i > index; --i) {
    this->keys[i] = this->keys[i - 1];
  }
  for (i32 i = this->size; i > index; --i) {
    this->values[i] = std::move(this->values[i - 1]);
  }

  this->keys[index] = key;
  this->values[index] = std::move(value);
  ++this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::insert(
    Key key, Value&& value
) noexcept {
  isize comparison = 0;

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
    Key key, Value&& value
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
  Key key = this->next->front_key();
  this->push_back(key, std::move(this->next->values[0]));
  this->next->erase(0);

  for (i32 i = 0;; ++i) {
    if (this->parent->get_children()[i] == this) {
      this->parent->get_keys()[i] = this->next->front_key();
      break;
    }
  }
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::
    borrow_left_sibling() noexcept {
  i32 sz = this->prev->size - 1;

  Key key = this->prev->keys[sz];
  this->insert_indexed(0, key, std::move(this->prev->values[sz]));
  this->prev->erase(sz);

  for (i32 i = 1;; ++i) {
    if (this->parent->get_children()[i] == this) {
      this->parent->get_keys()[i - 1] = key;
      break;
    }
  }
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::
    merge_right_sibling() noexcept {
  // Move all the children of the right child to this node
  leaf_node* sibling = this->next;
  for (i32 i = 0; i < sibling->size; ++i) {
    this->push_back(sibling->keys[i], std::move(sibling->values[i]));
  }

  // Adjust the pointers to the correct values
  this->next = sibling->get_next();
  if (this->next) {
    this->next->set_prev(this);
  }

  // Remove the sibling reference from the parent node
  for (i32 i = 1;; ++i) {
    if (sibling == this->parent->get_children()[i]) {
      this->parent->erase(i - 1);

      // Change the parent pointer, but not to the very leftmost leaf node
      if (i >= 2) {
        this->parent->set_key(i - 2, this->front_key());
      }
      break;
    }
  }

  // Merged to right sibling, delete this pointer
  std::free(sibling); // NOLINT
}

// === Lookup === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
Value*
base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::get_value(Key key
) noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) == 0) {
      return this->values + i;
    }
  }

  return nullptr;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<Derived, Key, Value, KeyCompare>::leaf_node::find_index(
    Key key
) const noexcept {
  for (i32 i = 0U; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) == 0) {
      return i;
    }
  }

  return -1;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<
    Derived, Key, Value, KeyCompare>::leaf_node::find_smaller_index(Key key
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
    Derived, Key, Value, KeyCompare>::leaf_node::find_larger_index(Key key
) const noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) < 0) {
      return i;
    }
  }

  return -1;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<
    Derived, Key, Value, KeyCompare>::leaf_node::find_not_smaller_index(Key key
) const noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) <= 0) {
      return i;
    }
  }

  return -1;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<
    Derived, Key, Value, KeyCompare>::leaf_node::find_not_larger_index(Key key
) const noexcept {
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

// === Destructor === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::~inner_node(
) noexcept {
  this->destroy();
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::destroy(
) noexcept {
  if (this->size == 0) {
    return;
  }

  if constexpr (std::is_class<Key>::value) {
    for (i32 i = 0;
         i < base_bptree_map<Derived, Key, Value, KeyCompare>::get_degree();
         ++i) {
      this->keys[i].~Key();
    }
  }

  this->size = 0;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    destroy_last_child() noexcept {
  // NOTE: inner_node & leaf_node parent are aligned
  std::free(this->children[this->size]); // NOLINT
  --this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    destroy_leaf_children() noexcept {
  for (i32 i = 0; i <= this->size; ++i) {
    std::free((leaf_node*)this->children[i]); // NOLINT
  }
}

// === Setters === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::set_parent(
    inner_node* parent
) noexcept {
  this->parent = parent;
}

// === Element Access === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void* base_bptree_map<
    Derived, Key, Value, KeyCompare>::inner_node::find_smaller_child(Key key
) const noexcept {
  for (i32 i = 0; i < this->size; ++i) {
    if (KeyCompare{}(key, this->keys[i]) < 0) {
      return this->children[i];
    }
  }

  return this->children[this->size];
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
i32 base_bptree_map<
    Derived, Key, Value, KeyCompare>::inner_node::find_smaller_index(Key key
) const noexcept {
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
    insert_indexed(i32 index, Key key, void* child) noexcept {
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
    Key key, void* child
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
    Key key, void* child
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
    Key key, void* child
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
  for (i32 i = index + 1; i < this->size; ++i) {
    this->keys[i - 1] = this->keys[i];
    this->children[i] = this->children[i + 1];
  }

  --this->size;
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::set_key(
    i32 index, Key key
) noexcept {
  this->keys[index] = key;
}

// === Reparenting === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<
    Derived, Key, Value, KeyCompare>::inner_node::reparent_children() noexcept {
  // NOTE: inner_node & leaf_node parent are aligned
  for (i32 i = 0; i <= this->size; ++i) {
    static_cast<inner_node*>(this->children[i])->set_parent(this);
  }
}

// === Inserting === //
template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::redistribute(
    inner_node* other, i32 mid
) noexcept {
  for (i32 i = mid; i < this->size; ++i) {
    other->push_back(this->keys[i], this->children[i + 1]);
  }
  // The mid should have the node that is transferred to the parent
  this->size = mid - 1;
}

// === Erasing === //

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    borrow_from_right_uncle(
        inner_node* right_uncle, i32 parent_index
    ) noexcept {
  // Get the grandparent key and the first child pointer from the right uncle
  void* child = right_uncle->front_child();
  // NOTE: inner_node & leaf_node parent are aligned
  static_cast<inner_node*>(child)->set_parent(this);

  // Put the first child of the uncle to the last child of this node
  this->push_back(this->parent->at_key(parent_index), child);

  // Grandparents key change to the right uncle's key
  this->parent->set_key(parent_index, right_uncle->front_key());

  // Erase the first element from the uncle
  right_uncle->pop_front();
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<Derived, Key, Value, KeyCompare>::inner_node::
    borrow_from_left_uncle(inner_node* left_uncle, i32 parent_index) noexcept {
  // Get the grandparent key and the last child pointer from the left uncle
  auto* child = left_uncle->back_child();

  // NOTE: inner_node & leaf_node parent are aligned
  static_cast<inner_node*>(child)->set_parent(this);
  --parent_index;

  // Put the last child of the uncle to the first child of this node
  this->push_front(this->parent->at_key(parent_index), child);

  // Grandparents key change to the left uncle's key
  this->parent->set_key(parent_index, left_uncle->back_key());

  // Erase the last element from the uncle
  left_uncle->pop_back();
}

template <typename Derived, typename Key, typename Value, typename KeyCompare>
void base_bptree_map<
    Derived, Key, Value, KeyCompare>::inner_node::merge_children(i32 index
) noexcept {
  auto* child = static_cast<inner_node*>(this->children[index]);
  auto* sibling = static_cast<inner_node*>(this->children[index + 1]);

  // child <- sibling
  Key* sibling_keys = sibling->keys;
  void** sibling_children = sibling->children;

  void* grandchild = sibling_children[0];
  // NOTE: inner_node & leaf_node parent are aligned
  static_cast<inner_node*>(grandchild)->parent = child;
  child->push_back(this->keys[index], grandchild);

  for (i32 i = 0; i < sibling->size; ++i) {
    grandchild = sibling_children[i + 1];
    // NOTE: inner_node & leaf_node parent are aligned
    static_cast<inner_node*>(grandchild)->parent = child;
    child->push_back(sibling_keys[i], grandchild);
  }

  std::free(sibling); // NOLINT
  this->erase(index);
}

} // namespace ds

#endif
