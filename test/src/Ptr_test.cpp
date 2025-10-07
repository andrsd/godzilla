#include "gmock/gmock.h"
#include "godzilla/Ptr.h"
#include "godzilla/Types.h"

using namespace godzilla;

namespace {

struct Data {
    virtual ~Data() = default;

    int i;
    double d;

    Data(int j, double e) : i(j), d(e) {}
};

struct BigData : public Data {
    float f;

    BigData(float f, double d, int i) : Data(i, d), f(f) {}
};

struct SmallData : public Data {
    SmallData(double d, int i) : Data(i, d) {}

    void
    fn1()
    {
    }
};

Ptr<double>
alloc_double(double d)
{
    return Ptr<double>::alloc(d);
}

std::shared_ptr<double>
alloc_double2(double d)
{
    return std::make_shared<double>(d);
}

Ptr<Data>
alloc_small_data_with_base()
{
    return Ptr<SmallData>::alloc(0.1, 1);
}

class Evaluator {
private:
    struct AbstractEvalr {
        virtual ~AbstractEvalr() = default;
        virtual Real evaluate(Real time, Real x) = 0;
    };

    template <typename KLASS>
    struct Evalr : public AbstractEvalr {
        KLASS obj_;

        explicit Evalr(KLASS obj) : obj_(obj) {}

        Real
        evaluate(Real time, Real x) override
        {
            return this->obj_.evaluate(time, x);
        }
    };

    Ptr<AbstractEvalr> impl_;

public:
    Evaluator() = default;

    template <typename KLASS>
    explicit Evaluator(KLASS obj) : impl_(Ptr<Evalr<KLASS>>::alloc(obj))
    {
    }

    operator bool() const { return this->impl_ != nullptr; }

    Real
    evaluate(Real time, Real x)
    {
        assert(this->impl_ != nullptr);
        return this->impl_->evaluate(time, x);
    }
};

} // namespace

TEST(PtrTest, default_ctor)
{
    Ptr<Data> p;
    EXPECT_TRUE(p == nullptr);
}

TEST(PtrTest, nullptr_ctor)
{
    Ptr<Data> p(nullptr);
    EXPECT_TRUE(p == nullptr);
}

TEST(PtrTest, converting_ctor)
{
    auto q = Ptr<BigData>::alloc(12.3, 345.67, 89);
    // Ptr<Data> p = q;
    // ASSERT_TRUE(p != nullptr);
    // EXPECT_EQ(p->i, 89);
    // EXPECT_DOUBLE_EQ(p->d, 345.67);
    // if constexpr (std::is_convertible_v<BigData *, Data *>)
    //     std::cerr << "a = yes" << std::endl;
    // else
    //     std::cerr << "a = no" << std::endl;
}

TEST(PtrTest, converting_copy_ctor)
{
    auto p = alloc_small_data_with_base();
    ASSERT_TRUE(p != nullptr);
    EXPECT_EQ(p->i, 1);
    EXPECT_DOUBLE_EQ(p->d, 0.1);
}

TEST(PtrTest, alloc)
{
    auto q = Ptr<int>::alloc(1);
    EXPECT_FALSE(q.is_null());
    EXPECT_EQ(*q, 1);
}

TEST(PtrTest, alloc_by_function)
{
    auto q = alloc_double(122.);
    EXPECT_EQ(q.ref_count(), 1);
}

TEST(PtrTest, assign_op)
{
    auto q = Ptr<int>::alloc(1);
    auto r = Ptr<int>::alloc(2);
    q = r;
    EXPECT_FALSE(q.is_null());
    EXPECT_FALSE(r.is_null());
    EXPECT_EQ(q.ref_count(), 2);
    EXPECT_EQ(r.ref_count(), 2);
}

TEST(PtrTest, assign_op_nullptr)
{
    auto q = Ptr<int>::alloc(1);
    q = nullptr;
    EXPECT_TRUE(q == nullptr);
}

TEST(PtrTest, coverting_assign_op)
{
    Ptr<Data> p;
    auto q = Ptr<SmallData>::alloc(0.5, 20);
    p = q;
    ASSERT_TRUE(p != nullptr);
    EXPECT_EQ(p->i, 20);
    EXPECT_DOUBLE_EQ(p->d, 0.5);
}

TEST(PtrTest, star_op)
{
    auto q = alloc_double(122.);
    EXPECT_DOUBLE_EQ(*q, 122.);
}

TEST(PtrTest, arrow_op)
{
    auto q = Ptr<Data>::alloc(13, 987.65);
    ASSERT_TRUE(q);
    EXPECT_EQ(q->i, 13);
    EXPECT_DOUBLE_EQ(q->d, 987.65);
}

TEST(PtrTest, equal_op)
{
    auto q = alloc_double(122.);
    auto r = q;
    EXPECT_TRUE(q == r);

    Ptr<double> n;
    EXPECT_TRUE(n == nullptr);
}

TEST(PtrTest, not_equal_op)
{
    auto q = alloc_double(122.);
    auto r = alloc_double(122.);
    EXPECT_TRUE(q != r);
}

TEST(PtrTest, is_null)
{
    auto q = alloc_double(122.);
    EXPECT_FALSE(q.is_null());

    Ptr<double> n;
    EXPECT_TRUE(n.is_null());
}

TEST(PtrTest, downcast_to_child_succeeds)
{
    auto q = Ptr<BigData>::alloc(12.3, 345.67, 89);
    Ptr<Data> p = q;
    auto r = dynamic_ptr_cast<BigData>(p);
    ASSERT_TRUE(r != nullptr);
    EXPECT_NEAR(r->f, 12.3, 1e-8);
    EXPECT_DOUBLE_EQ(r->d, 345.67);
    EXPECT_EQ(r->i, 89);
}

TEST(PtrTest, downcast_to_child_is_nullptr)
{
    auto q = Ptr<SmallData>::alloc(345.67, 89);
    Ptr<Data> p = q;
    auto r = dynamic_ptr_cast<BigData>(p);
    ASSERT_TRUE(r == nullptr);
}

TEST(PtrTest, assign_to_base_type)
{
    // Ptr<Data> q = Ptr<BigData>::alloc(12.3, 345.67, 89);
    // std::cerr << "q.get = " << q.get() << std::endl;

    auto q = Ptr<BigData>::alloc(12.3, 345.67, 89);
    Ptr<Data> p = q;
    auto r = dynamic_ptr_cast<BigData>(p);
}

TEST(PtrTest, type_erasure)
{
    class TestE {
    public:
        Real
        evaluate(Real time, Real x)
        {
            return time * x;
        }
    };


    TestE t;
    Evaluator e(t);
    // auto v = e.evaluate(2., 5.);

}
