/*
 * CS:APP Data Lab
 *
 * Peter Ye
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  // easily to find this equation by using the truth table
  return ~(x & y) & ~(~x & ~y);
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  // simply using << to get the Tmin (10000...000)
  return 0x1 << 31;
}
// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  // core: only -1 and Tmax can realize ~x == x+1
  // new_x = x if x!= -1;     if x == -1, new_x = 0
  // we do this to remove -1
  int new_x = !(x + 1) + x;
  // after removing -1 here, we can use ~x == x+1 to answer
  return !((new_x + 1) ^ ~new_x);
}
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  // construct tmp = 0xAAAAAAAA(1010...1010)
  // Note that all odd-numbered bits in word should be set to 1, so only
  // allOddBits(0xAAAAAAAA) = 1
  int tmp = ((0xAA << 8 | 0xAA) << 8 | 0xAA) << 8 | 0xAA;
  return !((tmp & x) ^ tmp);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  // easy question: simply ~ and plus 1
  return ~x + 1;
}
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0'
 * to '9') Example: isAsciiDigit(0x35) = 1. isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  // for 0x30~0x37, (x >> 3) ^ 0x06(0000 0110) = 0
  // for 0x38,0x39, (x >> 1) ^ 0x1C(0001 1100) = 0
  return !((x >> 3) ^ 0x06) | !((x >> 1) ^ 0x1C);
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  // 0000...0000 & any number = 0; 1111...1111 & any number = any number
  // ~(!x) + 1 = 0000...0000 if x != 0,  ~(!x) + 1 = 1111...1111 if x == 0
  int tmp = ~(!x) + 1;
  return (~tmp & y) | (tmp & z);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  // core: judge (y - x) is positive or negative
  // Note: y-x = y + (~x + 1)
  // Note: if signs of x and y are different, it may cause an overflow
  // So make sure signs of x and y are the same!
  int sign_x = x >> 31;
  int sign_y = y >> 31;
  // cond1: signs of x and y are different
  // cond1 is true when x<0, y>0
  int cond_1 = (sign_x ^ sign_y) & (sign_x & 1);
  // cond2: signs are the same and y - x >=0 (since same signs, no overflow
  // here) cond2 is true when y - x >=0
  int cond_2 = !(sign_x ^ sign_y) & !((y + (~x + 1)) >> 31 & 1);
  // if y < 0 and x > 0, none of the conds would be true
  return cond_1 | cond_2;
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x) {
  // x and -x(~x+1) must have different signs if x != 0
  // So (x | -x) >> 31 (arithmetic right shift) = -1 if x != 0
  // While  (x | -x) >> 31 = 0 if x == 0
  // So we get the answer
  return ((x | (~x + 1)) >> 31) + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  // 1. for a positive number, find the first `1` from left to right
  //    the result is the number of bits from first 1 to the end then plus
  //    1(sign bit) for example: 12(0000 1100) = 4 + 1 = 5
  // 2. How to find the first `1`? By dichotomy, judging the high 16 bits,
  //    Then 8 bits, then 4 bits ... Remember to add the first `1` itself
  // 3. The negative number, the question is transformed into finding the first
  // 0
  //    Try to reuse the pattern for positive number by turning x into ~x
  //    for example: -5(1111....1111 1011), result = 3+1 =4, 5(0101), result = 3
  //    + 1 = 4
  int m16, m8, m4, m2, m1, m0;
  x = (x >> 31) ^ x;       // if x is positive, nothing changed, else x = ~ x
  m16 = !!(x >> 16) << 4;  // if 1 in higher 16 bits, m16=16, else 0
  x = x >> m16;
  m8 = !!(x >> 8)
       << 3;  // m8 = 8 if 1 in 16~8 bit position or 32~24 bit position
  x = x >> m8;
  m4 = !!(x >> 4) << 2;
  x = x >> m4;
  m2 = !!(x >> 2) << 1;
  x = x >> m2;
  m1 = !!(x >> 1);  // m1 = 1 if 1 in the left, else in the right
  x = x >> m1;
  m0 = x;  // the bit count for the first `1`
  return m16 + m8 + m4 + m2 + m1 + m0 +
         1;  // 1 is for the sign(both positive and negative number)
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  if (!((((uf << 1) >> 24) ^ 0x000000ff))) {
    // uf = inf(exp = 0xff), return uf
    // Note: for unsigned number, >> is always logical (no additional ones)
    return uf;
  }
  if (!(((uf << 1) >> 24) ^ 0)) {
    // uf is denormalized number: exp = 0, normally we can << 1 to *2
    // eg: 0 0000 001(1/8 * 2^-6 = 1/512), 0 0000 010(2/8 * 2^-6 = 2/512)
    // eg: 0 0000 110(7/8 * 2^-6 = 6/512), 0 0001 100((1+4/8) * 2^-6 =12/512)
    // result = sign + uf << 1
    return ((uf >> 31) << 31) + (uf << 1);
  }
  // for normalized number, exp + 1
  // eg: 0 0110 111(15/8 * 2 ^-1 = 15/16), 0 0111 111(15/8 * 2^0 = 15/8)
  // eg: 0 1110 111(15/8 * 2 ^ 7 = 240), 0 1111 111(inf) is ok
  return uf + 0x00800000;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  // Note: in C, int(0.9) = 0, which means we can just discard the frac
  int E = ((uf << 1) >> 24) - 127;  // exp - bias(127)
  int pow_2_E = 1;
  if (E >= 31) {
    // 1. uf = inf(exp = 255, E = 128), return 0x80000000u
    // 2. uf to int > Tmax(E >= 31), overflow for int, return 0x80000000u
    return 0x80000000u;
  } else if (E < 0) {
    // 1. uf is denormalized number: exp = 0, E = -127, return 0
    // 2. 0< uf'exp < 127, E < 0, so it must be a number in (-1, 1)
    return 0;
  }
  // uf is normalized number, value = (-1)^s * 2^E * 1
  for (; E != 0; E--) {
    pow_2_E = pow_2_E * 2;
  }
  return (uf >> 31) ? -pow_2_E : pow_2_E;  // return positive if sign == 0
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  // 1. Basically, float number = (-1)^s * 2^E * frac
  // 2. for normalized number: x in [-126, 127]
  //    2.0^x for float means sign=0, exp = E + 127 = x + 127, frac = 0
  //    smallest norm is 2^(-126)(0x00800000), x = -126 here (exp = 1)
  //    biggest norm is 3.402823466e+38(0x7f7fffff) > 2^127, <2^128
  //    so x should small than 128
  // 3. for denormalized number: x in [-149, -127]
  //    2.0^x for float means sign=0, exp = 0, E = -126, frac depends on x
  //    a. the smallest denorm FloatMin is (0x0000 0001) = 1.401298464e-45
  //       2^(-150) < FloatMin, 2^(-149) > FloatMin
  //       So x should big than -150
  //    b. frac depends on x
  //       eg: x=-127, frac = 1000....0000; x=-149, frac = 0000...0001
  if (x > 127) {
    return 0x7f800000u;  // inf
  } else if (-126 <= x && x <= 127) {
    return (x + 127) << 23;  // normalized number, just need to consider exp
  } else if (-149 <= x && x <= -127) {
    return 0x1 << (x + 149);  // denormalized number, just need to consider frac
  } else {
    return 0;  // too small
  }
}
