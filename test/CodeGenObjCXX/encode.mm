// RUN: %clang_cc1 %s -triple=x86_64-apple-darwin10 -emit-llvm -o - | FileCheck %s

// CHECK: v17@0:8{vector<float, float, float>=}16
// CHECK: {vector<float, float, float>=}
// CHECK: v24@0:816

template <typename T1, typename T2, typename T3> struct vector {
  vector();
  vector(T1,T2,T3);
};

typedef vector< float, float, float > vector3f;

@interface SceneNode
{
 vector3f position;
}

@property (assign, nonatomic) vector3f position;

@end

@interface MyOpenGLView
{
@public
  vector3f position;
}
@property vector3f position;
@end

@implementation MyOpenGLView

@synthesize position;

-(void)awakeFromNib {
 SceneNode *sn;
 vector3f VF3(1.0, 1.0, 1.0);
 [sn setPosition:VF3];
}
@end


class Int3 { int x, y, z; };

// Enforce @encoding for member pointers.
@interface MemPtr {}
- (void) foo: (int (Int3::*)) member;
@end
@implementation MemPtr
- (void) foo: (int (Int3::*)) member {
}
@end

// rdar: // 8519948
typedef float HGVec4f __attribute__ ((vector_size(16)));

@interface RedBalloonHGXFormWrapper {
  HGVec4f m_Transform[4];
}
@end

@implementation RedBalloonHGXFormWrapper
@end

// rdar://9357400
namespace rdar9357400 {
  template<int Dim1 = -1, int Dim2 = -1> struct fixed {
      template<int D> struct rebind { typedef fixed<D> other; };
  };
  
  template<typename Element, int Size>
  class fixed_1D
  {
  public:
      typedef Element value_type;
      typedef value_type array_impl[Size];
    protected:
      array_impl                  m_data;
  };
  
  template<typename Element, typename Alloc>
  class vector;
  
  template<typename Element, int Size>
  class vector< Element, fixed<Size> >
  : public fixed_1D<Element,Size> { };

  typedef vector< float,  fixed<4> > vector4f;

  // CHECK: @_ZN11rdar9357400L2ggE = internal constant [49 x i8] c"{vector<float, rdar9357400::fixed<4, -1> >=[4f]}\00"
  const char gg[] = @encode(vector4f);
}

struct Base1 {
  char x;
};

struct DBase : public Base1 {
  double x;
  virtual ~DBase();
};

struct Sub_with_virt : virtual DBase {
  long x;
};

struct Sub2 : public Sub_with_virt, public Base1, virtual DBase {
  float x;
};

// CHECK: @_ZL2g1 = internal constant [10 x i8] c"{Base1=c}\00"
const char g1[] = @encode(Base1);

// CHECK: @_ZL2g2 = internal constant [14 x i8] c"{DBase=^^?cd}\00"
const char g2[] = @encode(DBase);

// CHECK: @_ZL2g3 = internal constant [26 x i8] c"{Sub_with_virt=^^?q^^?cd}\00"
const char g3[] = @encode(Sub_with_virt);

// CHECK: @_ZL2g4 = internal constant [19 x i8] c"{Sub2=^^?qcf^^?cd}\00"
const char g4[] = @encode(Sub2);

// http://llvm.org/PR9927
class allocator {
};
class basic_string     {
struct _Alloc_hider : allocator       {
char* _M_p;
};
_Alloc_hider _M_dataplus;
};

// CHECK: @_ZL2g5 = internal constant [32 x i8] c"{basic_string={_Alloc_hider=*}}\00"
const char g5[] = @encode(basic_string);
