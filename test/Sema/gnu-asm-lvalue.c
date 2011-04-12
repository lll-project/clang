// RUN: %clang_cc1 %s

void foo() {
  int a;
  asm("nop" : : "m"((int)(a)));
  asm("nop" : "=r"((unsigned)a));
}
