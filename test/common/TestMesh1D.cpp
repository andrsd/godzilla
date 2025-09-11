#include "TestMesh1D.h"

using namespace godzilla;

Parameters
TestMesh1D::parameters()
{
    Parameters params = MeshObject::parameters();
    return params;
}

TestMesh1D::TestMesh1D(const Parameters & parameters) : MeshObject(parameters) {}

Qtr<Mesh>
TestMesh1D::create_mesh()
{
    const Int N_ELEM_NODES = 2;
    std::vector<Int> cells = { 0, 1, 1, 2 };
    std::vector<Real> coords = { 0, 0.4, 1 };
    auto m = UnstructuredMesh::build_from_cell_list(get_comm(),
                                                    1_D,
                                                    N_ELEM_NODES,
                                                    cells,
                                                    1_D,
                                                    coords,
                                                    true);

    // create "side sets"
    m->create_label("Face Sets");
    auto face_sets = m->get_label("Face Sets");

    create_side_set(m, face_sets, 1, { 2 }, "left");
    create_side_set(m, face_sets, 2, { 4 }, "right");

    std::map<Int, std::string> face_set_names;
    face_set_names[1] = "left";
    face_set_names[2] = "right";
    m->create_face_set_labels(face_set_names);
    for (auto it : face_set_names)
        m->set_face_set_name(it.first, it.second);

    return m;
}

void
TestMesh1D::create_side_set(Qtr<UnstructuredMesh> & mesh,
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
