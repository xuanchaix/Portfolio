#include "hashmap.hpp"
// command: g++ -std=c++17 test.cpp

using namespace std;

#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <sstream>
#include <set>
#include <iomanip>
#include <chrono>
using clock_type = std::chrono::high_resolution_clock;
using ns = std::chrono::nanoseconds;

const std::vector<std::pair<std::string, int> > vec {
    {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1}
};

const std::vector<std::string> keys {"A", "B", "C", "Not found"};
template <typename Map1, typename Map2> bool check_map_equal(Map1& map, Map2& answer) {
    if (answer.empty() != map.empty() || answer.size() != map.size()) return false;

    for (const auto& [key, mapped] : answer) {
        if(map.contains(key) == false || map.at(key) != mapped) return false;
    }
    return true;
}


void VERIFY_TRUE(bool condition, int line) {
    if (!condition) {
        std::cout<< "not pass the test in line " << line << std::endl;
    }
}


// ----------------------------------------------------------------------------------------------
/* Starter Code Test Cases (DO NOT EDIT) */

void B_insert() {
    /*
    * Verifies functionality of insert.
    */
    std::map<std::string, int> answer;
    HashMap<std::string, int> map;
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    VERIFY_TRUE(map.bucket_count() == 10, __LINE__);

    for (const auto& kv_pair : vec) {
        answer.insert(kv_pair);
        map.insert(kv_pair);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }

    VERIFY_TRUE(map.bucket_count() == 10, __LINE__);
}

