/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_BPTREE_MAP_NODE_HPP
#define DS_BPTREE_MAP_NODE_HPP

#include "./types.hpp"
#include <cstring>
#include <type_traits>

#ifdef DS_TEST
#include <cstdio>
#endif

namespace ds::_bptree_map {

template <
    typename Derived, typename Key, typename Value, typename KeyCompare,
    typename Allocator>
class base_bptree_map;
template <
    usize Degree, typename Key, typename Value, typename KeyCompare,
    typename Allocator>
class _inner_node;

template <
    usize Degree, typename Key, typename Value, typename KeyCompare,
    typename Allocator>
class _leaf_node {
private:
  // === Definitions === //
  [[nodiscard]] static constexpr i32 get_degree() noexcept {
    // Minimum degree is 3
    return 64U / sizeof(Key) > 3U ? 64U / sizeof(Key) : 3U;
  }

public:
  using inner_node = _inner_node<Degree, Key, Value, KeyCompare, Allocator>;

  _leaf_node() = default;
  _leaf_node(const _leaf_node&) = delete;
  _leaf_node& operator=(const _leaf_node&) = delete;

  // === Move === //

  _leaf_node(_leaf_node&& other) noexcept
      : parent(other.parent),
        next(other.next),
        prev(other.prev),
        size(other.size) {
    this->move(std::move(other));
    other.size = 0;
  }

  constexpr void move(_leaf_node&& other) noexcept {
    if constexpr (std::is_class_v<Key> || std::is_class_v<Value>) {
      if constexpr (std::is_class_v<Key>) {
        for (u32 i = 0; i < this->size; ++i) {
          this->get_keys()[i] = std::move(other.get_keys()[i]);
        }
      } else {
        std::memcpy(
            this->get_keys(), other.get_keys(), sizeof(Key) * get_degree()
        );
      }

      if constexpr (std::is_class_v<Value>) {
        for (u32 i = 0; i < this->size; ++i) {
          this->get_values()[i] = std::move(other.get_values()[i]);
        }
      } else {
        std::memcpy(
            this->get_values(), other.get_values(), sizeof(Value) * get_degree()
        );
      }
    } else {
      std::memcpy(
          this->get_keys(), other.get_keys(),
          (sizeof(Key) + sizeof(Value)) * get_degree()
      );
    }
  }

  _leaf_node& operator=(_leaf_node&& rhs) noexcept {
    if (&rhs == this) {
      return *this;
    }

    this->parent = rhs.parent;
    this->next = rhs.next;
    this->prev = rhs.prev;

    this->size = rhs.size;
    this->move(std::move(rhs));
    rhs.size = 0;

    return *this;
  }

  // === Destructor === //

  ~_leaf_node() noexcept {
    this->destroy();
  }

  void destroy() noexcept {
    if (this->size == 0) {
      return;
    }

    if constexpr (std::is_class_v<Key>) {
      for (u32 i = 0U; i < Degree; ++i) {
        this->keys[i].~Key();
      }
    }

    if constexpr (std::is_class_v<Value>) {
      for (u32 i = 0U; i < Degree; ++i) {
        this->values[i].~Value();
      }
    }

    this->size = 0;
  }

  // === Setters === //

  void set_parent(inner_node* parent) noexcept {
    this->parent = parent;
  }

  void set_next(_leaf_node* next) noexcept {
    this->next = next;
  }

  void set_prev(_leaf_node* prev) noexcept {
    this->prev = prev;
  }

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

  [[nodiscard]] _leaf_node* get_next() const noexcept {
    return this->next;
  }

  [[nodiscard]] _leaf_node* get_prev() const noexcept {
    return this->prev;
  }

  // === Element Access === //

  [[nodiscard]] Key& at_key(i32 index) noexcept {
    return this->keys[index];
  }

  [[nodiscard]] const Key& at_key(i32 index) const noexcept {
    return this->keys[index];
  }

  [[nodiscard]] Key& front_key() noexcept {
    return this->keys[0];
  }

  [[nodiscard]] const Key& front_key() const noexcept {
    return this->keys[0];
  }

