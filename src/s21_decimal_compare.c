#include "s21_decimal.h"

void decimal_to_dec(s21_decimal value, int *result) {
  int value_bin[96], one[29] = {0};
  one[28] = 1;
  decimal_to_bin(value, value_bin);
  zeroing_char(result, 29);
  for (int i = 0; i < 96; ++i) {
    sum_mas(result, result, result, 28);
    if (value_bin[i] == 1) {
      sum_mas(result, one, result, 28);
    }
  }
}

void decimal_to_bin(s21_decimal value, int *result) {
  unsigned int buf;
  zeroing_char(result, 96);
  for (int i = 0; i < 3; ++i) {
    buf = value.bits[i];
    int k = 0;
    while (k != 32) {
      if ((buf & (1 << k)) > 0) {
        result[(31 - k) + (2 - i) * 32] = 1;
      }
      k++;
    }
  }
}

void zering_decimal(s21_decimal *dst) {
  for (int i = 0; i < 3; ++i) dst->bits[i] = 0;
}

void set_bit(s21_decimal *value, int place, int bit) {
  if (bit == 1)
    value->bits[place / 32] |= (1 << (place % 32));
  else if (bit == 0)
    value->bits[place / 32] &= ~(1 << (place % 32));
  else
    printf("error");
}

void dec_to_decimal(int *number, s21_decimal *result) {
  zering_decimal(result);
  s21_decimal buff = {{0}};
  int mas_duplicate[29];
  duplication(number, mas_duplicate, 29);
  for (int i = 95; i >= 0; --i) {
    int buf_int[29];
    set_bit(&buff, i, 1);
    decimal_to_dec(buff, buf_int);
    if (less_or_equal_mas(buf_int, mas_duplicate, 29) == 1) {
      set_bit(result, i, 1);
      mas_int_sub(mas_duplicate, buf_int, mas_duplicate, 29);
    }
    set_bit(&buff, i, 0);
  }
}

void mas_int_sub(int *sub1, int *sub2, int *result, int size) {
  int rank = 0;
  for (int i = size - 1; i >= 0; --i) {
    if (sub1[i] - sub2[i] - rank < 0) {
      result[i] = (10 + sub1[i]) - sub2[i] - rank;
      rank = 1;
    } else {
      result[i] = sub1[i] - sub2[i] - rank;
      rank = 0;
    }
  }
}

void duplication(int *mas_main, int *mas_duplicate, int size) {
  for (int i = 0; i < size; ++i) {
    mas_duplicate[i] = mas_main[i];
  }
}

int less_or_equal_mas(int *mas1, int *mas2, int size) {
  int back = 0;
  for (int i = 0; i < size; ++i) {
    if (mas1[i] > mas2[i]) break;
    if ((i == size - 1) || (mas1[i] < mas2[i])) back = 1;
  }
  return back;
}

void shift(int direction, int shift, int *mas_int, int size) {
  if (shift > 0) {
    if (direction == 1) {
      for (int i = 0; i < size - shift - 1; ++i) {
        mas_int[i] = mas_int[i + shift];
        mas_int[i + shift] = 0;
      }
    } else if (direction == 2) {
      for (int i = size - 1; i >= shift; --i) {
        mas_int[i] = mas_int[i - shift];
        mas_int[i - shift] = 0;
      }
    }
  }
}

