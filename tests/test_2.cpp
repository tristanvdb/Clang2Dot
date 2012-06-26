
class A {};

class B : public A {};

class C {
  friend class A;
};

