/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LIBUNWINDSTACK_DEX_FILE_H
#define _LIBUNWINDSTACK_DEX_FILE_H

#include <stdint.h>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <unwindstack/SharedString.h>

#include <art_api/dex_file_support.h>

namespace unwindstack {

struct MapInfo;
class Memory;

class DexFile {
  struct Info {
    uint32_t offset;  // Symbol start offset (relative to start of dex file).
    SharedString name;
  };

 public:
  bool IsValidPc(uint64_t dex_pc) {
    return base_addr_ <= dex_pc && (dex_pc - base_addr_) < file_size_;
  }

  bool GetFunctionName(uint64_t dex_pc, SharedString* method_name, uint64_t* method_offset);

  static std::unique_ptr<DexFile> Create(uint64_t base_addr, uint64_t file_size, Memory* memory,
                                         MapInfo* info);

 private:
  DexFile(std::unique_ptr<Memory>&& memory, uint64_t base_addr, uint64_t file_size,
          std::unique_ptr<art_api::dex::DexFile>&& dex)
      : memory_(std::move(memory)),
        base_addr_(base_addr),
        file_size_(file_size),
        dex_(std::move(dex)) {}

  std::unique_ptr<Memory> memory_;  // Memory range containing the dex file.
  uint64_t base_addr_ = 0;          // Absolute address where this DEX file is in memory.
  uint64_t file_size_ = 0;          // Total number of bytes in the dex file.
  std::unique_ptr<art_api::dex::DexFile> dex_;  // Loaded underling dex object.

  std::map<uint32_t, Info> symbols_;  // Cache of read symbols (keyed by *end* offset).
};

}  // namespace unwindstack

#endif  // _LIBUNWINDSTACK_DEX_FILE_H
