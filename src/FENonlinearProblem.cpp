#include "FENonlinearProblem.h"
#include "CallStack.h"
#include "UnstructuredMesh.h"
#include "IndexSet.h"
#include "WeakForm.h"
#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "petscdm.h"
#include <petscds.h>

namespace godzilla {

namespace internal {

void
__dummy_jacobian_func(PetscInt,
                      PetscInt,
                      PetscInt,
                      const PetscInt[],
                      const PetscInt[],
                      const PetscScalar[],
                      const PetscScalar[],
                      const PetscScalar[],
                      const PetscInt[],
                      const PetscInt[],
                      const PetscScalar[],
                      const PetscScalar[],
                      const PetscScalar[],
                      PetscReal,
                      PetscReal,
                      const PetscReal[],
                      PetscInt,
                      const PetscScalar[],
                      PetscScalar[])
{
}

} // namespace internal

static PetscErrorCode
__fep_compute_residual(DM, Vec x, Vec F, void * user)
{
    _F_;
    auto * fep = static_cast<FENonlinearProblem *>(user);
    fep->compute_residual(x, F);
    return 0;
}

static PetscErrorCode
__fep_compute_jacobian(DM, Vec x, Mat J, Mat Jp, void * user)
{
    _F_;
    auto * fep = static_cast<FENonlinearProblem *>(user);
    fep->compute_jacobian(x, J, Jp);
    return 0;
}

static PetscErrorCode
PetscContainerUserDestroy_PetscFEGeom(void * ctx)
{
    PetscFEGeom * geom = (PetscFEGeom *) ctx;

    PetscFunctionBegin;
    PetscCall(PetscFEGeomDestroy(&geom));
    PetscFunctionReturn(0);
}

static PetscErrorCode
DMSNESGetFEGeom(DMField coord_field,
                IS point_is,
                PetscQuadrature quad,
                PetscBool face_data,
                PetscFEGeom ** geom)
{
    char compose_str[33] = { 0 };
    PetscObjectId id;
    PetscContainer container;

    PetscFunctionBegin;
    PetscCall(PetscObjectGetId((PetscObject) quad, &id));
    PetscCall(PetscSNPrintf(compose_str, 32, "DMSNESGetFEGeom_%x\n", id));
    PetscCall(PetscObjectQuery((PetscObject) point_is, compose_str, (PetscObject *) &container));
    if (container) {
        PetscCall(PetscContainerGetPointer(container, (void **) geom));
    }
    else {
        PetscCall(DMFieldCreateFEGeom(coord_field, point_is, quad, face_data, geom));
        PetscCall(PetscContainerCreate(PETSC_COMM_SELF, &container));
        PetscCall(PetscContainerSetPointer(container, (void *) *geom));
        PetscCall(PetscContainerSetUserDestroy(container, PetscContainerUserDestroy_PetscFEGeom));
        PetscCall(PetscObjectCompose((PetscObject) point_is, compose_str, (PetscObject) container));
        PetscCall(PetscContainerDestroy(&container));
    }
    PetscFunctionReturn(0);
}

static PetscErrorCode
DMSNESRestoreFEGeom(DMField /*coord_field*/,
                    IS /*point_is*/,
                    PetscQuadrature /*quad*/,
                    PetscBool /*face_data*/,
                    PetscFEGeom ** geom)
{
    PetscFunctionBegin;
    *geom = nullptr;
    PetscFunctionReturn(0);
}

///

Parameters
FENonlinearProblem::parameters()
{
    Parameters params = NonlinearProblem::parameters();
    return params;
}

FENonlinearProblem::FENonlinearProblem(const Parameters & parameters) :
    NonlinearProblem(parameters),
    FEProblemInterface(this, parameters)
{
    _F_;
}

void
FENonlinearProblem::create()
{
    _F_;
    FEProblemInterface::create();
    NonlinearProblem::create();
}

void
FENonlinearProblem::init()
{
    _F_;
    NonlinearProblem::init();
    FEProblemInterface::init();
}

void
FENonlinearProblem::set_up_callbacks()
{
    _F_;
    DM dm = this->get_dm();
    PETSC_CHECK(DMPlexSetSNESLocalFEM(dm, this, this, this));
    PETSC_CHECK(DMSNESSetFunctionLocal(dm, __fep_compute_residual, this));
    PETSC_CHECK(DMSNESSetJacobianLocal(dm, __fep_compute_jacobian, this));
    PETSC_CHECK(SNESSetJacobian(this->snes, this->J, this->J, nullptr, nullptr));
}

void
FENonlinearProblem::set_up_initial_guess()
{
    _F_;
    lprintf(9, "Setting initial guess");
    FEProblemInterface::set_up_initial_guess();
}

void
FENonlinearProblem::allocate_objects()
{
    NonlinearProblem::allocate_objects();
    FEProblemInterface::allocate_objects();
}

PetscErrorCode
FENonlinearProblem::compute_residual(Vec x, Vec f)
{
    _F_;
    // this is based on DMSNESComputeResidual()
    DM plex = get_dm();
    IndexSet all_cells = this->unstr_mesh->get_all_elements();

    PetscInt n_ds;
    PETSC_CHECK(DMGetNumDS(plex, &n_ds));
    for (PetscInt s = 0; s < n_ds; ++s) {
        PetscDS ds;
        DMLabel label;
        PETSC_CHECK(DMGetRegionNumDS(plex, s, &label, nullptr, &ds));

        for (auto & res_key : this->wf->get_residual_keys()) {
            IndexSet cells;
            if (res_key.label == nullptr) {
                all_cells.inc_ref();
                cells = all_cells;
            }
            else {
                IndexSet points = IndexSet::stratum_from_label(res_key.label, res_key.value);
                cells = IndexSet::intersect_caching(all_cells, points);
                points.destroy();
            }
            compute_residual_internal(plex, res_key, cells, PETSC_MIN_REAL, x, nullptr, 0.0, f);
            cells.destroy();
        }
    }
    all_cells.destroy();
    return 0;
}

PetscErrorCode
FENonlinearProblem::compute_residual_internal(DM dm,
                                              PetscFormKey key,
                                              const IndexSet & cell_is,
                                              PetscReal time,
                                              Vec loc_x,
                                              Vec loc_x_t,
                                              PetscReal t,
                                              Vec loc_f)
{
    _F_;
    DM dm_aux = nullptr;
    PetscDS ds_aux = nullptr;
    PetscBool is_implicit = (loc_x_t || time == PETSC_MIN_REAL) ? PETSC_TRUE : PETSC_FALSE;
    PetscScalar *u = nullptr, *u_t, *a;
    IS chunk_is;
    PetscInt tot_dim_aux;
    PetscQuadrature affine_quad = nullptr, *quads = nullptr;
    PetscFEGeom *affine_geom = nullptr, **geoms = nullptr;

    /// TODO The places where we have to use isFE are probably the member functions for the
    /// PetscDisc class
    PetscInt c_start, c_end;
    const PetscInt * cells;
    PetscCall(ISGetPointRange((IS) cell_is, &c_start, &c_end, &cells));
    PetscInt f_start, f_end;
    PetscCall(DMPlexGetHeightStratum(dm, 1, &f_start, &f_end));
    /* 1: Get sizes from dm and dm_aux */
    PetscSection section = nullptr;
    PetscCall(DMGetLocalSection(dm, &section));
    DMLabel ghost_label = nullptr;
    PetscCall(DMGetLabel(dm, "ghost", &ghost_label));
    PetscDS ds = nullptr;
    PetscCall(DMGetCellDS(dm, cells ? cells[c_start] : c_start, &ds));
    PetscInt n_fields;
    PetscCall(PetscDSGetNumFields(ds, &n_fields));
    PetscInt tot_dim;
    PetscCall(PetscDSGetTotalDimension(ds, &tot_dim));
    Vec loc_a;
    PetscCall(DMGetAuxiliaryVec(dm, key.label, key.value, key.part, &loc_a));
    if (loc_a) {
        PetscInt subcell;
        PetscCall(VecGetDM(loc_a, &dm_aux));
        PetscCall(DMGetEnclosurePoint(dm_aux, dm, DM_ENC_UNKNOWN, c_start, &subcell));
        PetscCall(DMGetCellDS(dm_aux, subcell, &ds_aux));
        PetscCall(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));
    }
    /* 2: Get geometric data */
    for (PetscInt f = 0; f < n_fields; ++f) {
        PetscBool fimp;
        PetscCall(PetscDSGetImplicit(ds, f, &fimp));
        if (is_implicit != fimp)
            continue;
    }
    DMField coord_field = nullptr;
    PetscCall(DMGetCoordinateField(dm, &coord_field));
    PetscInt max_degree = PETSC_MAX_INT;
    PetscCall(DMFieldGetDegree(coord_field, (IS) cell_is, nullptr, &max_degree));
    if (max_degree <= 1) {
        PetscCall(DMFieldCreateDefaultQuadrature(coord_field, (IS) cell_is, &affine_quad));
        if (affine_quad) {
            PetscCall(
                DMSNESGetFEGeom(coord_field, (IS) cell_is, affine_quad, PETSC_FALSE, &affine_geom));
        }
    }
    else {
        PetscCall(PetscCalloc2(n_fields, &quads, n_fields, &geoms));
        for (PetscInt f = 0; f < n_fields; ++f) {
            PetscBool fimp;
            PetscCall(PetscDSGetImplicit(ds, f, &fimp));
            if (is_implicit != fimp)
                continue;
            PetscObject obj;
            PetscCall(PetscDSGetDiscretization(ds, f, &obj));
            PetscClassId id;
            PetscCall(PetscObjectGetClassId(obj, &id));
            if (id == PETSCFE_CLASSID) {
                PetscFE fe = (PetscFE) obj;
                PetscCall(PetscFEGetQuadrature(fe, &quads[f]));
                PetscCall(PetscObjectReference((PetscObject) quads[f]));
                PetscCall(
                    DMSNESGetFEGeom(coord_field, (IS) cell_is, quads[f], PETSC_FALSE, &geoms[f]));
            }
        }
    }
    /* Loop over chunks */
    PetscCall(ISCreate(PETSC_COMM_SELF, &chunk_is));
    PetscInt n_cells = c_end - c_start;
    PetscInt n_chunks = 1;
    PetscInt cell_chunk_size = n_cells / n_chunks;
    n_chunks = PetscMin(1, n_cells);
    for (PetscInt chunk = 0; chunk < n_chunks; ++chunk) {
        PetscScalar * elem_vec;
        PetscInt cS = c_start + chunk * cell_chunk_size;
        PetscInt cE = PetscMin(cS + cell_chunk_size, c_end);
        PetscInt n_chunk_cells = cE - cS;

        /* Extract field coefficients */
        PetscCall(ISGetPointSubrange(chunk_is, cS, cE, cells));
        PetscCall(DMPlexGetCellFields(dm, chunk_is, loc_x, loc_x_t, loc_a, &u, &u_t, &a));
        PetscCall(DMGetWorkArray(dm, n_chunk_cells * tot_dim, MPIU_SCALAR, &elem_vec));
        PetscCall(PetscArrayzero(elem_vec, n_chunk_cells * tot_dim));
        /* Loop over fields */
        for (PetscInt f = 0; f < n_fields; ++f) {
            PetscBool fimp;
            PetscCall(PetscDSGetImplicit(ds, f, &fimp));
            if (is_implicit != fimp)
                continue;

            PetscObject obj;
            PetscCall(PetscDSGetDiscretization(ds, f, &obj));
            PetscClassId id;
            PetscCall(PetscObjectGetClassId(obj, &id));
            if (id == PETSCFE_CLASSID) {
                key.field = f;

                PetscFE fe = (PetscFE) obj;
                PetscFEGeom * geom = affine_geom ? affine_geom : geoms[f];
                PetscQuadrature quad = affine_quad ? affine_quad : quads[f];

                PetscInt n_batches, n_blocks;
                PetscCall(PetscFEGetTileSizes(fe, nullptr, &n_blocks, nullptr, &n_batches));
                PetscInt n_qpts;
                PetscCall(
                    PetscQuadratureGetData(quad, nullptr, nullptr, &n_qpts, nullptr, nullptr));
                PetscInt n_basis;
                PetscCall(PetscFEGetDimension(fe, &n_basis));
                PetscInt block_size = n_basis;
                PetscInt batch_size = n_blocks * block_size;
                PetscCall(PetscFESetTileSizes(fe, block_size, n_blocks, batch_size, n_batches));
                PetscInt n_chunks_field = n_chunk_cells / (n_batches * batch_size);
                PetscInt n_elems = n_chunks_field * n_batches * batch_size;
                PetscInt n_remdr = n_chunk_cells % (n_batches * batch_size);
                PetscInt offset = n_chunk_cells - n_remdr;
                /* Integrate FE residual to get elemVec (need fields at quadrature points) */
                PetscFEGeom * chunk_geom = nullptr;
                PetscCall(PetscFEGeomGetChunk(geom, 0, offset, &chunk_geom));
                PetscCall(PetscFEIntegrateResidual(ds,
                                                   key,
                                                   n_elems,
                                                   chunk_geom,
                                                   u,
                                                   u_t,
                                                   ds_aux,
                                                   a,
                                                   t,
                                                   elem_vec));
                PetscCall(PetscFEGeomGetChunk(geom, offset, n_chunk_cells, &chunk_geom));
                PetscCall(PetscFEIntegrateResidual(ds,
                                                   key,
                                                   n_remdr,
                                                   chunk_geom,
                                                   &u[offset * tot_dim],
                                                   u_t ? &u_t[offset * tot_dim] : nullptr,
                                                   ds_aux,
                                                   &a[offset * tot_dim_aux],
                                                   t,
                                                   &elem_vec[offset * tot_dim]));
                PetscCall(PetscFEGeomRestoreChunk(geom, offset, n_chunk_cells, &chunk_geom));
            }
            else
                SETERRQ(PetscObjectComm((PetscObject) dm),
                        PETSC_ERR_ARG_WRONG,
                        "Unknown discretization type for field %D",
                        f);
        }
        /* Loop over domain */
        /* Add elemVec to locX */
        for (PetscInt c = cS; c < cE; ++c) {
            const PetscInt cell = cells ? cells[c] : c;
            const PetscInt cind = c - c_start;

            if (ghost_label) {
                PetscInt ghost_val;
                PetscCall(DMLabelGetValue(ghost_label, cell, &ghost_val));
                if (ghost_val > 0)
                    continue;
            }
            PetscCall(DMPlexVecSetClosure(dm,
                                          section,
                                          loc_f,
                                          cell,
                                          &elem_vec[cind * tot_dim],
                                          ADD_ALL_VALUES));
        }

        PetscCall(DMPlexRestoreCellFields(dm, chunk_is, loc_x, loc_x_t, loc_a, &u, &u_t, &a));
        PetscCall(DMRestoreWorkArray(dm, n_chunk_cells * tot_dim, MPIU_SCALAR, &elem_vec));
    }
    PetscCall(ISDestroy(&chunk_is));
    PetscCall(ISRestorePointRange((IS) cell_is, &c_start, &c_end, &cells));

