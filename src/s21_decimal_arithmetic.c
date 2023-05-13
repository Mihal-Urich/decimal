#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int back = 0;
  zering_decimal(result);
  result->bits[3] = 0;
  if (value_1.bits[3] >> 31 == value_2.bits[3] >> 31) {
    back = under_add(value_1, value_2, result);
    result->bits[3] |= (value_1.bits[3] >> 31) << 31;
  } else {
    if (value_1.bits[3] >> 31) {
      value_1.bits[3] &= ~(1 << 31);
      result->bits[3] |= (s21_is_greater(value_1, value_2) << 31);
    } else {
      value_2.bits[3] &= ~(1 << 31);
      result->bits[3] |= (s21_is_greater(value_2, value_1) << 31);
    }
    back = under_sub(value_1, value_2, result);
  }
  return back;
}

void arifmetik(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
               int *error) {
  int tmp = 0, bit_value1 = 0, bit_value2 = 0;
  for (int i = 0; i <= 95; i++) {
    bit_value1 = s21_get_bit(value_1, i);
    bit_value2 = s21_get_bit(value_2, i);
    if (bit_value1 && !bit_value2 && tmp) {
      s21_set_bit(result, i, 0);
    } else if (!bit_value1 && bit_value2 && tmp) {
      s21_set_bit(result, i, 0);
    } else if (!bit_value1 && !bit_value2 && tmp) {
      s21_set_bit(result, i, 1);
      tmp = 0;
    } else if (bit_value1 && !bit_value2 && !tmp) {
      s21_set_bit(result, i, 1);
    } else if (!bit_value1 && bit_value2 && !tmp) {
      s21_set_bit(result, i, 1);
    } else if (!bit_value1 && !bit_value2 && !tmp) {
      s21_set_bit(result, i, 0);
    } else if (bit_value1 && bit_value2 && !tmp) {
      s21_set_bit(result, i, 0);

    } else if (bit_value1 && bit_value2 && tmp) {
      s21_set_bit(result, i, 1);
    }
    if (bit_value1 && bit_value2) tmp = 1;
    if (i == 95 && tmp == 1) {
      *error = 1;
      memset(result, 0, sizeof(*result));
      break;
    }
  }
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal_long value_long_1 = {0}, value_long_2 = {0}, result_long = {0};
  s21_equality_degree(value_1, value_2, &value_long_1, &value_long_2);
  if (!s21_get_sign_(value_long_1.bits, SIZE_LONG)) {
    if (!s21_get_sign_(value_long_2.bits, SIZE_LONG))
      s21_sub_without_equal_degree(value_long_1.bits, value_long_2.bits,
                                   result_long.bits, SIZE_LONG);
    else
      s21_sum_without_equal_degree(value_long_1.bits, value_long_2.bits,
                                   result_long.bits, SIZE_LONG);
  } else {
    if (!s21_get_sign_(value_long_2.bits, SIZE_LONG)) {
      s21_sum_without_equal_degree(value_long_1.bits, value_long_2.bits,
                                   result_long.bits, SIZE_LONG);
      result_long.bits[SIZE_LONG - 1] =
          result_long.bits[SIZE_LONG - 1] | 1u << 31;
    } else {
      s21_sub_without_equal_degree(value_long_1.bits, value_long_2.bits,
                                   result_long.bits, SIZE_LONG);
    }
  }
  return s21_long_to_decimal(result_long, result);
}

void s21_equality_degree(
    s21_decimal value_1, s21_decimal value_2, s21_decimal_long *value_long_1,
    s21_decimal_long *value_long_2) {  // приведение чисел к одной степени
  int difference_degree = 0;
  s21_decimal_to_long(value_1, value_long_1);
  s21_decimal_to_long(value_2, value_long_2);
  difference_degree = s21_get_degree(value_long_1->bits, SIZE_LONG) -
                      s21_get_degree(value_long_2->bits, SIZE_LONG);
  if (difference_degree > 0) {
    s21_change_degree(value_long_2, difference_degree);
  } else if (difference_degree < 0) {
    s21_change_degree(value_long_1, difference_degree);
  }
}