void C_clear() {
    /*
    * Tests clear operations, and ensure map is in valid
    * state after a call to clear.
    */
    std::map<std::string, int> answer;
    HashMap<std::string, int> map;

    for (size_t j = 0; j < 3; ++j) {
       for (const auto& kv_pair : vec) {
           answer.insert(kv_pair);
           map.insert(kv_pair);
       }

       VERIFY_TRUE(map.empty() == false, __LINE__);
       answer.clear();
       map.clear();

       VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
}
void D_at() {
    /*
    * Tests whether at correctly returns a reference.
    */
    std::map<std::string, int> answer;
    HashMap<std::string, int> map;
    answer.insert({"A", 42});
    answer.insert({"B", 137});
    answer.insert({"C", -1});

    // at returns a reference, so assigning their return value on the left-hand value should
    // change the contents of the map.
    answer.at("A") = 42;
    answer.at("B") = 137;
    answer.at("A") = -42;
    answer.at("C") = answer.at("A");
    VERIFY_TRUE(answer.at("A") == -42, __LINE__);
    VERIFY_TRUE(answer.at("B") == 137, __LINE__);
    VERIFY_TRUE(answer.at("C") == -42, __LINE__);

    // verify that we can save the reference returned by at.
    auto copy = answer.at("A");
    auto& ref = answer.at("B");
    copy = 0;
    ref = 0;
    VERIFY_TRUE(answer.at("A") == -42, __LINE__);
    VERIFY_TRUE(answer.at("B") == 0, __LINE__);

    // verify that correct exceptions are thrown
    HashMap<std::string, int> map2;
    bool correct_exception = false;
    try {
        map2.at("Exists") = 5;
    } catch (const std::out_of_range& e) {
        correct_exception = true;
    }
    VERIFY_TRUE(correct_exception, __LINE__);
    map2.insert({"Exists", 4});
    VERIFY_TRUE(map2.contains("Exists"), __LINE__);
    VERIFY_TRUE(!map2.contains("Does not exist"), __LINE__);
    correct_exception = true;
    try {
        map2.at("Does not exist") = 5;
    } catch (const std::out_of_range& e) {
        correct_exception = true;
    }
    VERIFY_TRUE(correct_exception, __LINE__);
}

void E_custom_bucket_count() {
    /*
    * Tests ctor taking in num_buckets, while hash function
    * still uses the default. Also tests correct function of load_factor.
    */
    HashMap<int, int> many_buckets(10000);
    HashMap<int, int> one_bucket(1);
    std::map<int, int> answer;

    for (int i = 0; i < 100; ++i) {
       many_buckets.insert({i, -i});
       one_bucket.insert({i, -i});
       answer.insert({i, -i});
    }

    VERIFY_TRUE(check_map_equal(many_buckets, answer), __LINE__);
    VERIFY_TRUE(check_map_equal(one_bucket, answer), __LINE__);

    VERIFY_TRUE(many_buckets.bucket_count() == 10000, __LINE__);
    VERIFY_TRUE(one_bucket.bucket_count() == 1, __LINE__);

    float epsilon = 0.001;
    VERIFY_TRUE(many_buckets.load_factor() - 0.01 < epsilon, __LINE__);
    VERIFY_TRUE(one_bucket.load_factor() - 100 < epsilon, __LINE__);
}
void F_custom_hash_function() {
    /*
    * Tests ctor taking in a num_buckets and hash function.
    * We use a hash function that distributes similar inputs
    * more randomly across the buckets.
    */
    using K = int;
    using V = char;
    auto identity_shifted = [](const K& key) {
       return (key * 43037 + 52081) % 79229;
    };

    // hey, didn't you use this from assignment 1?
    // now you know what the priority queue decltype comes from!
    HashMap<K, V, decltype(identity_shifted)> map(75, identity_shifted);
    std::map<K, V> answer;

    for (int i = 0; i < 50; ++i) {
       map.insert({i, 'a' + i});
       answer.insert({i, 'a' + i});
       VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
}

void G_erase() {
    /*
    * Tests erase operation in combination with basic operations.
    */
    std::map<std::string, int> answer;
    HashMap<std::string, int> map;

    for (const auto& kv_pair : vec) {
        answer.insert(kv_pair);
        map.insert(kv_pair);
    }

    // remove one and insert two back at a time
    for (size_t i = 0; i < vec.size(); ++i) {
        auto& [key1, mapped1] = vec[i];
        auto& pair2 = vec[(i+3) % vec.size()];
        answer.erase(key1);
        map.erase(key1);
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

        answer.erase("Not a key");
        map.erase("Not a key");
        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

        answer.insert(pair2);
        map.insert(pair2);
    }

    // remove one at a time, until map is empty
    for (size_t i = 0; i < vec.size(); ++i) {
        auto& [key1, mapped1] = vec[(i+3) % vec.size()];
        answer.erase(key1);
        map.erase(key1);

        VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    }
}


void A_rehash_basic() {
    /*
    * Verifies external correctness after call to M1_Review.
    * Note that this does not actually verify if the linked lists are correct,
    * merely verifies that after call to M1_Review, all method calls are still valid,
    * and bucket_count is correct.
    */
    HashMap<int, int> map;
    std::map<int, int> answer;
    std::vector<int> vals;

    VERIFY_TRUE(map.bucket_count() == 10, 28);
    for (size_t M1_Review_count : {10, 20, 20, 15, 1000, 2, 1, 2, 1}) {
       map.rehash(M1_Review_count);
       VERIFY_TRUE(map.bucket_count() == M1_Review_count, __LINE__);

       for (size_t i = 0; i < 18; ++i) {
           VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
           map.erase(i);
           answer.erase(i);
       }

       map.clear();
       answer.clear();
       VERIFY_TRUE(check_map_equal(map, answer), __LINE__);

       for (size_t i = 1; i < 17; ++i) {
           map.insert({i, i});
           answer.insert({i, i});
       }
    }
    bool exception_correct = false;
    try {
        map.rehash(0);
    } catch (const std::out_of_range& e) {
        // Great! You threw the correct exception.
        exception_correct = true;
    } catch (const std::exception& e) {
        // you threw the wrong exception!
    }
    VERIFY_TRUE(exception_correct, __LINE__);
}

void B_rehash_correctness_by_time() {
    /*
    * This test tries to check if you hashed elements to the buckets correctly
    * by adding a specific number of elements, and measuring the time it takes
    * to call contains. The idea is that if bucket 0 is supposed to have 10 times
    * the number of elements as bucket 1, calling contains on an element
    * that should be hashed to bucket 0 but is not present should take 10 times
    * longer that calling contains on an element that should be hashed to bucket 1
    * but is not present.
    *
    * Obviously, this is less than perfect, since it's hard to predict how
    * fast your computer is running linked list operations. There are two parameters
    * below. One is called tolerance (let me know if I'm using the term incorrectly)
    * which determines the amount of leeway we give for any differences from the
    * expected time (0.5 means 50% leeway). The second is trials. You can try
    * increasing that to see if you get better results.
    *
    * Play around with this and let me know if you find something interesting,
    * or if you have suggestions to make this test better. There are also
    * cool number theory results, as hashing is inherently an idea from
    * cryptography. Ask Avery if you are curious!
    */
    using K = int;
    using V = int;
    float tolerance = 0.5;      // makes me feel like an engineer, probably using the term wrong
    int trials = 1000;           // increase if necessary, Central Limit Theorem!

    // in case std::hash<K> is different on your compiler.
    auto identity = [](const K& key) { return key; };

    HashMap<K, V, decltype(identity)> map(6, identity);

    // bucket(elements): 0(0), 1(100), 2(1500), 3(500), 4(1500), 5(6000)
    for (int i = 1; i <= 100; ++i) map.insert({6*i+1, i}); // bucket 1
    for (int i = 1; i <= 1500; ++i) map.insert({6*i+2, i}); // bucket 2
    for (int i = 1; i <= 500; ++i) map.insert({6*i+3, i}); // bucket 3
    for (int i = 1; i <= 1500; ++i) map.insert({6*i+4, i}); // bucket 4
    for (int i = 1; i <= 6000; ++i) map.insert({6*i+5, i}); // bucket 5
    std::map<int, float> bucket_times_6;

    for (int i = 0; i < 6; ++i) {
        auto start = clock_type::now();
        for (int j = 0; j < trials; ++j) map.contains(i);
        auto end = clock_type::now();
        auto elapsed = std::chrono::duration_cast<ns>(end - start);
        bucket_times_6.insert({i, elapsed.count()});
    }

    map.rehash(3);
    std::map<int, float> bucket_times_3;
    for (int i = 0; i < 3; ++i) {
        auto start = clock_type::now();
        for (int j = 0; j < trials; ++j) map.contains(i);
        auto end = clock_type::now();
        auto elapsed = std::chrono::duration_cast<ns>(end - start);
        bucket_times_3.insert({i, elapsed.count()});
    }

    map.rehash(2);
    std::map<int, float> bucket_times_2;
    for (int i = 0; i < 2; ++i) {
        auto start = clock_type::now();
        for (int j = 0; j < trials; ++j) map.contains(i);
        auto end = clock_type::now();
        auto elapsed = std::chrono::duration_cast<ns>(end - start);
        bucket_times_2.insert({i, elapsed.count()});
    }

    // Time to pull out the Chinese Remainder Theorem!
    // take the unique bijection Z/6 -> Z/3 x Z/2
    // bucket(elements) mod 6: 0(0), 1(100), 2(1500), 3(500), 4(1500), 5(6000)
    // bucket(elements) mod 3: 0+3(500), 1+4(1600), 2+5(7500)
    // bucket(elements) mod 2: 0+2+4(3000), 1+3+5(7500)

    float ratio6_10 = float(bucket_times_6[1])/(bucket_times_6[0]+1);
    float ratio6_23 = bucket_times_6[2]/bucket_times_6[3]; // expected: 1500/500
    float ratio6_54 = bucket_times_6[5]/bucket_times_6[4]; // expected: 6000/1500
    float ratio3_10 = bucket_times_3[1]/bucket_times_3[0]; // expected: 1600/500
    float ratio3_21 = bucket_times_3[2]/bucket_times_3[1]; // expected: 7500/1600
    float ratio2_10 = bucket_times_2[1]/bucket_times_2[0]; // expected: 7500/3000

    // experiments are noisy, so let's give you some acceptable tolerance
    VERIFY_TRUE(ratio6_10 > 10, __LINE__);
    VERIFY_TRUE(ratio6_23 < (1+tolerance)*1500/500 && ratio6_23 > 1/(1+tolerance)*1500/500, __LINE__);
    VERIFY_TRUE(ratio6_54 < (1+tolerance)*6000/1500 && ratio6_54 > 1/(1+tolerance)*6000/1500, __LINE__);
    VERIFY_TRUE(ratio3_10 < (1+tolerance)*1600/500 && ratio3_10 > 1/(1+tolerance)*1600/500, __LINE__);
    VERIFY_TRUE(ratio3_21 < (1+tolerance)*7500/1600 && ratio3_21 > 1/(1+tolerance)*7500/1600, __LINE__);
    VERIFY_TRUE(ratio2_10 < (1+tolerance)*7500/3000 && ratio2_10 > 1/(1+tolerance)*7500/3000, __LINE__);

    // fun fact: we had to add an -O0 flag because the compiler was optimizing our code
    // a little too well. Turns out that the runtime all of these is the same with optimization (!)
}



void A_index_operator() {
   
    /*
     * Tests the indexing operator to ensure it has the functionality of at(),
     * and also supports auto-insertion.
     */
    std::map<std::string, int> answer;
    HashMap<std::string, int> map;
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
    VERIFY_TRUE(answer["Not found"] == map["Not found"], __LINE__);
    for (const auto& [key, value] : vec) {
       answer[key] = value;
       map[key] = value;
       VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
       VERIFY_TRUE(answer["Not found"] == map["Not found"], __LINE__);
    }
    VERIFY_TRUE(map.size() == answer.size(), __LINE__);
    // check for parameter const correctness
    const string c_key = "Not found again";
    VERIFY_TRUE(answer[c_key] == map[c_key], __LINE__);
    VERIFY_TRUE(check_map_equal(map, answer), __LINE__);
}



void B_stream_insertion_operator() {
    /*
     * Tries to insert the map into various streams.
     * The format is described on the handout:
     * {Avery:2, Anna:3}
     * {}
     * {Avery:2}
     */
    HashMap<std::string, int> map;

    // Test 1: print empty map
    std::ostringstream oss1;
    oss1 << map;
    VERIFY_TRUE(oss1.str() == "{}", __LINE__);

    // Test 2: print map with a single element
    std::ostringstream oss2;
    map.insert({"Anna", 2});
    oss2 << map;
    VERIFY_TRUE(oss2.str() == "{Anna:2}", __LINE__);

    // Test 3: print map with two elements
    std::ostringstream oss3;
    map.insert({"Avery", 3});
    oss3 << map;
    VERIFY_TRUE(oss3.str() == "{Avery:3, Anna:2}" || oss3.str() == "{Anna:2, Avery:3}", __LINE__);
    auto s = oss3.str();

    // Test 4: print map after a call to erase
    std::ostringstream oss4;
    map.erase("Anna");
    map.at("Avery") = 4;
    oss4 << map;
    VERIFY_TRUE(oss4.str() == "{Avery:4}", __LINE__);

    // Test 5: print map that is key/value pairs reversed
    std::ostringstream oss5;
    HashMap<int, std::string> reverse;
    reverse.insert({3, "Avery"});
    reverse.insert({2, "Anna"});
    oss5 << reverse;
    VERIFY_TRUE(oss5.str() == "{3:Avery, 2:Anna}" || oss5.str() == "{2:Anna, 3:Avery}", __LINE__);

    // Test 6: check ability to chain printing HashMaps to streams.
    std::ostringstream oss;
    oss << "A" << map << 3 << reverse << '\n' << map << map << std::endl;
    VERIFY_TRUE(oss.str() == "A{Avery:4}3{3:Avery, 2:Anna}\n{Avery:4}{Avery:4}\n" ||
                oss.str() == "A{Avery:4}3{2:Anna, 3:Avery}\n{Avery:4}{Avery:4}\n", __LINE__);
}


void C_equality_operator() {
    /* Checks functionality of == and != */
    HashMap<int, int> map1(100);
    HashMap<int, int> map2(1);
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);

    // at this point we're assuming that all your operations
    // already work, so we're just testing whether == is correct.

    // Insert exact same elements
    for (int i = -1; i < 100; ++i) {
        map1.insert({i, i*i});
        map2.insert({i, i*i});
        VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
    }
    VERIFY_TRUE(map1.empty() == false , __LINE__);
    VERIFY_TRUE(map2.empty() == false , __LINE__);
    // Change the two maps' elements in a different order.
    // This means the the maps won't be equal until the very end
    for (int i = 0; i < 99; ++i) {
        map1.at(i) = -i*i;
        map2.at(99-i) = -(99-i)*(99-i);
        VERIFY_TRUE(map1 != map2 && map2 != map1 && map1 == map1 && map2 == map2, __LINE__);
    }
    map1.at(99) = -99*99;
    map2.at(0) = 0;
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);

    // Try that same thing again, but insert a ton of rehash calls between them.
    for (int i = 0; i < 99; ++i) {
        map1.erase(i);
        map2.erase(99-i);
        VERIFY_TRUE(map1 != map2 && map2 != map1 && map1 == map1 && map2 == map2, __LINE__);
    }
    map1.erase(99);
    map2.erase(0);
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);

    // consistency after a call to clear
    map1.clear();
    map2.clear();
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
}



