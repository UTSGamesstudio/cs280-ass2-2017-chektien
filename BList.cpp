/******************************************************************************/
/*!
@file   BList.cpp
@author Chek Tien Tan
@date   22/08/2017
@par    CS280 Data Structures
@brief  BList implementation
@TODO   - all ptrs should start with p_
        - so much stats duplicity, e.g., Size/stats_.ArraySize
*/
/******************************************************************************/
//#define DEBUG
//#define DEBUG_SUBSCRIPT
#define SYNTACTIC

#include <iostream>
#include "ObjectAllocator.h"

#ifdef SYNTACTIC
#include "BList.h"
#endif

const auto DEFAULT_VAL = -1;

template <typename T, unsigned Size>
BList<T, Size>::BList(): head_{nullptr}, tail_{nullptr} {
    stats_.NodeSize = nodesize();
    stats_.ArraySize = Size;

#ifdef DEBUG_OA
    std::cout << "BList ctor: oa blocksize=" << oa_.GetBlockSize() << std::endl;
#endif
}

template <typename T, unsigned Size>
BList<T, Size>::BList(const BList &rhs): stats_{rhs.stats_} {
    const BNode* p_rnode = rhs.GetHead();
    BNode* p_node = nullptr;
    BNode* p_prev = nullptr;
    while (p_rnode) {
        // create new node 
        p_node = new (oa_.Allocate()) BNode();
        p_node->count = p_rnode->count;
        for (auto i=0; i<p_rnode->count; ++i)
            p_node->values[i] = p_rnode->values[i];

        // link up properly
        if (p_prev) {
            p_prev->next = p_node;
            p_node->prev = p_prev;
        }
        else {
            head_ = p_node;
        }

#ifdef DEBUG_CPYCTOR
        std::cout << "cpyctor: p_node<" << p_node << "> vals=";
        for (auto i=0u; i<Size; ++i)
            std::cout << " " << p_node->values[i];
        std::cout << " prev=" << p_node->prev << " next=" << p_node->next << std::endl; 

        std::cout << "cpyctor: p_rnode<" << p_rnode << "> vals=";
        for (auto i=0u; i<Size; ++i)
            std::cout << " " << p_rnode->values[i];
        std::cout << " prev=" << p_rnode->prev << " next=" << p_rnode->next << std::endl; 
#endif
        
        // next node
        p_prev = p_node;
        p_rnode = p_rnode->next;
    }

    // link up tail
    tail_ = p_prev;

#ifdef DEBUG_CPYCTOR
    std::cout << "cpyctor: tail_<" << tail_ << std::endl;
    std::cout << "cpyctor: head_<" << head_ << std::endl;
#endif
}

template <typename T, unsigned Size>
BList<T, Size>::~BList() {

}

template <typename T, unsigned Size>
BList<T, Size>& BList<T, Size>::operator=(const BList &rhs) throw(BListException) {
    if (this == &rhs)
        return *this;

    // delete everything this is holding
    clear();

    const BNode* p_rnode = rhs.GetHead();
    BNode* p_node = nullptr;
    BNode* p_prev = nullptr;
    while (p_rnode) {
        // create new node 
        p_node = new (oa_.Allocate()) BNode();
        p_node->count = p_rnode->count;
        for (auto i=0; i<p_rnode->count; ++i)
            p_node->values[i] = p_rnode->values[i];

        // link up properly
        if (p_prev) {
            p_prev->next = p_node;
            p_node->prev = p_prev;
        }
        else {
            head_ = p_node;
        }

#ifdef DEBUG_CPYCTOR
    std::cout << "cpyctor: p_node<" << p_node << "> vals=";
    for (auto i=0u; i<Size; ++i)
        std::cout << " " << p_node->values[i];
    std::cout << " prev=" << p_node->prev << " next=" << p_node->next << std::endl; 

    std::cout << "cpyctor: p_rnode<" << p_rnode << "> vals=";
    for (auto i=0u; i<Size; ++i)
        std::cout << " " << p_rnode->values[i];
    std::cout << " prev=" << p_rnode->prev << " next=" << p_rnode->next << std::endl; 
#endif
        
        // next node
        p_prev = p_node;
        p_rnode = p_rnode->next;
    }

    // link up tail
    tail_ = p_prev;
   
    // copy stats
    stats_ = rhs.GetStats();

    // copy the whole thing
    return *this;
}

