/**
 *   BSD LICENSE
 *
 *   Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Samsung Electronics Co., Ltd. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef KVS_EMUL_HPP_
#define KVS_EMUL_HPP_

#include "private_types.h"
#include <sstream>
#include <list>
#include <map>
#include <mutex>
#include <algorithm>
#include <queue>
#include <atomic>
#include <kvs_adi.h>

class KvEmulator: public KvsDriver
{

  kv_device_handle    devH;
  kv_namespace_handle nsH ;
  kv_queue_handle     sqH ;
  kv_queue_handle     cqH ;
  //kv_interrupt_handler int_handler;
  int queuedepth;

public:

  typedef struct {
    kv_iocb iocb;
    _on_iocomplete on_complete;
    kv_key *key;
    kv_value *value;
    KvEmulator* owner;
    int free_ctx;
  } kv_emul_context;
  
  kv_interrupt_handler int_handler;
  std::condition_variable done_cond;
  std::atomic<int> done;
  std::mutex lock;
  std::queue<kv_key*> kv_key_pool;
  std::queue<kv_value*> kv_value_pool;
  std::queue<kv_emul_context*> kv_ctx_pool;
  
public:
  KvEmulator(kv_device_priv *dev,_on_iocomplete user_io_complete_);
  virtual ~KvEmulator();
  virtual int32_t init(const char*devpath, const char* configfile, int queuedepth, int is_polling) override;
  virtual int32_t process_completions(int max) override;
  virtual int32_t store_tuple(int contid, const kvs_key *key, const kvs_value *value, uint8_t option, void *private1=NULL, void *private2=NULL, bool sync = false) override;
  virtual int32_t retrieve_tuple(int contid, const kvs_key *key, kvs_value *value, uint8_t option, void *private1=NULL, void *private2=NULL, bool sync = false) override;
  virtual int32_t delete_tuple(int contid, const kvs_key *key, uint8_t option, void *private1=NULL, void *private2=NULL, bool sync = false) override;
  virtual int32_t open_iterator(int contid,  uint8_t option, uint32_t bitmask, uint32_t bit_pattern, kvs_iterator_handle *iter_hd) override;
  virtual int32_t iterator_next(kvs_iterator_handle hiter, kvs_iterator_list *iter_list, void *private1=NULL, void *private2=NULL, bool sync = false) override;
  virtual int32_t close_iterator(int contid, kvs_iterator_handle hiter) override;
  virtual float get_waf() override;
  virtual int32_t get_used_size()override;
  virtual int64_t get_total_size() override;
  
private:
    int create_queue(int qdepth, uint16_t qtype, kv_queue_handle *handle, int cqid, int is_polling);
    kv_emul_context* prep_io_context(int opcode, int contid, const kvs_key *key, const kvs_value *value, uint8_t option, void *private1, void *private2, bool syncio);
  //int add_ioevent(int opcode, int32_t cont_id, const kvs_key *key, const kvs_value *value, uint8_t option,void *private1, void *private2, bool sync, int result);
	bool ispersist;
  //std::map<kvemul_key, kvemul_value> main_index;
  //std::queue<kv_iocb*> iocb_pool;
  //	std::queue<kv_iocb*> event_pool;
	std::string datapath;
};


#endif /* KVDRAM_HPP_ */