void D_const_correctness() {
    /*
     * Checks that your HashMap class is const-correct.
     * The hard part about this test is not getting it to pass.
     * It is to get it to compile!
     */
    std::map<std::string, int> answer;
    HashMap<std::string, int> map1;
    HashMap<std::string, int> map2;
    VERIFY_TRUE(check_map_equal(map1, answer), __LINE__);

    for (const auto& [key, value] : vec) {
       answer[key] = value;
       map1[key] = value;
       map2[key] = value;
       VERIFY_TRUE(check_map_equal(map1, answer), __LINE__);
    }

    // create const references to the maps
    // to see if these const references work correctly
    const auto& c_ref_answer = answer;
    const auto& c_ref_map1 = map1;
    const auto& c_ref_map2 = map2;

    // check that size, empty, load_factor, and bucket_count are const functions
    // already handled in the starter code,
    VERIFY_TRUE(c_ref_map1.size() == c_ref_answer.size(), __LINE__);
    VERIFY_TRUE(c_ref_map1.empty() == c_ref_answer.empty(), __LINE__);
    VERIFY_TRUE(c_ref_map1.load_factor() != 0, __LINE__);
    VERIFY_TRUE(c_ref_map1.bucket_count() != 0, __LINE__);

    // check if we can still check contents of maps as equal
    // even when dealing with const references
    VERIFY_TRUE(check_map_equal(c_ref_map1, c_ref_answer), __LINE__);

    VERIFY_TRUE(answer.empty() == c_ref_map1.empty(), __LINE__);
    VERIFY_TRUE(answer.size() == c_ref_map1.size(), __LINE__);

    for (const auto& [key, mapped] : answer) {
        VERIFY_TRUE(map1.contains(key), __LINE__);
        VERIFY_TRUE(map1.at(key) == mapped, __LINE__);
    }

    // Check const correctness of == and != operator
    map1.erase("A");
    map2.erase("A");
    VERIFY_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2, __LINE__);
    VERIFY_TRUE(c_ref_map1 == map2 && map2 == c_ref_map1 && c_ref_map1 == c_ref_map1, __LINE__);
    VERIFY_TRUE(c_ref_map2 == map1 && map1 == c_ref_map2 && c_ref_map2 == c_ref_map2, __LINE__);
    VERIFY_TRUE(c_ref_map2 == c_ref_map1 && c_ref_map1 == c_ref_map2, __LINE__);

    std::ostringstream oss;
    oss << c_ref_map1;
    auto size = oss.str().size();
    oss << c_ref_map2 << c_ref_map1 << c_ref_map2;
    VERIFY_TRUE(oss.str().size() == 4*size, __LINE__);
}


