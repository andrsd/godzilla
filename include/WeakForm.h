#pragma once

#include "Common.h"
#include "DenseMatrix.h"
#include "Forms.h"
#include "MeshFunction1D.h"
#include "Space.h"
#include <vector>
#include <set>

namespace godzilla {

#if 0

// Bilinear form symmetry flag, see WeakForm::add_biform
enum SymFlag { ANTISYM = -1, UNSYM = 0, SYM = 1 };

/// Represents the weak formulation of a problem.
///
/// The WeakForm class represents the weak formulation of a system of linear PDEs.
/// The number of equations ("neq") in the system is fixed and is passed to the constructor.
/// The weak formulation of the system A(U,V) = L(V) has a block structure. A(U,V) is
/// a (neq x neq) matrix of bilinear forms a_mn(u,v) and L(V) is a neq-component vector
/// of linear forms l(v). U and V are the vectors of basis and test functions.
class WeakForm {
    typedef Scalar (*jacform_val_t)(uint np,
                                    Real * wt,
                                    Fn1D<Scalar> * u[],
                                    Fn1D<Real> * vi,
                                    Fn1D<Real> * vj,
                                    Geom1D * e,
                                    UserData *);
    typedef Scalar (*resform_val_t)(uint np,
                                    Real * wt,
                                    Fn1D<Scalar> * u[],
                                    Fn1D<Real> * vi,
                                    Geom1D * e,
                                    UserData *);

public:
    WeakForm();
    virtual ~WeakForm();

    void add_jacobian_block(uint i,
                            uint j,
                            jacform_val_t fn,
                            SymFlag sym = UNSYM,
                            uint area = ANY,
                            uint nx = 0,
                            ...);

    void
    add_jacobian_block_surf(uint i, uint j, jacform_val_t fn, uint area = ANY, uint nx = 0, ...);

    void add_residual_block(uint i, resform_val_t fn, uint area = ANY, uint nx = 0, ...);

    void add_residual_block_surf(uint i, resform_val_t fn, uint area = ANY, uint nx = 0, ...);

    void set_ext_fns(void * fn, uint nx, ...);

    uint get_int_order();

protected:
    /// Number of equations in the weak form
    uint neq;

    struct JacFormVol {
        uint i, j;
        SymFlag sym;
        uint area;
        /// callback for evaluating the form
        jacform_val_t fn;
        /// external functions
        std::vector<MeshFunction1D *> ext;
    };
    struct JacFormSurf {
        uint i, j;
        uint area;
        jacform_val_t fn;
        std::vector<MeshFunction1D *> ext;
    };
    struct ResFormVol {
        uint i;
        uint area;
        resform_val_t fn;
        std::vector<MeshFunction1D *> ext;
    };
    struct ResFormSurf {
        uint i;
        uint area;
        resform_val_t fn;
        std::vector<MeshFunction1D *> ext;
    };

    std::vector<JacFormVol> jfvol;
    std::vector<JacFormSurf> jfsurf;
    std::vector<ResFormVol> rfvol;
    std::vector<ResFormSurf> rfsurf;

    struct Stage {
        std::vector<int> idx;
        std::vector<Mesh *> meshes;
        std::vector<RealFunction1D *> fns;
        std::vector<MeshFunction1D *> ext;

        std::vector<JacFormVol *> jfvol;
        std::vector<JacFormSurf *> jfsurf;
        std::vector<ResFormVol *> rfvol;
        std::vector<ResFormSurf *> rfsurf;

        std::set<int> idx_set;
        std::set<unsigned> seq_set;
        std::set<MeshFunction1D *> ext_set;
    };

    /// Constructs a list of assembling stages. Each stage contains a list of forms
    /// that share the same meshes. Each stage is then assembled separately. This
    /// improves the performance of multi-mesh assembling.
    ///
    void get_stages(Space ** spaces, std::vector<Stage> & stages, bool rhsonly);

    /// @return  An (neq x neq) array containing true in each element, if the corresponding
    /// block of weak forms is used, and false otherwise.
    DenseMatrix<bool> get_blocks();

    bool
    is_sym() const
    {
        return false; /* not impl. yet */
    }

protected:
    /// Finds an assembling stage with the same set of meshes as [m1, m2, ext]. If no such
    /// stage can be found, a new one is created and returned.
    ///
    Stage * find_stage(std::vector<Stage> & stages,
                       uint ii,
                       uint jj,
                       Mesh * m1,
                       Mesh * m2,
                       std::vector<MeshFunction1D *> & ext);
};

#endif

} // namespace godzilla
