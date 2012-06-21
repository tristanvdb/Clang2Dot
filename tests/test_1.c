
int g();

int f(int a) {
   int b = 5;
   b *= b;
   return a + b;
}

int h() {
  return f(g());
}

int g() {
  return 1;
}

