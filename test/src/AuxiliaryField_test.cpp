#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/FENonlinearProblem.h"
#include "godzilla/AuxiliaryField.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "FENonlinearProblem_test.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;

namespace {

class AuxiliaryFieldTest : public FENonlinearProblemTest {
public:
};

} // namespace

TEST_F(AuxiliaryFieldTest, api)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & pars) : AuxiliaryField(pars) {}

        Int
        get_num_components() const override
        {
            return 1;
        }
        void
        evaluate(Real time, const Real x[], Scalar u[]) override
        {
        }

        UnstructuredMesh *
        get_msh() const
        {
            return AuxiliaryField::get_mesh();
        }

        Problem *
        get_prblm() const
        {
            return AuxiliaryField::get_problem();
        }
    };

    prob->set_aux_field(FieldID(0), "fld", 1, Order(1));

    auto params = AuxiliaryField::parameters();
    params.set<App *>("app", app)
        .set<std::string>("name", "aux")
        .set<std::string>("field", "fld")
        .set<std::string>("region", "rgn");
    auto aux = prob->add_auxiliary_field<TestAuxFld>(params);
    prob->create();

    EXPECT_EQ(aux->get_label(), nullptr);
    EXPECT_EQ(aux->get_region(), "rgn");
    EXPECT_EQ(aux->get_block_id(), -1);
    EXPECT_EQ(aux->get_field(), "fld");
    EXPECT_EQ(aux->get_field_id(), FieldID(0));
    // EXPECT_EQ(aux.get_msh(), this->mesh->get_mesh<Mesh>());
    EXPECT_EQ(aux->get_prblm(), prob);
    EXPECT_EQ(aux->get_dimension(), 1_D);

    EXPECT_TRUE(prob->has_aux("aux"));
    EXPECT_FALSE(prob->has_aux("no-aux"));
    EXPECT_EQ(prob->get_aux("aux"), aux);
    EXPECT_EQ(prob->get_aux("no-aux"), nullptr);
}

TEST_F(AuxiliaryFieldTest, non_existent_field)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_num_components() const override
        {
            return 2;
        }
        void
        evaluate(Real time, const Real x[], Scalar u[]) override
        {
        }
    };

    testing::internal::CaptureStderr();

    prob->set_aux_field(FieldID(0), "aux1", 1, Order(1));

    auto params = AuxiliaryField::parameters();
    params.set<App *>("app", app);
    params.set<std::string>("name", "aux");
    prob->add_auxiliary_field<TestAuxFld>(params);

    prob->create();

    EXPECT_FALSE(app->check_integrity());
    app->get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' does not exist."));
}

TEST_F(AuxiliaryFieldTest, inconsistent_comp_number)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_num_components() const override
        {
            return 2;
        }
        void
        evaluate(Real time, const Real x[], Scalar u[]) override
        {
        }
    };

    testing::internal::CaptureStderr();

    prob->set_aux_field(FieldID(0), "aux", 1, Order(1));

    auto params = AuxiliaryField::parameters();
    params.set<App *>("app", app);
    params.set<std::string>("name", "aux");
    prob->add_auxiliary_field<TestAuxFld>(params);

    prob->create();

    EXPECT_FALSE(app->check_integrity());
    app->get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Auxiliary field 'aux' has 2 component(s), but is set on a "
                                   "field with 1 component(s)."));
}

TEST_F(AuxiliaryFieldTest, non_existent_region)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_num_components() const override
        {
            return 1;
        }
        void
        evaluate(Real time, const Real x[], Scalar u[]) override
        {
        }
    };

    testing::internal::CaptureStderr();

    prob->set_aux_field(FieldID(0), "aux", 1, Order(1));

    auto params = AuxiliaryField::parameters();
    params.set<App *>("app", app)
        .set<std::string>("name", "aux")
        .set<std::string>("region", "asdf");
    prob->add_auxiliary_field<TestAuxFld>(params);

    prob->create();

    EXPECT_FALSE(app->check_integrity());
    app->get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Region 'asdf' does not exists. Typo?"));
}

TEST_F(AuxiliaryFieldTest, name_already_taken)
{
    prob->set_aux_field(FieldID(0), "fld", 1, Order(1));

    auto params = ConstantAuxiliaryField::parameters();
    params.set<App *>("app", app)
        .set<std::string>("name", "aux")
        .set<std::vector<Real>>("value", { 1 });
    prob->add_auxiliary_field<ConstantAuxiliaryField>(params);

    EXPECT_THROW_MSG(prob->add_auxiliary_field<ConstantAuxiliaryField>(params),
                     "Cannot add auxiliary object 'aux'. Name already taken.");
}

TEST_F(AuxiliaryFieldTest, get_value)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_num_components() const override
        {
            return 1;
        }
        void
        evaluate(Real time, const Real x[], Scalar u[]) override
        {
            u[0] = time * (x[0] + 1234);
        }
    };

    prob->set_aux_field(FieldID(0), "aux", 1, Order(1));

    auto params = AuxiliaryField::parameters();
    params.set<App *>("app", app)
        .set<std::string>("name", "aux")
        .set<std::string>("region", "asdf");
    auto aux = prob->add_auxiliary_field<TestAuxFld>(params);

    prob->create();

    DenseVector<Real, 1> coord({ 2. });
    auto val = aux->get_value(3., coord);
    EXPECT_DOUBLE_EQ(val, 3708.);
}

TEST_F(AuxiliaryFieldTest, get_vector_value)
{
    class TestAuxFld : public AuxiliaryField {
    public:
        explicit TestAuxFld(const Parameters & params) : AuxiliaryField(params) {}
        Int
        get_num_components() const override
        {
            return 3;
        }
        void
        evaluate(Real time, const Real x[], Scalar u[]) override
        {
            u[0] = x[0] + 2;
            u[1] = time;
            u[2] = 42.;
        }
    };

    prob->set_aux_field(FieldID(0), "aux", 1, Order(1));

    auto params = AuxiliaryField::parameters();
    params.set<App *>("app", app)
        .set<std::string>("name", "aux")
        .set<std::string>("region", "asdf");
    auto aux = prob->add_auxiliary_field<TestAuxFld>(params);

    prob->create();

    DenseVector<Real, 1> coord({ 3. });
    auto val = aux->get_vector_value<3>(2., coord);
    EXPECT_DOUBLE_EQ(val(0), 5.);
    EXPECT_DOUBLE_EQ(val(1), 2.);
    EXPECT_DOUBLE_EQ(val(2), 42.);
}