void s21_sub_without_equal_degree(unsigned int *value_1, unsigned int *value_2,
                                  unsigned int *result,
                                  int size) {  // вычитание
  unsigned int mod = 0;
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < 32; j++) {
      unsigned int a = s21_get_bit_(value_1, i, j);
      unsigned int b = s21_get_bit_(value_2, i, j);
      if ((int)(a - mod - b) < 0) {
        a = a + 2 - mod;
        mod = 1;
      } else if (a == 1 && mod == 1) {
        a = 0;
        mod = 0;
      }
      result[i] = result[i] | (a - b) << j;
    }
  }
  if (s21_get_sign_(value_1, size))
    // если вычитаемое было отриц, значит весь
    // результат отрицателен
    result[size - 1] = result[size - 1] | 1u << 31;
  if ((mod && result[size - 2] >> 31)) {
    // смотрим переносы в знаковый разряд и из знакового (в
    // внешний разряд). Если они имеют одинаковый статус (оба
    // есть или обоих нет), то все нормально переполнения нет
    s21_decimal_long temp = {0};
    for (int i = 0; i < size; i++) temp.bits[i] = result[i];
    s21_invert_long_decimal(&temp);
    for (int i = 0; i < size; i++) result[i] = temp.bits[i];
  }
  result[size - 1] = result[size - 1] | (s21_get_degree(value_1, size) << 15);
  // вставляем степень как у вэлью 1
}

void s21_invert_long_decimal(s21_decimal_long *value) {
  s21_decimal_long one = {0}, temp = {0};
  one.bits[0] = 1;
  for (int i = 0; i < SIZE_LONG - 1; i++)
    value->bits[i] = ~value->bits[i];  // инвертируем
  temp = *value;
  memset(value, 0, sizeof(s21_decimal_long));
  s21_sum_without_equal_degree(temp.bits, one.bits, value->bits,
                               SIZE_LONG);  // и добавляем единицу
  if (!s21_get_sign_(temp.bits, SIZE_LONG)) {  // меняем знак числа
    value->bits[SIZE_LONG - 1] = value->bits[SIZE_LONG - 1] | 1u << 31;
    // ставим единицу в бит знака (говорим, что число
    // отрицательное)
  } else {
    value->bits[SIZE_LONG - 1] = value->bits[SIZE_LONG - 1] & ~(1u << 31);
    // ставим ноль в бит знака (говорим, что число положительное)
  }
}

void s21_sum_without_equal_degree(unsigned int *value_1, unsigned int *value_2,
                                  unsigned int *result, int size) {
  unsigned int mod = 0;
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < 32; j++) {
      unsigned int a = s21_get_bit_(value_1, i, j);
      unsigned int b = s21_get_bit_(value_2, i, j);
      result[i] = result[i] | (a + b + mod) % 2 << j;
      mod = (a + b + mod) / 2;
    }
  }
  result[size - 1] = result[size - 1] | (s21_get_degree(value_1, size) << 15);
  // вставляем степень value 1
}

unsigned int s21_get_sign_(unsigned int *value_byte, int size) {
  // получение бита символа знака
  return (unsigned int)!!(value_byte[size - 1] & 1u << 31);
}

unsigned int s21_get_bit_(unsigned int *value_byte, int num_byte,
                          int i) {  // получение бита в опред. позиции
  return (unsigned int)!!(value_byte[num_byte] & 1u << i);
}

void s21_decimal_to_long(s21_decimal value, s21_decimal_long *value_long) {
  for (int i = 0; i < SIZE_DEC - 1; i++) {
    value_long->bits[i] = value.bits[i];
  }
  value_long->bits[SIZE_LONG - 1] = value.bits[SIZE_DEC - 1];
}

unsigned int s21_get_degree(unsigned int *value_byte,
                            int size) {  // получение степени числа
  return (unsigned int)(value_byte[size - 1] & ~(1u << 31)) >> 15;
}

void s21_change_degree(s21_decimal_long *value, int difference_degree) {
  s21_decimal_long temp = {0}, result = {0};
  unsigned int sign = s21_get_sign_(value->bits, SIZE_LONG);
  temp = *value;
  for (int i = 0; i < abs(difference_degree); i++) {
    for (int j = 0; j < 9; j++) {
      s21_sum_without_equal_degree(temp.bits, value->bits, result.bits,
                                   SIZE_LONG);
      temp = result;
      memset(&result, 0, sizeof(s21_decimal_long));
    }
    temp.bits[SIZE_LONG - 1] += 32768;
    // добавляем единичку в биты степеней (2^15)
    *value = temp;
  }
  value->bits[SIZE_LONG - 1] =
      value->bits[SIZE_LONG - 1] |
      sign << 31;  // установка знака числа, какой он был
}

