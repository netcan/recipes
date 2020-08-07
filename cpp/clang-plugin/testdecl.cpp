
class IFoo {
public:
    IFoo() = default;
    virtual ~IFoo() = default;
    virtual const void* foo(int, int, char) const noexcept = 0;
    virtual const int* foo3(int, int, char) = 0;
    virtual int foo4(int, int = 0) const= 0;
    virtual int foo5() const = 0;
};
