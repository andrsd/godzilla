// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "cxxopts/cxxopts.hpp"
#include "fmt/printf.h"
#include "godzilla/Exception.h"
#include "godzilla/Init.h"
#include "godzilla/App.h"
#include "godzilla/String.h"
#include "godzilla/CommandLineInterface.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/FileMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Partitioner.h"
#include "godzilla/IO.h"
#include "godzilla/CallStack.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include "petscdmplex.h"
#include "petscsf.h"
#include <exodusII.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

using namespace godzilla;

const String version = "1.0";

mpi::Communicator comm_world;

struct GlobalExodusInfo {
    std::string title;
    int dim = 0;
    int64_t num_nodes_g = 0;
    int64_t num_elems_g = 0;
    int64_t num_elem_blks_g = 0;
    int64_t num_node_sets_g = 0;
    int64_t num_side_sets_g = 0;

    std::vector<std::string> coord_names;

    // Element blocks
    std::vector<int> el_blk_ids;
    std::vector<int> el_blk_cnts;
    std::vector<std::string> el_blk_elem_types;
    std::vector<int64_t> el_blk_nodes_per_elem;
    std::vector<std::string> el_blk_names;

    // Node sets
    std::vector<int> ns_ids;
    std::vector<int> ns_cnts;
    std::vector<int> ns_df_cnts;
    std::vector<std::string> ns_names;

    // Side sets
    std::vector<int> ss_ids;
    std::vector<int> ss_cnts;
    std::vector<int> ss_df_cnts;
    std::vector<std::string> ss_names;
};

static void
read_global_exodus_info(const std::string & file_name,
                        GlobalExodusInfo & info,
                        std::vector<int> & elem_map,
                        std::vector<int> & node_map)
{
    exodusIIcpp::File exo(file_name, exodusIIcpp::FileAccess::READ);

    exo.init();
    auto num_dim = exo.get_dim();
    auto num_nodes = exo.get_num_nodes();
    auto num_elem = exo.get_num_elements();
    auto num_elem_blk = exo.get_num_element_blocks();
    auto num_node_sets = exo.get_num_node_sets();
    auto num_side_sets = exo.get_num_side_sets();

    info.title = exo.get_title();
    info.dim = static_cast<int>(num_dim);
    info.num_nodes_g = num_nodes;
    info.num_elems_g = num_elem;
    info.num_elem_blks_g = num_elem_blk;
    info.num_node_sets_g = num_node_sets;
    info.num_side_sets_g = num_side_sets;

    // Coordinate names
    exo.read_coord_names();
    info.coord_names = exo.get_coord_names();

    // Element blocks
    info.el_blk_ids.resize(num_elem_blk);
    info.el_blk_cnts.resize(num_elem_blk);
    info.el_blk_elem_types.resize(num_elem_blk);
    info.el_blk_nodes_per_elem.resize(num_elem_blk);
    info.el_blk_names.resize(num_elem_blk);

    if (num_elem_blk > 0) {
        exo.read_blocks();
        for (int i = 0; i < num_elem_blk; ++i) {
            const auto & eb = exo.get_element_block(i);
            info.el_blk_ids[i] = eb.get_id();
            info.el_blk_names[i] = eb.get_name();
            info.el_blk_cnts[i] = eb.get_num_elements();
            info.el_blk_elem_types[i] = eb.get_element_type();
            info.el_blk_nodes_per_elem[i] = eb.get_num_nodes_per_element();
        }
    }

    // Node sets
    if (num_node_sets > 0) {
        info.ns_ids.resize(num_node_sets);
        info.ns_cnts.resize(num_node_sets);
        info.ns_df_cnts.resize(num_node_sets, 0);
        info.ns_names.resize(num_node_sets);

        exo.read_node_sets();
        for (int i = 0; i < num_node_sets; ++i) {
            const auto & ns = exo.get_node_sets()[i];
            info.ns_ids[i] = ns.id;
            info.ns_names[i] = ns.name;
            info.ns_cnts[i] = ns.node_ids.size();
            info.ns_df_cnts[i] = ns.n_distance_factors;
        }
    }

    // Side sets
    if (num_side_sets > 0) {
        info.ss_ids.resize(num_side_sets);
        info.ss_cnts.resize(num_side_sets);
        info.ss_df_cnts.resize(num_side_sets, 0);
        info.ss_names.resize(num_side_sets);

        exo.read_side_sets();
        for (int i = 0; i < num_side_sets; ++i) {
            const auto & ss = exo.get_side_sets()[i];
            info.ss_ids[i] = ss.id;
            info.ss_names[i] = ss.name;
            info.ss_cnts[i] = ss.elem_ids.size();
            info.ss_df_cnts[i] = ss.n_distance_factors;
        }
    }

    // Maps
    exo.read_elem_map();
    elem_map = exo.get_elem_map();

    exo.read_node_map();
    node_map = exo.get_node_map();
}