    PetscCall(compute_bnd_residual_internal(dm, loc_x, loc_x_t, t, loc_f));

    if (max_degree <= 1) {
        PetscCall(
            DMSNESRestoreFEGeom(coord_field, (IS) cell_is, affine_quad, PETSC_FALSE, &affine_geom));
        PetscCall(PetscQuadratureDestroy(&affine_quad));
    }
    else {
        for (PetscInt f = 0; f < n_fields; ++f) {
            PetscCall(
                DMSNESRestoreFEGeom(coord_field, (IS) cell_is, quads[f], PETSC_FALSE, &geoms[f]));
            PetscCall(PetscQuadratureDestroy(&quads[f]));
        }
        PetscCall(PetscFree2(quads, geoms));
    }

    /* FEM */
    /* 1: Get sizes from dm and dmAux */
    /* 2: Get geometric data */
    /* 3: Handle boundary values */
    /* 4: Loop over domain */
    /*   Extract coefficients */
    /* Loop over fields */
    /*   Set tiling for FE*/
    /*   Integrate FE residual to get elemVec */
    /*     Loop over subdomain */
    /*       Loop over quad points */
    /*         Transform coords to real space */
    /*         Evaluate field and aux fields at point */
    /*         Evaluate residual at point */
    /*         Transform residual to real space */
    /*       Add residual to elemVec */
    /* Loop over domain */
    /*   Add elemVec to locX */

