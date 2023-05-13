#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  int back = 0;
  int rank = make_rank(value);
  zering_decimal(result);
  result->bits[3] = 0;
  if (rank != 0) {
    s21_truncate(value, result);
    if (value.bits[3] >> 31) {
      int mas1[29], mas2[29];
      zeroing_char(mas2, 29);
      mas2[28] = 1;
      decimal_to_dec(*result, mas1);
      sum_mas(mas1, mas2, mas1, 28);
      dec_to_decimal(mas1, result);
    }
  } else {
    for (int i = 0; i < 4; ++i) result->bits[i] = value.bits[i];
  }
  return back;
}

int make_rank(s21_decimal value) { return (value.bits[3] & ~(1 << 31)) >> 16; }

int s21_negate(s21_decimal value, s21_decimal *result) {
  zering_decimal(result);
  result->bits[3] = 0;
  for (int i = 0; i < 4; ++i) {
    result->bits[i] = value.bits[i];
  }
  set_bit(result, 127, !(value.bits[3] >> 31));
  return 0;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  zering_decimal(result);
  result->bits[3] = 0;
  int back = 0;
  int value_int[29];
  int rank = ((value.bits[3] & ~(1 << 31)) >> 16);
  if (rank != 0) {
    decimal_to_dec(value, value_int);
    for (int i = 28; i > (28 - rank); --i) {
      value_int[i] = 0;
    }
    shift(_right, rank, value_int, 29);
    dec_to_decimal(value_int, result);
    result->bits[3] = ((value.bits[3] >> 31) << 31);
  } else {
    for (int i = 0; i < 4; ++i) result->bits[i] = value.bits[i];
  }
  return back;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int back = 0;
  int mas_int[29];
  int rank = make_rank(value);
  zering_decimal(result);
  result->bits[3] = 0;
  if (rank != 0) {
    decimal_to_dec(value, mas_int);
    if (mas_int[28 - rank + 1] >= 5) {
      int buff[29];
      zeroing_char(buff, 29);
      buff[28 - rank] = 1;
      sum_mas(mas_int, buff, mas_int, 28);
      shift(_right, rank, mas_int, 29);
      dec_to_decimal(mas_int, result);
      result->bits[3] = value.bits[3] & (1 << 31);
    } else {
      s21_truncate(value, result);
    }
  } else {
    for (int i = 0; i < 4; ++i) result->bits[i] = value.bits[i];
  }
  return back;
}

void zeroing_char(int *mas, int size) {
  for (int i = 0; i < size; ++i) mas[i] = 0;
}
