/*
 * Copyright (C) 2016 The Android Open Source Project
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
#ifndef BUB_UTIL_H_
#define BUB_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Converts a UTF-8 string, |utf8_data|, to UCS-2 stored in |ucs2_data|. Caller
 * must specify |utf8_num_bytes| as the number of bytes of the null-terminated
 * |utf8_data|. Caller must have allocated enough bytes for the resulting UCS-2
 * specified by |ucs2_data_capacity_num_bytes|. 2 * len(|utf8_data|) + 1 would
 * be the upper bound of |ucs2_data_capacity_num_bytes|.
 *
 */
int utf8_to_ucs2(const uint8_t* utf8_data,
                 size_t utf8_num_bytes,
                 uint16_t* ucs2_data,
                 size_t ucs2_data_capacity_num_bytes);

/* Allocates |size| bytes set to all zeros. Returns NULL if no memory is
 * available, otherwise a pointer to the allocated memory.
 *
 * The memory is not initialized.
 *
 * The pointer returned is guaranteed to be word-aligned.
 *
 * The memory should be freed with bub_free() when you are done with it.
 */
void* bub_calloc(size_t size);

/* Converts a 32-bit unsigned integer from big-endian to host byte order. */
uint32_t bub_be32toh(uint32_t in);

/* Converts a 64-bit unsigned integer from big-endian to host byte order. */
uint64_t bub_be64toh(uint64_t in);

/* Calculates and returns crc32 value of |data| given byte size, |data_size|. */
uint32_t bub_crc32(uint32_t crc_in, const void *buf, int size);

#ifdef __cplusplus
}
#endif

#endif /* BUB_UTIL_H_ */