// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/FENonlinearProblem.h"
#include "godzilla/CallStack.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/IndexSet.h"
#include "godzilla/WeakForm.h"
#include "petscdm.h"
#include "petscds.h"
#include "petsc/private/dmimpl.h"
#include "petsc/private/dmpleximpl.h"

namespace godzilla {

ErrorCode
FENonlinearProblem::invoke_compute_boundary_delegate(DM, Vec x, void * context)
{
    CALL_STACK_MSG();
    auto * delegate = static_cast<Delegate<void(Vector &)> *>(context);
    Vector vec_x(x);
    delegate->invoke(vec_x);
    return 0;
}

ErrorCode
FENonlinearProblem::invoke_compute_residual_delegate(DM, Vec x, Vec F, void * context)
{
    CALL_STACK_MSG();
    auto * delegate = static_cast<Delegate<void(const Vector &, Vector &)> *>(context);
    Vector vec_x(x);
    Vector vec_F(F);
    delegate->invoke(vec_x, vec_F);
    return 0;
}

ErrorCode
FENonlinearProblem::invoke_compute_jacobian_delegate(DM, Vec x, Mat J, Mat Jp, void * context)
{
    CALL_STACK_MSG();
    auto * delegate =
        static_cast<Delegate<void(const Vector & x, Matrix & J, Matrix & Jp)> *>(context);
    Vector vec_x(x);
    Matrix mat_J(J);
    Matrix mat_Jp(Jp);
    delegate->invoke(vec_x, mat_J, mat_Jp);
    return 0;
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
    FEProblemInterface(this, parameters),
    state(INITIAL)
{
    CALL_STACK_MSG();
}

void
FENonlinearProblem::create()
{
    CALL_STACK_MSG();
    FEProblemInterface::create();
    NonlinearProblem::create();
}

void
FENonlinearProblem::init()
{
    CALL_STACK_MSG();
    NonlinearProblem::init();
    FEProblemInterface::init();
}

void
FENonlinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    set_boundary_local(this, &FENonlinearProblem::compute_boundary_local);
    set_function_local(this, &FENonlinearProblem::compute_residual_local);
    set_jacobian_local(this, &FENonlinearProblem::compute_jacobian_local);
}

void
FENonlinearProblem::set_up_initial_guess()
{
    CALL_STACK_MSG();
    TIMED_EVENT(9, "InitialGuess", "Setting initial guess");
    FEProblemInterface::set_up_initial_guess();
}

void
FENonlinearProblem::allocate_objects()
{
    CALL_STACK_MSG();
    NonlinearProblem::allocate_objects();
    FEProblemInterface::allocate_objects();
}

void
FENonlinearProblem::compute_boundary_local(Vector & x)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexInsertBoundaryValues(get_dm(),
                                           PETSC_TRUE,
                                           x,
                                           PETSC_MIN_REAL,
                                           nullptr,
                                           nullptr,
                                           nullptr));
}

void
FENonlinearProblem::compute_residual_local(const Vector & x, Vector & f)
{
    CALL_STACK_MSG();
    // this is based on DMSNESComputeResidual()
    auto all_cells = get_mesh()->get_all_cells();

    for (auto & region : get_weak_form()->get_residual_regions()) {
        IndexSet cells;
        if (region.label.is_null()) {
            all_cells.inc_ref();
            cells = all_cells;
        }
        else {
            IndexSet points = region.label.get_stratum(region.value);
            cells = IndexSet::intersect_caching(all_cells, points);
            points.destroy();
        }
        compute_residual_internal(get_dm(), region, cells, PETSC_MIN_REAL, x, Vector(), 0.0, f);
        cells.destroy();
    }

    all_cells.destroy();
}

void
FENonlinearProblem::compute_residual_internal(DM dm,
                                              const WeakForm::Region & region,
                                              const IndexSet & cell_is,
                                              Real time,
                                              const Vector & loc_x,
                                              const Vector & loc_x_t,
                                              Real t,
                                              Vector & loc_f)
{
    CALL_STACK_MSG();
    DM dm_aux = nullptr;
    PetscDS ds_aux = nullptr;
    PetscBool is_implicit = (loc_x_t || time == PETSC_MIN_REAL) ? PETSC_TRUE : PETSC_FALSE;
    Scalar *u = nullptr, *u_t, *a;
    IndexSet chunk_is;
    Int tot_dim_aux;
    PetscQuadrature affine_quad = nullptr, *quads = nullptr;
    PetscFEGeom *affine_geom = nullptr, **geoms = nullptr;

    /// TODO The places where we have to use isFE are probably the member functions for the
    /// PetscDisc class
    Int c_start, c_end;
    const Int * cells;
    cell_is.get_point_range(c_start, c_end, cells);
    Int f_start, f_end;
    PETSC_CHECK(DMPlexGetHeightStratum(dm, 1, &f_start, &f_end));
    /* 1: Get sizes from dm and dm_aux */
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetLocalSection(dm, &section));
    DMLabel ghost_label = nullptr;
    PETSC_CHECK(DMGetLabel(dm, "ghost", &ghost_label));
    PetscDS ds = nullptr;
#if PETSC_VERSION_GE(3, 19, 0)
    PETSC_CHECK(DMGetCellDS(dm, cells ? cells[c_start] : c_start, &ds, nullptr));
#else
    PETSC_CHECK(DMGetCellDS(dm, cells ? cells[c_start] : c_start, &ds));
