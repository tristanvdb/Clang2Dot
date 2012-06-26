
template <class T>
class A {
  protected:
    T a;

  public:
    A(T a_);
};

void f() {
  A<int> a_int(1);
}