    return 0;
}

PetscErrorCode
FENonlinearProblem::compute_bnd_residual_internal(DM dm,
                                                  Vec loc_x,
                                                  Vec loc_x_t,
                                                  PetscReal t,
                                                  Vec loc_f)
{
    _F_;

    PetscDS prob;
    PetscCall(DMGetDS(dm, &prob));
    DMLabel depth_label;
    PetscCall(DMPlexGetDepthLabel(dm, &depth_label));
    PetscInt dim;
    PetscCall(DMGetDimension(dm, &dim));
    IndexSet facets = IndexSet::stratum_from_label(depth_label, dim - 1);
    PetscInt n_bnd;
    PetscCall(PetscDSGetNumBoundary(prob, &n_bnd));
    for (PetscInt bd = 0; bd < n_bnd; ++bd) {
        DMBoundaryConditionType type;
        DMLabel label;
        const PetscInt * values;
        PetscInt field, n_values;
        PetscFormKey key;

        PetscCall(PetscDSGetBoundary(prob,
                                     bd,
                                     nullptr,
                                     &type,
                                     nullptr,
                                     &label,
                                     &n_values,
                                     &values,
                                     &field,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr));

        PetscObject obj;
        PetscCall(PetscDSGetDiscretization(prob, field, &obj));
        PetscClassId id;
        PetscCall(PetscObjectGetClassId(obj, &id));
        if ((id != PETSCFE_CLASSID) || (type & DM_BC_ESSENTIAL))
            continue;
        DMField coord_field = nullptr;
        PetscCall(DMGetCoordinateField(dm, &coord_field));
        for (PetscInt v = 0; v < n_values; ++v) {
            key.label = label;
            key.value = values[v];
            key.field = field;
            key.part = 0;
            PetscCall(compute_bnd_residual_single_internal(dm,
                                                           t,
                                                           key,
                                                           loc_x,
                                                           loc_x_t,
                                                           loc_f,
                                                           coord_field,
                                                           facets));
        }
    }
    facets.destroy();
    return 0;
}

