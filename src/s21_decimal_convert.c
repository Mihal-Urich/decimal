#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int result = 0;
  memset(dst, 0, sizeof(*dst));
  if (src != S21_NAN || src != S21_INF) {
    if (src < 0) {
      src *= -1;
      s21_set_sign(dst, 1);
    }
    dst->bits[LOW] = src;
    if (s21_get_bit(*dst, 31)) {
      memset(dst, 0, sizeof(*dst));
      result = 1;
    }
  } else {
    result = 1;
  }
  return result;
}

int errorsConvertInt(s21_decimal src) {
  int flag = 0;
  if (src.bits[MIDL] != 0 || src.bits[HIGH] != 0) flag = 1;
  if (!flag) flag = s21_get_bit(src, 31);
  if (!flag) flag = src.bits[LOW] <= INT_MAX ? 0 : 1;
  return flag;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int flag = 0;
  *dst = 0;
  s21_truncate(src, &src);
  flag = errorsConvertInt(src);
  if (!flag) {
    *dst = src.bits[LOW];
    if (s21_get_sign(src)) *dst *= -1;
  }
  return flag;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int flag = 0, scale = s21_get_scale(src),
      hightPos = s21_get_high_bit_pos(src);
  double buffer = 0;
  *dst = 0;
  for (int i = 0; i <= hightPos; i++) buffer += s21_get_bit(src, i) * pow(2, i);
  for (int i = 0; i < scale; i++) buffer /= 10.0;
  *dst = (float)buffer;
  if (s21_get_sign(src)) *dst *= -1;
  return flag;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int flag = 0;
  int count = 0;
  int scale = 0;
  float temp_int = 0.0;
  if (isnan(src) || isinf(src)) flag = 1;
  memset(dst, 0, sizeof(*dst));
  if (!flag) {
    if (fabs(src) < 1e-28 || fabs(src) > 79228162514264337593543950335.0) {
      flag = 1;
    } else {
      float temp_float = modff(src, &temp_int);
      while (temp_int >= 10000000) {
        temp_int /= 10;
        count = 7;
      }
      float temp_int_1 = fabs(temp_int);
      if (count < 7) {
        while (temp_int_1 >= 1) {
          temp_int_1 /= 10;
          count++;
        }
        scale = 7 - count;
        if (scale <= 7) {
          temp_float *= pow(10, scale);
          temp_int *= pow(10, scale);
          temp_int += temp_float;
        }
      }
      int int_temp = temp_int;
      for (int i = 0; i < 96; i++) {
        s21_set_bit(dst, i, int_temp % 2);
        int_temp /= 2;
      }
      if (src < 0) s21_set_sign(dst, 1);
      s21_set_scale(dst, scale);
    }
  }
  return flag;
}