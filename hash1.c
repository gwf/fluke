
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

typedef struct HashNode {
  unsigned hash;
  int key, val;
  struct HashNode *next;
} *HashNode;

typedef struct Hash {
  unsigned capacity, used;
  HashNode *nodes;
} *Hash;

double max_load_capacity = 0.8;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

Hash hash_new(unsigned cap) {
  Hash hash = malloc(sizeof(struct Hash));
  hash->capacity = cap;
  hash->used = 0;
  hash->nodes = malloc(sizeof(HashNode) * cap);
  bzero(hash->nodes, sizeof(HashNode) * cap);
  return hash;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void hash_destroy(Hash hash) {
  for (int i = 0; i < hash->capacity; i++) {
    HashNode node = hash->nodes[i];
    while (node) {
      HashNode next = node->next;
      free(node);
      node = next;
    }
  }
  free(hash->nodes);
  free(hash);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

unsigned int compute_hash(int x) {
  unsigned h = x;
  h = ((h >> 16) ^ h) * 0x45d9f3b;
  h = ((h >> 16) ^ h) * 0x45d9f3b;
  h = (h >> 16) ^ h;
  return h;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void hash_expand(Hash hash) {
  unsigned cap = hash->capacity * 2, mask = cap - 1;
  HashNode *nodes = malloc(sizeof(HashNode) * cap);
  bzero(nodes, sizeof(HashNode) * cap);
  for (int i = 0; i < hash->capacity; i++) {
    HashNode node = hash->nodes[i];
    while (node) {
      HashNode next = node->next;
      unsigned index = node->hash & mask;
      node->next = nodes[index];
      nodes[index] = node;
      node = next;
    }
  }
  free(hash->nodes);
  hash->capacity = cap;
  hash->nodes = nodes;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void hash_set(Hash hash, int key, int val) {
  if (hash->used > hash->capacity * max_load_capacity)
    hash_expand(hash);
  unsigned h = compute_hash(key);
  unsigned mask = hash->capacity - 1;
  unsigned index = h & mask;
  for (HashNode node = hash->nodes[index]; node; node = node->next) {
    // Note: we're testing the hash before the key because in the most
    // generic case (where values are something besides ints) testing the
    // hash is much quicker than test the key (e.g., if the key was a
    // string instead).
    if (node->hash == h && node->key == key) {
      // key was found so overwrite the value
      node->val = val;
      return;
    }
  }
  // key was not found, so insert it
  HashNode node = malloc(sizeof(struct HashNode));
  node->hash = h;
  node->key = key;
  node->val = val;
  node->next = hash->nodes[index];
  hash->nodes[index] = node;
  hash->used += 1;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int hash_get(Hash hash, int key) {
  unsigned h = compute_hash(key);
  unsigned mask = hash->capacity - 1;
  unsigned index = h & mask;
  for (HashNode node = hash->nodes[index]; node; node = node->next){
    if (node->hash == h && node->key == key){
      //if node is found return its value
      return node->val;
    }
  }
  //The node was not found
  return -1;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

void hash_del(Hash hash, int key) {
  unsigned h = compute_hash(key);
  unsigned mask = hash->capacity - 1;
  unsigned index = h & mask;

  HashNode *prev = &(hash->nodes[index]);
  HashNode node = *prev;
  while (node) {
    if (node->hash == h && node->key == key) {
      *prev = node->next;
      free(node);
      hash->used -= 1;
      return;
    }
    prev = &(node->next);
    node = *prev;
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int is_prime(int i) {
  if (i <= 1)
    return 0;
  if (i % 2 == 0 && i > 2)
    return 0;
  for (int n = i / 2, j = 3; j < n ; j++){
    if (i % j == 0)
      return 0;
  }
  return 1;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int largest_prime_factor(int num) {
  for (int i = num - 1; i > 1; i--) {
    if (is_prime(i) && num % i == 0)
      return i;
  }
  return 0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int main(void) {
  Hash hash = hash_new(4);
  for (int i = 0; i < 1000; i++) {
    int key = random() % 10000;
    int val = largest_prime_factor(key);
    printf("%d -> %d\n", key, val);
    hash_set(hash, key, val);
  }
  hash_destroy(hash);
  return 0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