template <typename T, unsigned Size>
void BList<T, Size>::push_back(const T& value) throw(BListException) {
    // add value to current node if still have space
    if (tail_ && tail_->count < Size) {
        tail_->values[tail_->count] = value;
        inc_count(tail_);

#ifdef DEBUG
        std::cout << "push_back: added to existing tail_=" << tail_ << " values[0]=" << tail_->values[0] << " count=" << tail_->count << " next=" << head_->next << std::endl;
#endif

    }
    // if no space in tail node OR will be first node, create new node with new value
    else {
        BNode* p_node = new (oa_.Allocate()) BNode();
        p_node->values[0] = value;
        inc_count(p_node); 

        // if first node, do some special linkages
        if (stats_.NodeCount == 0)
            tail_ = head_ = p_node;
        // else just link to tail
        else {
            p_node->prev = tail_;
            tail_->next = p_node;
            tail_ = p_node;
        }
        ++stats_.NodeCount;

#ifdef DEBUG
        std::cout << "push_back: created new p_node=" << p_node << " values[0]=" << p_node->values[0] << " count=" << p_node->count << " next=" << p_node->next << std::endl;
#endif

    }
    ++stats_.ItemCount;
}

template <typename T, unsigned Size>
void BList<T, Size>::push_front(const T& value) throw(BListException) {
    // add value to current node if still have space
    if (head_ && head_->count < stats_.ArraySize) {
        // shift all to the back
        for (auto i=head_->count; i>0u; --i)
           head_->values[i] = head_->values[i-1];

        // insert at front
        head_->values[0] = value;
        inc_count(head_);

#ifdef DEBUG
        std::cout << "push_front: added to existing head_=" << head_ << " values[0]=" << head_->values[0] << " count=" << head_->count << " next=" << head_->next << std::endl;
#endif

    }
    // if no space in head node OR will be first node, create new node with new value
    else {
        BNode* p_node = new (oa_.Allocate()) BNode();
        p_node->values[0] = value;
        inc_count(p_node);

        // if first node, do some special linkages
        if (stats_.NodeCount == 0)
            tail_ = head_ = p_node;
        // else link to head
        else {
            head_->prev = p_node;
            p_node->next = head_;
            head_ = p_node;
        }    
        ++stats_.NodeCount;

#ifdef DEBUG
        std::cout << "push_front: created new p_node=" << p_node << " values[0]=" << p_node->values[0] << " count=" << p_node->count << " next=" << p_node->next << std::endl;
#endif

    }
    ++stats_.ItemCount;
}