  [[nodiscard]] Key& back_key() noexcept {
    return this->keys[this->size - 1];
  }

  [[nodiscard]] const Key& back_key() const noexcept {
    return this->keys[this->size - 1];
  }

  [[nodiscard]] Value& at_value(i32 index) noexcept {
    return this->values[index];
  }

  [[nodiscard]] const Value& at_value(i32 index) const noexcept {
    return this->values[index];
  }

  // === Modifiers === //

  i32 insert(Key key, Value&& value) noexcept {
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

    this->push(key, std::move(value));
    return this->size;
  }

  void push(Key key, Value&& value) noexcept {
    this->keys[this->size] = key;
    this->values[this->size] = std::move(value);
    ++this->size;
  }

  void remove(i32 index) noexcept {
    for (i32 i = index + 1; i < this->size; ++i) {
      this->keys[i - 1] = this->keys[i];
      this->values[i - 1] = std::move(this->values[i]);
    }

    --this->size;
  }

  void clear() noexcept {
    this->size = 0;
  }

  void redistribute(_leaf_node* other, i32 mid) noexcept {
    for (i32 i = mid; i < this->size; ++i) {
      other->push(this->keys[i], std::move(this->values[i]));
    }
    this->size = mid;
  }

  void borrow_right_sibling() noexcept {
    Key key = this->next->front_key();
    this->push(key, std::move(this->next->values[0]));
    this->next->remove(0);

    for (i32 i = 0;; ++i) {
      if (this->parent->get_children()[i] == this) {
        this->parent->get_keys()[i] = this->next->front_key();
        break;
      }
    }
  }

  void borrow_left_sibling() noexcept {
    i32 sz = this->prev->size - 1;

    Key key = this->prev->keys[sz];
    this->insert_indexed(0, key, std::move(this->prev->values[sz]));
    this->prev->remove(sz);

    for (i32 i = 1;; ++i) {
      if (this->parent->get_children()[i] == this) {
        this->parent->get_keys()[i - 1] = key;
        break;
      }
    }
  }

  void merge_right_sibling() noexcept {
    // Move all the children of the right child to this node
    _leaf_node* sibling = this->next;
    for (i32 i = 0; i < sibling->size; ++i) {
      this->push(sibling->keys[i], std::move(sibling->values[i]));
    }

    // Adjust the pointers to the correct values
    this->next = sibling->get_next();
    if (this->next) {
      this->next->set_prev(this);
    }

    // Remove the sibling reference from the parent node
    for (i32 i = 1;; ++i) {
      if (sibling == this->parent->get_children()[i]) {
        this->parent->remove(i - 1);

        // Change the parent pointer, but not to the very leftmost leaf node
        if (i >= 2) {
          this->parent->set_key(i - 2, this->front_key());
        }
        break;
      }
    }

    // Merged to right sibling, delete this pointer
    Allocator{}.deallocate(sibling);
  }

  // === Lookup === //

  [[nodiscard]] Value* get_value(Key key) noexcept {
    for (i32 i = 0; i < this->size; ++i) {
      if (KeyCompare{}(key, this->keys[i]) == 0) {
        return this->values + i;
      }
    }

    return nullptr;
  }

  [[nodiscard]] i32 find_index(Key key) const noexcept {
    for (i32 i = 0U; i < this->size; ++i) {
      if (KeyCompare{}(key, this->keys[i]) == 0) {
        return i;
      }
    }

    return -1;
  }

  [[nodiscard]] i32 find_smaller_index(Key key) const noexcept {
    for (i32 i = this->size - 1; i > -1; --i) {
      if (KeyCompare{}(key, this->keys[i]) > 0) {
        return i;
      }
    }

    return -1;
  }

  [[nodiscard]] i32 find_larger_index(Key key) const noexcept {
    for (i32 i = 0; i < this->size; ++i) {
      if (KeyCompare{}(key, this->keys[i]) < 0) {
        return i;
      }
    }

    return -1;
  }

