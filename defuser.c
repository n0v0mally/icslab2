#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t lo  = (uint32_t)challenge;
    uint32_t mid = (uint32_t)(challenge >> 16);
    uint32_t hi  = (uint32_t)(challenge >> 32);

    // p1_step1：第二个参数异或常数，再加上第一个参数的两倍
    uint32_t t1 = (mid ^ 0x1b02u) + (hi << 1);

    // p1_step0：第二个参数加上常数，再与第一个参数异或
    uint32_t t0 = (hi + 0xedfdu) ^ lo;

    // p1_step2：第一个参数加上第二个参数的四倍，再异或常数
    values[0] = (t0 + (t1 << 2)) ^ 0xd121u;

    return write_answer(out, cap, values, count);
}



int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;

    // 第一个数
    values[0] = (uint32_t)((challenge >> 34) & 0x3fu) + 4u;

    // 后面五个数：由前一个数递推得到
    for (uint32_t i = 1; i < 6; ++i) {
        uint32_t t =
            (uint32_t)((challenge >> (10u + 3u * i)) & 0x0fu);

        values[i] = 3u * (values[i - 1] + i) - t;
    }

    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    uint32_t index = (uint32_t)(challenge & 7u);
    uint32_t x = (uint32_t)(challenge >> 32)
               ^ (uint32_t)(challenge >> 3);

    values[0] = index;

    switch (index) {
        case 0:  // p3_case_2
            values[1] = (x ^ 0xc066u) + (x >> 1);
            break;

        case 1:  // p3_case_3
            values[1] = (2u * x) ^ 0xe5acu;
            break;

        case 2:  // p3_case_7
            values[1] = 3u * x + 0x953cu;
            break;

        case 3:  // p3_case_0
            values[1] = 5u * x + 0x3fb38u;
            break;

        case 4:  // p3_case_6
            values[1] = 5u * x + 0x3b2b3u;
            break;

        case 5:  // p3_case_1
            values[1] = 5u * x + 0xf7ebu;
            break;

        case 6:  // p3_case_4
            values[1] = (x ^ 0x98bdu) + (x >> 1);
            break;

        case 7:  // p3_case_5
            values[1] = (2u * x) ^ 0xf66cu;
            break;
    }

    return write_answer(out, cap, values, count);
}

static uint32_t phase4_walk(uint32_t target, uint32_t low,
                            uint32_t high, uint32_t key) {
    uint32_t mid = low + ((high - low) >> 1);

    // 找到目标：递归结束
    if (target == mid) {
        return (mid + 0x26u) ^ key;
    }

    uint32_t child;
    uint32_t branch;

    if (target < mid) {
        // 左半区间
        child = phase4_walk(target, low, mid - 1u, key);
        branch = 0x17u;
    } else {
        // 右半区间
        child = phase4_walk(target, mid + 1u, high, key);
        branch = 0x0du;
    }

    return (5u * child) ^ (mid + key) ^ branch;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t target = (uint32_t)(challenge & 0x7fu);
    uint32_t key = (uint32_t)(challenge >> 7)
                 ^ (uint32_t)(challenge >> 32);

    values[0] = phase4_walk(target, 0u, 127u, key);

    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    struct node {
        uint32_t next;   // 下一个节点的数组下标
        uint32_t id;     // 节点编号
        uint32_t value;  // 节点数值
    };

    // 按内存地址从低到高排列；12 表示空指针
    static const struct node nodes[12] = {
        { 9u, 0x095eu, 0x4e8du },
        { 7u, 0x0428u, 0x8673u },
        { 1u, 0x0a3eu, 0x3c9cu },
        { 4u, 0x0012u, 0x3acau },
        { 0u, 0x0166u, 0x5cb3u },
        { 8u, 0x054bu, 0xdcceu },
        { 3u, 0x0776u, 0xa824u },
        {11u, 0x0b34u, 0x6188u },
        { 2u, 0x0614u, 0x3db0u },
        {12u, 0x0376u, 0x2a92u },
        { 6u, 0x082eu, 0xe544u },
        {10u, 0x0281u, 0x821eu }
    };

    static const uint32_t heads[8] = {
        5u, 8u, 2u, 1u, 7u, 11u, 10u, 6u
    };

    uint32_t index = heads[(uint32_t)(challenge & 7u)];
    uint32_t moves = (uint32_t)((challenge >> 3) & 3u);

    uint32_t acc = (uint32_t)(challenge >> 6)
                 ^ (uint32_t)(challenge >> 32)
                 ^ 0xee17u;

    // 起点也参与计算，所以共访问 moves + 1 个节点
    for (uint32_t i = 0; i <= moves; ++i) {
        acc = (3u * acc + nodes[index].value) ^ nodes[index].id;
        values[1] = nodes[index].id;

        if (i < moves) {
            index = nodes[index].next;
        }
    }

    values[0] = acc;

    return write_answer(out, cap, values, count);
}

static uint32_t phase6_apply(uint32_t index, uint32_t x) {
    switch (index) {
        case 0:
        case 3:
            // p6_op_0：32 位循环右移 7 位
            return (x >> 7) | (x << 25);

        case 1:
            // p6_op_2
            return ((x << 5) + 0x4693u) ^ (3u * x);

        case 2:
        case 7:
            // p6_op_4
            return (x + 0xdec9u) ^ (x >> 1);

        case 4:
            // p6_op_5：32 位循环左移 3 位，再异或
            return ((x << 3) | (x >> 29)) ^ 0x0f0bu;

        case 5:
            // p6_op_3
            return (x ^ 0x6f4bu) + (x << 3);

        case 6:
            // p6_op_1
            return ((x >> 1) ^ x) + 0x22c9u;

        default:
            return 0;
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t first = (uint32_t)(challenge & 7u);
    uint32_t second = (uint32_t)((challenge >> 3) & 7u);

    uint32_t x = (uint32_t)(challenge >> 6)
               ^ (uint32_t)(challenge >> 32);

    x = phase6_apply(first, x);
    x ^= 0x822du;
    x = phase6_apply(second, x);

    values[0] = x + 0xcdacu;

    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context,
                 char *out, size_t cap) {
    uint32_t mode = (uint32_t)(context & 3u);

    uint32_t a = (uint32_t)challenge
               ^ (uint32_t)(challenge >> 32);

    uint32_t b = (uint32_t)context
               ^ (uint32_t)(context >> 32);

    // secret_mix：循环左移 1 位，加常数，再异或
    uint32_t x = a ^ b;
    uint32_t mixed = (x << 1) | (x >> 31);
    mixed = (mixed + 0x59f4u) ^ 0xc343u;

    uint32_t second = mixed
                    ^ (mode * 0x59f4u)
                    ^ (uint32_t)(context >> 16);

    uint32_t third = second
                   ^ (b + mode + 0xc343u + (a >> 1));

    // 相邻字符串会自动拼接：共 32 个 A
    static const char padding[] =
        "AAAAAAAA"
        "AAAAAAAA"
        "AAAAAAAA"
        "AAAAAAAA";

    int n = snprintf(out, cap,
                     "%sOPEN%" PRIu32 " %" PRIu32 " %" PRIu32,
                     padding, mode, second, third);

    return n < 0 || (size_t)n >= cap;
}