#endif
    Int n_fields;
    PETSC_CHECK(PetscDSGetNumFields(ds, &n_fields));
    Int tot_dim;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));
    Vec loc_a;
    PETSC_CHECK(DMGetAuxiliaryVec(dm, region.label, region.value, 0, &loc_a));
    if (loc_a) {
        Int subcell;
        PETSC_CHECK(VecGetDM(loc_a, &dm_aux));
        PETSC_CHECK(DMGetEnclosurePoint(dm_aux, dm, DM_ENC_UNKNOWN, c_start, &subcell));
#if PETSC_VERSION_GE(3, 19, 0)
        PETSC_CHECK(DMGetCellDS(dm_aux, subcell, &ds_aux, nullptr));
#else
        PETSC_CHECK(DMGetCellDS(dm_aux, subcell, &ds_aux));
#endif
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));
    }
    /* 2: Get geometric data */
    for (Int f = 0; f < n_fields; ++f) {
        PetscBool fimp;
        PETSC_CHECK(PetscDSGetImplicit(ds, f, &fimp));
        if (is_implicit != fimp)
            continue;
    }
    DMField coord_field = nullptr;
    PETSC_CHECK(DMGetCoordinateField(dm, &coord_field));
    Int max_degree = PETSC_MAX_INT;
    PETSC_CHECK(DMFieldGetDegree(coord_field, cell_is, nullptr, &max_degree));
    if (max_degree <= 1) {
        PETSC_CHECK(DMFieldCreateDefaultQuadrature(coord_field, cell_is, &affine_quad));
        if (affine_quad) {
            PETSC_CHECK(
                DMSNESGetFEGeom(coord_field, cell_is, affine_quad, PETSC_FALSE, &affine_geom));
        }
    }
    else {
        PETSC_CHECK(PetscCalloc2(n_fields, &quads, n_fields, &geoms));
        for (Int f = 0; f < n_fields; ++f) {
            PetscBool fimp;
            PETSC_CHECK(PetscDSGetImplicit(ds, f, &fimp));
            if (is_implicit != fimp)
                continue;
            PetscObject obj;
            PETSC_CHECK(PetscDSGetDiscretization(ds, f, &obj));
            PetscClassId id;
            PETSC_CHECK(PetscObjectGetClassId(obj, &id));
            if (id == PETSCFE_CLASSID) {
                PetscFE fe = (PetscFE) obj;
                PETSC_CHECK(PetscFEGetQuadrature(fe, &quads[f]));
                PETSC_CHECK(PetscObjectReference((PetscObject) quads[f]));
                PETSC_CHECK(
                    DMSNESGetFEGeom(coord_field, cell_is, quads[f], PETSC_FALSE, &geoms[f]));
            }
        }
    }
    /* Loop over chunks */
    chunk_is.create(PETSC_COMM_SELF);
    Int n_cells = c_end - c_start;
    Int n_chunks = 1;
    Int cell_chunk_size = n_cells / n_chunks;
    n_chunks = PetscMin(1, n_cells);
    for (Int chunk = 0; chunk < n_chunks; ++chunk) {
        Scalar * elem_vec;
        Int cS = c_start + chunk * cell_chunk_size;
        Int cE = PetscMin(cS + cell_chunk_size, c_end);
        Int n_chunk_cells = cE - cS;

        /* Extract field coefficients */
        chunk_is.get_point_subrange(cS, cE, cells);
        PETSC_CHECK(DMPlexGetCellFields(dm, chunk_is, loc_x, loc_x_t, loc_a, &u, &u_t, &a));
        PETSC_CHECK(DMGetWorkArray(dm, n_chunk_cells * tot_dim, MPIU_SCALAR, &elem_vec));
        PETSC_CHECK(PetscArrayzero(elem_vec, n_chunk_cells * tot_dim));
        /* Loop over fields */
        for (Int f = 0; f < n_fields; ++f) {
            PetscBool fimp;
            PETSC_CHECK(PetscDSGetImplicit(ds, f, &fimp));
            if (is_implicit != fimp)
                continue;

            PetscObject obj;
            PETSC_CHECK(PetscDSGetDiscretization(ds, f, &obj));
            PetscClassId id;
            PETSC_CHECK(PetscObjectGetClassId(obj, &id));
            if (id == PETSCFE_CLASSID) {
                WeakForm::Key key(region, f, 0);

                PetscFE fe = (PetscFE) obj;
                PetscFEGeom * geom = affine_geom ? affine_geom : geoms[f];
                PetscQuadrature quad = affine_quad ? affine_quad : quads[f];

                Int n_batches, n_blocks;
                PETSC_CHECK(PetscFEGetTileSizes(fe, nullptr, &n_blocks, nullptr, &n_batches));
                Int n_qpts;
                PETSC_CHECK(
                    PetscQuadratureGetData(quad, nullptr, nullptr, &n_qpts, nullptr, nullptr));
                Int n_basis;
                PETSC_CHECK(PetscFEGetDimension(fe, &n_basis));
                Int block_size = n_basis;
                Int batch_size = n_blocks * block_size;
                PETSC_CHECK(PetscFESetTileSizes(fe, block_size, n_blocks, batch_size, n_batches));
                Int n_chunks_field = n_chunk_cells / (n_batches * batch_size);
                Int n_elems = n_chunks_field * n_batches * batch_size;
                Int n_remdr = n_chunk_cells % (n_batches * batch_size);
                Int offset = n_chunk_cells - n_remdr;
                /* Integrate FE residual to get elemVec (need fields at quadrature points) */
                PetscFEGeom * chunk_geom = nullptr;
                PETSC_CHECK(PetscFEGeomGetChunk(geom, 0, offset, &chunk_geom));
                integrate_residual(ds, key, n_elems, chunk_geom, u, u_t, ds_aux, a, t, elem_vec);
                PETSC_CHECK(PetscFEGeomGetChunk(geom, offset, n_chunk_cells, &chunk_geom));
                integrate_residual(ds,
                                   key,
                                   n_remdr,
                                   chunk_geom,
                                   &u[offset * tot_dim],
                                   u_t ? &u_t[offset * tot_dim] : nullptr,
                                   ds_aux,
                                   &a[offset * tot_dim_aux],
                                   t,
                                   &elem_vec[offset * tot_dim]);
                PETSC_CHECK(PetscFEGeomRestoreChunk(geom, offset, n_chunk_cells, &chunk_geom));
            }
            else
                throw Exception("Unknown discretization type for field {}", f);
        }
        /* Loop over domain */
        /* Add elemVec to locX */
        for (Int c = cS; c < cE; ++c) {
            const Int cell = cells ? cells[c] : c;
            const Int cind = c - c_start;

            if (ghost_label) {
                Int ghost_val;
                PETSC_CHECK(DMLabelGetValue(ghost_label, cell, &ghost_val));
                if (ghost_val > 0)
                    continue;
            }
            PETSC_CHECK(DMPlexVecSetClosure(dm,
                                            section,
                                            loc_f,
                                            cell,
                                            &elem_vec[cind * tot_dim],
                                            ADD_ALL_VALUES));
        }

        PETSC_CHECK(DMPlexRestoreCellFields(dm, chunk_is, loc_x, loc_x_t, loc_a, &u, &u_t, &a));
        PETSC_CHECK(DMRestoreWorkArray(dm, n_chunk_cells * tot_dim, MPIU_SCALAR, &elem_vec));
    }
    chunk_is.destroy();
    cell_is.restore_point_range(c_start, c_end, cells);

    compute_bnd_residual_internal(dm, loc_x, loc_x_t, t, loc_f);

    if (max_degree <= 1) {
        PETSC_CHECK(
            DMSNESRestoreFEGeom(coord_field, cell_is, affine_quad, PETSC_FALSE, &affine_geom));
        PETSC_CHECK(PetscQuadratureDestroy(&affine_quad));
    }
    else {
        for (Int f = 0; f < n_fields; ++f) {
            PETSC_CHECK(
                DMSNESRestoreFEGeom(coord_field, cell_is, quads[f], PETSC_FALSE, &geoms[f]));
            PETSC_CHECK(PetscQuadratureDestroy(&quads[f]));
        }
        PETSC_CHECK(PetscFree2(quads, geoms));
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
}

