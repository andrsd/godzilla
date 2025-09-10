#include "TestMesh2D.h"

using namespace godzilla;

Parameters
TestMesh2D::parameters()
{
    auto params = MeshObject::parameters();
    return params;
}

TestMesh2D::TestMesh2D(const Parameters & parameters) : MeshObject(parameters) {}

Qtr<Mesh>
TestMesh2D::create_mesh()
{
    const Int N_ELEM_NODES = 3;
    std::vector<Int> cells = { 0, 1, 2, 1, 3, 2 };
    std::vector<Real> coords = { 0, 0, 1, 0, 0, 1, 1, 1 };
    auto m = UnstructuredMesh::build_from_cell_list(get_comm(),
                                                    2_D,
                                                    N_ELEM_NODES,
                                                    cells,
                                                    2_D,
                                                    coords,
                                                    true);

    // create "side sets"
    m->create_label("Face Sets");
    auto face_sets = m->get_label("Face Sets");

    create_side_set(m, face_sets, 1, { 8 }, "left");
    create_side_set(m, face_sets, 2, { 6 }, "bottom");
    create_side_set(m, face_sets, 3, { 9, 10 }, "top_right");

    std::map<Int, std::string> face_set_names;
    face_set_names[1] = "left";
    face_set_names[2] = "bottom";
    face_set_names[3] = "top_right";
    m->create_face_set_labels(face_set_names);
    for (auto it : face_set_names)
        m->set_face_set_name(it.first, it.second);

    return m;
}

void
TestMesh2D::create_side_set(Qtr<UnstructuredMesh> & mesh,
                            Label & face_sets,
                            Int id,
                            const std::vector<Int> & faces,
                            const char * name)
{
    for (auto & f : faces) {
        face_sets.set_value(f, id);
        PETSC_CHECK(DMSetLabelValue(mesh->get_dm(), name, f, id));
    }
}