int under_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int back = 0;
  int mas1[29], mas2[29];
  decimal_to_dec(value_1, mas1);
  decimal_to_dec(value_2, mas2);
  int long_mas1[58], long_mas2[58];
  int rank1 = make_rank(value_1), rank2 = make_rank(value_2);
  zeroing_char(long_mas1, 58);
  zeroing_char(long_mas2, 58);
  copy_int_bigger_mas(mas1, long_mas1, 29);
  copy_int_bigger_mas(mas2, long_mas2, 29);
  shift(_right, rank1, long_mas1, 58);
  shift(_right, rank2, long_mas2, 58);
  int long_mas_result[58];
  if (s21_is_greater(value_1, value_2) == 1)
    mas_int_sub(long_mas1, long_mas2, long_mas_result, 58);
  else
    mas_int_sub(long_mas2, long_mas1, long_mas_result, 58);
  int new_rank = counter_zero_in_mas(long_mas_result, 58, _left);
  int start = counter_zero_in_mas(long_mas_result, 58, _left);
  int mas_result[29];
  zeroing_char(mas_result, 29);
  for (int i = start; i <= start + 28; ++i) {
    if (start == 58) break;
    mas_result[i - start] = long_mas_result[i];
  }
  int end = counter_zero_in_mas(long_mas_result, 58, _right);
  if ((start + 29 != 58 - end) && (start != 58)) {
    for (int i = 57; i > 28; --i) {
      if ((long_mas_result[i] >= 5) && (long_mas_result[i - 1] % 2 == 1))
        long_mas_result[i - 1] += 1;
    }
    mas_result[28] = long_mas_result[start + 29 - 1];
    correctoin_rank(mas_result, &new_rank);
    dec_to_decimal(mas_result, result);
    if (new_rank < 29) result->bits[3] |= (new_rank << 16);
  } else {
    correctoin_rank(mas_result, &new_rank);
    dec_to_decimal(mas_result, result);
    if (new_rank < 29) result->bits[3] |= (new_rank << 16);
  }
  return back;
}

void correctoin_rank(int *mas_result, int *new_rank) {
  int result_end = counter_zero_in_mas(mas_result, 29, _right);
  if (result_end > *new_rank) result_end = *new_rank;
  if (result_end > 0) {
    *new_rank -= result_end;
    shift(_right, result_end, mas_result, 29);
  }
}

int under_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int back = 0;
  int mas1[29], mas2[29];
  decimal_to_dec(value_1, mas1);
  decimal_to_dec(value_2, mas2);
  int long_mas1[58], long_mas2[58];
  int rank1 = make_rank(value_1), rank2 = make_rank(value_2);
  zeroing_char(long_mas1, 58);
  zeroing_char(long_mas2, 58);
  copy_int_bigger_mas(mas1, long_mas1, 29);
  copy_int_bigger_mas(mas2, long_mas2, 29);
  shift(_right, rank1, long_mas1, 58);
  shift(_right, rank2, long_mas2, 58);
  int long_mas_result[58];
  if ((sum_mas(long_mas1, long_mas2, long_mas_result, 57)) ||
      (correction_max(long_mas_result))) {
    back = 1;
    if (correction_max(long_mas_result) && (value_1.bits[3] >> 31)) back = 2;
  } else {
    int new_rank = counter_zero_in_mas(long_mas_result, 58, _left);
    int start = counter_zero_in_mas(long_mas_result, 58, _left);
    int mas_result[29];
    zeroing_char(mas_result, 29);
    if (start != 58) {
      for (int i = start - 1; i < start + 29; ++i) {
        mas_result[i - start] = long_mas_result[i];
      }
      int end = counter_zero_in_mas(long_mas_result, 58, _right);
      if ((start + 29 != 58 - end) && (end != 58)) {
        for (int i = 57; i > 28; --i) {
          if ((long_mas_result[i] >= 5) && (long_mas_result[i - 1] % 2 == 1))
            long_mas_result[i - 1] += 1;
        }
        mas_result[28] = long_mas_result[start + 29 - 1];
        correctoin_rank(mas_result, &new_rank);
        dec_to_decimal(mas_result, result);
        if (new_rank < 29) result->bits[3] |= (new_rank << 16);
      } else {
        correctoin_rank(mas_result, &new_rank);
        dec_to_decimal(mas_result, result);
        if (new_rank < 29) result->bits[3] |= (new_rank << 16);
      }
    } else {
      dec_to_decimal(mas_result, result);
    }
  }
  return back;
}

int correction_max(int *mas) {
  int back = 0;
  int mas_max[29] = {7, 9, 2, 2, 8, 1, 6, 2, 5, 1, 4, 2, 6, 4, 3,
                     3, 7, 5, 9, 3, 5, 4, 3, 9, 5, 0, 3, 3, 5};
  for (int i = 0; i < 29; ++i) {
    if (mas[i] > mas_max[i]) {
      back = 1;
    } else if (mas[i] < mas_max[i]) {
      break;
    }
  }
  return back;
}

