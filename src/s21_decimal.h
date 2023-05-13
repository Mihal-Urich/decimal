#ifndef S21_DECIMAL_H_
#define S21_DECIMAL_H_

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _left 1
#define _right 2

#define SIZE_STR 32
#define BIT 8
#define SIZE_DEC 4
#define SIZE_LONG 7

#define S21_INF __builtin_inf()
#define S21_NAN __builtin_nan("")
#define S21_NINF __builtin_isinf
#define S21_NNAN __builtin_isnan

typedef struct {
  unsigned int bits[4];
} s21_decimal;
enum bits { LOW, MIDL, HIGH, SCALE };

typedef struct long_decimal {
  unsigned int bits[SIZE_LONG];
} s21_decimal_long;

typedef union special {
  float a;
  unsigned int b;
} s21_special;

//
int make_rank(s21_decimal value);
void zeroing_char(int *mas, int size);
void decimal_to_dec(s21_decimal value, int *result);
void decimal_to_bin(s21_decimal value, int *result);
void zering_decimal(s21_decimal *dst);
void set_bit(s21_decimal *value, int place, int bit);
void dec_to_decimal(int *number, s21_decimal *result);
void mas_int_sub(int *sub1, int *sub2, int *result, int size);
void duplication(int *mas_main, int *mas_duplicate, int size);
int less_or_equal_mas(int *mas1, int *mas2, int size);
void shift(int direction, int shift, int *mas_int, int size);
int under_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
void correctoin_rank(int *mas_result, int *new_rank);
int under_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int correction_max(int *mas);
void copy_int_bigger_mas(int *small_mas, int *big_mas, int small_size);
int counter_zero_in_mas(int *mas, int size, int direction);
int sum_mas(int *mas1, int *mas2, int *result, int size);
void s21_equality_degree(s21_decimal value_1, s21_decimal value_2,
                         s21_decimal_long *value_long_1,
                         s21_decimal_long *value_long_2);
void s21_sub_without_equal_degree(unsigned int *value_1, unsigned int *value_2,
                                  unsigned int *result, int size);
void s21_invert_long_decimal(s21_decimal_long *value);
void s21_sum_without_equal_degree(unsigned int *value_1, unsigned int *value_2,
                                  unsigned int *result, int size);
unsigned int s21_get_sign_(unsigned int *value_byte, int size);
unsigned int s21_get_bit_(unsigned int *value_byte, int num_byte, int i);
int s21_long_to_decimal(s21_decimal_long value_long, s21_decimal *value);
void s21_decimal_to_long(s21_decimal value, s21_decimal_long *value_long);
int s21_zero_decimal(unsigned int *decimal_byte, int size, int shift);
int s21_extract_num_grade(int degree, unsigned int *byte, int size,
                          int *sum_num_mod);
int s21_digit_extracting(unsigned int *decimal_byte, int size);
void s21_change_degree(s21_decimal_long *value, int difference_degree);
unsigned int s21_get_degree(unsigned int *value_byte, int size);

// сравнение
int s21_is_less(s21_decimal, s21_decimal);                           // 1 < 2
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);  // 1 <= 2
int s21_is_greater(s21_decimal, s21_decimal);                        //  1 > 2
int s21_is_greater_or_equal(s21_decimal, s21_decimal);               // 1 >= 2
int s21_is_equal(s21_decimal, s21_decimal);                          // 1 == 2
int s21_is_not_equal(s21_decimal, s21_decimal);                      // 1 != 2

//  Возвращаемое значение - код ошибки:
//  - 0 - OK
//  - 1 - ошибка вычисления
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);  //

// конвертации
int s21_from_int_to_decimal(int src, s21_decimal *dst);

int s21_from_float_to_decimal(float src, s21_decimal *dst);

int s21_from_decimal_to_int(s21_decimal src, int *dst);

int s21_from_decimal_to_float(s21_decimal src, float *dst);

// арифметика
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// Вспомогательные функции
// установить знак  1 "-" 0 "+"
void s21_set_sign(s21_decimal *dst, int sign);

// получить знак 1 "-" 0 "+"
int s21_get_sign(s21_decimal dst);

// получить степень
void s21_set_scale(s21_decimal *dst, int scale);

// установить степень
int s21_get_scale(s21_decimal dst);

// получить значение бита
int s21_get_bit(s21_decimal src, int pos);

// установить значение бита
void s21_set_bit(s21_decimal *number, int bit, int value);

// двинуть влево
void s21_shift_left(s21_decimal *value);

// получить макс позицию
int s21_get_high_bit_pos(s21_decimal value);

void arifmetik(s21_decimal value_1, s21_decimal value_2, s21_decimal *container,
               int *error);

void multiplakete(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *container, int *error);

int proverka_mul(s21_decimal value_1, s21_decimal value_2);

void arifmetik_minus(s21_decimal value_1, s21_decimal value_2,
                     s21_decimal *result);

#endif  // S21_DECIMAL_H_