template <typename T, unsigned Size>
void BList<T, Size>::split_node(BNode* p_node, T value, unsigned insertion_index) {
    // create new node and link it up properly
    BNode* p_newnode = new (oa_.Allocate()) BNode();
    p_newnode->prev = p_node;
    if (p_node->next) {
        p_node->next->prev = p_newnode;
        p_newnode->next = p_node->next;
    }
    p_node->next = p_newnode;

    // single value node 
    if (Size == 1) {
        // if insertion index is < Size then insert left
        if (insertion_index == 0) {
            p_newnode->values[0] = p_node->values[0];
            p_node->values[0] = value;
        }
        else {
            p_newnode->values[0] = value;
        }
        inc_count(p_newnode);
    }

    // else split node
    else {
        unsigned mid = Size/2; 

        // move right half of current node into new node
        auto j = 0u;
        for (unsigned k=mid; k<Size; ++k) {
            p_newnode->values[j++] = p_node->values[k];
            inc_count(p_newnode);
        }

        // reduce count of left node
        p_node->count = mid;
    
        // if insertion index is before mid, insert into left 
        if (insertion_index <= mid) {
            // shift value into the right place
            for (auto k=p_node->count; k>insertion_index; --k)
                p_node->values[k] = p_node->values[k-1];
            p_node->values[insertion_index] = value;
            inc_count(p_node);
        }

        // else insert into right
        else {
            // special case whereby this split is called by the next node
            // - i.e., insertion index is the start of the next node
            // - simply append to back
            if (insertion_index == Size) {
                p_newnode->values[p_newnode->count] = value;
                inc_count(p_newnode);
            }

            // else find the right place to insert in the right node
            else {
                insertion_index -= mid;
                // shift value into the right place
                for (auto k=p_newnode->count; k>insertion_index; --k)
                    p_newnode->values[k] = p_newnode->values[k-1];
                p_newnode->values[insertion_index] = value;
                inc_count(p_newnode);
            }
        }
    }

    // shift tail pointer if needed
    if (p_node == tail_)
        tail_ = p_newnode;

    ++stats_.ItemCount;
    ++stats_.NodeCount;

#ifdef DEBUG
    std::cout << "insert " << value  << ": i=" << insertion_index <<  " no slots left, splitted left<" << p_node << "> vals=";
    for (auto i=0u; i<Size; ++i)
        std::cout << " " << p_node->values[i];
    std::cout << " prev=" << p_node->prev << " next=" << p_node->next << " count=" << p_node->count << std::endl; 

    std::cout << "insert " << value << ": i=" << insertion_index << " no slots left, splitted right<" << p_newnode << "> vals=";
    for (auto i=0u; i<Size; ++i)
        std::cout << " " << p_newnode->values[i];
    std::cout << " prev=" << p_newnode->prev << " next=" << p_newnode->next << " count=" << p_newnode->count << std::endl; 
#endif
}

