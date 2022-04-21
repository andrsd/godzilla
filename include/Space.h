#pragma once

#include "Mesh.h"
#include "Shapeset.h"

namespace godzilla {

/// Boundary condition types
enum EBCType {
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
    static const uint DIRICHLET_DOF = (uint) -1;
    static const uint DOF_UNASSIGNED = (uint) -2;
    static const uint DOF_NOT_ANALYZED = (uint) -3;

public:
    Space(Mesh * mesh, Shapeset * shapeset);
    virtual ~Space();

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

protected:
    /// Initialize data tables
    void init_data_tables();

    /// Free data tables
    void free_data_tables();

    /// Enforce minimum rule
    void enforce_minimum_rule();

    /// Set boundary condition inromation
    void set_bc_information();

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

    virtual void assign_vertex_dofs(Index vid);
    virtual void assign_edge_dofs(Index eid);
    virtual void assign_face_dofs(Index fid);
    virtual void assign_bubble_dofs(Index eid);

    /// Data associated with a node (vertex, edge, face, interior)
    struct NodeData {
        /// boundary condition marker
        uint marker;
        /// boundary condition type
        EBCType bc_type;

        NodeData() : marker(MARKER_UNDEFINED), bc_type(BC_NONE) {}
    };

    struct VertexData : public NodeData {
        /// Number of first degree of freedom
        uint dof;
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
        uint dof;
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
        uint dof;
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
        uint dof;
        /// Total number of degrees of freedom belonging to the node.
        uint n;

        ElementData(uint order) : order(order), dof(DOF_NOT_ANALYZED), n(0) {}
    };

    /// Mesh
    const Mesh * mesh;
    /// Shapeset
    const Shapeset * shapeset;

    /// First degree of freedom
    uint first_dof;
    /// Next degree of freedom
    uint next_dof;
    ///  degree of freedom stride
    uint stride;

    /// Vertex node data
    Array<VertexData *> vertex_data;
    /// Edge node data
    Array<EdgeData *> edge_data;
    /// Face node data
    Array<FaceData *> face_data;
    /// Element node data
    Array<ElementData *> elem_data;
};

} // namespace godzilla