  [[nodiscard]] i32 find_not_smaller_index(Key key) const noexcept {
    for (i32 i = 0; i < this->size; ++i) {
      if (KeyCompare{}(key, this->keys[i]) <= 0) {
        return i;
      }
    }

    return -1;
  }

  [[nodiscard]] i32 find_not_larger_index(Key key) const noexcept {
    for (i32 i = this->size - 1; i > -1; --i) {
      if (KeyCompare{}(key, this->keys[i]) >= 0) {
        return i;
      }
    }

    return -1;
  }

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
  Key keys[Degree]; // NOLINT
  i32 size = 0;

  inner_node* parent = nullptr;
  _leaf_node* next = nullptr;
  _leaf_node* prev = nullptr;

  Value values[Degree]; // NOLINT

  void insert_indexed(i32 index, Key key, Value&& value) noexcept {
    if (index == this->size) {
      this->push(key, std::move(value));
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
};

// * === Inner Node === * //

template <
    usize Degree, typename Key, typename Value, typename KeyCompare,
    typename Allocator>
class _inner_node {
public:
  using leaf_node = _leaf_node<Degree, Key, Value, KeyCompare, Allocator>;

  _inner_node() = default;
  _inner_node(const _inner_node&) = delete;
  _inner_node& operator=(const _inner_node&) = delete;

  // === Move === //

  _inner_node(_inner_node&&) = delete;
  _inner_node& operator=(_inner_node&&) = delete;

  // === Destructor === //

  ~_inner_node() noexcept {
    this->destroy();
  }

  void destroy() noexcept {
    if (this->size == 0) {
      return;
    }

    if constexpr (std::is_class_v<Key>) {
      for (i32 i = 0; i < Degree; ++i) {
        this->keys[i].~Key();
      }
    }

    this->size = 0;
  }

  void destroy_last_child() noexcept {
    // NOTE: inner_node & leaf_node parent are aligned
    Allocator{}.deallocate(this->children[this->size]);
    --this->size;
  }

  void destroy_leaf_children() noexcept {
    for (i32 i = 0; i <= this->size; ++i) {
      Allocator{}.deallocate(this->children[i]);
    }
  }

  // === Setters === //

  void set_parent(_inner_node* parent) noexcept {
    this->parent = parent;
  }

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

  [[nodiscard]] _inner_node* get_parent() const noexcept {
    return this->parent;
  }

  // === Element Access === //

  /**
   * First child that is less than the key, if all keys are smaller than the
   * key passed, this returns the last child
   **/
  [[nodiscard]] void* find_smaller_child(Key key) const noexcept {
    for (i32 i = 0; i < this->size; ++i) {
      if (KeyCompare{}(key, this->keys[i]) < 0) {
        return this->children[i];
      }
    }

    return this->children[this->size];
  }

  [[nodiscard]] i32 find_smaller_index(Key key) const noexcept {
    for (i32 i = 0; i < this->size; ++i) {
      if (KeyCompare{}(key, this->keys[i]) < 0) {
        return i;
      }
    }

    return this->size;
  }

  [[nodiscard]] Key at_key(i32 index) const noexcept {
    return this->keys[index];
  }

  [[nodiscard]] Key front_key() const noexcept {
    return this->keys[0];
  }

  [[nodiscard]] Key back_key() const noexcept {
    return this->keys[this->size - 1];
  }

  [[nodiscard]] void* front_child() const noexcept {
    return this->children[0];
  }

  [[nodiscard]] void* back_child() const noexcept {
    return this->children[this->size];
  }

  // === Modifiers === //

  void insert(Key key, void* child) noexcept {
    for (i32 i = 0; i < this->size; ++i) {
      if (KeyCompare{}(key, this->keys[i]) < 0) {
        this->insert_indexed(i, key, child);
        return;
      }
    }

    this->push(key, child);
  }

  void push(Key key, void* child) noexcept {
    this->keys[this->size] = key;
    this->children[++this->size] = child;
  }

  void pop() noexcept {
    --this->size;
  }

  void push_front(Key key, void* child) noexcept {
    this->children[this->size + 1] = this->children[this->size];
    for (i32 i = this->size; i > 0; --i) {
      this->keys[i] = this->keys[i - 1];
      this->children[i] = this->children[i - 1];
    }

    this->keys[0] = key;
    this->children[0] = child;
    ++this->size;
  }

  void pop_front() noexcept {
    for (i32 i = 1; i < this->size; ++i) {
      this->keys[i - 1] = this->keys[i];
      this->children[i - 1] = this->children[i];
    }
    this->children[this->size - 1] = this->children[this->size];

    --this->size;
  }

  void remove(i32 index) noexcept {
    for (i32 i = index + 1; i < this->size; ++i) {
      this->keys[i - 1] = this->keys[i];
      this->children[i] = this->children[i + 1];
    }

    --this->size;
  }

  void set_key(i32 index, Key key) noexcept {
    this->keys[index] = key;
  }

  void reparent_children() noexcept {
    // NOTE: inner_node & leaf_node parent are aligned
    for (i32 i = 0; i <= this->size; ++i) {
      static_cast<_inner_node*>(this->children[i])->set_parent(this);
    }
  }

  // === Inserting === //

  void redistribute(_inner_node* other, i32 mid) noexcept {
    for (i32 i = mid; i < this->size; ++i) {
      other->push(this->keys[i], this->children[i + 1]);
    }
    // The mid should have the node that is transferred to the parent
    this->size = mid - 1;
  }

  // === Removing === //

  void
  borrow_from_right_uncle(_inner_node* right_uncle, i32 parent_index) noexcept {
    // Get the grandparent key and the first child pointer from the right uncle
    void* child = right_uncle->front_child();
    // NOTE: inner_node & leaf_node parent are aligned
    static_cast<_inner_node*>(child)->set_parent(this);

    // Put the first child of the uncle to the last child of this node
    this->push(this->parent->at_key(parent_index), child);

    // Grandparents key change to the right uncle's key
    this->parent->set_key(parent_index, right_uncle->front_key());

    // Erase the first element from the uncle
    right_uncle->pop_front();
  }

  void
  borrow_from_left_uncle(_inner_node* left_uncle, i32 parent_index) noexcept {
    // Get the grandparent key and the last child pointer from the left uncle
    auto* child = left_uncle->back_child();

    // NOTE: inner_node & leaf_node parent are aligned
    static_cast<_inner_node*>(child)->set_parent(this);
    --parent_index;

    // Put the last child of the uncle to the first child of this node
    this->push_front(this->parent->at_key(parent_index), child);

    // Grandparents key change to the left uncle's key
    this->parent->set_key(parent_index, left_uncle->back_key());

    // Erase the last element from the uncle
    left_uncle->pop();
  }

  // Merges index and index + 1
  void merge_children(i32 index) noexcept {
    auto* child = static_cast<_inner_node*>(this->children[index]);
    auto* sibling = static_cast<_inner_node*>(this->children[index + 1]);

    // child <- sibling
    Key* sibling_keys = sibling->keys;
    void** sibling_children = sibling->children;

    void* grandchild = sibling_children[0];
    // NOTE: inner_node & leaf_node parent are aligned
    static_cast<_inner_node*>(grandchild)->parent = child;
    child->push(this->keys[index], grandchild);

    for (i32 i = 0; i < sibling->size; ++i) {
      grandchild = sibling_children[i + 1];
      // NOTE: inner_node & leaf_node parent are aligned
      static_cast<_inner_node*>(grandchild)->parent = child;
      child->push(sibling_keys[i], grandchild);
    }

    Allocator{}.deallocate(sibling);
    this->remove(index);
  }

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
  Key keys[Degree]; // NOLINT
  i32 size = 0;

  _inner_node* parent = nullptr;

  void* children[Degree + 1]{}; // NOLINT

  void insert_indexed(i32 index, Key key, void* child) noexcept {
    for (i32 i = this->size; i > index; --i) {
      this->keys[i] = this->keys[i - 1];
      this->children[i + 1] = this->children[i];
    }

    this->keys[index] = key;
    this->children[index + 1] = child;

    ++this->size;
  }
};

} // namespace ds::_bptree_map

#endif