void copy_int_bigger_mas(int *small_mas, int *big_mas, int small_size) {
  for (int i = 0; i < small_size; ++i) big_mas[i] = small_mas[i];
}

int counter_zero_in_mas(int *mas, int size, int direction) {
  int back = 0;
  if (direction == 1) {
    for (int i = 0; i < size; ++i) {
      if (mas[i] == 0)
        back++;
      else
        break;
    }
  } else if (direction == 2) {
    for (int i = size - 1; i >= 0; --i) {
      if (mas[i] == 0)
        back++;
      else
        break;
    }
  } else {
    back = 2;
  }
  return back;
}

int sum_mas(int *mas1, int *mas2, int *result, int size) {
  int back = 0, rank = 0, buf = 0;
  for (int i = size; i >= 0; i--) {
    buf = mas1[i] + mas2[i] + rank;
    result[i] = buf % 10;
    if (buf > 9)
      rank = 1;
    else
      rank = 0;
  }
  if (buf > 9) back = 1;
  return back;
}

int s21_is_equal(s21_decimal value1, s21_decimal value2) {
  int back = 1, sign1, sign2;
  sign1 = (value1.bits[3] >> 31);
  sign2 = (value2.bits[3] >> 31);
  if (sign1 == sign2) {
    int mas_buf_29[29], mas1[58], mas2[58];
    zeroing_char(mas_buf_29, 29);
    zeroing_char(mas1, 58);
    zeroing_char(mas2, 58);
    decimal_to_dec(value1, mas_buf_29);
    copy_int_bigger_mas(mas_buf_29, mas1, 29);
    zeroing_char(mas_buf_29, 29);
    decimal_to_dec(value2, mas_buf_29);
    copy_int_bigger_mas(mas_buf_29, mas2, 29);
    int extent1 = (int)((value1.bits[3] & ~(1 << 31)) >> 16);
    int extent2 = (int)((value2.bits[3] & ~(1 << 31)) >> 16);
    shift(_right, extent1, mas1, 58);
    shift(_right, extent2, mas2, 58);
    for (int i = 0; i < 58; ++i) {
      if (mas1[i] > mas2[i]) {
        back = 0;
        break;
      } else if (mas1[i] < mas2[i]) {
        back = 0;
        break;
      }
    }
  } else {
    int sum = 0;
    for (int i = 0; i < 3; ++i) {
      sum += value1.bits[i] + value2.bits[i];
    }
    if (sum != 0) back = 0;
  }
  return back;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  return !(s21_is_equal(value_1, value_2));
}

int s21_is_greater(s21_decimal value1, s21_decimal value2) {
  int back = 1, sign1, sign2;
  sign1 = (value1.bits[3] >> 31);
  sign2 = (value2.bits[3] >> 31);
  if (sign1 == sign2) {
    int mas_buf_29[29], mas1[58], mas2[58];
    zeroing_char(mas_buf_29, 29);
    zeroing_char(mas1, 58);
    zeroing_char(mas2, 58);
    decimal_to_dec(value1, mas_buf_29);
    copy_int_bigger_mas(mas_buf_29, mas1, 29);
    zeroing_char(mas_buf_29, 29);
    decimal_to_dec(value2, mas_buf_29);
    copy_int_bigger_mas(mas_buf_29, mas2, 29);
    int extent1 = (int)((value1.bits[3] & ~(1 << 31)) >> 16);
    int extent2 = (int)((value2.bits[3] & ~(1 << 31)) >> 16);
    shift(_right, extent1, mas1, 58);
    shift(_right, extent2, mas2, 58);
    for (int i = 0; i < 58; ++i) {
      if (mas1[i] > mas2[i]) {
        break;
      } else if (mas1[i] < mas2[i]) {
        back = 0;
        break;
      }
      if (i == 57) back = 0;
    }
    if (sign1 == 1) back = !back;
  } else {
    int sum = 0;
    for (int i = 0; i < 3; ++i) {
      sum += value1.bits[i] + value2.bits[i];
    }
    if ((sum == 0) || (sign1 > sign2)) back = 0;
  }
  return back;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_greater(value_1, value_2) || s21_is_equal(value_1, value_2);
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_greater_or_equal(value_1, value_2);
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_less(value_1, value_2) || s21_is_equal(value_1, value_2);
}
