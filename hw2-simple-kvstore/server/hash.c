#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define TABLE_SIZE (1024*1024)
struct ob
{
    struct ob* next;
    char* key;
    char* value;
    void(*free_value)(void*);
};
struct HashTable
{
    struct ob ** table;
};
static void init_ob(struct ob* ob)
{
    ob->next = NULL;
    ob->key = NULL;
    ob->value = NULL;
    ob->free_value = NULL;
}
static void free_ob(struct ob* ob)
{
    if (ob)
    {
        if (ob->free_value)
        {
            ob->free_value(ob->value);
        }
        free(ob->key);
        ob->key = NULL;
        free(ob);
    }
}
static unsigned int hash_33(char* key)
{
    unsigned int hash = 0;
    while (*key)
    {
        hash = (hash << 5) + hash + *key++;
    }
    return hash;
}
HashTable* hash_table_new()
{
    HashTable* ht = malloc(sizeof(HashTable));
    if (NULL == ht)
    {
        hash_table_delete(ht);
        return NULL;
    }
    ht->table = malloc(sizeof(struct ob*) * TABLE_SIZE);
    if (NULL == ht->table)
    {
        hash_table_delete(ht);
        return NULL;
    }
    memset(ht->table, 0, sizeof(struct ob*) * TABLE_SIZE);

    return ht;
}
void hash_table_delete(HashTable* ht)
{
    if (ht)
    {
        if (ht->table)
        {
            int i = 0;
            for (i = 0; i<TABLE_SIZE; i++)
            {
                struct ob* point = ht->table[i];
                struct ob* q = NULL;
                while (point)
                {
                    q = point ->next;
                    free_ob(point);
                    point  = q;
                }
            }
            free(ht->table);
            ht->table = NULL;
        }
        free(ht);
    }
}
int hash_table_put(HashTable* ht, char* key, char* value, void(*free_value)(void*))
{
    int i = hash_33(key) % TABLE_SIZE;
    struct ob* point = ht->table[i];
    struct ob* pre = point ;
    while (point)   /* if key is already stroed, update its value */
    {
        if (strcmp(point ->key, key) == 0)
        {
            return 1;
        }
    }
    if (point  == NULL)  /* if key has not been stored, then add it */
    {
        char* kstr = malloc(strlen(key) + 1);
        char*str_val = malloc(strlen(value) + 1);
        if (kstr == NULL)
        {
            free(str_val);
            str_val=NULL;
            return -1;
        }
        struct ob * ob = malloc(sizeof(struct ob));
        if (NULL == ob)
        {
            free(kstr);
            free(str_val);
            str_val=NULL;
            kstr = NULL;
            return -1;
        }
        init_ob(ob);
        ob->next = NULL;
        strcpy(kstr, key);
        ob->key = kstr;
        strcpy(str_val,value);
        ob->value = str_val;
        ob->free_value = free_value;
        if (pre == NULL)
        {
            ht->table[i] = ob;
        }
        else
        {
            pre->next = ob;
        }
    }
    return 0;
}
char* hash_table_get(HashTable* ht, char* key)
{
    int i = hash_33(key) % TABLE_SIZE;
    struct ob* point  = ht->table[i];
    while (point !=NULL)
    {
        if (strcmp(key, point ->key) == 0)
        {
            return point ->value;
        }
        point = point ->next;
    }
    return NULL;
}
int hash_table_remove(HashTable* ht, char* key,int find)
{
    int i = hash_33(key) % TABLE_SIZE;
    struct ob* p = ht->table[i];
    struct ob* pre = p;
    while (p)
    {
        if (strcmp(key, p->key) == 0)
        {
            find=1;
            p->value = NULL;
            free_ob(p);
            if (p == pre)
            {
                ht->table[i] = NULL;
            }
            else
            {
                pre->next = p->next;
            }
        }
        pre = p;
        p = p->next;
    }
    return find;
}

