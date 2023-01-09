// please learn to reuse your code, especially some basic functions, that's to say if you have implemented == operator, 
// you can just use == to implement != and some other cases. Please be careful about memory allocation.
// you need only need to implement the functions labled with to do.
// copy this file directy to .hpp file(remove the line : #include "hashmap.hpp") if you are using ide like visual studio, otherwise your code will not compile.
// still you don't need to worry about concurrency.
// you can directly submit this file to oj and you can pass the compile phase.


// if you cannot decide the type of a variable, please use `auto` keyword.(especially when you override the == operator)

// you may copy a lot of code from copy construtor and know how to throw a std::out_of_range exception
// from function at

// please carefully read related descriptions from .hpp when you implement a function.

#include "hashmap.hpp"

// In fact these functions are for you, but a lot of people are stuck here, so I implement it for you.

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap() : HashMap(kDefaultBuckets) { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H& hash) :
    _size(0),
    _hash_function(hash),
    _buckets_array(bucket_count, nullptr) {}

template <typename K, typename M, typename H>
HashMap<K, M, H>::~HashMap() {
    clear();
}

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size() const noexcept {
    return _size;
}

template <typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty() const noexcept {
    return _size == 0;
}

template <typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor() const noexcept {
    return static_cast<float>(size())/bucket_count();
};

template <typename K, typename M, typename H>
size_t HashMap<K, M, H>::bucket_count() const noexcept {
    return _buckets_array.size();
};

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) const noexcept {
    return find_node(key).second != nullptr;
}

template <typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key)const {
    auto [prev, node_found] = find_node(key);
    if (node_found == nullptr) {
        throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    }
    return node_found->value.second;
}

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(HashMap const& other) {
    this->_hash_function = other._hash_function;
    this->_buckets_array = std::vector<node*>(other.bucket_count(), nullptr);
    this->_size = 0;
    for (size_t i = 0; i < other.bucket_count(); ++i) {
        auto curr = other._buckets_array[i];
        while (curr != nullptr) {
            auto value = curr->value;
            auto node = std::make_pair(value.first,value.second);
            insert(node);
            curr = curr->next;
        }
    }
}


template <typename K, typename M, typename H>
void HashMap<K, M, H>::debug() const {
    std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ')
        << "Printing debug information for your HashMap implementation\n"
        << "Size: " << size() << std::setw(15) << std::right
        << "Buckets: " << bucket_count() << std::setw(20) << std::right
        << "(load factor: " << std::setprecision(2) << load_factor() << ") \n\n";

    for (size_t i = 0; i < bucket_count(); ++i) {
        std::cout << "[" << std::setw(3) << i << "]:";
        auto curr = _buckets_array[i];
        while (curr != nullptr) {
            const auto& [key, mapped] = curr->value;
            // next line will not compile if << not supported for K or M
            std::cout << " -> " << key << ":" << mapped;
            curr = curr->next;
        }
        std::cout << " /" << '\n';
    }
    std::cout << std::setw(30) << std::setfill('-') << '\n';
}

template <typename K, typename M, typename H>
M& HashMap<K, M, H>::operator[](const K& key) {
    if(!contains(key)) {
        auto res = new value_type(key, M());
        insert(*res);
    }
    return at(key);
}

template <typename K, typename M, typename H>
std::ostream& operator<<(std::ostream& os, const HashMap<K, M, H>& map){
    os<<"{";
    std::string str = "";
    for (size_t i = 0; i < map.bucket_count(); ++i) {
        auto curr = map._buckets_array[i];
        while (curr != nullptr) {
            auto node = curr;
            auto value = node->value;
            os<<str<<value.first<<":"<<value.second;
            str = ", ";
            curr = curr->next;
        }
    }
    os<<"}";
    return os;
}

// you work starts from here

template <typename K, typename M, typename H>
void HashMap<K, M, H>::clear() noexcept {
    //to do
    /*for(auto i: _buckets_array){
        if(i){
            node *cur = i;
            while(cur != nullptr && cur->next != nullptr){
                node *tmp = cur->next;
                cur->next = cur->next->next;
                delete tmp;
            }
            delete i;
        }
    }*/
    size_t t = _buckets_array.size();
    _buckets_array.clear();
    _buckets_array.resize(t);
    _size = 0;
}

