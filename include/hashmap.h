#include<stdlib.h>
#include<stdbool.h>

#include "common.h"


#define HM_INIT_CAPACITY 7


typedef struct hm_ele
{
    // key
    size_t key;
    
    // value
    size_t val;

    // is deleted
    bool deleted;
} hm_ele;

typedef struct hm_handle
{
    // pointer to hash function
    int (*hashcode) (size_t key);

    // pointer to equals fuction
    bool (*is_equals) (size_t key1, size_t key2);

    // array of hm_ele*
    hm_ele** arr;

    // current capacity a.k.a table size
    int capacity;

    // size
    int size;

} hm_h;

void hm_put(hm_h* hndl, size_t key, size_t val);

/**
 * @brief function to initialize hash map
 * 
 * @param hashcode_func_ function to compute hashcode of key
 * @param is_equals_ function to compare two keys
 * @return hm_h* 
 */
hm_h* hm_init(int (*hashcode_func_)(size_t), bool (*is_equals_)(size_t, size_t))
{
    CHECK_NOT_NULL(hashcode_func_);
    CHECK_NOT_NULL(is_equals_);

    hm_h* hndl = (hm_h*) calloc(1, sizeof(hm_h));

    hndl->hashcode = hashcode_func_;
    hndl->is_equals = is_equals_;
    hndl->arr = (hm_ele**) calloc(HM_INIT_CAPACITY, sizeof(hm_ele**));
    hndl->size = 0;
    hndl->capacity = HM_INIT_CAPACITY;

    return hndl;
}

/**
 * @brief private function to modify the capacity of hashmap
 * 
 * @param hndl 
 * @param new_capacity 
 * @return int 
 */
int hm_modify_capacity(hm_h* hndl, int new_capacity)
{
    hm_ele** old_table = hndl->arr;
    int old_capacity = hndl->capacity;

    hndl->arr = (hm_ele**) calloc(new_capacity, sizeof(hm_ele*));
    hndl->capacity = new_capacity;
    hndl->size = 0;

    if(!hndl->arr)
        return -1;

    for(int i=0; i<old_capacity; i++)
    {
        if(old_table[i] != NULL && old_table[i]->deleted == false)
            hm_put(hndl, old_table[i]->key, old_table[i]->val);
    }

    return 0;
}

/**
 * @brief private function
 * 
 * @param hndl 
 * @return int 
 */
int hm_increase_capacity(hm_h* hndl)
{
    return hm_modify_capacity(hndl, hndl->capacity*2);
}

/**
 * @brief private function
 * 
 * @param hndl 
 * @return int 
 */
int hm_decrease_capacity(hm_h* hndl)
{
    int new_capacity = (hndl->capacity)/2;
    if(new_capacity < HM_INIT_CAPACITY) new_capacity = HM_INIT_CAPACITY;
    return hm_modify_capacity(hndl, new_capacity);
}

/**
 * @brief private function
 * 
 * @param hndl_ 
 * @param hashcode 
 * @return int 
 */
int hm_hash_func1(hm_h* hndl_, int hashcode)
{
    hashcode = hashcode%(hndl_->capacity);
    return ((hashcode > 0) ? hashcode : (hashcode+hndl_->capacity-1));
}

/**
 * @brief private function
 * 
 * @param hndl_ 
 * @param hashcode 
 * @return int 
 */
int hm_hash_func2(hm_h* hndl_, int hashcode)
{
    hashcode = 7-(hashcode%7);
    hashcode = hashcode % hndl_->capacity;
    return ((hashcode > 0) ? hashcode : (hashcode+hndl_->capacity-1));
}

/**
 * @brief function to put a key and value pair
 * 
 * @param hndl handle to hashmap structure
 * @param key key
 * @param val value
 */