int s21_long_to_decimal(s21_decimal_long value_long, s21_decimal *value) {
  int res = 0, num_mod = 0, sum_num_mod = 0;
  unsigned int degree = s21_get_degree(value_long.bits, SIZE_LONG);
  unsigned int sign = s21_get_sign_(value_long.bits, SIZE_LONG);
  while (degree > 28 ||
         (!s21_zero_decimal(value_long.bits, SIZE_LONG, SIZE_DEC - 1) &&
          degree > 0)) {
    // пока степень не будет меньше 29 или высшие биты не опустеют
    num_mod =
        s21_extract_num_grade(1, value_long.bits, SIZE_LONG, &sum_num_mod);
    degree--;
  }
  if (num_mod > 5 ||
      (num_mod == 5 && sum_num_mod == 5 && (value_long.bits[0] & 1u))) {
    s21_decimal_long one = {0}, temp = value_long;
    one.bits[0] = 1;
    memset(&value_long, 0, sizeof(s21_decimal_long));
    s21_sum_without_equal_degree(temp.bits, one.bits, value_long.bits,
                                 SIZE_LONG);  // то добавляем единицу
    if (sign)
      value_long.bits[SIZE_LONG - 1] =
          value_long.bits[SIZE_LONG - 1] | 1u << 31;
  }
  if (s21_zero_decimal(value_long.bits, SIZE_LONG, SIZE_DEC - 1)) {
    for (int i = 0; i < SIZE_DEC - 1; i++) {
      value->bits[i] = value_long.bits[i];
    }
    value->bits[SIZE_DEC - 1] = value_long.bits[SIZE_LONG - 1];
  } else {
    if (!sign) {  // если знак минус
      res = 1;
    } else {  // если знак плюс
      res = 2;
    }
  }
  return res;
}

int s21_zero_decimal(unsigned int *decimal_byte, int size,
                     int shift) {  // проверка на то, что массив пустой
  int result = 1;
  for (int i = 0 + shift; i < size - 1 && result; i++)
    if (decimal_byte[i]) result = 0;
  return result;  // 1 - все массивы равны 0, 0 - в противоположном случае
}

int s21_extract_num_grade(int degree, unsigned int *byte, int size,
                          int *sum_num_mod) {
  int num_mod = 0;
  while (degree > 0) {
    num_mod = s21_digit_extracting(byte, size);
    // извлекаем самый низший разряд в числе (в случае наличия
    // степени эти разряды будут относится к дробной части)
    degree--;
    byte[size - 1] -= 32768;  // понижаем также саму степень
    *sum_num_mod += num_mod;
    // считаем сумму извлеченных цифр, если она была больше 5 при
    // последней цифре 5, то значит раньше были цифры
  }
  return num_mod;
}

int s21_digit_extracting(unsigned int *decimal_byte, int size) {
  unsigned long long int number_bits =
      0;  // создаем буффер в 2 раза больший, чем int элемент массива
  for (int i = size - 2; i >= 0; i--) {
    // к содержимому буфера, смещенному на 32 бита (изначально нулевому),
    // добавляем содержимое iго int-элемента
    number_bits = (number_bits << sizeof(unsigned int) * 8) + decimal_byte[i];
    // перезаписываем int элемент с учетом того, что младший разряд выводится на
    // печать в строку
    decimal_byte[i] = (unsigned int)(number_bits / 10);
    // эта цифра выведется на печать, если это была последняя итерация цикла.
    // Если это не так, то цифра спускается в более младший int элемент, тем
    // самым для следующей итерации буфера становится равным этой цифрею
    number_bits %= 10;
  }
  return (int)number_bits;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error = 0;
  multiplakete(value_1, value_2, result, &error);
  s21_set_scale(result, s21_get_scale(value_1) + s21_get_scale(value_2));
  if (!s21_get_sign(value_1) && s21_get_sign(value_2)) {
    s21_set_sign(result, 1);
  } else if (s21_get_sign(value_1) && !s21_get_sign(value_2)) {
    s21_set_sign(result, 1);
  }
  return error;
}

void multiplakete(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int *error) {
  memset(result, 0, sizeof(*result));
  s21_decimal tmp = {0};
  if ((*error = proverka_mul(value_1, value_2)) == 0) {
    for (int i = 0; i <= 95; i++) {
      if (s21_get_bit(value_2, i) == 1) {
        tmp = value_1;
      }
      for (int z = 0; z < i; z++) {
        s21_shift_left(&tmp);
      }
      arifmetik(*result, tmp, result, error);
      memset(&tmp, 0, sizeof(tmp));
    }
  }
  if (*error == 1 && ((s21_get_sign(value_1) && !s21_get_sign(value_2)) ||
                      (!s21_get_sign(value_1) && s21_get_sign(value_2)))) {
    *error = 2;
  }
}