template <typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::value_type*, bool>
HashMap<K, M, H>::insert(const value_type& value) {
    if(_buckets_array.size() != 0){
        node_pair np = find_node(value.first);
        if(np.second == nullptr){
            size_t i = _hash_function(value.first) % _buckets_array.size();
            node *cur = _buckets_array[i];
            while(cur != nullptr && cur->next != nullptr){
                cur = cur->next;
            }
            node *n_node = new node(value, nullptr);
            if(cur == nullptr){
                _buckets_array[i] = n_node;
            }
            else{
                cur->next = n_node;
            }
            _size++;
            return {&(n_node->value), true};
        }
        else{
            return {&(np.second->value), false};
        }
    }
    return {nullptr, false};
    //return { new value_type{ },true };
}


template <typename K, typename M, typename H>
bool HashMap<K, M, H>::erase(const K& key) {
    //to do
    node_pair np = find_node(key);
    if(np.second != nullptr){
        size_t i = _hash_function(key) % _buckets_array.size();
        if(np.first == nullptr){
            if(np.second->next != nullptr){
                _buckets_array[i] = np.second->next;
            }
            else{
                _buckets_array[i] = nullptr;
            }
        }
        else{
            np.first->next = np.second->next;
        }
        delete np.second;
        _size--;
    }
    else{
        return false;
    }
    return true;
}


template <typename K, typename M, typename H>
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K& key) const {
    //to do
    node *first_i = nullptr;
    bool find = false;
    node *last_i = nullptr;
    size_t i = _hash_function(key) % _buckets_array.size();
    if(_buckets_array[i] != nullptr){
        node *cur = _buckets_array[i];
        while(cur != nullptr){
            if(cur->value.first == key){
                last_i = cur;
                find = true;
                break;
            }
            else{
                first_i = cur;
                cur = cur->next;
            }
        }
    }
    if(find){
        return {first_i, last_i};
    }
    else{
        return {nullptr, nullptr};
    }
}


// 
template <typename K, typename M, typename H>
void HashMap<K, M, H>::rehash(size_t new_bucket_count) {
    //to do
    if(new_bucket_count == 0){
        throw std::out_of_range("");
    }
    std::vector<node *> new_array(new_bucket_count, nullptr);
    for(auto i: _buckets_array){
        if(i){
            node *cur = i;
            while(cur != nullptr){
                size_t index = _hash_function(cur->value.first) % new_bucket_count;
                node *n_cur = new_array[index];
                while(n_cur != nullptr && n_cur->next != nullptr){
                    n_cur = n_cur->next;
                }
                node *n_node = new node(cur->value, nullptr);
                if(n_cur == nullptr){
                    new_array[index] = n_node;
                }
                else{
                    n_cur->next = n_node;
                }
                cur = cur->next;
            }
        }
    }
    _buckets_array = new_array;
}




template <typename K, typename M, typename H>
bool operator==(const HashMap<K, M, H>& lhs,
    const HashMap<K,M,H>& rhs){
    //to do
    if(lhs.empty() != rhs.empty() || lhs.size() != rhs.size()){
        return false;
    }
    for(auto i: rhs._buckets_array){
        auto *cur = i;
        while(cur != nullptr){
            auto &[key, mapped] = cur->value;
            if(lhs.contains(key) == false || lhs.at(key) != mapped){
                return false;
            }
            cur = cur->next;
        }
        
    }
    return true;
}

template <typename K, typename M, typename H>
bool operator!=(const HashMap<K, M, H>& lhs,
    const HashMap<K, M, H>& rhs) {
    //to do
    
    if (lhs.empty() != rhs.empty() || lhs.size() != rhs.size()){
        return true;
    }
    for(auto i: rhs._buckets_array){
        auto *cur = i;
        while(cur != nullptr){
            auto &[key, mapped] = cur->value;
            if(lhs.contains(key) == false || lhs.at(key) != mapped){
                return true;
            }
            cur = cur->next;
        }
    }
    return false;
}


// copy some codes above
template<typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator=(const HashMap& other) {
    if(this == &other){
        return *this;
    }
    this->_hash_function = other._hash_function;
    this->_buckets_array = std::vector<node*>(other.bucket_count(), nullptr);
    this->_size = 0;
    for (size_t i = 0; i < other.bucket_count(); ++i) {
        auto curr = other._buckets_array[i];
        while (curr != nullptr) {
            auto value = curr->value;
            auto node = std::make_pair(value.first,value.second);
            insert(node);
            curr = curr->next;
        }
    }
    return *this;
}