static void
broadcast_global_info(const mpi::Communicator & comm, GlobalExodusInfo & info)
{
    comm.broadcast(info.title, 0);
    comm.broadcast(info.dim, 0);
    comm.broadcast(info.num_nodes_g, 0);
    comm.broadcast(info.num_elems_g, 0);
    comm.broadcast(info.num_elem_blks_g, 0);
    comm.broadcast(info.num_node_sets_g, 0);
    comm.broadcast(info.num_side_sets_g, 0);

    int n_cnames = static_cast<int>(info.coord_names.size());
    comm.broadcast(n_cnames, 0);
    info.coord_names.resize(n_cnames);
    for (int i = 0; i < n_cnames; ++i)
        comm.broadcast(info.coord_names[i], 0);

    comm.broadcast(info.el_blk_ids, 0);
    comm.broadcast(info.el_blk_cnts, 0);
    comm.broadcast(info.el_blk_nodes_per_elem, 0);
    int n_blks = static_cast<int>(info.num_elem_blks_g);
    info.el_blk_elem_types.resize(n_blks);
    info.el_blk_names.resize(n_blks);
    for (int i = 0; i < n_blks; ++i) {
        comm.broadcast(info.el_blk_elem_types[i], 0);
        comm.broadcast(info.el_blk_names[i], 0);
    }

    if (info.num_node_sets_g > 0) {
        comm.broadcast(info.ns_ids, 0);
        comm.broadcast(info.ns_cnts, 0);
        comm.broadcast(info.ns_df_cnts, 0);
        info.ns_names.resize(info.num_node_sets_g);
        for (int i = 0; i < info.num_node_sets_g; ++i)
            comm.broadcast(info.ns_names[i], 0);
    }

    if (info.num_side_sets_g > 0) {
        comm.broadcast(info.ss_ids, 0);
        comm.broadcast(info.ss_cnts, 0);
        comm.broadcast(info.ss_df_cnts, 0);
        info.ss_names.resize(info.num_side_sets_g);
        for (int i = 0; i < info.num_side_sets_g; ++i)
            comm.broadcast(info.ss_names[i], 0);
    }
}

class MeshPartApp : public App, public CommandLineInterface {
public:
    MeshPartApp(int argc, const char * const * argv);

    String
    get_version() const override
    {
        return version;
    }

    int run() override;

protected:
    cxxopts::Options create_command_line_options();
    cxxopts::ParseResult parse(cxxopts::Options & opts);
    Qtr<UnstructuredMesh> load_mesh(const std::string & file_name);
    void partition_mesh_file(const std::string & mesh_file_name, const std::string & base_name);

    cxxopts::Options cli_opts;
    cxxopts::ParseResult cli_result;
};

MeshPartApp::MeshPartApp(int argc, const char * const * argv) :
    App(comm_world, "mesh-part"),
    CommandLineInterface(*this, argc, argv),
    cli_opts(create_command_line_options()),
    cli_result(parse(this->cli_opts))
{
}

cxxopts::Options
MeshPartApp::create_command_line_options()
{
    cxxopts::Options opts(get_app_name());

    // clang-format off
    opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    opts.add_option("", "v", "version", "Show the version", cxxopts::value<bool>(), "");
    opts.add_option("", "p", "partitioner", "Name of the partitioner", cxxopts::value<std::string>(), "");
    opts.add_option("", "o", "output", "Base name for output files", cxxopts::value<std::string>()->default_value("out"), "");
    opts.add_option("", "", "mesh-file", "The mesh file name", cxxopts::value<std::string>(), "");
    opts.positional_help("<mesh-file>");
    // clang-format on
    return opts;
}