void hm_put(hm_h* hndl, size_t key, size_t val)
{
    CHECK_NOT_NULL(hndl);

    if(hndl->size == hndl->capacity/2)
        hm_increase_capacity(hndl);
    
    int hashcode = hndl->hashcode(key);
    int probe = hm_hash_func1(hndl, hashcode);
    int offset = hm_hash_func2(hndl, hashcode);

    assert(probe>=0 && probe<hndl->capacity);
    assert(offset>=0 && probe<hndl->capacity);

    int probe_cnt = 0;
    while ((probe_cnt < hndl->capacity) && (hndl->arr[probe] != NULL))
    {
        // if key matches, replace it
        if(hndl->is_equals(key, hndl->arr[probe]->key)) break;
        
        // if element is deleted, replace it
        if(hndl->arr[probe]->deleted) break;

        // else find next location
        probe = (probe + offset) % hndl->capacity;
        probe_cnt += 1;
    }

    /**
     * Since the table size is doubled upon reaching capacity/2, 
     * an empty slot is expected to be found in fewer iterations than the 
     * capacity. Otherwise, hash functions may be inefficient.
     */
    assert(probe_cnt < hndl->capacity);
    
    /**
     * If a non-empty slot is identified,
     * then either the element is deleted 
     * or the key at that slot should match the key passed in.
     */
    if(hndl->arr[probe] != NULL)
    {
        hm_ele* exist_ele = hndl->arr[probe];
        assert(exist_ele->deleted || hndl->is_equals(key, exist_ele->key));
        free(exist_ele);
        hndl->size -= 1;
    }
    hm_ele* ele = (hm_ele*) malloc(sizeof(hm_ele));
    ele->key = key;
    ele->val = val;
    ele->deleted = false;
    hndl->arr[probe] = ele;
    hndl->size += 1;
}

/**
 * @brief function to map a key to its value if key exists
 * 
 * @param hndl handle to hashmap structure
 * @param key key
 * @param val val
 * @return true if key exists
 * @return false if key does not exist
 */
bool hm_get(hm_h* hndl, size_t key, size_t* val)
{
    CHECK_NOT_NULL(hndl);

    bool ret_val = false;

    int hashcode = hndl->hashcode(key);
    int probe = hm_hash_func1(hndl, hashcode);
    int offset = hm_hash_func2(hndl, hashcode);

    assert(probe>=0 && probe<hndl->capacity);
    assert(offset>=0 && probe<hndl->capacity);

    int probe_cnt = 0;
    while((probe_cnt < hndl->capacity) && (hndl->arr[probe] != NULL))
    {
        hm_ele* exist_ele = hndl->arr[probe];

        /**
         * If the existing element is not deleted
         * and the key matches, then save the value
         */
        if((!exist_ele->deleted) && (hndl->is_equals(key, exist_ele->key)))
        {
            *val = exist_ele->val;
            ret_val = true;
            break;
        }
        probe = (probe + offset) % hndl->capacity;
        probe_cnt += 1;
    }

    /**
     * Since at least half the table is empty,
     * if the hash functions are good,
     * expect to hit an empty slot faster than iterating over capacity.
     */
    assert(probe_cnt < hndl->capacity);

    return ret_val;
}

/**
 * @brief Function to check if key exists in the hash map
 * 
 * @param hndl handle to hash map structure
 * @param key key
 * @return true if key is found in the hashmap
 * @return false if key is not found in the hashmap
 */
bool hm_contains(hm_h* hndl, size_t key)
{
    CHECK_NOT_NULL(hndl);

    size_t dummy_val;
    bool ret = hm_get(hndl, key, &dummy_val);

    return ret;
}


bool hm_remove(hm_h* hndl, size_t key)
{
    CHECK_NOT_NULL(hndl);

    bool ret_val = false;

    int hashcode = hndl->hashcode(key);
    int probe = hm_hash_func1(hndl, hashcode);
    int offset = hm_hash_func2(hndl, hashcode);

    assert(probe>=0 && probe<hndl->capacity);
    assert(offset>=0 && probe<hndl->capacity);

    int probe_cnt = 0;
    while((probe_cnt < hndl->capacity) && (hndl->arr[probe] != NULL))
    {
        hm_ele* exist_ele = hndl->arr[probe];

        /**
         * If the existing element is not deleted
         * and the key matches, then save the value
         */
        if((!exist_ele->deleted) && (hndl->is_equals(key, exist_ele->key)))
        {
            exist_ele->deleted = true;
            exist_ele->key = 0;
            exist_ele->val = 0;
            ret_val = true;
            hndl->size -= 1;
            break;
        }

        probe = (probe + offset) % hndl->capacity;
        probe_cnt += 1;
    }

    assert(probe_cnt < hndl->capacity);

    if(hndl->size <= (hndl->capacity/4))
    {
        hm_decrease_capacity(hndl);
    }

    return ret_val;

}

