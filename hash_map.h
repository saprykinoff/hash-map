//#include "bits/stdc++.h"
//using namespace std;
#include <cassert>
#include <iostream>
#include <iterator>
#include <list>
#include <unordered_map>
#include <utility>
#include <vector>

const double max_load = 0.82;
const double min_load = 0.3;
//using KeyType = int;
//using ValueType = int;
//using Hash = std::hash<KeyType>;
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>

class HashMap {
    using Item = std::pair<KeyType, ValueType>;
    using Return = std::pair<const KeyType, ValueType>;
    using CostType = short;
    //Robin Hood работает (+~0.2 c)
    //size работает
    //Insert работает
    //Erase работает
    //FitSize(inc) работает
    //FitSize(dec) работает
    //keyExists работает
    //clear работает
    //operator[] работает
    //at работает
    //find работает
    //iterators работает

    std::vector<Item> table_;
    std::vector<CostType> cost_;

    std::vector<int> caps_ = {};
    int curr_cap_ = 0;

    int size_ = 0;
    Hash hasher_;


    void Rehash() {
        int cap = caps_[curr_cap_];
        std::vector<CostType> cost(cap, -1);
        std::vector<Item> table(cap);
        for (size_t i = 0; i < cost_.size(); ++i) {
            if (cost_[i] != -1) {
                pushItem(table_[i], table, cost, false);
            }
        }
        std::swap(table, table_);
        std::swap(cost, cost_);
    }
    void FitSpace() {
        if (size_ > caps_[curr_cap_] * max_load) {
            curr_cap_++;
            std::cerr << "Increased" << std::endl;
            Rehash();
        }
        if (curr_cap_ > 0 && size_ < caps_[curr_cap_ - 1] * min_load) {
            curr_cap_--;
            std::cerr << "Decrease" << std::endl;
            Rehash();
        }
    }

    bool keyExists(KeyType key) const {
        bool ans = find_key(key) != -1;
        return ans;
    }

    void insertItem(Item it) {
        if (keyExists(it.first)) return;
        ++size_;
        pushItem(it, table_, cost_);
        FitSpace();
    }

    void pushItem(Item it, std::vector<Item> &table, std::vector<CostType> &cost, bool may_dead = true) {
        int ind = hasher_(it.first) % cost.size();
        int steps = 0;
        CostType mycost = 0;
        while (true) {
            int i = (int) ((ind + steps) % cost.size());
            if (cost[i] == -1) {
                cost[i] = mycost;
                table[i] = it;
                break;
            } else {
                if (cost[i] < mycost) {
                    std::swap(mycost, cost[i]);
                    std::swap(it, table[i]);
                }
            }
            steps++;
            mycost++;
            assert(steps <= (int) table_.size() + 1);
        }
    }


    int find_key(KeyType key) const {
        int ind = hasher_(key) % cost_.size();
        int steps = 0;
        while (cost_[ind] != -1 ) {
            if (cost_[ind] != -1 && table_[ind].first == key) {
                return ind;
            }
            ind = (ind + 1) % cost_.size();
            steps++;
            assert(steps <= (int) table_.size() + 1);
        }
        return -1;
    }

    void eraseItem(KeyType key) {
        if (!keyExists(key)) return;
        --size_;
        int ind = find_key(key);
        cost_[ind] = -1;
        while (true) {
            int next =(ind  + 1)% cost_.size();
            if (cost_[next] < 1) {
                break;
            } else {
                cost_[next] --;
            }
            std::swap(table_[ind], table_[next]);
            std::swap(cost_[ind], cost_[next]);
            ind = next;
        }
    }

    void on_start() {
        curr_cap_ = 0;
        caps_ = {2, 5, 11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877, 205759, 411527, 823117, 1646237, 3292489, 6584983, 13169977, 26339969, 52679969, 105359939, 210719881};//простые модули дают - ~0.26с
        Rehash();
    }

public:
    //1.
    HashMap(Hash hasher = Hash()) : hasher_(hasher) {
        on_start();
    }

    //2.
    template<class Iterator>
    HashMap(Iterator begin, Iterator end, Hash hasher = Hash()) {
        hasher_ = hasher;
        on_start();
        while (begin != end) {
            insertItem(*begin);
            ++begin;
        }
    }

    //3.
    HashMap(std::initializer_list<Item> init, Hash hasher = Hash()) : hasher_(hasher) {
        on_start();
        for (auto i: init) {
            insertItem(i);
        }
    }
    //4

    //5
    int size() const {
        return size_;
    }
    bool empty() const {
        return size_ == 0;
    }

    //6.
    Hash hash_function() const {
        return hasher_;
    }

    //7.
    void insert(Item i) {
        insertItem(i);
    }

    //8.
    void erase(KeyType k) {
        eraseItem(k);
    }


    //9.
    class iterator {
        HashMap *h;
        int i;
        void validate() {
            if (h != nullptr) {
                while (i < (int) h->cost_.size() && h->cost_[i] == -1) {
                    i++;
                }
            }
        }

    public:
        iterator() = default;

        iterator(HashMap *map, int i) : h(map), i(i) {
            validate();
        }

        iterator &operator++() {
            i++;
            validate();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            i++;
            validate();
            return tmp;
        }

        Return &operator*() {
            return *reinterpret_cast<Return *>(&h->table_[i]);
        }

        Return *operator->() {
            return reinterpret_cast<Return *>(&h->table_[i]);
        }

        bool operator==(const iterator &other) const {
            return other.i == i;
        }
        bool operator!=(const iterator &other) const {
            return other.i != i;
        }
    };

    class const_iterator {
        const HashMap *h;
        int i;
        void validate() {
            if (h != nullptr) {
                while (i < (int) h->cost_.size() && h->cost_[i] == -1) {
                    i++;
                }
            }
        }

    public:
        const_iterator() = default;

        const_iterator(const HashMap *map, int i) : h(map), i(i) {
            validate();
        }

        const_iterator &operator++() {
            i++;
            validate();
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            i++;
            validate();
            return tmp;
        }

        const Return &operator*() {
            return *reinterpret_cast<const Return *>(&h->table_[i]);
        }

        const Return *operator->() {
            return reinterpret_cast<const Return *>(&h->table_[i]);
        }

        bool operator==(const const_iterator &other) const {
            return other.i == i;
        }
        bool operator!=(const const_iterator &other) const {
            return other.i != i;
        }
    };


    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, (int) cost_.size());
    }

    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    const_iterator end() const {
        return const_iterator(this, (int) cost_.size());
    }
    //10.

    const_iterator find(KeyType key) const {
        int ind = find_key(key);
        if (ind == -1) {
            return end();
        }
        return const_iterator(this, ind);
    }

    iterator find(KeyType key) {
        int ind = find_key(key);
        if (ind == -1) {
            return end();
        }
        return iterator(this, ind);
    }

    //11.
    ValueType &operator[](KeyType key) {
        if (!keyExists(key)) {
            insertItem({key, ValueType()});
        }
        int ind = find_key(key);
        return table_[ind].second;
    }

    //12.
    const ValueType &at(KeyType key) const {
        if (!keyExists(key)) {
            throw std::out_of_range("");
        }
        int ind = find_key(key);
        return table_[ind].second;
    }

    //13.
    void clear() {
        size_ = 0;
        curr_cap_ = 0;
        table_.resize(caps_[curr_cap_]);
        cost_.assign(caps_[curr_cap_], -1);
    }
};