PetscErrorCode
FENonlinearProblem::compute_bnd_residual_single_internal(DM dm,
                                                         PetscReal t,
                                                         PetscFormKey key,
                                                         Vec loc_x,
                                                         Vec loc_x_t,
                                                         Vec loc_f,
                                                         DMField coord_field,
                                                         const IndexSet & facets)
{
    DM plex = nullptr, plex_aux = nullptr;
    DMEnclosureType enc_aux;
    PetscDS prob, prob_aux = nullptr;
    PetscSection section, section_aux = nullptr;
    Vec loc_a = nullptr;
    PetscScalar *u = nullptr, *u_t = nullptr, *a = nullptr, *elem_vec = nullptr;
    PetscInt tot_dim, tot_dim_aux = 0;

    PetscCall(DMConvert(dm, DMPLEX, &plex));
    PetscCall(DMGetLocalSection(dm, &section));
    PetscCall(DMGetDS(dm, &prob));
    PetscCall(PetscDSGetTotalDimension(prob, &tot_dim));
    PetscCall(DMGetAuxiliaryVec(dm, key.label, key.value, key.part, &loc_a));
    if (loc_a) {
        DM dm_aux;
        PetscCall(VecGetDM(loc_a, &dm_aux));
        PetscCall(DMGetEnclosureRelation(dm_aux, dm, &enc_aux));
        PetscCall(DMConvert(dm_aux, DMPLEX, &plex_aux));
        PetscCall(DMGetDS(plex_aux, &prob_aux));
        PetscCall(PetscDSGetTotalDimension(prob_aux, &tot_dim_aux));
        PetscCall(DMGetLocalSection(plex_aux, &section_aux));
    }

    IndexSet points = IndexSet::stratum_from_label(key.label, key.value);
    if (!points.empty()) {
        PetscQuadrature q_geom = nullptr;

        /* TODO: Special cases of ISIntersect where it is quick to check a priori if one is a
         * superset of the other */
        IndexSet isect = IndexSet::intersect_caching(facets, points);
        points.destroy();
        points = isect;

        PetscInt n_faces = points.get_local_size();
        points.get_indices();

        PetscCall(PetscMalloc4(n_faces * tot_dim,
                               &u,
                               loc_x_t ? n_faces * tot_dim : 0,
                               &u_t,
                               n_faces * tot_dim,
                               &elem_vec,
                               loc_a ? n_faces * tot_dim_aux : 0,
                               &a));
        PetscInt max_degree;
        PetscCall(DMFieldGetDegree(coord_field, (IS) points, nullptr, &max_degree));
        if (max_degree <= 1) {
            PetscCall(DMFieldCreateDefaultQuadrature(coord_field, (IS) points, &q_geom));
        }
        if (!q_geom) {
            PetscFE fe;
            PetscCall(PetscDSGetDiscretization(prob, key.field, (PetscObject *) &fe));
            PetscCall(PetscFEGetFaceQuadrature(fe, &q_geom));
            PetscCall(PetscObjectReference((PetscObject) q_geom));
        }
        PetscInt n_qpts;
        PetscCall(PetscQuadratureGetData(q_geom, nullptr, nullptr, &n_qpts, nullptr, nullptr));
        PetscFEGeom * fgeom;
        PetscCall(DMSNESGetFEGeom(coord_field, (IS) points, q_geom, PETSC_TRUE, &fgeom));
        for (PetscInt face = 0; face < n_faces; ++face) {
            const PetscInt * support;
            PetscCall(DMPlexGetSupport(dm, points[face], &support));
            PetscScalar * x = nullptr;
            PetscCall(DMPlexVecGetClosure(plex, section, loc_x, support[0], nullptr, &x));
            for (PetscInt i = 0; i < tot_dim; ++i)
                u[face * tot_dim + i] = x[i];
            PetscCall(DMPlexVecRestoreClosure(plex, section, loc_x, support[0], nullptr, &x));
            if (loc_x_t) {
                PetscCall(DMPlexVecGetClosure(plex, section, loc_x_t, support[0], nullptr, &x));
                for (PetscInt i = 0; i < tot_dim; ++i)
                    u_t[face * tot_dim + i] = x[i];
                PetscCall(DMPlexVecRestoreClosure(plex, section, loc_x_t, support[0], nullptr, &x));
            }
            if (loc_a) {
                PetscInt subp;
                PetscCall(DMGetEnclosurePoint(plex_aux, dm, enc_aux, support[0], &subp));
                PetscCall(DMPlexVecGetClosure(plex_aux, section_aux, loc_a, subp, nullptr, &x));
                for (PetscInt i = 0; i < tot_dim_aux; ++i)
                    a[face * tot_dim_aux + i] = x[i];
                PetscCall(DMPlexVecRestoreClosure(plex_aux, section_aux, loc_a, subp, nullptr, &x));
            }
        }
        PetscCall(PetscArrayzero(elem_vec, n_faces * tot_dim));

        PetscFE fe;
        PetscCall(PetscDSGetDiscretization(prob, key.field, (PetscObject *) &fe));
        PetscInt n_basis;
        PetscCall(PetscFEGetDimension(fe, &n_basis));
        /* Conforming batches */
        PetscInt n_batches, n_blocks;
        PetscCall(PetscFEGetTileSizes(fe, nullptr, &n_blocks, nullptr, &n_batches));
        /* TODO: documentation is unclear about what is going on with these numbers: how should Nb /
         * Nq factor in ? */
        PetscInt block_size = n_basis;
        PetscInt batch_size = n_blocks * block_size;
        PetscCall(PetscFESetTileSizes(fe, block_size, n_blocks, batch_size, n_batches));
        PetscInt n_chunks = n_faces / (n_batches * batch_size);
        PetscInt n_elems = n_chunks * n_batches * batch_size;
        /* Remainder */
        PetscInt n_remdr = n_faces % (n_batches * batch_size);
        PetscInt offset = n_faces - n_remdr;
        PetscFEGeom * chunk_geom = nullptr;
        PetscCall(PetscFEGeomGetChunk(fgeom, 0, offset, &chunk_geom));
        PetscCall(PetscFEIntegrateBdResidual(prob,
                                             nullptr,
                                             key,
                                             n_elems,
                                             chunk_geom,
                                             u,
                                             u_t,
                                             prob_aux,
                                             a,
                                             t,
                                             elem_vec));
        PetscCall(PetscFEGeomRestoreChunk(fgeom, 0, offset, &chunk_geom));
        PetscCall(PetscFEGeomGetChunk(fgeom, offset, n_faces, &chunk_geom));
        PetscCall(PetscFEIntegrateBdResidual(prob,
                                             nullptr,
                                             key,
                                             n_remdr,
                                             chunk_geom,
                                             &u[offset * tot_dim],
                                             u_t ? &u_t[offset * tot_dim] : nullptr,
                                             prob_aux,
                                             a ? &a[offset * tot_dim_aux] : nullptr,
                                             t,
                                             &elem_vec[offset * tot_dim]));
        PetscCall(PetscFEGeomRestoreChunk(fgeom, offset, n_faces, &chunk_geom));

        for (PetscInt face = 0; face < n_faces; ++face) {
            const PetscInt * support;
            PetscCall(DMPlexGetSupport(plex, points[face], &support));
            PetscCall(DMPlexVecSetClosure(plex,
                                          nullptr,
                                          loc_f,
                                          support[0],
                                          &elem_vec[face * tot_dim],
                                          ADD_ALL_VALUES));
        }
        PetscCall(DMSNESRestoreFEGeom(coord_field, (IS) points, q_geom, PETSC_TRUE, &fgeom));
        PetscCall(PetscQuadratureDestroy(&q_geom));
        points.restore_indices();
        points.destroy();
        PetscCall(PetscFree4(u, u_t, elem_vec, a));
    }

    PetscCall(DMDestroy(&plex));
    PetscCall(DMDestroy(&plex_aux));
    return 0;
}

