
class A {
  protected:
    void m1();
    void m2() const;

  public:
    A();
    A(int a_);

    ~A();

  private:
    int a;
    double b;
};

void A::m1() {}
void A::m2() const {}

A::A() :
  a(0),
  b(1.0)
{}

A::A(int a_) :
  a(a_),
  b(.0)
{}

A::~A() {}

