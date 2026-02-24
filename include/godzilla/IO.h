// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Enums.h"
#include "godzilla/String.h"
#include "godzilla/Span.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include <string>
#include <vector>

namespace godzilla {

class UnstructuredMesh;
class DiscreteProblemInterface;
class FEProblemInterface;
class FVProblemInterface;
class DGProblemInterface;

namespace io {

/// Block ID used in ExodusII file when there are no cell sets
constexpr int SINGLE_BLK_ID = 0;

// High-level mesh writing

/// Write the full mesh (coordinates + connectivity + node/face sets) for a
/// continuous (CG) problem.
void write_mesh(exodusIIcpp::File & f, const UnstructuredMesh & mesh);

/// Write the full mesh for a discontinuous (DG) problem.
/// Nodes are duplicated per element; no node/face sets are written.
void write_mesh_discontinuous(exodusIIcpp::File & f, const UnstructuredMesh & mesh);

/// Write all node sets (vertex sets) from the mesh.
void write_node_sets(exodusIIcpp::File & f, const UnstructuredMesh & mesh);

/// Write all face/side sets from the mesh.
void write_face_sets(exodusIIcpp::File & f, const UnstructuredMesh & mesh);

// Variable declaration

/// Get names of variables to use in the output files
///
/// @param dpi Problem with the the field variables
/// @param fid Field ID
/// @return List of variable names
///
/// For a single-component variable we just return the field name. For multi-component
/// variable we return names representing each component (for example, 2D velocity field
/// could return "velocity_x" and "velocity_y")
std::vector<String> get_var_names(DiscreteProblemInterface & dpi, FieldID fid);

/// Get names of auxiliary variables to use in the output files
///
/// @param dpi Problem with the the field variables
/// @param fid Field ID
/// @return List of variable names
///
/// For a single-component variable we just return the field name. For multi-component
/// variable we return names representing each component (for example, 2D velocity field
/// could return "velocity_x" and "velocity_y")
std::vector<String> get_aux_var_names(DiscreteProblemInterface & dpi, FieldID fid);

// Solution writing

/// Write field values at one time step.
///
/// @param f ExodusII file to write to
/// @param fepi Problem interface with solution
/// @param step_num Time step number
/// @param time Current time value
/// @param fid Field ID
/// @param exo_var_id ExodusII variable ID to write into
void write_field_values(exodusIIcpp::File & f,
                        const DiscreteProblemInterface & dpi,
                        int step_num,
                        Real time,
                        FieldID field_id,
                        int exo_var_id);

void write_field_values(exodusIIcpp::File & f,
                        const DGProblemInterface & dgpi,
                        int step_num,
                        Real time,
                        FieldID field_id,
                        int exo_var_id);

/// Write field values at one time step.
///
/// @param f ExodusII file to write to
/// @param dpi Problem interface with solution
/// @param step_num Time step number
/// @param time Current time value
/// @param fid Auxiliary field ID
/// @param exo_var_id ExodusII variable ID to write into
void write_aux_field_values(exodusIIcpp::File & f,
                            const DiscreteProblemInterface & dpi,
                            int step_num,
                            Real time,
                            FieldID field_id,
                            int exo_var_id);

void write_aux_field_values(exodusIIcpp::File & f,
                            const DGProblemInterface & dgpi,
                            int step_num,
                            Real time,
                            FieldID field_id,
                            int exo_var_id);

void write_elemental_field_values(exodusIIcpp::File & f,
                                  DiscreteProblemInterface & dpi,
                                  int step_num,
                                  Real time,
                                  FieldID fid,
                                  int exo_var_id);

void write_aux_elemental_field_values(exodusIIcpp::File & f,
                                      DiscreteProblemInterface & dpi,
                                      int step_num,
                                      Real time,
                                      FieldID fid,
                                      int exo_var_id);

/// Map PolytopeType -> ExodusII element type string ("TRI3", "HEX8", ...).
const char * get_elem_type(PolytopeType elem_type);

/// Node reordering permutation: DMPLEX order -> ExodusII order.
Span<const Int> get_elem_node_ordering(PolytopeType elem_type);

/// Side reordering permutation for ExodusII side numbering.
Span<const Int> get_elem_side_ordering(PolytopeType elem_type);

} // namespace io
} // namespace godzilla
