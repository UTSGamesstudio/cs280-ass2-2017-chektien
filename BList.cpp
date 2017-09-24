
template <typename T, unsigned Size>
BList<T, Size>::BList() {

}

template <typename T, unsigned Size>
BList<T, Size>::BList(const BList &rhs) {

}

template <typename T, unsigned Size>
BList<T, Size>::~BList() {
}

template <typename T, unsigned Size>
BList<T, Size>& BList<T, Size>::operator=(const BList &rhs) throw(BListException) {

}

template <typename T, unsigned Size>
void BList<T, Size>::push_back(const T& value) throw(BListException) {

}

template <typename T, unsigned Size>
void BList<T, Size>::push_front(const T& value) throw(BListException) {
 
}

template <typename T, unsigned Size>
void BList<T, Size>::insert(const T& value) throw(BListException) {

}

template <typename T, unsigned Size>
void BList<T, Size>::remove(int index) throw(BListException) {

}

template <typename T, unsigned Size>
void BList<T, Size>::remove_by_value(const T& value) {

}

template <typename T, unsigned Size>
int BList<T, Size>::find(const T& value) const {
    return 0;
}

template <typename T, unsigned Size>
T& BList<T, Size>::operator[](int index) throw(BListException) {

}

template <typename T, unsigned Size>
const T& BList<T, Size>::operator[](int index) const throw(BListException) {

}

template <typename T, unsigned Size>
unsigned BList<T, Size>::size(void) const {

}

template <typename T, unsigned Size>
void BList<T, Size>::clear() {

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

}