void
FENonlinearProblem::compute_bnd_residual_internal(DM dm, Vec loc_x, Vec loc_x_t, Real t, Vec loc_f)
{
    CALL_STACK_MSG();

    PetscDS prob;
    PETSC_CHECK(DMGetDS(dm, &prob));
    auto depth_label = get_mesh()->get_depth_label();
    Int dim = get_dimension();
    auto facets = depth_label.get_stratum(dim - 1);
    Int n_bnd;
    PETSC_CHECK(PetscDSGetNumBoundary(prob, &n_bnd));
    for (Int bd = 0; bd < n_bnd; ++bd) {
        DMBoundaryConditionType type;
        DMLabel label;
        const Int * values;
        Int field, n_values;

        PETSC_CHECK(PetscDSGetBoundary(prob,
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
        PETSC_CHECK(PetscDSGetDiscretization(prob, field, &obj));
        PetscClassId id;
        PETSC_CHECK(PetscObjectGetClassId(obj, &id));
        if ((id != PETSCFE_CLASSID) || (type & DM_BC_ESSENTIAL))
            continue;
        DMField coord_field = nullptr;
        PETSC_CHECK(DMGetCoordinateField(dm, &coord_field));
        for (Int v = 0; v < n_values; ++v) {
            WeakForm::Key key(label, values[v], field, 0);
            compute_bnd_residual_single_internal(dm,
                                                 t,
                                                 key,
                                                 loc_x,
                                                 loc_x_t,
                                                 loc_f,
                                                 coord_field,
                                                 facets);
        }
    }
    facets.destroy();
}

void
FENonlinearProblem::compute_bnd_residual_single_internal(DM dm,
                                                         Real t,
                                                         const WeakForm::Key & key,
                                                         Vec loc_x,
                                                         Vec loc_x_t,
                                                         Vec loc_f,
                                                         DMField coord_field,
                                                         const IndexSet & facets)
{
    CALL_STACK_MSG();
    DM plex = nullptr, plex_aux = nullptr;
    DMEnclosureType enc_aux;
    PetscDS prob, prob_aux = nullptr;
    PetscSection section, section_aux = nullptr;
    Vec loc_a = nullptr;
    Scalar *u = nullptr, *u_t = nullptr, *a = nullptr, *elem_vec = nullptr;
    Int tot_dim, tot_dim_aux = 0;

    PETSC_CHECK(DMConvert(dm, DMPLEX, &plex));
    PETSC_CHECK(DMGetLocalSection(dm, &section));
    PETSC_CHECK(DMGetDS(dm, &prob));
    PETSC_CHECK(PetscDSGetTotalDimension(prob, &tot_dim));
    PETSC_CHECK(DMGetAuxiliaryVec(dm, key.label, key.value, key.part, &loc_a));
    if (loc_a) {
        DM dm_aux;
        PETSC_CHECK(VecGetDM(loc_a, &dm_aux));
        PETSC_CHECK(DMGetEnclosureRelation(dm_aux, dm, &enc_aux));
        PETSC_CHECK(DMConvert(dm_aux, DMPLEX, &plex_aux));
        PETSC_CHECK(DMGetDS(plex_aux, &prob_aux));
        PETSC_CHECK(PetscDSGetTotalDimension(prob_aux, &tot_dim_aux));
        PETSC_CHECK(DMGetLocalSection(plex_aux, &section_aux));
    }

    auto points = key.label.get_stratum(key.value);
    if (points) {
        PetscQuadrature q_geom = nullptr;

        /* TODO: Special cases of ISIntersect where it is quick to check a priori if one is a
         * superset of the other */
        IndexSet isect = IndexSet::intersect_caching(facets, points);
        points.destroy();
        points = isect;

        Int n_faces = points.get_local_size();
        points.get_indices();

        PETSC_CHECK(PetscMalloc4(n_faces * tot_dim,
                                 &u,
                                 loc_x_t ? n_faces * tot_dim : 0,
                                 &u_t,
                                 n_faces * tot_dim,
                                 &elem_vec,
                                 loc_a ? n_faces * tot_dim_aux : 0,
                                 &a));
        Int max_degree;
        PETSC_CHECK(DMFieldGetDegree(coord_field, points, nullptr, &max_degree));
        if (max_degree <= 1) {
            PETSC_CHECK(DMFieldCreateDefaultQuadrature(coord_field, points, &q_geom));
        }
        if (!q_geom) {
            PetscFE fe;
            PETSC_CHECK(PetscDSGetDiscretization(prob, key.field, (PetscObject *) &fe));
            PETSC_CHECK(PetscFEGetFaceQuadrature(fe, &q_geom));
            PETSC_CHECK(PetscObjectReference((PetscObject) q_geom));
        }
        Int n_qpts;
        PETSC_CHECK(PetscQuadratureGetData(q_geom, nullptr, nullptr, &n_qpts, nullptr, nullptr));
        PetscFEGeom * fgeom;
        PETSC_CHECK(DMSNESGetFEGeom(coord_field, points, q_geom, PETSC_TRUE, &fgeom));
        for (Int face = 0; face < n_faces; ++face) {
            const Int * support;
            PETSC_CHECK(DMPlexGetSupport(dm, points[face], &support));
            Scalar * x = nullptr;
            PETSC_CHECK(DMPlexVecGetClosure(plex, section, loc_x, support[0], nullptr, &x));
            for (Int i = 0; i < tot_dim; ++i)
                u[face * tot_dim + i] = x[i];
            PETSC_CHECK(DMPlexVecRestoreClosure(plex, section, loc_x, support[0], nullptr, &x));
            if (loc_x_t) {
                PETSC_CHECK(DMPlexVecGetClosure(plex, section, loc_x_t, support[0], nullptr, &x));
                for (Int i = 0; i < tot_dim; ++i)
                    u_t[face * tot_dim + i] = x[i];
                PETSC_CHECK(
                    DMPlexVecRestoreClosure(plex, section, loc_x_t, support[0], nullptr, &x));
            }
            if (loc_a) {
                Int subp;
                PETSC_CHECK(DMGetEnclosurePoint(plex_aux, dm, enc_aux, support[0], &subp));
                PETSC_CHECK(DMPlexVecGetClosure(plex_aux, section_aux, loc_a, subp, nullptr, &x));
                for (Int i = 0; i < tot_dim_aux; ++i)
                    a[face * tot_dim_aux + i] = x[i];
                PETSC_CHECK(
                    DMPlexVecRestoreClosure(plex_aux, section_aux, loc_a, subp, nullptr, &x));
            }
        }
        PETSC_CHECK(PetscArrayzero(elem_vec, n_faces * tot_dim));

        PetscFE fe;
        PETSC_CHECK(PetscDSGetDiscretization(prob, key.field, (PetscObject *) &fe));
        Int n_basis;
        PETSC_CHECK(PetscFEGetDimension(fe, &n_basis));
        /* Conforming batches */
        Int n_batches, n_blocks;
        PETSC_CHECK(PetscFEGetTileSizes(fe, nullptr, &n_blocks, nullptr, &n_batches));
        /* TODO: documentation is unclear about what is going on with these numbers: how should Nb /
         * Nq factor in ? */
        Int block_size = n_basis;
        Int batch_size = n_blocks * block_size;
        PETSC_CHECK(PetscFESetTileSizes(fe, block_size, n_blocks, batch_size, n_batches));
        Int n_chunks = n_faces / (n_batches * batch_size);
        Int n_elems = n_chunks * n_batches * batch_size;
        /* Remainder */
        Int n_remdr = n_faces % (n_batches * batch_size);
        Int offset = n_faces - n_remdr;
        PetscFEGeom * chunk_geom = nullptr;
        PETSC_CHECK(PetscFEGeomGetChunk(fgeom, 0, offset, &chunk_geom));
        integrate_bnd_residual(prob, key, n_elems, chunk_geom, u, u_t, prob_aux, a, t, elem_vec);
        PETSC_CHECK(PetscFEGeomRestoreChunk(fgeom, 0, offset, &chunk_geom));
        PETSC_CHECK(PetscFEGeomGetChunk(fgeom, offset, n_faces, &chunk_geom));
        integrate_bnd_residual(prob,
                               key,
                               n_remdr,
                               chunk_geom,
                               &u[offset * tot_dim],
                               u_t ? &u_t[offset * tot_dim] : nullptr,
                               prob_aux,
                               a ? &a[offset * tot_dim_aux] : nullptr,
                               t,
                               &elem_vec[offset * tot_dim]);
        PETSC_CHECK(PetscFEGeomRestoreChunk(fgeom, offset, n_faces, &chunk_geom));

        for (Int face = 0; face < n_faces; ++face) {
            const Int * support;
            PETSC_CHECK(DMPlexGetSupport(plex, points[face], &support));
            PETSC_CHECK(DMPlexVecSetClosure(plex,
                                            nullptr,
                                            loc_f,
                                            support[0],
                                            &elem_vec[face * tot_dim],
                                            ADD_ALL_VALUES));
        }
        PETSC_CHECK(DMSNESRestoreFEGeom(coord_field, points, q_geom, PETSC_TRUE, &fgeom));
        PETSC_CHECK(PetscQuadratureDestroy(&q_geom));
        points.restore_indices();
        points.destroy();
        PETSC_CHECK(PetscFree4(u, u_t, elem_vec, a));
    }

    PETSC_CHECK(DMDestroy(&plex));
    PETSC_CHECK(DMDestroy(&plex_aux));
}

void
FENonlinearProblem::compute_jacobian_local(const Vector & x, Matrix & J, Matrix & Jp)
{
    CALL_STACK_MSG();
    // based on DMPlexSNESComputeJacobianFEM and DMSNESComputeJacobianAction
    auto all_cells = get_mesh()->get_all_cells();

    auto wf = get_weak_form();
    auto has_jac = wf->has_jacobian();
    auto has_precond = wf->has_jacobian_preconditioner();
    if (has_jac && has_precond)
        J.zero();
    Jp.zero();

    for (auto & region : wf->get_jacobian_regions()) {
        IndexSet cells;
        if (region.label.is_null()) {
            all_cells.inc_ref();
            cells = all_cells;
        }
        else {
            auto points = region.label.get_stratum(region.value);
            cells = IndexSet::intersect_caching(all_cells, points);
            points.destroy();
        }
        compute_jacobian_internal(get_dm(), region, cells, 0.0, 0.0, x, Vector(), J, Jp);
        cells.destroy();
    }

    all_cells.destroy();
}

void
FENonlinearProblem::compute_jacobian_internal(DM dm,
                                              const WeakForm::Region & region,
                                              const IndexSet & cell_is,
                                              Real t,
                                              Real x_t_shift,
                                              const Vector & X,
                                              const Vector & X_t,
                                              Matrix & J,
                                              Matrix & Jp)
{
    CALL_STACK_MSG();
    Int n_cells = cell_is.get_local_size();
    Int c_start, c_end;
    const Int * cells;
    cell_is.get_point_range(c_start, c_end, cells);
    PetscBool transform;
    PETSC_CHECK(DMHasBasisTransform(dm, &transform));
    DM tdm;
    PETSC_CHECK(DMGetBasisTransformDM_Internal(dm, &tdm));
    Vec tv;
    PETSC_CHECK(DMGetBasisTransformVec_Internal(dm, &tv));
    PetscSection section;
    PETSC_CHECK(DMGetLocalSection(dm, &section));
    PetscSection global_section;
    PETSC_CHECK(DMGetGlobalSection(dm, &global_section));
    PetscDS prob;
#if PETSC_VERSION_GE(3, 19, 0)
    PETSC_CHECK(DMGetCellDS(dm, cells ? cells[c_start] : c_start, &prob, nullptr));
#else
    PETSC_CHECK(DMGetCellDS(dm, cells ? cells[c_start] : c_start, &prob));
#endif
    Int n_fields;
    PETSC_CHECK(PetscDSGetNumFields(prob, &n_fields));
    Int tot_dim;
    PETSC_CHECK(PetscDSGetTotalDimension(prob, &tot_dim));
    auto wf = get_weak_form();
    auto has_jac = wf->has_jacobian();
    auto has_prec = wf->has_jacobian_preconditioner();
    // user passed in the same matrix, avoid double contributions and only assemble the Jacobian
    if (has_jac && J == Jp)
        has_prec = PETSC_FALSE;

    Vec A;
    PETSC_CHECK(DMGetAuxiliaryVec(dm, region.label, region.value, 0, &A));
    DM dm_aux = nullptr;
    DMEnclosureType enc_aux;
    PetscDS prob_aux = nullptr;
    PetscSection section_aux;
    DM plex;
    Int tot_dim_aux;
    if (A) {
        PETSC_CHECK(VecGetDM(A, &dm_aux));
        PETSC_CHECK(DMGetEnclosureRelation(dm_aux, dm, &enc_aux));
        PETSC_CHECK(DMConvert(dm_aux, DMPLEX, &plex));
        PETSC_CHECK(DMGetLocalSection(plex, &section_aux));
        PETSC_CHECK(DMGetDS(dm_aux, &prob_aux));
        PETSC_CHECK(PetscDSGetTotalDimension(prob_aux, &tot_dim_aux));
    }

    Scalar *u, *u_t;
    Scalar *elem_mat, *elem_mat_P;
    PETSC_CHECK(PetscMalloc4(n_cells * tot_dim,
                             &u,
                             (X_t ? n_cells * tot_dim : 0),
                             &u_t,
                             n_cells * tot_dim * tot_dim,
                             &elem_mat,
                             has_prec ? n_cells * tot_dim * tot_dim : 0,
                             &elem_mat_P));
    Scalar * a = nullptr;
    if (dm_aux)
        PETSC_CHECK(PetscMalloc1(n_cells * tot_dim_aux, &a));

    DMField coord_field;
    PETSC_CHECK(DMGetCoordinateField(dm, &coord_field));
    for (Int c = c_start; c < c_end; ++c) {
        const Int cell = cells ? cells[c] : c;
        const Int cind = c - c_start;

        Scalar *x = nullptr, *x_t = nullptr;
        PETSC_CHECK(DMPlexVecGetClosure(dm, section, X, cell, nullptr, &x));

        for (Int i = 0; i < tot_dim; ++i)
            u[cind * tot_dim + i] = x[i];
        PETSC_CHECK(DMPlexVecRestoreClosure(dm, section, X, cell, nullptr, &x));
        if (X_t) {
            PETSC_CHECK(DMPlexVecGetClosure(dm, section, X_t, cell, nullptr, &x_t));
            for (Int i = 0; i < tot_dim; ++i)
                u_t[cind * tot_dim + i] = x_t[i];
            PETSC_CHECK(DMPlexVecRestoreClosure(dm, section, X_t, cell, nullptr, &x_t));
        }
        if (dm_aux) {
            Int subcell;
            PETSC_CHECK(DMGetEnclosurePoint(dm_aux, dm, enc_aux, cell, &subcell));
            PETSC_CHECK(DMPlexVecGetClosure(plex, section_aux, A, subcell, nullptr, &x));
            for (Int i = 0; i < tot_dim_aux; ++i)
                a[cind * tot_dim_aux + i] = x[i];
            PETSC_CHECK(DMPlexVecRestoreClosure(plex, section_aux, A, subcell, nullptr, &x));
        }
    }
    if (has_jac)
        PETSC_CHECK(PetscArrayzero(elem_mat, n_cells * tot_dim * tot_dim));
    if (has_prec)
        PETSC_CHECK(PetscArrayzero(elem_mat_P, n_cells * tot_dim * tot_dim));
    for (Int field_i = 0; field_i < n_fields; ++field_i) {
        PetscFE fe;
        PETSC_CHECK(PetscDSGetDiscretization(prob, field_i, (PetscObject *) &fe));
        PetscClassId id;
        PETSC_CHECK(PetscObjectGetClassId((PetscObject) fe, &id));
        Int n_basis;
        PETSC_CHECK(PetscFEGetDimension(fe, &n_basis));
        Int n_batches, n_blocks;
        PETSC_CHECK(PetscFEGetTileSizes(fe, nullptr, &n_blocks, nullptr, &n_batches));
        Int max_degree;
        PETSC_CHECK(DMFieldGetDegree(coord_field, cell_is, nullptr, &max_degree));
        PetscQuadrature q_geom = nullptr;
        if (max_degree <= 1) {
            PETSC_CHECK(DMFieldCreateDefaultQuadrature(coord_field, cell_is, &q_geom));
        }
        if (!q_geom) {
            PETSC_CHECK(PetscFEGetQuadrature(fe, &q_geom));
            PETSC_CHECK(PetscObjectReference((PetscObject) q_geom));
        }
        Int n_qpts;
        PETSC_CHECK(PetscQuadratureGetData(q_geom, nullptr, nullptr, &n_qpts, nullptr, nullptr));
        PetscFEGeom * cgeom_fem;
        PETSC_CHECK(DMSNESGetFEGeom(coord_field, cell_is, q_geom, PETSC_FALSE, &cgeom_fem));
        Int block_size = n_basis;
        Int batch_size = n_blocks * block_size;
        PETSC_CHECK(PetscFESetTileSizes(fe, block_size, n_blocks, batch_size, n_batches));
        Int num_chunks = n_cells / (n_batches * batch_size);
        Int n_elems = num_chunks * n_batches * batch_size;
        Int n_remdr = n_cells % (n_batches * batch_size);
        Int offset = n_cells - n_remdr;
        PetscFEGeom * chunk_geom = nullptr;
        PETSC_CHECK(PetscFEGeomGetChunk(cgeom_fem, 0, offset, &chunk_geom));
        PetscFEGeom * rem_geom = nullptr;
        PETSC_CHECK(PetscFEGeomGetChunk(cgeom_fem, offset, n_cells, &rem_geom));
        for (Int field_j = 0; field_j < n_fields; ++field_j) {
            WeakForm::Key key(region, field_i, field_j);
            if (has_jac) {
                integrate_jacobian(prob,
                                   PETSCFE_JACOBIAN,
                                   key,
                                   n_elems,
                                   chunk_geom,
                                   u,
                                   u_t,
                                   prob_aux,
                                   a,
                                   t,
                                   x_t_shift,
                                   elem_mat);
                integrate_jacobian(prob,
                                   PETSCFE_JACOBIAN,
                                   key,
                                   n_remdr,
                                   rem_geom,
                                   &u[offset * tot_dim],
                                   u_t ? &u_t[offset * tot_dim] : nullptr,
                                   prob_aux,
                                   &a[offset * tot_dim_aux],
                                   t,
                                   x_t_shift,
                                   &elem_mat[offset * tot_dim * tot_dim]);
            }
            if (has_prec) {
                integrate_jacobian(prob,
                                   PETSCFE_JACOBIAN_PRE,
                                   key,
                                   n_elems,
                                   chunk_geom,
                                   u,
                                   u_t,
                                   prob_aux,
                                   a,
                                   t,
                                   x_t_shift,
                                   elem_mat_P);
                integrate_jacobian(prob,
                                   PETSCFE_JACOBIAN_PRE,
                                   key,
                                   n_remdr,
                                   rem_geom,
                                   &u[offset * tot_dim],
                                   u_t ? &u_t[offset * tot_dim] : nullptr,
                                   prob_aux,
                                   &a[offset * tot_dim_aux],
                                   t,
                                   x_t_shift,
                                   &elem_mat_P[offset * tot_dim * tot_dim]);
            }
        }
        PETSC_CHECK(PetscFEGeomRestoreChunk(cgeom_fem, offset, n_cells, &rem_geom));
        PETSC_CHECK(PetscFEGeomRestoreChunk(cgeom_fem, 0, offset, &chunk_geom));
        PETSC_CHECK(DMSNESRestoreFEGeom(coord_field, cell_is, q_geom, PETSC_FALSE, &cgeom_fem));
        PETSC_CHECK(PetscQuadratureDestroy(&q_geom));
    }
    // Add contribution from X_t
    // Insert values into matrix
    for (Int c = c_start; c < c_end; ++c) {
        const Int cell = cells ? cells[c] : c;
        const Int cind = c - c_start;

        // Transform to global basis before insertion in Jacobian
        if (transform)
            PETSC_CHECK(
                DMPlexBasisTransformPointTensor_Internal(dm,
                                                         tdm,
                                                         tv,
                                                         cell,
                                                         PETSC_TRUE,
                                                         tot_dim,
                                                         &elem_mat[cind * tot_dim * tot_dim]));
        if (has_prec) {
            if (has_jac)
                PETSC_CHECK(DMPlexMatSetClosure(dm,
                                                section,
                                                global_section,
                                                J,
                                                cell,
                                                &elem_mat[cind * tot_dim * tot_dim],
                                                ADD_VALUES));
            PETSC_CHECK(DMPlexMatSetClosure(dm,
                                            section,
                                            global_section,
                                            Jp,
                                            cell,
                                            &elem_mat_P[cind * tot_dim * tot_dim],
                                            ADD_VALUES));
        }
        else {
            if (has_jac)
                PETSC_CHECK(DMPlexMatSetClosure(dm,
                                                section,
                                                global_section,
                                                Jp,
                                                cell,
                                                &elem_mat[cind * tot_dim * tot_dim],
                                                ADD_VALUES));
        }
    }
    cell_is.restore_point_range(c_start, c_end, cells);
    PETSC_CHECK(PetscFree4(u, u_t, elem_mat, elem_mat_P));
    if (dm_aux) {
        PETSC_CHECK(PetscFree(a));
        PETSC_CHECK(DMDestroy(&plex));
    }
    // Compute boundary integrals
    compute_bnd_jacobian_internal(dm, X, X_t, t, x_t_shift, J, Jp);
    // Assemble matrix
    PetscBool ass_op = has_jac && has_prec ? PETSC_TRUE : PETSC_FALSE, gass_op;
    MPI_Allreduce(&ass_op, &gass_op, 1, MPIU_BOOL, MPI_LOR, PetscObjectComm((PetscObject) dm));

    if (has_jac & has_prec)
        J.assemble();
    Jp.assemble();
}

void
FENonlinearProblem::compute_bnd_jacobian_internal(DM dm,
                                                  Vec X_loc,
                                                  Vec X_t_loc,
                                                  Real t,
                                                  Real x_t_shift,
                                                  Mat J,
                                                  Mat Jp)
{
    CALL_STACK_MSG();
    PetscDS prob;
    PETSC_CHECK(DMGetDS(dm, &prob));
    auto depth_label = get_mesh()->get_depth_label();
    Int dim = get_dimension();
    auto facets = depth_label.get_stratum(dim - 1);
    Int n_bnd;
    PETSC_CHECK(PetscDSGetNumBoundary(prob, &n_bnd));
    DMField coord_field = nullptr;
    PETSC_CHECK(DMGetCoordinateField(dm, &coord_field));
    for (Int bd = 0; bd < n_bnd; ++bd) {
        DMBoundaryConditionType type;
        DMLabel dm_label;
        Int n_values;
        const Int * values;
        Int field_i;
        PETSC_CHECK(PetscDSGetBoundary(prob,
                                       bd,
                                       nullptr,
                                       &type,
                                       nullptr,
                                       &dm_label,
                                       &n_values,
                                       &values,
                                       &field_i,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr));
        PetscObject obj;
        PETSC_CHECK(PetscDSGetDiscretization(prob, field_i, &obj));
        PetscClassId id;
        PETSC_CHECK(PetscObjectGetClassId(obj, &id));
        if ((id != PETSCFE_CLASSID) || (type & DM_BC_ESSENTIAL))
            continue;
        Label label(dm_label);
        compute_bnd_jacobian_single_internal(dm,
                                             t,
                                             label,
                                             n_values,
                                             values,
                                             field_i,
                                             X_loc,
                                             X_t_loc,
                                             x_t_shift,
                                             J,
                                             Jp,
                                             coord_field,
                                             facets);
    }
    facets.destroy();
}

void
FENonlinearProblem::compute_bnd_jacobian_single_internal(DM dm,
                                                         Real t,
                                                         const Label & label,
                                                         Int n_values,
                                                         const Int values[],
                                                         Int field_i,
                                                         Vec X_loc,
                                                         Vec X_t_loc,
                                                         Real x_t_shift,
                                                         Mat J,
                                                         Mat Jp,
                                                         DMField coord_field,
                                                         const IndexSet & facets)
{
    CALL_STACK_MSG();
    DM plex = nullptr;
    PETSC_CHECK(DMConvert(dm, DMPLEX, &plex));
    PetscBool transform;
    PETSC_CHECK(DMHasBasisTransform(dm, &transform));
    DM tdm;
    PETSC_CHECK(DMGetBasisTransformDM_Internal(dm, &tdm));
    Vec tv;
    PETSC_CHECK(DMGetBasisTransformVec_Internal(dm, &tv));
    PetscSection section;
    PETSC_CHECK(DMGetLocalSection(dm, &section));
    PetscDS prob;
    PETSC_CHECK(DMGetDS(dm, &prob));
    Int n_fields;
    PETSC_CHECK(PetscDSGetNumFields(prob, &n_fields));
    Int tot_dim;
    PETSC_CHECK(PetscDSGetTotalDimension(prob, &tot_dim));
    Vec locA = nullptr;
    PETSC_CHECK(DMGetAuxiliaryVec(dm, label, values[0], 0, &locA));

    Int tot_dim_aux = 0;
    DM plexA = nullptr;
    DMEnclosureType enc_aux;
    PetscDS prob_aux = nullptr;
    PetscSection section_aux = nullptr;
    if (locA) {
        DM dm_aux;
        PETSC_CHECK(VecGetDM(locA, &dm_aux));
        PETSC_CHECK(DMGetEnclosureRelation(dm_aux, dm, &enc_aux));
        PETSC_CHECK(DMConvert(dm_aux, DMPLEX, &plexA));
        PETSC_CHECK(DMGetDS(plexA, &prob_aux));
        PETSC_CHECK(PetscDSGetTotalDimension(prob_aux, &tot_dim_aux));
        PETSC_CHECK(DMGetLocalSection(plexA, &section_aux));
    }

    PetscSection global_section;
    PETSC_CHECK(DMGetGlobalSection(dm, &global_section));
    for (Int v = 0; v < n_values; ++v) {
        auto points = label.get_stratum(values[v]);
        if (!points)
            continue; /* No points with that id on this process */

        // TODO: Special cases of ISIntersect where it is quick to check a prior if one is a
        // superset of the other
        IndexSet isect = IndexSet::intersect_caching(facets, points);
        points.destroy();
        points = isect;

        Int n_faces = points.get_local_size();
        points.get_indices();

        Scalar *u = nullptr, *u_t = nullptr, *a = nullptr, *elem_mat = nullptr;
        PETSC_CHECK(PetscMalloc4(n_faces * tot_dim,
                                 &u,
                                 X_t_loc ? n_faces * tot_dim : 0,
                                 &u_t,
                                 n_faces * tot_dim * tot_dim,
                                 &elem_mat,
                                 locA ? n_faces * tot_dim_aux : 0,
                                 &a));
        Int max_degree;
        PETSC_CHECK(DMFieldGetDegree(coord_field, points, nullptr, &max_degree));
        PetscQuadrature q_geom = nullptr;
        if (max_degree <= 1) {
            PETSC_CHECK(DMFieldCreateDefaultQuadrature(coord_field, points, &q_geom));
        }
        if (!q_geom) {
            PetscFE fe;
            PETSC_CHECK(PetscDSGetDiscretization(prob, field_i, (PetscObject *) &fe));
            PETSC_CHECK(PetscFEGetFaceQuadrature(fe, &q_geom));
            PETSC_CHECK(PetscObjectReference((PetscObject) q_geom));
        }
        Int n_qpts;
        PETSC_CHECK(PetscQuadratureGetData(q_geom, nullptr, nullptr, &n_qpts, nullptr, nullptr));
        PetscFEGeom * fgeom;
        PETSC_CHECK(DMSNESGetFEGeom(coord_field, points, q_geom, PETSC_TRUE, &fgeom));
        for (Int face = 0; face < n_faces; ++face) {
            const Int * support;
            PETSC_CHECK(DMPlexGetSupport(dm, points[face], &support));
            Scalar * x = nullptr;
            PETSC_CHECK(DMPlexVecGetClosure(plex, section, X_loc, support[0], nullptr, &x));
            for (Int i = 0; i < tot_dim; ++i)
                u[face * tot_dim + i] = x[i];
            PETSC_CHECK(DMPlexVecRestoreClosure(plex, section, X_loc, support[0], nullptr, &x));
            if (X_t_loc) {
                PETSC_CHECK(DMPlexVecGetClosure(plex, section, X_t_loc, support[0], nullptr, &x));
                for (Int i = 0; i < tot_dim; ++i)
                    u_t[face * tot_dim + i] = x[i];
                PETSC_CHECK(
                    DMPlexVecRestoreClosure(plex, section, X_t_loc, support[0], nullptr, &x));
            }
            if (locA) {
                Int subp;
                PETSC_CHECK(DMGetEnclosurePoint(plexA, dm, enc_aux, support[0], &subp));
                PETSC_CHECK(DMPlexVecGetClosure(plexA, section_aux, locA, subp, nullptr, &x));
                for (Int i = 0; i < tot_dim_aux; ++i)
                    a[face * tot_dim_aux + i] = x[i];
                PETSC_CHECK(DMPlexVecRestoreClosure(plexA, section_aux, locA, subp, nullptr, &x));
            }
        }
        PETSC_CHECK(PetscArrayzero(elem_mat, n_faces * tot_dim * tot_dim));

        PetscFE fe;
        PETSC_CHECK(PetscDSGetDiscretization(prob, field_i, (PetscObject *) &fe));
        Int n_basis;
        PETSC_CHECK(PetscFEGetDimension(fe, &n_basis));
        Int n_batches, n_blocks;
        PETSC_CHECK(PetscFEGetTileSizes(fe, nullptr, &n_blocks, nullptr, &n_batches));
        Int block_size = n_basis;
        Int batch_size = n_blocks * block_size;
        PETSC_CHECK(PetscFESetTileSizes(fe, block_size, n_blocks, batch_size, n_batches));
        Int n_chunks = n_faces / (n_batches * batch_size);
        Int n_elems = n_chunks * n_batches * batch_size;
        Int n_remdr = n_faces % (n_batches * batch_size);
        Int offset = n_faces - n_remdr;
        PetscFEGeom * chunk_geom = nullptr;
        PETSC_CHECK(PetscFEGeomGetChunk(fgeom, 0, offset, &chunk_geom));
        for (Int field_j = 0; field_j < n_fields; ++field_j) {
            WeakForm::Key key(label, values[v], field_i, field_j, 0);
            integrate_bnd_jacobian(prob,
                                   key,
                                   n_elems,
                                   chunk_geom,
                                   u,
                                   u_t,
                                   prob_aux,
                                   a,
                                   t,
                                   x_t_shift,
                                   elem_mat);
        }
        PETSC_CHECK(PetscFEGeomGetChunk(fgeom, offset, n_faces, &chunk_geom));
        for (Int fieldJ = 0; fieldJ < n_fields; ++fieldJ) {
            WeakForm::Key key(label, values[v], field_i, fieldJ, 0);
            integrate_bnd_jacobian(prob,
                                   key,
                                   n_remdr,
                                   chunk_geom,
                                   &u[offset * tot_dim],
                                   u_t ? &u_t[offset * tot_dim] : nullptr,
                                   prob_aux,
                                   a ? &a[offset * tot_dim_aux] : nullptr,
                                   t,
                                   x_t_shift,
                                   &elem_mat[offset * tot_dim * tot_dim]);
        }
        PETSC_CHECK(PetscFEGeomRestoreChunk(fgeom, offset, n_faces, &chunk_geom));

        for (Int face = 0; face < n_faces; ++face) {
            // Transform to global basis before insertion in Jacobian
            const Int * support;
            PETSC_CHECK(DMPlexGetSupport(plex, points[face], &support));
            if (transform)
                PETSC_CHECK(
                    DMPlexBasisTransformPointTensor_Internal(dm,
                                                             tdm,
                                                             tv,
                                                             support[0],
                                                             PETSC_TRUE,
                                                             tot_dim,
                                                             &elem_mat[face * tot_dim * tot_dim]));
            PETSC_CHECK(DMPlexMatSetClosure(plex,
                                            section,
                                            global_section,
                                            Jp,
                                            support[0],
                                            &elem_mat[face * tot_dim * tot_dim],
                                            ADD_VALUES));
        }
        PETSC_CHECK(DMSNESRestoreFEGeom(coord_field, points, q_geom, PETSC_TRUE, &fgeom));
        PETSC_CHECK(PetscQuadratureDestroy(&q_geom));
        points.restore_indices();
        points.destroy();
        PETSC_CHECK(PetscFree4(u, u_t, elem_mat, a));
    }
    if (plex)
        PETSC_CHECK(DMDestroy(&plex));
    if (plexA)
        PETSC_CHECK(DMDestroy(&plexA));
}

void
FENonlinearProblem::on_initial()
{
    CALL_STACK_MSG();
    this->state = INITIAL;
    NonlinearProblem::on_initial();
    compute_aux_fields();
}

void
FENonlinearProblem::on_final()
{
    CALL_STACK_MSG();
    this->state = FINAL;
    NonlinearProblem::on_final();
}

Real
FENonlinearProblem::get_time() const
{
    CALL_STACK_MSG();
    if (this->state == INITIAL)
        return 0.;
    else
        return 1.;
}

void
FENonlinearProblem::compute_solution_vector_local()
{
    CALL_STACK_MSG();
    auto loc_sln = get_solution_vector_local();
    global_to_local(get_solution_vector(), INSERT_VALUES, loc_sln);
    this->compute_boundary_delegate.invoke(loc_sln);
}

} // namespace godzilla