PetscErrorCode
FENonlinearProblem::compute_jacobian(Vec x, Mat J, Mat Jp)
{
    _F_;
    return DMPlexSNESComputeJacobianFEM(get_dm(), x, J, Jp, this);
}

void
FENonlinearProblem::set_residual_block(PetscInt field_id,
                                       ResidualFunc * f0,
                                       ResidualFunc * f1,
                                       DMLabel label,
                                       PetscInt val)
{
    _F_;
    this->wf->add(PETSC_WF_F0, label, val, field_id, 0, f0);
    this->wf->add(PETSC_WF_F1, label, val, field_id, 0, f1);
}

void
FENonlinearProblem::set_jacobian_block(PetscInt fid,
                                       PetscInt gid,
                                       JacobianFunc * g0,
                                       JacobianFunc * g1,
                                       JacobianFunc * g2,
                                       JacobianFunc * g3)
{
    _F_;
    this->wf->add(PETSC_WF_G0, nullptr, 0, fid, gid, 0, g0);
    this->wf->add(PETSC_WF_G1, nullptr, 0, fid, gid, 0, g1);
    this->wf->add(PETSC_WF_G2, nullptr, 0, fid, gid, 0, g2);
    this->wf->add(PETSC_WF_G3, nullptr, 0, fid, gid, 0, g3);

    // So that PETSc thinks we have a Jacobian
    PetscDSSetJacobian(this->ds,
                       fid,
                       gid,
                       g0 ? internal::__dummy_jacobian_func : nullptr,
                       g1 ? internal::__dummy_jacobian_func : nullptr,
                       g2 ? internal::__dummy_jacobian_func : nullptr,
                       g3 ? internal::__dummy_jacobian_func : nullptr);
}

void
FENonlinearProblem::on_initial()
{
    _F_;
    NonlinearProblem::on_initial();
    compute_aux_fields();
}

} // namespace godzilla