template <typename T, unsigned Size>
void BList<T, Size>::insert(const T& value) throw(BListException) {
    // add value by iterating each array item of each node 
    if (head_) {
        BNode* p_node = head_;
        auto i=0u;
        while (p_node) {
            // keep going until hit a spot > value 
            // - use operator< as cannot assume that operator> is implemented
            while (i<Size && p_node->values[i] < value) {
                ++i;
            }

            // found insertion point when i stops before last element of array in curr node
            if (i < p_node->count)
                break;

            // next node
            p_node = p_node->next;
            i=0;
        }

        // if p_node is not null, means found insertion point
        if (p_node) {

#ifdef DEBUG
            std::cout << "insert " << value << ": found slot in p_node<" << p_node << "> vals=";
            for (auto i=0u; i<Size; ++i)
                std::cout << " " << p_node->values[i];
            std::cout << std::endl; 
#endif

            // if this the first elem of node
            if (i == 0)  {
                // if slots left in prev node, append to prev
                if (p_node->prev && p_node->prev->count < Size) {
                    p_node->prev->values[p_node->prev->count] = value;
                    inc_count(p_node->prev);
                    ++stats_.ItemCount;

#ifdef DEBUG
                    std::cout << "insert " << value << ": not head, i=0, slots avail in prev node, appended to prev node<" << p_node->prev << "> vals=";
                    for (auto i=0u; i<Size; ++i)
                        std::cout << " " << p_node->prev->values[i];
                    std::cout << " prev=" << p_node->prev->prev << " next=" << p_node->prev->next << std::endl; 
#endif
                }

                // if slots in curr node, pre-pend to current
                else if (p_node->count < Size) {
                    // shift all elems 1 slot to the right from i in curr node
                    auto j = p_node->count;
                    while (j>i) {
                        p_node->values[j] = p_node->values[j-1];
                        --j;
                    }

                    // insert value at i
                    p_node->values[i] = value;
                    inc_count(p_node);
                    ++stats_.ItemCount;

#ifdef DEBUG
                    std::cout << "insert " << value << ": i=0, slots avail in curr node, pre-pended in curr node p_node<" << p_node << "> vals=";
                    for (auto i=0u; i<Size; ++i)
                        std::cout << " " << p_node->values[i];
                    std::cout << " prev=" << p_node->prev << " next=" << p_node->next << std::endl; 
#endif
                }

                // no slots in prev node, split and add to prev node
                else if (p_node->prev) {
#ifdef DEBUG
                    std::cout << "insert " << value << ": not head, i pointing to 1st of curr node, no slots in prev node, splitting prev node<" << p_node->prev << "> vals=";
                    for (auto i=0u; i<Size; ++i)
                        std::cout << " " << p_node->prev->values[i];
                    std::cout << " prev=" << p_node->prev->prev << " next=" << p_node->prev->next << std::endl; 
#endif

                    split_node(p_node->prev, value, Size);
                } 

                // is head and no space, split head
                else {

#ifdef DEBUG
                    std::cout << "insert " << value << ": is head, i pointing to 1st of curr node, no slots in prev node, splitting curr node<" << p_node << "> vals=";
                    for (auto i=0u; i<Size; ++i)
                        std::cout << " " << p_node->values[i];
                    std::cout << " prev=" << p_node->prev << " next=" << p_node->next << std::endl; 
#endif

                    split_node(p_node, value, i);
                }
            }

            // i in middle of curr node
            else {
                // if there is space in curr node, shift and insert 
                if (p_node->count < Size) {
                    // shift all elems 1 slot to the right from i in curr node
                    auto j = p_node->count;
                    while (j>i) {
                        p_node->values[j] = p_node->values[j-1];
                        --j;
                    }

                    // insert value at i
                    p_node->values[i] = value;
                    inc_count(p_node);
                    ++stats_.ItemCount;

#ifdef DEBUG
                    std::cout << "insert " << value << ": i=" << i << ", slots avail in curr node, pre-pended in curr node p_node<" << p_node << "> vals=";
                    for (auto i=0u; i<Size; ++i)
                        std::cout << " " << p_node->values[i];
                    std::cout << " prev=" << p_node->prev << " next=" << p_node->next << std::endl; 
#endif

                }

                // no space in curr node, split node and add
                else {
#ifdef DEBUG
                    std::cout << "insert " << value << ": i in midst of curr node, no slots in curr node, splitting curr node<" << p_node << "> vals=";
                    for (auto i=0u; i<Size; ++i)
                        std::cout << " " << p_node->values[i];
                    std::cout << " prev=" << p_node->prev << " next=" << p_node->next << std::endl; 
#endif

                    split_node(p_node, value, i);
                }
            }
        }

        // else reached end of list and add new node at the end
        else {
            // if there are slots left in tail, append to last pos 
            if (tail_->count < Size) {
                tail_->values[tail_->count] = value;
                inc_count(tail_);
                ++stats_.ItemCount;

#ifdef DEBUG
                std::cout << "insert " << value << ": reached end of list, has slots in tail, appended to tail<" << tail_ << "> with vals=";
                for (auto i=0u; i<Size; ++i)
                    std::cout << " " << tail_->values[i];
                std::cout << std::endl; 
#endif
            }
            
            // no slots in tail, split tail and insert 
            else {
                split_node(tail_, value, tail_->count);
                //BNode* p_newnode = new (oa_.Allocate()) BNode();
                //p_newnode->prev = tail_;
                //p_newnode->next = nullptr;
                //tail_->next = p_newnode;

                //// insert value in new node
                //p_newnode->values[0] = value;
                //inc_count(p_newnode);

//#ifdef DEBUG
                //std::cout << "insert " << value << ": reached end of list, no slots in tail, added newnode<" << p_newnode << "> with vals=";
                //for (auto i=0u; i<Size; ++i)
                    //std::cout << " " << p_newnode->values[i];
                //std::cout << " prev=" << p_newnode->prev << " next=" << p_newnode->next << std::endl; 
//#endif

            }
        }
    }
    
    // create first node
    else {
        BNode* p_node = new (oa_.Allocate()) BNode();
        p_node->values[0] = value;
        inc_count(p_node);

        tail_ = head_ = p_node;
        ++stats_.ItemCount;
        ++stats_.NodeCount;

#ifdef DEBUG
        std::cout << "insert: created first p_node<" << p_node << "> vals=" << p_node->values[0] << " count=" << p_node->count << " next=" << p_node->next << std::endl;
#endif
        
    }

}

