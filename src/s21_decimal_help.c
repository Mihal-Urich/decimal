#include "s21_decimal.h"

void s21_set_sign(s21_decimal *number, int sign) {
  unsigned int shift = 1;
  if (sign == 1)
    number->bits[3] = number->bits[3] | shift << 31;
  else
    number->bits[3] = number->bits[3] & ~(shift << 31);
}

int s21_get_sign(s21_decimal dst) {
  int sign = (int)((dst.bits[SCALE] >> 31) & 1u);
  return sign;
}

int s21_get_scale(s21_decimal dst) {
  int scale = 0;
  for (int i = 16; i <= 23; i++) {
    scale += ((dst.bits[SCALE] >> (i)) & 1u) * pow(2, i - 16);
  }
  return scale;
}

void s21_set_scale(s21_decimal *value, int scale) {
  int scale_arr[8] = {0}, tmp = scale;
  for (int i = 7; i >= 0; i--) {
    scale_arr[i] = tmp % 2;
    tmp /= 2;
  }
  int scale_i = 119;
  for (int i = 0; i < 8; i++) {
    s21_set_bit(value, scale_i, scale_arr[i]);
    scale_i--;
  }
}

int s21_get_bit(s21_decimal src, int pos) {
  int bit_group = pos / 32;
  int bit_pos = pos % 32;

  return (src.bits[bit_group] >> bit_pos) & 1u;
}

void s21_set_bit(s21_decimal *number, int bit, int value) {
  unsigned int shift = 1;
  if (bit >= 0 && bit < 128) {
    if (value)
      number->bits[bit / 32] = number->bits[bit / 32] | shift << (bit % 32);
    else
      number->bits[bit / 32] = number->bits[bit / 32] & ~(shift << (bit % 32));
  }
}

void s21_shift_left(s21_decimal *value) {
  int last_bit_midl = 0;
  int last_bit_low = 0;
  last_bit_low = s21_get_bit(*value, 31);
  last_bit_midl = s21_get_bit(*value, 63);
  value->bits[0] <<= 1;
  value->bits[1] <<= 1;
  value->bits[2] <<= 1;
  s21_set_bit(value, 32, last_bit_low);
  s21_set_bit(value, 64, last_bit_midl);
}

int s21_get_high_bit_pos(s21_decimal value) {
  int res = -1;

  for (int i = 95; i >= 0 && res == -1; i--) {
    if (s21_get_bit(value, i) == 1) {
      res = i;
    }
  }
  return res;
}
