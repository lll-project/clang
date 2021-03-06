// RUN: %clang_cc1 -fsyntax-only -Wuninitialized -fsyntax-only -fcxx-exceptions %s -verify

// Stub out types for 'typeid' to work.
namespace std { class type_info {}; }

int test1_aux(int &x);
int test1() {
  int x;
  test1_aux(x);
  return x; // no-warning
}

int test2_aux() {
  int x;
  int &y = x;
  return x; // no-warning
}

// Don't warn on unevaluated contexts.
void unevaluated_tests() {
  int x;
  (void)sizeof(x);
  (void)typeid(x);
}

// Warn for glvalue arguments to typeid whose type is polymorphic.
struct A { virtual ~A() {} };
void polymorphic_test() {
  A *a; // expected-note{{declared here}} expected-note{{add initialization}}
  (void)typeid(*a); // expected-warning{{variable 'a' is uninitialized when used here }}
}

// Handle cases where the CFG may constant fold some branches, thus
// mitigating the need for some path-sensitivity in the analysis.
unsigned test3_aux();
unsigned test3() {
  unsigned x = 0;
  const bool flag = true;
  if (flag && (x = test3_aux()) == 0) {
    return x;
  }
  return x;
}
unsigned test3_b() {
  unsigned x ;
  const bool flag = true;
  if (flag && (x = test3_aux()) == 0) {
    x = 1;
  }
  return x; // no-warning
}
unsigned test3_c() {
  unsigned x; // expected-note{{declared here}} expected-note{{add initialization}}
  const bool flag = false;
  if (flag && (x = test3_aux()) == 0) {
    x = 1;
  }
  return x; // expected-warning{{variable 'x' is uninitialized when used here}}
}

enum test4_A {
 test4_A_a, test_4_A_b
};
test4_A test4() {
 test4_A a; // expected-note{{variable 'a' is declared here}}
 return a; // expected-warning{{variable 'a' is uninitialized when used here}}
}

// This test previously crashed Sema.
class Rdar9188004A {
public: 
  virtual ~Rdar9188004A();
};

template< typename T > class Rdar9188004B : public Rdar9188004A {
virtual double *foo(Rdar9188004B *next) const  {
    double *values = next->foo(0);
    try {
    }
    catch(double e) {
      values[0] = e;
    }
    return 0;
  }
};
class Rdar9188004C : public Rdar9188004B<Rdar9188004A> {
  virtual void bar(void) const;
};
void Rdar9188004C::bar(void) const {}

// Don't warn about uninitialized variables in unreachable code.
void PR9625() {
  if (false) {
    int x;
    (void)static_cast<float>(x); // no-warning
  }
}

// Don't warn about variables declared in "catch"
void RDar9251392_bar(const char *msg);

void RDar9251392() {
  try {
    throw "hi";
  }
  catch (const char* msg) {
    RDar9251392_bar(msg); // no-warning
  }
}