template <typename T, unsigned Size>
void BList<T, Size>::remove(int index) throw(BListException) {
    // get node from index
    auto p_node = get_node(index);

    // calc the local index in node
    auto i = index % Size;

#ifdef DEBUG_REMOVE
    std::cout << "remove: deleting i="<< i << " in node<" << p_node << "> count=" << p_node->count << "  vals=";
    for (auto i=0u; i<Size; ++i)
        std::cout << " " << p_node->values[i];
    std::cout << std::endl; 
#endif

    // delete element
    for (auto j=i; j<p_node->count; ++j)
       p_node->values[j] = p_node->values[j+1]; 
    --p_node->count;
    --stats_.ItemCount;

    // TODO if < half size, fill it up and if next node <half , shift and remove

    // delete node if no more elements
    if (p_node->count == 0) {

#ifdef DEBUG_REMOVE
        std::cout << "remove: deleting node<" << p_node << "> count=" << p_node->count << "  vals=";
        for (auto i=0u; i<Size; ++i)
            std::cout << " " << p_node->values[i];
        std::cout << std::endl; 
#endif

        delete_node(p_node);
        --stats_.NodeCount;
    }
}

template <typename T, unsigned Size>
void BList<T, Size>::remove_by_value(const T& value) {
    // find index
    auto deletion_index = 0; 
    auto p_node = head_;
    auto is_broken = false;
    while (p_node) {
        for (auto i=0; i<p_node->count; ++i)
            if (p_node->values[i] == value) {
                deletion_index = i;
                is_broken = true;
                break;
            }
        if (is_broken)
            break;

        p_node = p_node->next;
    }

    // if it exists
    if (p_node) {
        // delete element
        for (auto j=deletion_index; j<p_node->count; ++j)
            p_node->values[j] = p_node->values[j+1]; 
        --p_node->count;
        --stats_.ItemCount;
    
        // delete node if no more elements
        if (p_node->count == 0) {

#ifdef DEBUG_REMOVE
            std::cout << "remove: deleting node<" << p_node << "> count=" << p_node->count << " vals=";
            for (auto i=0u; i<Size; ++i)
                std::cout << " " << p_node->values[i];
            std::cout << std::endl; 
#endif

            delete_node(p_node);
            --stats_.NodeCount;
        }
    }
}

template< typename T, unsigned Size>
void BList<T, Size>::delete_node(BNode* p_node) {
    if (p_node->prev) {
        p_node->prev->next = p_node->next;
    }
    else { // is head
        head_ = p_node->next;
    }

    if (p_node->next) {
        p_node->next->prev = p_node->prev;
    }
    else { // is tail
        tail_ = p_node->prev;
    }

    p_node->~BNode();
    //::operator delete(p_node);
    oa_.Free(reinterpret_cast<void*>(p_node));
}