void A_copy_ctor_assignment() {
    /* Checks correctness of copy ctor and copy assignment operator */
    HashMap<std::string, int> map;

    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
    }
    VERIFY_TRUE(map.empty() == false, __LINE__);

    HashMap<std::string, int> copy_constructed{map};
    HashMap<std::string, int> copy_assigned;
    copy_assigned = map;
    VERIFY_TRUE(map == copy_constructed, __LINE__);
    VERIFY_TRUE(map == copy_assigned, __LINE__);

    map.erase("A");
    // check that map and its copies are independent
    VERIFY_TRUE(map != copy_constructed, __LINE__);
    VERIFY_TRUE(map != copy_assigned, __LINE__);
    VERIFY_TRUE(copy_constructed == copy_assigned, __LINE__);

    map.clear();
    VERIFY_TRUE(map != copy_constructed, __LINE__);
    VERIFY_TRUE(map != copy_assigned, __LINE__);
    VERIFY_TRUE(copy_constructed == copy_assigned, __LINE__);

    // make more copies so we can test self-assignment
    auto original_copy_constructed(copy_constructed);
    HashMap<std::string, int> original_copy_assigned;
    original_copy_assigned = copy_assigned;

    // // suppress the really annoying warnings
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-assign-overloaded"

    // start doing weird things to the map
    copy_constructed = copy_constructed;
    VERIFY_TRUE(copy_constructed == original_copy_constructed, __LINE__);
    copy_assigned = (copy_assigned = copy_assigned) = copy_assigned;
    VERIFY_TRUE(copy_assigned == original_copy_assigned, __LINE__);
    map = map = map = map;
    copy_assigned = map = map = copy_assigned = map;

    // Both maps should be empty here.
    VERIFY_TRUE(map.empty(), __LINE__);
    VERIFY_TRUE(copy_assigned.empty(), __LINE__);
    #pragma GCC diagnostic pop
    // your code could crash at this line due to memory problems!
}





int main(){
         {  try{A_copy_ctor_assignment();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{A_index_operator();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{A_rehash_basic();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{B_insert();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{B_rehash_correctness_by_time();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{B_stream_insertion_operator();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{C_clear();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{C_equality_operator();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{D_at();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{D_const_correctness();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{E_custom_bucket_count();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{F_custom_hash_function();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
         {  try{G_erase();}catch(std::exception e){cout << "error in line " << __LINE__ << endl;}}
}

