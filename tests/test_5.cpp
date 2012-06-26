
namespace nA {
  class cA {
    public:
      cA();
      ~cA();
  };

  template <typename tA, class tB, unsigned tpl_val>
  class cB {
    protected:
      tA v1;
      tB arr[tpl_val];

    public:
      cB(tA v1_);
      ~cB();
  };
}

template <unsigned tpl_val>
class cC : public nA::cA, protected nA::cB<double, nA::cA, tpl_val> {
  protected:
    
  public:
    cC(double val);
    ~cC();
};

