#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "register_set.h"

struct reg_set_t {
    uint32_t count;
    uint32_t size;
    uint32_t M[1];
};

static const LOG2_BITS_PER_WORD = 6;

static const REGISTER_SIZE = 5;

static uint32_t reg_set_getbits(uint32_t count)
{
    return (uint32_t)floor(count / LOG2_BITS_PER_WORD);
}

reg_set_t *rs_init(uint32_t count, uint32_t *values, uint32_t len)
{
    uint32_t bits = reg_set_getbits(count);
    reg_set_t *rs;

    if (values) {
        rs = (reg_set_t *)malloc(sizeof(reg_set_t) + len - 1);
        memcpy(rs->M, values, len);
        rs->size = len;
    } else {
        if (bits == 0) {
            rs = (reg_set_t *)malloc(sizeof(reg_set_t));
            memset(rs->M, 0, 1);
            rs->size = 1;
        } else if (bits % 32 == 0) {
            rs = (reg_set_t *)malloc(sizeof(reg_set_t) + bits - 1);
            memset(rs->M, 0, bits);
            rs->size = bits;
        } else {
            rs = (reg_set_t *)malloc(sizeof(reg_set_t) + bits);
            memset(rs->M, 0, bits + 1);
            rs->size = bits + 1;
        }
    }

    rs->count = count;
}

int rs_set(reg_set_t *rs, uint32_t pos, uint32_t value)
{
    if (!rs) {
        return -1;
    }

    uint32_t bucket_pos = (uint32_t)floor(pos / LOG2_BITS_PER_WORD);
    uint32_t shift = REGISTER_SIZE * (pos - (bucket_pos * LOG2_BITS_PER_WORD));
    rs->M[bucket_pos] = (rs->M[bucket_pos] & ~(0x1f << shift)) | (value << shift);

    return 0;
}

int rs_get(reg_set_t *rs, uint32_t pos, uint32_t *value)
{
    if (!rs) {
        return -1;
    }

    uint32_t bucket_pos = (uint32_t)floor(pos / LOG2_BITS_PER_WORD);
    uint32_t shift = REGISTER_SIZE * (pos - (bucket_pos * LOG2_BITS_PER_WORD));
    *value = (ctx->M[bucket_pos] & (0x1f << shift)) >> shift;

    return 0;
}

int rs_bits(reg_set_t *rs, uint32_t *bits, uint32_t *len)
{
    if (!rs || !bits || (*len < rs->size)) {
        return -1;
    }

    memcpy(bits, rs->M, rs->size);
    *len = rs->size;

    return 0;
}

int rs_fini(reg_set_t *rs)
{
    if (rs) {
        free(rs);
        return 0;
    }

    return -1;
}
