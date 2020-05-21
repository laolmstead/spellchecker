/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Lindsey A. Olmstead
 * Date: December 4, 2019
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);

}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    for (int i = 0; i < map->capacity; i++)
    {
        while(map->table[i] != NULL)
        {
            HashLink *temp = map->table[i];
            map->table[i] = map->table[i]->next;
            hashLinkDelete(temp);
            temp = NULL;
            map->size--;
        }
    }
    free(map->table);
    map->table = NULL;
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
    map = NULL;
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map != NULL);
    
    if (hashMapContainsKey(map, key))
    {
        int r = HASH_FUNCTION(key) % map->capacity;
        HashLink *temp = map->table[r];
        while(temp != NULL)
        {
            if (strcmp(temp->key, key) == 0)
            {
                int *ptr = &temp->value;
                return ptr;
            }
            temp = temp->next;
        }
    }
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: implement
    assert(map->size > 0);
    
    // Create a new map with table of size capacity.
    struct HashMap *newTable = hashMapNew(capacity);
        
    // Copy values of map to newMap
    for (int i = 0; i < map->capacity; i++)
    {
        struct HashLink *mapCurrent = map->table[i];
        while (mapCurrent != NULL)
        {
            int index = HASH_FUNCTION(mapCurrent->key) % capacity;
            if (index < 0)
            {
                index += map->capacity;
            }
            hashMapPut(newTable, mapCurrent->key, mapCurrent->value);
            mapCurrent = mapCurrent->next;
        }
    }
    
    // Delete original map and set it equal to new map.
    hashMapCleanUp(map);
    map->capacity = capacity;
    map->size = newTable->size;
    map->table = newTable->table;
    newTable->table = NULL;
    free(newTable);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement
    assert (map != NULL && key != NULL);
    
    // Expand hashtable if necessary.
    if (hashMapTableLoad(map) >= MAX_TABLE_LOAD)
    {
        resizeTable(map, (map->capacity)*2);
    }
    
    // Find bucket index for the Link's key.
    int index = HASH_FUNCTION(key) % map->capacity;
    if (index < 0)
    {
        index += map->capacity;
    }
    
    // If key is not already in list, create new link for it.
    if (!hashMapContainsKey(map, key))
    {
        struct HashLink *temp;
        // Add link to beginning of index if it's NULL.
        if (map->table[index] == NULL)
        {
            temp = hashLinkNew(key, value, NULL);
        }
        else
        {
            // Create new link at beginning of bucket and move other links down.
            temp = hashLinkNew(key, value, map->table[index]);
        }
        map->table[index] = temp;
        map->size++;
    }
    else
    {
        // If key already exists, update value to new value.
        struct HashLink *current = map->table[index];
        while (current != NULL)
        {
            if (strcmp(current->key, key) == 0)
            {
                current->value = value;
                return;
            }
            current = current->next;
        }
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement
    if (hashMapContainsKey(map, key))
    {
        int index = HASH_FUNCTION(key) % map->capacity;
        if (index < 0)
        {
            index += map->capacity;
        }
        HashLink *current = map->table[index];
        HashLink *prev = NULL;
        while (current != NULL)
        {
            if (strcmp(current->key, key) == 0)
            {
                // If link is at the beginning of the bucket, set 1st = to next.
                if (prev == NULL)
                {
                    map->table[index] = current->next;
                }
                // If link is in the middle or end of list, link prev and next.
                else
                {
                    prev->next = current->next;
                }
                // delete current link.
                hashLinkDelete(current);
                map->size--;
                return;
            }
            prev = current;
            current = current->next;
        }
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement
    int index = HASH_FUNCTION(key) % map->capacity;
    if (index < 0)
    {
        index += map->capacity;
    }
    HashLink *current = map->table[index];
    while(current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    int emptyBuckets = 0;
    for (int i = 0; i < map->capacity; i++)
    {
        if (map->table[i] == NULL)
        {
            emptyBuckets++;
        }
    }
    return emptyBuckets;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    float tableLoad = (float)(map->size) / (float)(map->capacity);
    
    return tableLoad;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
  // FIXME: implement
    assert(map != NULL);
    printf("\n");
    for (int index = 0; index < map->capacity; index++)
    {
        HashLink *current = map->table[index];
        printf("Bucket %d: ", index);
        while (current != NULL)
        {
            printf("Key: %s, Value: %d", current->key, current->value);
            current = current->next;
            if (current != NULL)
            {
                printf(" | ");
            }
        }
        printf("\n");
    }
}
