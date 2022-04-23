#pragma once

#include "Common.h"
#include "Quadrature1D.h"
#include "Element.h"
#include "CallStack.h"

namespace godzilla {

// Plenty of checking stuff for the debug version
#ifdef DEBUG

    #define CHECK_PARAMS                                                                           \
        if (component < 0 || component > num_components)                                           \
            error("Invalid component. You are probably using scalar-valued shapeset for an Hcurl " \
                  "problem.");                                                                     \
        if (cur_node == NULL)                                                                      \
            error("Invalid node. Did you call precalculate()?");

    #define CHECK_TABLE(n, msg)                                                                \
        if (cur_node->values[component][n] == NULL)                                            \
            error(msg " not precalculated for component %d. Did you call precalculate() with " \
                      "correct mask?",                                                         \
                  component);

#else

    #define CHECK_PARAMS
    #define CHECK_TABLE(n, msg)

#endif

template <typename TYPE>
class Function1D {
public:
    /// Only 1 component in 1D
    static const uint NUM_COMPONENTS = 1;

    static const int NUM_VALUE_TYPES = 3;

    enum EValueType { FN = 0, DX = 1, DXX = 2 };

    // Precalculation masks
    enum {
        /// Function values of the 1st component required
        FN_VAL_0 = 0x00000001,
        /// First derivative in x of the 1st component required
        FN_DX_0 = 0x00000002,
        /// Second derivative in x of the 1st component required
        FN_DXX_0 = 0x00000010,
    };

    // All components are usually requested together...
    static const uint FN_VAL = FN_VAL_0;
    static const uint FN_DX = FN_DX_0;
    static const uint FN_DXX = FN_DXX_0;

    // The most common case is FN_DEFAULT, ie. values and the gradient.
    /// Default precalculation mask
    static const uint FN_DEFAULT = FN_VAL | FN_DX;
    /// Precalculate everything
    static const uint FN_ALL = FN_DEFAULT | FN_DXX;

    // First component
    static const uint FN_COMPONENT_0 = FN_VAL_0 | FN_DX_0 | FN_DXX_0;

public:
    Function1D(uint num_components);
    virtual ~Function1D();

    /// @return The polynomial degree of the function currently being represented by the class.
    uint
    get_fn_order() const
    {
        return this->order;
    }

    /// @return The number of vector components of the function being represented by the class.
    uint
    get_num_components() const
    {
        return this->num_components;
    }

    /// Called by the assembling procedure and by other functions. In PrecalcShapeset it
    /// sets an internal variable that can be later retrieved by get_active_element().
    /// In Solution it selects the element to retrieve solution values for, etc.
    /// @param[in] element - Element associated with the function being represented by the class.
    virtual void
    set_active_element(const Element1D * element)
    {
        this->element = element;
    }

    /// @return The element associated with the function being represented by the class.
    const Element1D *
    get_active_element() const
    {
        return this->element;
    }

    /// @param component [in] The component of the function (0-2).
    /// @return The values of the function at all points of the current integration rule.
    TYPE *
    get_fn_values(uint component = 0)
    {
        CHECK_PARAMS;
        CHECK_TABLE(FN, "Function values");
        return cur_node->values[component][FN];
    }

    /// @param component [in] The component of the function (0-2).
    /// @return The x partial derivative of the function at all points of the current integration
    /// rule.
    TYPE *
    get_dx_values(uint component = 0)
    {
        CHECK_PARAMS;
        CHECK_TABLE(DX, "DX values");
        return cur_node->values[component][DX];
    }

    /// This function provides the both often-used dx and dy values in one call.
    /// @param dx [out] Variable which receives the pointer to the first partial derivatives by x
    /// @param component [in] The component of the function (0 or 1).
    void
    get_dx_values(TYPE *& dx, uint component = 0)
    {
        CHECK_PARAMS;
        CHECK_TABLE(DX, "DX values");

        dx = cur_node->values[component][DX];
    }

    /// For internal use.
    TYPE *
    get_values(uint component, uint b)
    {
        CHECK_PARAMS;
        return cur_node->values[component][b];
    }

    /// Frees all precalculated tables.
    virtual void free() = 0;

    /// precalculates the current function at the current integration points.
    virtual void precalculate(const uint np, const QPoint1D * pt, uint mask) = 0;

protected:
    static const uint QUAD_COUNT = 8;

    /// Active element
    const Element1D * element;
    /// current function polynomial order
    uint order;
    /// number of vector components
    uint num_components;

    struct Node {
        /// a combination of FN_XXX: specifies which tables are present
        uint mask;
        /// size in bytes of this struct (for maintaining total_mem)
        uint size;
        /// pointers to 'data'
        TYPE * values[NUM_COMPONENTS][NUM_VALUE_TYPES];
        /// value tables
        TYPE data[0];
    };

    Node * cur_node;

    /// list of available quadratures
    Quadrature1D * quads[QUAD_COUNT];
    /// active quadrature (index into 'quads')
    uint cur_quad;

    /// allocates a new Node structure
    virtual Node * new_node(uint mask, uint num_points);

    void free_cur_node();

    void replace_cur_node(Node * node);

    /// index to mask table
    static uint idx2mask[][NUM_COMPONENTS];
};

template <typename TYPE>
Function1D<TYPE>::Function1D(uint num_components)
{
    _F_;
    this->order = 0;
    this->num_components = num_components;
    this->cur_node = NULL;
    memset(this->quads, 0, sizeof(this->quads));
    this->cur_quad = 0;
    assert(this->num_components == 1);
}

template <typename TYPE>
Function1D<TYPE>::~Function1D()
{
    _F_;
}

template <typename TYPE>
typename Function1D<TYPE>::Node *
Function1D<TYPE>::new_node(uint mask, uint num_points)
{
    _F_;
    // get the number of tables
    uint nt = 0, m = mask;
    if (this->num_components < 3)
        m &= FN_VAL_0 | FN_DX_0;
    while (m) {
        nt += m & 1;
        m >>= 1;
    }

    // allocate a node including its data part, init table pointers
    uint size = sizeof(Node) + sizeof(TYPE) * num_points * nt;
    Node * node = (Node *) malloc(size);
    node->mask = mask;
    node->size = size;
    memset(node->values, 0, sizeof(node->values));
    TYPE * data = node->data;
    for (uint j = 0; j < this->num_components; j++) {
        for (uint i = 0; i < NUM_VALUE_TYPES; i++)
            if (mask & idx2mask[i][j]) {
                node->values[j][i] = data;
                data += num_points;
            }
    }

    return node;
}

template <typename TYPE>
void
Function1D<TYPE>::free_cur_node()
{
    if (cur_node != NULL) {
        ::free(cur_node);
        cur_node = NULL;
    }
}

template <typename TYPE>
void
Function1D<TYPE>::replace_cur_node(typename Function1D<TYPE>::Node * node)
{
    free_cur_node();
    cur_node = node;
}

template <typename TYPE>
uint Function1D<TYPE>::idx2mask[][NUM_COMPONENTS] = { { FN_VAL_0 }, { FN_DX_0 }, { FN_DXX_0 } };

typedef Function1D<Real> RealFunction1D;

typedef Function1D<Scalar> ScalarFunction1D;

} // namespace godzilla
