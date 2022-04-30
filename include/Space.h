#pragma once

#include "Mesh.h"
#include "Shapeset.h"
#include "AssemblyList.h"

namespace godzilla {

class BoundaryCondition;

/// Boundary condition types
enum BoundaryConditionType {
    /// Essential (Dirichlet) BC
    BC_ESSENTIAL,
    /// Natural (Neumann, Newton) BC
    BC_NATURAL,
    /// Do-nothing BC
    BC_NONE
};

/// Base class for function spaces
class Space {
public:
    static const uint MARKER_UNDEFINED = (uint) -1;

    // Dirichlet lift is a special DOF with nubmer -1
    static const PetscInt DIRICHLET_DOF = (PetscInt) -1;
    static const PetscInt DOF_UNASSIGNED = (PetscInt) -2;
    static const PetscInt DOF_NOT_ANALYZED = (PetscInt) -3;

public:
    Space(Mesh * mesh, Shapeset * shapeset);
    virtual ~Space();

    const Shapeset * get_shapeset() const;

    /// Set order on all elements
    void set_uniform_order(uint order);

    virtual void assign_dofs(uint first_dof = 0, uint stride = 1);

    /// Get number of degrees of freedom
    uint
    get_dof_count() const
    {
        return (this->next_dof - this->first_dof) / this->stride;
    }

    /// Get maximum number of degrees of freedom
    uint
    get_max_dof() const
    {
        return this->next_dof - this->stride;
    }

    virtual void get_element_assembly_list(const Element * e, AssemblyList * al) = 0;

    virtual void add_boundary_condition(const BoundaryCondition * bc);

protected:
    /// Data associated with a node (vertex, edge, face, interior)
    struct NodeData {
        /// boundary condition marker
        uint marker;
        /// boundary condition type
        BoundaryConditionType bc_type;

        NodeData() : marker(MARKER_UNDEFINED), bc_type(BC_NONE) {}
    };

    struct VertexData : public NodeData {
        /// Number of first degree of freedom
        PetscInt dof;
        /// Number of DOFs
        uint n;
        /// Projection of boundary condition
        Scalar bc_proj;

        VertexData() : dof(DOF_NOT_ANALYZED), n(0), bc_proj(0.) {}
    };

    struct EdgeData : public NodeData {
        /// Polynomial order
        uint order;
        /// Number of first degree of freedom
        PetscInt dof;
        /// Number of DOFs
        uint n;
        /// Projection of boundary condition
        Scalar * bc_proj;

        EdgeData(uint order) : order(order), dof(DOF_NOT_ANALYZED), n(0), bc_proj(nullptr) {}
        virtual ~EdgeData() { delete[] this->bc_proj; }
    };

    struct FaceData : public NodeData {
        /// Polynomial order
        uint order;
        /// Number of first degree of freedom
        PetscInt dof;
        /// Number of DOFs
        uint n;
        /// Projection of boundary condition
        Scalar * bc_proj;

        FaceData(uint order) : order(order), dof(DOF_NOT_ANALYZED), n(0), bc_proj(nullptr) {}
        virtual ~FaceData() { delete[] this->bc_proj; }
    };

    struct ElementData {
        /// Polynomial degree associated to the element node (interior).
        uint order;
        /// The number of the first degree of freedom belonging to the node.
        PetscInt dof;
        /// Total number of degrees of freedom belonging to the node.
        uint n;

        ElementData(uint order) : order(order), dof(DOF_NOT_ANALYZED), n(0) {}
    };

    /// Information about a side boundary
    struct BoundaryInfo {
        BoundaryInfo(const Index & id, const uint & marker) :
            id(id),
            marker(marker)
        {
        }

        /// ID (vertex, edge or face)
        Index id;
        /// Marker
        uint marker;
    };

    /// Initialize data tables
    void init_data_tables();

    /// Free data tables
    void free_data_tables();

    /// Enforce minimum rule
    void enforce_minimum_rule();

    /// Set boundary condition information
    void set_bc_information();

    /// Set boundary condition information at a node
    ///
    /// @param[in] node Node data to modify
    /// @param[in] bc_type Type of the boundary condition
    /// @param[in] marker Marker associated with the boundary
    void set_bc_info(NodeData * node, BoundaryConditionType bc_type, uint marker);

    /// Update constraints
    void update_constraints();

    /// Calculate boundary condition projection at a vertex
    ///
    /// @param[in] ivertex Vertex ID
    virtual void calc_vertex_boundary_projection(Index vtx_idx) = 0;

    /// Calculate boundary condition projection on an edge
    ///
    /// @param[in] elem Element
    /// @param[in] iedge Local vertex number of a element `elem`
    virtual void calc_edge_boundary_projection(const Element * elem, uint iedge) = 0;

    /// Calculate boundary condition projection on a face
    ///
    /// @param[in] elem Element
    /// @param[in] iface Local face number of a element `elem`
    virtual void calc_face_boundary_projection(const Element * elem, uint iface) = 0;

    /// Implement this to assign DoFs
    virtual void assign_dofs_internal() = 0;

    /// Get number of vertex DoFs
    virtual uint get_vertex_ndofs() = 0;
    /// Get number of edge DoFs
    virtual uint get_edge_ndofs(uint order) = 0;
    /// Get number of face DoFs
    virtual uint get_face_ndofs(uint order) = 0;
    /// Get number of element DoFs
    virtual uint get_element_ndofs(uint order) = 0;

    virtual void assign_vertex_dofs(PetscInt vertex_id);
    virtual void assign_edge_dofs(EdgeData * node);
    virtual void assign_face_dofs(FaceData * node);
    virtual void assign_bubble_dofs(ElementData * node);

    /// Build assembly list associated with a vertex
    ///
    /// @param[in] e Element
    /// @param[in] ivertex Local vertex number
    /// @param[out] al Assembly list
    virtual void get_vertex_assembly_list(const Element * e, uint ivertex, AssemblyList * al);

    /// Build assembly list associated with an edge
    ///
    /// @param[in] e Element
    /// @param[in] iedge Local edge number
    /// @param[out] al Assembly list
    virtual void get_edge_assembly_list(const Element * e, uint iedge, AssemblyList * al);

    /// Build assembly list associated with a face
    ///
    /// @param[in] e Element
    /// @param[in] iface Local face number
    /// @param[out] al Assembly list
    virtual void get_face_assembly_list(const Element * e, uint iface, AssemblyList * al);

    /// Build assembly list associated with an element
    ///
    /// @param[in] e Element
    /// @param[out] al Assembly list
    virtual void get_bubble_assembly_list(const Element * e, AssemblyList * al);

    /// Mesh
    const Mesh * mesh;
    /// Shapeset
    const Shapeset * shapeset;

    /// First degree of freedom
    uint first_dof;
    /// Next degree of freedom
    uint next_dof;
    /// Degree of freedom stride
    uint stride;

    /// Vertex node data
    Array<VertexData *> vertex_data;
    /// Edge node data
    Array<EdgeData *> edge_data;
    /// Face node data
    Array<FaceData *> face_data;
    /// Element node data
    Array<ElementData *> elem_data;

    /// Boundary info
    Array<const BoundaryInfo *> side_boundaries;
    /// Map from markers to boundary condition objects
    Array<const BoundaryCondition *> marker_to_bcs;
};

} // namespace godzilla
