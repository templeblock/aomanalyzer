/*
 * Copyright (c) 2016, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 2 Clause License and
 * the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
 * was not distributed with this source code in the LICENSE file, you can
 * obtain it at www.aomedia.org/license/software. If the Alliance for Open
 * Media Patent License 1.0 was not distributed with this source code in the
 * PATENTS file, you can obtain it at www.aomedia.org/license/patent.
 */
#include "./vpx_config.h"
#include "./bitreader_buffer.h"

size_t vpx_rb_bytes_read(struct vpx_read_bit_buffer *rb) {
  return (rb->bit_offset + 7) >> 3;
}

int vpx_rb_read_bit(struct vpx_read_bit_buffer *rb) {
  const size_t off = rb->bit_offset;
  const size_t p = off >> 3;
  const int q = 7 - (int)(off & 0x7);
  if (rb->bit_buffer + p < rb->bit_buffer_end) {
    const int bit = (rb->bit_buffer[p] >> q) & 1;
    rb->bit_offset = off + 1;
    return bit;
  } else {
    rb->error_handler(rb->error_handler_data);
    return 0;
  }
}

int vpx_rb_read_literal(struct vpx_read_bit_buffer *rb, int bits) {
  int value = 0, bit;
  for (bit = bits - 1; bit >= 0; bit--) value |= vpx_rb_read_bit(rb) << bit;
  return value;
}

int vpx_rb_read_signed_literal(struct vpx_read_bit_buffer *rb, int bits) {
  const int value = vpx_rb_read_literal(rb, bits);
  return vpx_rb_read_bit(rb) ? -value : value;
}

int vpx_rb_read_inv_signed_literal(struct vpx_read_bit_buffer *rb, int bits) {
#if CONFIG_MISC_FIXES
  const int nbits = sizeof(unsigned) * 8 - bits - 1;
  const unsigned value = (unsigned)vpx_rb_read_literal(rb, bits + 1) << nbits;
  return ((int)value) >> nbits;
#else
  return vpx_rb_read_signed_literal(rb, bits);
#endif
}
