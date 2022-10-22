#pragma once
typedef struct HashTable HashTable;
#ifdef __cplusplus
extern "C" {
#endif
HashTable* hash_table_new();
void hash_table_delete(HashTable* ht);
int hash_table_put(HashTable* ht, char* key, char* value, void(*free_value)(void*));
char* hash_table_get(HashTable* ht, char* key);
int hash_table_remove(HashTable* ht, char* key,int find);

#ifdef __cplusplus
}
#endif