cxxopts::ParseResult
MeshPartApp::parse(cxxopts::Options & opts)
{
    opts.parse_positional({ "mesh-file" });
    return CommandLineInterface::parse(opts);
}

int
MeshPartApp::run()
{
    if (this->cli_result.count("help")) {
        if (get_comm().rank() == 0)
            fmt::print("{}", this->cli_opts.help());
    }
    else if (this->cli_result.count("version")) {
        if (get_comm().rank() == 0)
            fmt::print("{}, version {}\n", get_name(), get_version());
    }
    else if (this->cli_result.count("mesh-file")) {
        std::string base_name = "out";
        if (this->cli_result.count("output"))
            base_name = this->cli_result["output"].as<std::string>();
        partition_mesh_file(this->cli_result["mesh-file"].as<std::string>(), base_name);
    }
    else {
        if (get_comm().rank() == 0)
            fmt::print("{}", this->cli_opts.help());
    }
    return 0;
}

Qtr<UnstructuredMesh>
MeshPartApp::load_mesh(const std::string & file_name)
{
    auto pars = make_parameters<FileMesh>();
    pars.set<fs::path>("file", file_name);
    return MeshFactory::create<FileMesh>(pars);
}

void
MeshPartApp::partition_mesh_file(const std::string & mesh_file_name, const std::string & base_name)
{
    CALL_STACK_MSG();

    auto comm = get_comm();
    auto rank = comm.rank();
    auto n_procs = comm.size();

    GlobalExodusInfo global_info;
    std::vector<int> global_elem_map;
    std::vector<int> global_node_map;

    if (rank == 0)
        read_global_exodus_info(mesh_file_name, global_info, global_elem_map, global_node_map);
    broadcast_global_info(comm, global_info);

    auto mesh = load_mesh(mesh_file_name);

    if (this->cli_result.count("partitioner")) {
        auto part_name = this->cli_result["partitioner"].as<std::string>();
        Partitioner part(comm);
        part.set_type(part_name);
        mesh->set_partitioner(part);
    }

    Int pStart = 0, pEnd = 0;
    if (rank == 0)
        PETSC_CHECK(DMPlexGetChart(mesh->get_dm(), &pStart, &pEnd));

    std::vector<Int> orig_ids;
    if (rank == 0) {
        orig_ids.resize(pEnd - pStart, -1);
        for (int64_t c = 0; c < global_info.num_elems_g; ++c)
            orig_ids[c - pStart] = global_elem_map[c];
        for (int64_t v = 0; v < global_info.num_nodes_g; ++v)
            orig_ids[global_info.num_elems_g + v - pStart] = global_node_map[v];
    }

    DM dm_dist = nullptr;
    PetscSF migration_sf = nullptr;
    PETSC_CHECK(DMPlexDistribute(mesh->get_dm(), 0, &migration_sf, &dm_dist));
    if (dm_dist)
        mesh = Qtr<UnstructuredMesh>::alloc(dm_dist);

    Int pStart_dist = 0, pEnd_dist = 0;
    PETSC_CHECK(DMPlexGetChart(mesh->get_dm(), &pStart_dist, &pEnd_dist));
    std::vector<Int> dist_ids(pEnd_dist - pStart_dist, -1);

    if (migration_sf) {
        PETSC_CHECK(PetscSFBcastBegin(migration_sf,
                                      MPIU_INT,
                                      orig_ids.data(),
                                      dist_ids.data(),
                                      MPI_REPLACE));
        PETSC_CHECK(
            PetscSFBcastEnd(migration_sf, MPIU_INT, orig_ids.data(), dist_ids.data(), MPI_REPLACE));
        PETSC_CHECK(PetscSFDestroy(&migration_sf));
    }
    else {
        dist_ids = orig_ids;
    }

    Int vStart = 0, vEnd = 0;
    PETSC_CHECK(DMPlexGetDepthStratum(mesh->get_dm(), 0, &vStart, &vEnd));

    std::unordered_map<Int, std::vector<int>> node_neighbors;
    if (n_procs > 1) {
        std::vector<std::vector<Int>> send_g(n_procs);
        for (Int v = vStart; v < vEnd; ++v) {
            Int g = dist_ids[v - pStart_dist];
            int target = static_cast<int>((g - 1) % n_procs);
            send_g[target].push_back(g);
        }

        std::vector<int> send_counts(n_procs), recv_counts(n_procs);
        for (int i = 0; i < n_procs; ++i)
            send_counts[i] = static_cast<int>(send_g[i].size());

        PETSC_CHECK(
            MPI_Alltoall(send_counts.data(), 1, MPI_INT, recv_counts.data(), 1, MPI_INT, comm));

        std::vector<int> send_displs(n_procs, 0), recv_displs(n_procs, 0);
        for (int i = 1; i < n_procs; ++i) {
            send_displs[i] = send_displs[i - 1] + send_counts[i - 1];
            recv_displs[i] = recv_displs[i - 1] + recv_counts[i - 1];
        }

        std::vector<Int> flat_send_g(send_displs.back() + send_counts.back());
        for (int i = 0; i < n_procs; ++i)
            std::copy(send_g[i].begin(), send_g[i].end(), flat_send_g.begin() + send_displs[i]);

        std::vector<Int> flat_recv_g(recv_displs.back() + recv_counts.back());
        PETSC_CHECK(MPI_Alltoallv(flat_send_g.data(),
                                  send_counts.data(),
                                  send_displs.data(),
                                  MPIU_INT,
                                  flat_recv_g.data(),
                                  recv_counts.data(),
                                  recv_displs.data(),
                                  MPIU_INT,
                                  comm));

        std::unordered_map<Int, std::vector<int>> sharing;
        for (int p = 0; p < n_procs; ++p) {
            for (int idx = recv_displs[p]; idx < recv_displs[p] + recv_counts[p]; ++idx) {
                Int g = flat_recv_g[idx];
                sharing[g].push_back(p);
            }
        }

        std::vector<std::vector<Int>> reply_to_rank(n_procs);
        for (int p = 0; p < n_procs; ++p) {
            for (int idx = recv_displs[p]; idx < recv_displs[p] + recv_counts[p]; ++idx) {
                Int g = flat_recv_g[idx];
                for (int q : sharing[g]) {
                    if (q != p) {
                        reply_to_rank[p].push_back(g);
                        reply_to_rank[p].push_back(q);
                    }
                }
            }
        }

        for (int p = 0; p < n_procs; ++p)
            send_counts[p] = static_cast<int>(reply_to_rank[p].size());

        PETSC_CHECK(
            MPI_Alltoall(send_counts.data(), 1, MPI_INT, recv_counts.data(), 1, MPI_INT, comm));

        for (int i = 1; i < n_procs; ++i) {
            send_displs[i] = send_displs[i - 1] + send_counts[i - 1];
            recv_displs[i] = recv_displs[i - 1] + recv_counts[i - 1];
        }

        std::vector<Int> flat_send_reply(send_displs.back() + send_counts.back());
        for (int i = 0; i < n_procs; ++i)
            std::copy(reply_to_rank[i].begin(),
                      reply_to_rank[i].end(),
                      flat_send_reply.begin() + send_displs[i]);

        std::vector<Int> flat_recv_reply(recv_displs.back() + recv_counts.back());
        PETSC_CHECK(MPI_Alltoallv(flat_send_reply.data(),
                                  send_counts.data(),
                                  send_displs.data(),
                                  MPIU_INT,
                                  flat_recv_reply.data(),
                                  recv_counts.data(),
                                  recv_displs.data(),
                                  MPIU_INT,
                                  comm));

        for (size_t idx = 0; idx < flat_recv_reply.size(); idx += 2) {
            Int g = flat_recv_reply[idx];
            int neighbor = static_cast<int>(flat_recv_reply[idx + 1]);
            node_neighbors[g].push_back(neighbor);
        }
    }

    std::vector<Int> internal_vertices;
    std::vector<Int> border_vertices;
    for (Int v = vStart; v < vEnd; ++v) {
        Int g = dist_ids[v - pStart_dist];
        if (node_neighbors[g].empty())
            internal_vertices.push_back(v);
        else
            border_vertices.push_back(v);
    }

    int64_t num_int_nodes = internal_vertices.size();
    int64_t num_bor_nodes = border_vertices.size();
    int64_t num_nodes_local = num_int_nodes + num_bor_nodes;

    std::unordered_map<Int, int> local_node_id;
    std::vector<int> node_mapi(num_int_nodes);
    std::vector<int> node_mapb(num_bor_nodes);
    std::vector<int> exo_node_id_map(num_nodes_local);

    for (int64_t i = 0; i < num_int_nodes; ++i) {
        Int v = internal_vertices[i];
        local_node_id[v] = i + 1;
        node_mapi[i] = dist_ids[v - pStart_dist];
        exo_node_id_map[i] = node_mapi[i];
    }
    for (int64_t i = 0; i < num_bor_nodes; ++i) {
        Int v = border_vertices[i];
        local_node_id[v] = num_int_nodes + i + 1;
        node_mapb[i] = dist_ids[v - pStart_dist];
        exo_node_id_map[num_int_nodes + i] = node_mapb[i];
    }

    std::vector<double> x(num_nodes_local, 0.0);
    std::vector<double> y(num_nodes_local, 0.0);
    std::vector<double> z(num_nodes_local, 0.0);

    auto dm_coord = mesh->get_coordinate_dm();
    auto coord_vec = mesh->get_coordinates_local();
    auto coord_array = coord_vec.borrow_array_read();
    int dim = global_info.dim;

    for (Int v : internal_vertices) {
        Real * data = nullptr;
        PETSC_CHECK(DMPlexPointLocalRead(dm_coord, v, coord_array.data(), &data));
        int64_t idx = local_node_id[v] - 1;
        x[idx] = data[0];
        if (dim >= 2)
            y[idx] = data[1];
        if (dim >= 3)
            z[idx] = data[2];
    }
    for (Int v : border_vertices) {
        Real * data = nullptr;
        PETSC_CHECK(DMPlexPointLocalRead(dm_coord, v, coord_array.data(), &data));
        int64_t idx = local_node_id[v] - 1;
        x[idx] = data[0];
        if (dim >= 2)
            y[idx] = data[1];
        if (dim >= 3)
            z[idx] = data[2];
    }

    std::vector<int> elem_mapi;
    std::vector<int> elem_mapb;
    std::vector<int> exo_elem_id_map;
    std::unordered_map<Int, int64_t> local_elem_id;

    struct LocalBlockInfo {
        int64_t blk_id = 0;
        int64_t num_elems = 0;
        int64_t nodes_per_elem = 0;
        std::string elem_type;
        std::vector<int> conn;
    };
    std::vector<LocalBlockInfo> local_blocks;

    int64_t cur_local_elem = 0;
    auto dm = mesh->get_dm();
    auto cell_sets_label = mesh->get_label("Cell Sets");

    for (size_t b = 0; b < global_info.el_blk_ids.size(); ++b) {
        int64_t blk_id = global_info.el_blk_ids[b];
        std::string elem_type = global_info.el_blk_elem_types[b];
        int64_t nodes_per_elem = global_info.el_blk_nodes_per_elem[b];

        LocalBlockInfo lbi;
        lbi.blk_id = blk_id;
        lbi.nodes_per_elem = nodes_per_elem;
        lbi.elem_type = elem_type;

        std::vector<Int> blk_cells;
        if (cell_sets_label) {
            auto stratum = cell_sets_label.get_stratum(blk_id);
            if (stratum) {
                auto idxs = stratum.borrow_indices();
                blk_cells.assign(idxs.begin(), idxs.end());
            }
        }
        else {
            Int cStart = 0, cEnd = 0;
            PETSC_CHECK(DMPlexGetHeightStratum(dm, 0, &cStart, &cEnd));
            for (Int c = cStart; c < cEnd; ++c)
                blk_cells.push_back(c);
        }

        lbi.num_elems = blk_cells.size();
        for (Int c : blk_cells) {
            Int gid = dist_ids[c - pStart_dist];
            cur_local_elem++;
            local_elem_id[c] = cur_local_elem;
            exo_elem_id_map.push_back(gid);

            Int closure_size = 0;
            Int * closure = nullptr;
            PETSC_CHECK(DMPlexGetTransitiveClosure(dm, c, PETSC_TRUE, &closure_size, &closure));
            auto polytope_type = mesh->get_cell_type(c);
            auto ordering = io::get_elem_node_ordering(polytope_type);
            int n_nodes_per_elem = UnstructuredMesh::get_num_cell_nodes(polytope_type);

            bool is_border_elem = false;
            std::vector<int64_t> elem_conn(n_nodes_per_elem);
            for (int k = 0; k < n_nodes_per_elem; ++k) {
                Int l = 2 * (closure_size - n_nodes_per_elem + ordering[k]);
                Int v = closure[l];
                int64_t nid = local_node_id[v];
                elem_conn[k] = nid;
                if (nid > num_int_nodes)
                    is_border_elem = true;
            }
            PETSC_CHECK(DMPlexRestoreTransitiveClosure(dm, c, PETSC_TRUE, &closure_size, &closure));

            if (is_border_elem)
                elem_mapb.push_back(gid);
            else
                elem_mapi.push_back(gid);

            for (auto nid : elem_conn)
                lbi.conn.push_back(nid);
        }
        local_blocks.push_back(std::move(lbi));
    }

    int64_t num_int_elems = elem_mapi.size();
    int64_t num_bor_elems = elem_mapb.size();
    int64_t num_elems_local = num_int_elems + num_bor_elems;

    // Communication maps
    std::map<int, std::vector<int>> cmap_nodes_by_neighbor;
    for (Int v : border_vertices) {
        Int g = dist_ids[v - pStart_dist];
        int64_t loc_nid = local_node_id[v];
        for (int q : node_neighbors[g]) {
            cmap_nodes_by_neighbor[q].push_back(loc_nid);
        }
    }

    int64_t num_node_cmaps = cmap_nodes_by_neighbor.size();
    std::vector<int> node_cmap_ids;
    std::vector<int> node_cmap_cnts;
    for (const auto & [neighbor_q, nodes] : cmap_nodes_by_neighbor) {
        node_cmap_ids.push_back(neighbor_q);
        node_cmap_cnts.push_back(nodes.size());
    }

    // Node sets
    struct LocalNodeSetInfo {
        int64_t id = 0;
        std::vector<int> nodes;
    };
    std::vector<LocalNodeSetInfo> local_node_sets;

    if (global_info.num_node_sets_g > 0) {
        auto vertex_sets_label = mesh->get_label("Vertex Sets");
        for (auto ns_id : global_info.ns_ids) {
            LocalNodeSetInfo lns;
            lns.id = ns_id;
            if (vertex_sets_label) {
                auto stratum = vertex_sets_label.get_stratum(ns_id);
                if (stratum) {
                    auto indices = stratum.borrow_indices();
                    for (Int v : indices) {
                        if (local_node_id.count(v) > 0)
                            lns.nodes.push_back(local_node_id[v]);
                    }
                }
            }
            local_node_sets.push_back(std::move(lns));
        }
    }

    // Side sets
    struct LocalSideSetInfo {
        int64_t id = 0;
        std::vector<int> elems;
        std::vector<int> sides;
    };
    std::vector<LocalSideSetInfo> local_side_sets;

    if (global_info.num_side_sets_g > 0) {
        auto face_sets_label = mesh->get_label("Face Sets");
        for (auto ss_id : global_info.ss_ids) {
            LocalSideSetInfo lss;
            lss.id = ss_id;
            if (face_sets_label) {
                auto stratum = face_sets_label.get_stratum(ss_id);
                if (stratum) {
                    auto faces = stratum.borrow_indices();
                    for (Int f : faces) {
                        Int num_points = 0;
                        Int * points = nullptr;
                        PETSC_CHECK(
                            DMPlexGetTransitiveClosure(dm, f, PETSC_FALSE, &num_points, &points));
                        Int el = points[2];
                        PETSC_CHECK(DMPlexRestoreTransitiveClosure(dm,
                                                                   f,
                                                                   PETSC_FALSE,
                                                                   &num_points,
                                                                   &points));

                        if (local_elem_id.count(el) > 0) {
                            int64_t loc_el = local_elem_id[el];
                            points = nullptr;
                            PETSC_CHECK(DMPlexGetTransitiveClosure(dm,
                                                                   el,
                                                                   PETSC_TRUE,
                                                                   &num_points,
                                                                   &points));
                            auto ct = mesh->get_cell_type(el);
                            auto side_ordering = io::get_elem_side_ordering(ct);
                            for (Int j = 1; j < num_points; ++j) {
                                if (points[j * 2] == f) {
                                    lss.elems.push_back(loc_el);
                                    lss.sides.push_back(side_ordering[j - 1]);
                                    break;
                                }
                            }
                            PETSC_CHECK(DMPlexRestoreTransitiveClosure(dm,
                                                                       el,
                                                                       PETSC_TRUE,
                                                                       &num_points,
                                                                       &points));
                        }
                    }
                }
            }
            local_side_sets.push_back(std::move(lss));
        }
    }

    // Write Exodus file
    auto out_file_name = fmt::format("{}.{}.exo", base_name, rank);

    exodusIIcpp::File exo(out_file_name, exodusIIcpp::FileAccess::WRITE);

    // 1. Local init
    exo.init(global_info.title.c_str(),
             global_info.dim,
             num_nodes_local,
             num_elems_local,
             global_info.num_elem_blks_g,
             global_info.num_node_sets_g,
             global_info.num_side_sets_g);

    // 2. Global metadata definitions
    exo.init_info(n_procs, 1, "P");
    exo.init_global(global_info.num_nodes_g,
                    global_info.num_elems_g,
                    global_info.num_elem_blks_g,
                    global_info.num_node_sets_g,
                    global_info.num_side_sets_g);

    // 3. Global block/set parameters
    exo.write_elem_block_info_global(global_info.el_blk_ids, global_info.el_blk_cnts);

    if (global_info.num_node_sets_g > 0) {
        exo.write_node_set_param_global(global_info.ns_ids,
                                        global_info.ns_cnts,
                                        global_info.ns_df_cnts);
    }
    if (global_info.num_side_sets_g > 0) {
        exo.write_side_set_param_global(global_info.ss_ids,
                                        global_info.ss_cnts,
                                        global_info.ss_df_cnts);
    }

    // 4. Load balance & Communication parameters
    exo.write_loadbal_param(num_int_nodes,
                            num_bor_nodes,
                            0,
                            num_int_elems,
                            num_bor_elems,
                            num_node_cmaps,
                            0,
                            0);

    if (num_node_cmaps > 0) {
        exo.write_cmap_params(node_cmap_ids, node_cmap_cnts, {}, {}, 0);
        for (const auto & [neighbor_q, nodes] : cmap_nodes_by_neighbor) {
            std::vector<int> proc_ids(nodes.size(), neighbor_q);
            exo.write_node_cmap(neighbor_q, nodes, proc_ids, 0);
        }
    }

    exo.write_processor_node_maps(node_mapi, node_mapb, {}, 0);

    exo.write_processor_elem_maps(elem_mapi, elem_mapb, 0);

    // 5. Bulk data: Coordinates
    exo.write_coord_names(global_info.coord_names);

    exo.write_coords(x, y, z);

    // 6. Bulk data: Element Blocks
    for (size_t i = 0; i < local_blocks.size(); ++i) {
        const auto & lb = local_blocks[i];
        exo.write_block(lb.blk_id, lb.elem_type.c_str(), lb.num_elems, lb.conn);
    }
    exo.write_block_names(global_info.el_blk_names);

    // 7. Bulk data: Node Sets
    if (global_info.num_node_sets_g > 0) {
        for (size_t i = 0; i < local_node_sets.size(); ++i) {
            const auto & lns = local_node_sets[i];
            if (lns.nodes.empty())
                exo.write_node_set(lns.id, {});
            else
                exo.write_node_set(lns.id, lns.nodes);
        }
        exo.write_node_set_names(global_info.ns_names);
    }

    // 8. Bulk data: Side Sets
    if (global_info.num_side_sets_g > 0) {
        for (size_t i = 0; i < local_side_sets.size(); ++i) {
            const auto & lss = local_side_sets[i];
            if (lss.elems.empty())
                exo.write_side_set(lss.id, {}, {});
            else
                exo.write_side_set(lss.id, lss.elems, lss.sides);
        }
        exo.write_side_set_names(global_info.ss_names);
    }

    // 9. Global Exodus ID maps
    if (num_nodes_local > 0)
        exo.write_node_map(exo_node_id_map);
    if (num_elems_local > 0)
        exo.write_elem_map(exo_elem_id_map);
}

// ---

int
main(int argc, char * argv[])
{
    Init init(argc, argv);
    MeshPartApp app(argc, argv);
    try {
        app.run();
        return 0;
    }
    catch (std::exception & e) {
        fmt::println("{}", e.what());
        return -1;
    }
}