template <typename T, unsigned Size>
int BList<T, Size>::find(const T& value) const {
    auto p_node = head_;
    auto sum=0;
    while (p_node) {
        for (auto i=0; i<p_node->count; ++i)
            if (p_node->values[i] == value)
                return sum+i;
        sum += p_node->count;
        p_node = p_node->next;
    }
    return -1;

    //// find the node first, then get the specific index
    //auto p_node = head_;
    //auto sum = 0;
    //while (p_node->values[0] < value) {
        //sum += p_node->count;
        //p_node = p_node->next;
    //}

//#ifdef DEBUG_FIND
    //std::cout << "find: i=0 of node<" << p_node << "> >= value=" << value << " node arr=";
    //for (auto i=0u; i<Size; ++i)
        //std::cout << " " << p_node->values[i];
    //std::cout << std::endl; 
//#endif

    //// if the value is equals then return
    //if (p_node->values[0] == value)
        //return sum;

    //// iterate to correct index in prev node
    //// - it is less than curr nodes 1st val, so must be in prev
    //else {
        //// maybe looping reverse may be slightly better, but can't say for sure
        //for (auto i=0u; i<Size; ++i)  
           //if (p_node->values[i] == value)
               //return sum-Size+i;
    //}
    //return -1;
}

template <typename T, unsigned Size>
T& BList<T, Size>::operator[](int index) throw(BListException) {
#ifdef DEBUG_SUBSCRIPT
    std::cout << "\noperator[]: index=" << index << std::endl;
#endif
    return get_value(index);    
}

template <typename T, unsigned Size>
const T& BList<T, Size>::operator[](int index) const throw(BListException) {
#ifdef DEBUG_SUBSCRIPT
    std::cout << "\nconst operator[]: index=" << index << std::endl;
#endif
    return get_value(index);
}

template <typename T, unsigned Size>
T& BList<T, Size>::get_value(const int& index) const {
#ifdef DEBUG_SUBSCRIPT
    std::cout << "get_value: index=" << index << std::endl;
#endif
    
    // use the counts to jump
    auto sum = 0;
    auto p_node = head_;
    auto i = index;
    auto prev_i = i;
    while (sum <= index) {

#ifdef DEBUG_SUBSCRIPT
        std::cout << "get_value: looping1 sum=" << sum << " prev_i=" << prev_i << " i=" << i << "count=" << p_node->count << std::endl;
#endif

        sum += p_node->count;
        prev_i = i;
        i -= p_node->count;

#ifdef DEBUG_SUBSCRIPT
        std::cout << "get_value: looping2 sum=" << sum << "prev_i=" << prev_i << " i=" << i << " count=" << p_node->count << std::endl;
#endif

        p_node = p_node->next;
    }

    // handle tail node
    if (p_node == head_)
        return p_node->values[prev_i];
    else if (p_node)
        return p_node->prev->values[prev_i];
    else
        return tail_->values[prev_i];
}

template <typename T, unsigned Size>
typename BList<T, Size>::BNode* BList<T, Size>::get_node(const int& index) const {
#ifdef DEBUG_SUBSCRIPT
    std::cout << "get_node: index=" << index << std::endl;
#endif
    
    // use the counts to jump
    auto sum = 0;
    auto p_node = head_;
    while (sum <= index) {

#ifdef DEBUG_SUBSCRIPT
        std::cout << "get_node: looping1 sum=" << sum << " index=" << index << " count=" << p_node->count << std::endl;
#endif

        sum += p_node->count;

#ifdef DEBUG_SUBSCRIPT
        std::cout << "get_node: looping2 sum=" << sum << " index=" << index << " count=" << p_node->count << std::endl;
#endif

        p_node = p_node->next;
    }

    // handle tail node
    if (p_node == head_)
        return p_node;
    else if (p_node)
        return p_node->prev;
    else
        return tail_;
}

template <typename T, unsigned Size>
unsigned BList<T, Size>::size(void) const {
    return stats_.ItemCount;
}

template <typename T, unsigned Size>
void BList<T, Size>::clear() {
    while(stats_.ItemCount)
        remove(0);
}

template <typename T, unsigned Size>
unsigned BList<T, Size>::nodesize(void)
{
  return sizeof(BNode);
}

template <typename T, unsigned Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
  return head_; 
}

template <typename T, unsigned Size>
BListStats BList<T, Size>::GetStats() const {
    return stats_;
}

template <typename T, unsigned Size>
void BList<T, Size>::inc_count(BNode* p_node) {
    ++p_node->count;
    if (p_node->count > Size)
        p_node->count = Size;
}
