// RUN: %clang_cc1 -fsyntax-only -verify -fblocks %s -Wnon-pod-varargs

extern char version[];

class C {
public:
  C(int);
  void g(int a, ...);
  static void h(int a, ...);
};

void g(int a, ...);

void t1()
{
  C c(10);
  
  g(10, c); // expected-warning{{cannot pass object of non-POD type 'C' through variadic function; call will abort at runtime}}
  g(10, version);
}

void t2()
{
  C c(10);

  c.g(10, c); // expected-warning{{cannot pass object of non-POD type 'C' through variadic method; call will abort at runtime}}
  c.g(10, version);
  
  C::h(10, c); // expected-warning{{cannot pass object of non-POD type 'C' through variadic function; call will abort at runtime}}
  C::h(10, version);
}

int (^block)(int, ...);

void t3()
{
  C c(10);
  
  block(10, c); // expected-warning{{cannot pass object of non-POD type 'C' through variadic block; call will abort at runtime}}
  block(10, version);
}

class D {
public:
  void operator() (int a, ...);
};

void t4()
{
  C c(10);

  D d;
  
  d(10, c); // expected-warning{{cannot pass object of non-POD type 'C' through variadic method; call will abort at runtime}}
  d(10, version);
}

class E {
  E(int, ...); // expected-note 2{{implicitly declared private here}}
};

void t5()
{
  C c(10);
  
  E e(10, c); // expected-warning{{cannot pass object of non-POD type 'C' through variadic constructor; call will abort at runtime}} \
    // expected-error{{calling a private constructor of class 'E'}}
  (void)E(10, c); // expected-warning{{cannot pass object of non-POD type 'C' through variadic constructor; call will abort at runtime}} \
    // expected-error{{calling a private constructor of class 'E'}}

}

// PR5761: unevaluated operands and the non-POD warning
class Foo {
 public:
  Foo() {}
};

int Helper(...);
const int size = sizeof(Helper(Foo()));

namespace std {
  class type_info { };
}

struct Base { virtual ~Base(); };
Base &get_base(...);
int eat_base(...);

void test_typeid(Base &base) {
  (void)typeid(get_base(base)); // expected-warning{{cannot pass object of non-POD type 'Base' through variadic function; call will abort at runtime}}
  (void)typeid(eat_base(base)); // okay
}


// rdar://7985267 - Shouldn't warn, doesn't actually use __builtin_va_start is
// magic.

void t6(Foo somearg, ... ) {
  __builtin_va_list list;
  __builtin_va_start(list, somearg);
}

