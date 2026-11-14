#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    using BinarySearchTree<Key, Value>::nodeSwap;
    virtual void nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rebalance(AVLNode<Key, Value>* node, bool& subtreeShrank);
    void insertFix(AVLNode<Key, Value>* inserted);
    void removeFix(AVLNode<Key, Value>* parent, int change);

};


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    // TODO
    AVLNode<Key, Value>* child = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setRight(child->getLeft());
    if(child->getLeft() != NULL){
        child->getLeft()->setParent(node);
    }

    child->setLeft(node);
    child->setParent(parent);
    node->setParent(child);

    if(parent == NULL){
        this->root_ = child;
    }
    else if(parent->getLeft() == node){
        parent->setLeft(child);
    }
    else{
        parent->setRight(child);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* child = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setLeft(child->getRight());
    if(child->getRight() != NULL) {
        child->getRight()->setParent(node);
    }

    child->setRight(node);
    child->setParent(parent);
    node->setParent(child);

    if(parent == NULL) {
        this->root_ = child;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }
}

/**
 * Restore balance at a node whose balance has reached -2 or +2.
 * Return the new subtree root. subtreeShrank matters during deletion:
 * if the heavy child was balanced (balance 0), the subtree height stays
 * unchanged after rotation; otherwise it decreases by one.
 */
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance(
    AVLNode<Key, Value>* node, bool& subtreeShrank)
{
    if(node->getBalance() == -2) {
        AVLNode<Key, Value>* left = node->getLeft();
        if(left->getBalance() <= 0) {
            int oldBalance = left->getBalance();
            rotateRight(node);
            if(oldBalance == 0) {
                node->setBalance(-1);
                left->setBalance(1);
                subtreeShrank = false;
            }
            else {
                node->setBalance(0);
                left->setBalance(0);
                subtreeShrank = true;
            }
            return left;
        }

        AVLNode<Key, Value>* middle = left->getRight();
        int middleBalance = middle->getBalance();
        rotateLeft(left);
        rotateRight(node);

        node->setBalance(middleBalance == -1 ? 1 : 0);
        left->setBalance(middleBalance == 1 ? -1 : 0);
        middle->setBalance(0);
        subtreeShrank = true;
        return middle;
    }

    // node's balance is +2: the mirror image of the cases above.
    AVLNode<Key, Value>* right = node->getRight();
    if(right->getBalance() >= 0) {
        int oldBalance = right->getBalance();
        rotateLeft(node);
        if(oldBalance == 0) {
            node->setBalance(1);
            right->setBalance(-1);
            subtreeShrank = false;
        }
        else {
            node->setBalance(0);
            right->setBalance(0);
            subtreeShrank = true;
        }
        return right;
    }

    AVLNode<Key, Value>* middle = right->getLeft();
    int middleBalance = middle->getBalance();
    rotateRight(right);
    rotateLeft(node);

    node->setBalance(middleBalance == 1 ? -1 : 0);
    right->setBalance(middleBalance == -1 ? 1 : 0);
    middle->setBalance(0);
    subtreeShrank = true;
    return middle;
}


template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* inserted)
{
    AVLNode<Key, Value>* child = inserted;
    AVLNode<Key, Value>* parent = child->getParent();

    while(parent != NULL) {
        parent->updateBalance(parent->getRight() == child ? 1 : -1);
        int balance = parent->getBalance();
        if(balance == 0) {
            return; // This subtree's height did not change.
        }
        if(balance == -2 || balance == 2) {
            bool unused = false;
            rebalance(parent, unused);
            return; // Insertion needs no more fixes after rotation.
        }
        child = parent;
        parent = parent->getParent();
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* parent, int change)
{
    while(parent != NULL) {
        parent->updateBalance(static_cast<int8_t>(change));
        int balance = parent->getBalance();
        if(balance == -1 || balance == 1) {
            return; // This subtree's overall height has not changed.
        }

        AVLNode<Key, Value>* top = parent;
        if(balance == -2 || balance == 2) {
            bool subtreeShrank = false;
            top = rebalance(parent, subtreeShrank);
            if(!subtreeShrank) {
                return;
            }
        }
        // At balance 0, or after a height-reducing rotation, move up.
        parent = top->getParent();
        if(parent != NULL) {
            change = parent->getLeft() == top ? 1 : -1;
        }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* parent = NULL;
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);

    while(current != NULL) {
        parent = current;
        if(new_item.first < current->getKey()) {
            current = current->getLeft();
        }
        else if(current->getKey() < new_item.first) {
            current = current->getRight();
        }
        else {
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* inserted =
        new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if(parent == NULL) {
        this->root_ = inserted;
        return;
    }
    if(new_item.first < parent->getKey()) {
        parent->setLeft(inserted);
    }
    else {
        parent->setRight(inserted);
    }
    insertFix(inserted);
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(node == NULL) {
        return;
    }

    if(node->getLeft() != NULL && node->getRight() != NULL) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, pred); // Swap actual nodes, not const key-value pairs.
    }

    // Node now has at most one child.
    AVLNode<Key, Value>* child =
        node->getLeft() != NULL ? node->getLeft() : node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();
    int change = 0;

    if(parent == NULL) {
        this->root_ = child;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(child);
        change = 1;
    }
    else {
        parent->setRight(child);
        change = -1;
    }
    if(child != NULL) {
        child->setParent(parent);
    }
    delete node;

    if(parent != NULL) {
        removeFix(parent, change);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