int proverka_mul(s21_decimal value_1, s21_decimal value_2) {
  int error = 0;
  s21_decimal zero = {0};
  if ((value_1.bits[LOW] == UINT_MAX && value_1.bits[MIDL] == UINT_MAX &&
       value_1.bits[HIGH] == UINT_MAX) &&
      !(s21_is_equal(value_2, zero))) {
    error = 1;
  }
  return error;
}

void reverse4(s21_decimal private, s21_decimal *buffer, int highPos) {
  int bit = 0;
  int q = highPos;
  for (int i = 0; i <= highPos; i++) {
    bit = s21_get_bit(private, q);
    q--;
    s21_set_bit(buffer, i, bit);
  }
}

void delenie(s21_decimal value_1, s21_decimal value_2, s21_decimal *tmp,
             s21_decimal *private, int *position_in_private) {
  memset(tmp, 0, sizeof(*tmp));
  s21_set_scale(&value_1, 0);
  s21_set_scale(&value_2, 0);
  s21_decimal tmp1 = {0};
  int highPos = s21_get_high_bit_pos(value_1);
  int bit = 0;
  for (int i = highPos; i >= 0; i--) {
    if (s21_is_greater_or_equal(*tmp, value_2)) {
      s21_sub(*tmp, value_2, &tmp1);
      *tmp = tmp1;
      memset(&tmp1, 0, sizeof(tmp1));
      s21_set_bit(private, *position_in_private, 1);
      bit = s21_get_bit(value_1, i);
      s21_shift_left(tmp);
      s21_set_bit(tmp, 0, bit);
      *position_in_private = *position_in_private + 1;
    } else {
      bit = s21_get_bit(value_1, i);
      s21_shift_left(tmp);
      s21_set_bit(tmp, 0, bit);
      s21_set_bit(private, *position_in_private, 0);
      *position_in_private = *position_in_private + 1;
    }
  }
  if (s21_is_greater_or_equal(*tmp, value_2)) {
    s21_sub(*tmp, value_2, &tmp1);
    *tmp = tmp1;
    memset(&tmp1, 0, sizeof(tmp1));
    s21_set_bit(private, *position_in_private, 1);
  } else {
    s21_set_bit(private, *position_in_private, 0);
  }
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  memset(result, 0, sizeof(*result));
  int error = 0, position_in_private = 0, scale = 0;
  s21_decimal ZERO = {0};
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal tmp = {0};
  s21_decimal private = {0};
  s21_decimal private_sec = {0};
  s21_decimal private_thr = {0};
  if (s21_is_equal(value_2, ZERO))
    error = 3;
  else if (s21_is_equal(value_1, value_2)) {
    s21_set_bit(result, 1, 0);
  } else {
    delenie(value_1, value_2, &tmp, &private, &position_in_private);
    reverse4(private, &private, position_in_private);
    while (!s21_is_equal(tmp, ZERO) && scale < 28) {
      position_in_private = 0;
      s21_mul(tmp, ten, &tmp);
      scale++;
      delenie(tmp, value_2, &tmp, &private_thr, &position_in_private);
      reverse4(private_thr, &private_thr, position_in_private);
      s21_mul(private_sec, ten, &private_sec);
      s21_add(private_thr, private_sec, &private_sec);
    }
    s21_set_scale(&private_sec, scale);
    s21_add(private, private_sec, result);
    scale = scale + (s21_get_scale(value_1) - s21_get_scale(value_2));
    if (scale < 0) {
      for (int i = scale; i < 0; i++) {
        s21_mul(*result, ten, result);
        if (s21_get_scale(*result) != 0)
          s21_set_scale(result, s21_get_scale(*result) - 1);
      }
    } else {
      s21_set_scale(result, scale);
    }
    if (s21_get_sign(value_1) != s21_get_sign(value_2)) s21_set_sign(result, 1);
  }
  return error;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal ZERO = {0};
  memset(result, 0, sizeof(*result));
  int error = 0, z1 = 0;
  s21_decimal tmp = {0};
  s21_decimal private = {0};
  if (s21_is_equal(value_2, ZERO))
    error = 3;
  else {
    if (s21_is_equal(value_1, ZERO))
      *result = value_2;
    else {
      delenie(value_1, value_2, &tmp, &private, &z1);
      *result = tmp;
    }
  }
  if (s21_get_sign(value_1) != s21_get_sign(value_2)) s21_set_sign(result, 1);
  return error;
}