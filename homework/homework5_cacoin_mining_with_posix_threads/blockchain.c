/*********************************************************************
Homework 5
CS 110: Computer Architecture, Spring 2021
ShanghaiTech University

* Last Modified: 03/28/2021
*********************************************************************/
#define _GNU_SOURCE
#include "blockchain.h"
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define THREAD_NUM 20

typedef struct {
  blk_t *node;
  unsigned char *hash_buf;
  hash_func *func;
  unsigned char *one_diff;
  size_t diff_q;
  int *flag;
  int num;
} thread_args_t;

void blockchain_node_init(blk_t *node, uint32_t index, uint32_t timestamp,
                          unsigned char prev_hash[32], unsigned char *data,
                          size_t data_size) {
  if (!node || !data || !prev_hash)
    return;

  node->header.index = index;
  node->header.timestamp = timestamp;
  node->header.nonce = -1;

  memset(node->header.data, 0, sizeof(unsigned char) * 256);
  memcpy(node->header.prev_hash, prev_hash, HASH_BLOCK_SIZE);
  memcpy(node->header.data, data,
         sizeof(unsigned char) * ((data_size < 256) ? data_size : 256));
}

void blockchain_node_hash(blk_t *node, unsigned char hash_buf[HASH_BLOCK_SIZE],
                          hash_func func) {
  if (node)
    func((unsigned char *)node, sizeof(blkh_t), (unsigned char *)hash_buf);
}

BOOL blockchain_node_verify(blk_t *node, blk_t *prev_node, hash_func func) {
  unsigned char hash_buf[HASH_BLOCK_SIZE];

  if (!node || !prev_node)
    return False;

  blockchain_node_hash(node, hash_buf, func);
  if (memcmp(node->hash, hash_buf, sizeof(unsigned char) * HASH_BLOCK_SIZE))
    return False;

  blockchain_node_hash(prev_node, hash_buf, func);
  if (memcmp(node->header.prev_hash, hash_buf,
             sizeof(unsigned char) * HASH_BLOCK_SIZE))
    return False;

  return True;
}

void *try(void *thread_args) {
  thread_args_t *args = thread_args;
  blk_t *node = args->node;
  unsigned char *hash_buf = args->hash_buf;
  hash_func *func = args->func;
  unsigned char *one_diff = args->one_diff;
  int diff_q = args->diff_q;
  int *flag = args->flag;

  int num = args->num;
  int cmp1 = sizeof(unsigned char) * diff_q;
  int cmp2 = sizeof(unsigned char) * (HASH_BLOCK_SIZE - diff_q);
  node->header.nonce = num;

#pragma GCC ivdep
  while (*flag == -1) {
    blockchain_node_hash(node, hash_buf, func);
    if ((!memcmp(hash_buf, one_diff, cmp1)) &&
        memcmp(hash_buf + diff_q, one_diff + diff_q, cmp2) <= 0) {
      *flag = num;
      return 0;
    }

    node->header.nonce += THREAD_NUM;
  }
  return 0;
}

/* The sequiental implementation of mining implemented for you. */
void blockchain_node_mine(blk_t *node, unsigned char hash_buf[HASH_BLOCK_SIZE],
                          size_t diff, hash_func func) {
  unsigned char one_diff[HASH_BLOCK_SIZE];
  size_t diff_q, diff_m;
  diff_q = diff / 8;
  diff_m = diff % 8;
  memset(one_diff, 0xFF, sizeof(unsigned char) * HASH_BLOCK_SIZE);
  memset(one_diff, 0, sizeof(unsigned char) * diff_q);
  one_diff[diff_q] = ((uint8_t)0xFF) >> diff_m;

  int flag = -1;
  pthread_t tid[THREAD_NUM];
  thread_args_t args[THREAD_NUM];
  blk_t nodes[THREAD_NUM];
  unsigned char buf[THREAD_NUM][HASH_BLOCK_SIZE];

  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);

  for (int count = 0; count < THREAD_NUM;) {
    memcpy(nodes + count, node, sizeof(blk_t));
    args[count] = (thread_args_t){nodes + count, buf[count], func, one_diff,
                                  diff_q,        &flag,      count};

    if (!pthread_create(tid + count, NULL, try, args + count)) {
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      CPU_SET(count % num_cores, &cpuset);
      pthread_setaffinity_np(tid[count], sizeof(cpu_set_t), &cpuset);
      ++count;
    }
  }

  for (int count = 0; count < THREAD_NUM; ++count)
    pthread_join(tid[count], NULL);

  node->header.nonce = nodes[flag].header.nonce;
  memcpy(hash_buf, buf[flag], sizeof(unsigned char) * HASH_BLOCK_SIZE);
  memcpy(node->hash, hash_buf, sizeof(unsigned char) * HASH_BLOCK_SIZE);
}
