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

#define EXODUS_CHECK(call)                                                                      \
    do {                                                                                        \
        int ierr = (call);                                                                      \
        if (ierr < 0)                                                                           \
            throw Exception(fmt::format("Exodus error {} at {}:{}", ierr, __FILE__, __LINE__)); \
    } while (0)

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
    std::vector<int64_t> el_blk_ids;
    std::vector<int64_t> el_blk_cnts;
    std::vector<std::string> el_blk_elem_types;
    std::vector<int64_t> el_blk_nodes_per_elem;
    std::vector<std::string> el_blk_names;

    // Node sets
    std::vector<int64_t> ns_ids;
    std::vector<int64_t> ns_cnts;
    std::vector<int64_t> ns_df_cnts;
    std::vector<std::string> ns_names;

    // Side sets
    std::vector<int64_t> ss_ids;
    std::vector<int64_t> ss_cnts;
    std::vector<int64_t> ss_df_cnts;
    std::vector<std::string> ss_names;
};

static void
read_global_exodus_info(const std::string & file_name,
                        GlobalExodusInfo & info,
                        std::vector<int64_t> & elem_map,
                        std::vector<int64_t> & node_map)
{
    int cpu_ws = sizeof(double);
    int io_ws = 0;
    float ver = 0.0f;
    int exoid = ex_open(file_name.c_str(), EX_READ, &cpu_ws, &io_ws, &ver);
    if (exoid < 0)
        throw Exception(fmt::format("Unable to open Exodus file: {}", file_name));

    ex_set_int64_status(exoid, EX_ALL_INT64_API);

    char title[MAX_LINE_LENGTH + 1] = { 0 };
    int64_t num_dim = 0, num_nodes = 0, num_elem = 0, num_elem_blk = 0;
    int64_t num_node_sets = 0, num_side_sets = 0;

    EXODUS_CHECK(ex_get_init(exoid,
                             title,
                             &num_dim,
                             &num_nodes,
                             &num_elem,
                             &num_elem_blk,
                             &num_node_sets,
                             &num_side_sets));

    info.title = title;
    info.dim = static_cast<int>(num_dim);
    info.num_nodes_g = num_nodes;
    info.num_elems_g = num_elem;
    info.num_elem_blks_g = num_elem_blk;
    info.num_node_sets_g = num_node_sets;
    info.num_side_sets_g = num_side_sets;

    // Coordinate names
    info.coord_names.resize(num_dim);
    std::vector<char *> cnames(num_dim);
    std::vector<std::vector<char>> cname_bufs(num_dim, std::vector<char>(MAX_STR_LENGTH + 1, 0));
    for (int i = 0; i < num_dim; ++i)
        cnames[i] = cname_bufs[i].data();
    EXODUS_CHECK(ex_get_coord_names(exoid, cnames.data()));
    for (int i = 0; i < num_dim; ++i)
        info.coord_names[i] = cnames[i];

    // Element blocks
    info.el_blk_ids.resize(num_elem_blk);
    info.el_blk_cnts.resize(num_elem_blk);
    info.el_blk_elem_types.resize(num_elem_blk);
    info.el_blk_nodes_per_elem.resize(num_elem_blk);
    info.el_blk_names.resize(num_elem_blk);

    if (num_elem_blk > 0) {
        EXODUS_CHECK(ex_get_ids(exoid, EX_ELEM_BLOCK, info.el_blk_ids.data()));

        std::vector<char *> bnames(num_elem_blk);
        std::vector<std::vector<char>> bname_bufs(num_elem_blk,
                                                  std::vector<char>(MAX_STR_LENGTH + 1, 0));
        for (int i = 0; i < num_elem_blk; ++i)
            bnames[i] = bname_bufs[i].data();
        EXODUS_CHECK(ex_get_names(exoid, EX_ELEM_BLOCK, bnames.data()));

        for (int i = 0; i < num_elem_blk; ++i) {
            info.el_blk_names[i] = bnames[i];
            char elem_type[MAX_STR_LENGTH + 1] = { 0 };
            int64_t n_elem_this_blk = 0, n_nodes_per_elem = 0, n_edges = 0, n_faces = 0, n_attr = 0;
            EXODUS_CHECK(ex_get_block(exoid,
                                      EX_ELEM_BLOCK,
                                      info.el_blk_ids[i],
                                      elem_type,
                                      &n_elem_this_blk,
                                      &n_nodes_per_elem,
                                      &n_edges,
                                      &n_faces,
                                      &n_attr));
            info.el_blk_cnts[i] = n_elem_this_blk;
            info.el_blk_elem_types[i] = elem_type;
            info.el_blk_nodes_per_elem[i] = n_nodes_per_elem;
        }
    }

    // Node sets
    if (num_node_sets > 0) {
        info.ns_ids.resize(num_node_sets);
        info.ns_cnts.resize(num_node_sets);
        info.ns_df_cnts.resize(num_node_sets, 0);
        info.ns_names.resize(num_node_sets);

        EXODUS_CHECK(ex_get_ids(exoid, EX_NODE_SET, info.ns_ids.data()));

        std::vector<char *> ns_names_ptrs(num_node_sets);
        std::vector<std::vector<char>> ns_name_bufs(num_node_sets,
                                                    std::vector<char>(MAX_STR_LENGTH + 1, 0));
        for (int i = 0; i < num_node_sets; ++i)
            ns_names_ptrs[i] = ns_name_bufs[i].data();
        EXODUS_CHECK(ex_get_names(exoid, EX_NODE_SET, ns_names_ptrs.data()));

        for (int i = 0; i < num_node_sets; ++i) {
            info.ns_names[i] = ns_names_ptrs[i];
            int64_t n_nodes_in_set = 0, n_df_in_set = 0;
            EXODUS_CHECK(ex_get_set_param(exoid,
                                          EX_NODE_SET,
                                          info.ns_ids[i],
                                          &n_nodes_in_set,
                                          &n_df_in_set));
            info.ns_cnts[i] = n_nodes_in_set;
            info.ns_df_cnts[i] = n_df_in_set;
        }
    }

    // Side sets
    if (num_side_sets > 0) {
        info.ss_ids.resize(num_side_sets);
        info.ss_cnts.resize(num_side_sets);
        info.ss_df_cnts.resize(num_side_sets, 0);
        info.ss_names.resize(num_side_sets);

        EXODUS_CHECK(ex_get_ids(exoid, EX_SIDE_SET, info.ss_ids.data()));

        std::vector<char *> ss_names_ptrs(num_side_sets);
        std::vector<std::vector<char>> ss_name_bufs(num_side_sets,
                                                    std::vector<char>(MAX_STR_LENGTH + 1, 0));
        for (int i = 0; i < num_side_sets; ++i)
            ss_names_ptrs[i] = ss_name_bufs[i].data();
        EXODUS_CHECK(ex_get_names(exoid, EX_SIDE_SET, ss_names_ptrs.data()));

        for (int i = 0; i < num_side_sets; ++i) {
            info.ss_names[i] = ss_names_ptrs[i];
            int64_t n_sides_in_set = 0, n_df_in_set = 0;
            EXODUS_CHECK(ex_get_set_param(exoid,
                                          EX_SIDE_SET,
                                          info.ss_ids[i],
                                          &n_sides_in_set,
                                          &n_df_in_set));
            info.ss_cnts[i] = n_sides_in_set;
            info.ss_df_cnts[i] = n_df_in_set;
        }
    }

    // Maps
    elem_map.resize(num_elem);
    EXODUS_CHECK(ex_get_id_map(exoid, EX_ELEM_MAP, elem_map.data()));

    node_map.resize(num_nodes);
    EXODUS_CHECK(ex_get_id_map(exoid, EX_NODE_MAP, node_map.data()));

    EXODUS_CHECK(ex_close(exoid));
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

    opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    opts.add_option("", "v", "version", "Show the version", cxxopts::value<bool>(), "");
    opts.add_option("",
                    "p",
                    "partitioner",
                    "Name of the partitioner",
                    cxxopts::value<std::string>(),
                    "");
    opts.add_option("",
                    "o",
                    "output",
                    "Base name for output files",
                    cxxopts::value<std::string>()->default_value("out"),
                    "");
    opts.add_option("", "", "mesh-file", "The mesh file name", cxxopts::value<std::string>(), "");
    opts.positional_help("<mesh-file>");
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
    std::vector<int64_t> global_elem_map;
    std::vector<int64_t> global_node_map;

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

    std::unordered_map<Int, int64_t> local_node_id;
    std::vector<int64_t> node_mapi(num_int_nodes);
    std::vector<int64_t> node_mapb(num_bor_nodes);
    std::vector<int64_t> exo_node_id_map(num_nodes_local);

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

    std::vector<int64_t> elem_mapi;
    std::vector<int64_t> elem_mapb;
    std::vector<int64_t> exo_elem_id_map;
    std::unordered_map<Int, int64_t> local_elem_id;

    struct LocalBlockInfo {
        int64_t blk_id = 0;
        int64_t num_elems = 0;
        int64_t nodes_per_elem = 0;
        std::string elem_type;
        std::vector<int64_t> conn;
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
    std::map<int, std::vector<int64_t>> cmap_nodes_by_neighbor;
    for (Int v : border_vertices) {
        Int g = dist_ids[v - pStart_dist];
        int64_t loc_nid = local_node_id[v];
        for (int q : node_neighbors[g]) {
            cmap_nodes_by_neighbor[q].push_back(loc_nid);
        }
    }

    int64_t num_node_cmaps = cmap_nodes_by_neighbor.size();
    std::vector<int64_t> node_cmap_ids;
    std::vector<int64_t> node_cmap_cnts;
    for (const auto & [neighbor_q, nodes] : cmap_nodes_by_neighbor) {
        node_cmap_ids.push_back(neighbor_q);
        node_cmap_cnts.push_back(nodes.size());
    }

    // Node sets
    struct LocalNodeSetInfo {
        int64_t id = 0;
        std::vector<int64_t> nodes;
    };
    std::vector<LocalNodeSetInfo> local_node_sets;

    if (global_info.num_node_sets_g > 0) {
        auto vertex_sets_label = mesh->get_label("Vertex Sets");
        for (int64_t ns_id : global_info.ns_ids) {
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
        std::vector<int64_t> elems;
        std::vector<int64_t> sides;
    };
    std::vector<LocalSideSetInfo> local_side_sets;

    if (global_info.num_side_sets_g > 0) {
        auto face_sets_label = mesh->get_label("Face Sets");
        for (int64_t ss_id : global_info.ss_ids) {
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

    int cpu_ws = sizeof(double);
    int io_ws = sizeof(double);
    int cmode = EX_CLOBBER | EX_ALL_INT64_API | EX_ALL_INT64_DB;
    int exoid = ex_create(out_file_name.c_str(), cmode, &cpu_ws, &io_ws);
    if (exoid < 0)
        throw Exception(fmt::format("Unable to create Exodus file: {}", out_file_name));

    ex_set_int64_status(exoid, EX_ALL_INT64_API);

    // 1. Local init
    EXODUS_CHECK(ex_put_init(exoid,
                             global_info.title.c_str(),
                             global_info.dim,
                             num_nodes_local,
                             num_elems_local,
                             global_info.num_elem_blks_g,
                             global_info.num_node_sets_g,
                             global_info.num_side_sets_g));

    // 2. Global metadata definitions
    EXODUS_CHECK(ex_put_init_info(exoid, static_cast<int>(n_procs), 1, "P"));
    EXODUS_CHECK(ex_put_init_global(exoid,
                                    global_info.num_nodes_g,
                                    global_info.num_elems_g,
                                    global_info.num_elem_blks_g,
                                    global_info.num_node_sets_g,
                                    global_info.num_side_sets_g));

    // 3. Global block/set parameters
    EXODUS_CHECK(ex_put_eb_info_global(exoid,
                                       global_info.el_blk_ids.data(),
                                       global_info.el_blk_cnts.data()));

    if (global_info.num_node_sets_g > 0) {
        EXODUS_CHECK(ex_put_ns_param_global(exoid,
                                            global_info.ns_ids.data(),
                                            global_info.ns_cnts.data(),
                                            global_info.ns_df_cnts.data()));
    }
    if (global_info.num_side_sets_g > 0) {
        EXODUS_CHECK(ex_put_ss_param_global(exoid,
                                            global_info.ss_ids.data(),
                                            global_info.ss_cnts.data(),
                                            global_info.ss_df_cnts.data()));
    }

    // 4. Load balance & Communication parameters
    EXODUS_CHECK(ex_put_loadbal_param(exoid,
                                      num_int_nodes,
                                      num_bor_nodes,
                                      0,
                                      num_int_elems,
                                      num_bor_elems,
                                      num_node_cmaps,
                                      0,
                                      0));

    if (num_node_cmaps > 0) {
        EXODUS_CHECK(ex_put_cmap_params(exoid,
                                        node_cmap_ids.data(),
                                        node_cmap_cnts.data(),
                                        nullptr,
                                        nullptr,
                                        0));
        for (const auto & [neighbor_q, nodes] : cmap_nodes_by_neighbor) {
            std::vector<int64_t> proc_ids(nodes.size(), neighbor_q);
            EXODUS_CHECK(ex_put_node_cmap(exoid, neighbor_q, nodes.data(), proc_ids.data(), 0));
        }
    }

    EXODUS_CHECK(ex_put_processor_node_maps(exoid,
                                            num_int_nodes > 0 ? node_mapi.data() : nullptr,
                                            num_bor_nodes > 0 ? node_mapb.data() : nullptr,
                                            nullptr,
                                            0));

    EXODUS_CHECK(ex_put_processor_elem_maps(exoid,
                                            num_int_elems > 0 ? elem_mapi.data() : nullptr,
                                            num_bor_elems > 0 ? elem_mapb.data() : nullptr,
                                            0));

    // 5. Bulk data: Coordinates
    std::vector<char *> cnames(global_info.dim);
    for (int i = 0; i < global_info.dim; ++i)
        cnames[i] = const_cast<char *>(global_info.coord_names[i].c_str());
    EXODUS_CHECK(ex_put_coord_names(exoid, cnames.data()));

    EXODUS_CHECK(ex_put_coord(exoid, x.data(), y.data(), z.data()));

    // 6. Bulk data: Element Blocks
    std::vector<char *> bnames(global_info.num_elem_blks_g);
    for (size_t i = 0; i < local_blocks.size(); ++i) {
        const auto & lb = local_blocks[i];
        bnames[i] = const_cast<char *>(global_info.el_blk_names[i].c_str());
        if (lb.num_elems == 0) {
            EXODUS_CHECK(
                ex_put_block(exoid, EX_ELEM_BLOCK, lb.blk_id, lb.elem_type.c_str(), 0, 0, 0, 0, 0));
        }
        else {
            EXODUS_CHECK(ex_put_block(exoid,
                                      EX_ELEM_BLOCK,
                                      lb.blk_id,
                                      lb.elem_type.c_str(),
                                      lb.num_elems,
                                      lb.nodes_per_elem,
                                      0,
                                      0,
                                      0));
            EXODUS_CHECK(
                ex_put_conn(exoid, EX_ELEM_BLOCK, lb.blk_id, lb.conn.data(), nullptr, nullptr));
        }
    }
    EXODUS_CHECK(ex_put_names(exoid, EX_ELEM_BLOCK, bnames.data()));

    // 7. Bulk data: Node Sets
    if (global_info.num_node_sets_g > 0) {
        std::vector<char *> ns_names(global_info.num_node_sets_g);
        for (size_t i = 0; i < local_node_sets.size(); ++i) {
            const auto & lns = local_node_sets[i];
            ns_names[i] = const_cast<char *>(global_info.ns_names[i].c_str());
            if (lns.nodes.empty()) {
                EXODUS_CHECK(ex_put_set_param(exoid, EX_NODE_SET, lns.id, 0, 0));
            }
            else {
                EXODUS_CHECK(ex_put_set_param(exoid, EX_NODE_SET, lns.id, lns.nodes.size(), 0));
                EXODUS_CHECK(ex_put_set(exoid, EX_NODE_SET, lns.id, lns.nodes.data(), nullptr));
            }
        }
        EXODUS_CHECK(ex_put_names(exoid, EX_NODE_SET, ns_names.data()));
    }

    // 8. Bulk data: Side Sets
    if (global_info.num_side_sets_g > 0) {
        std::vector<char *> ss_names(global_info.num_side_sets_g);
        for (size_t i = 0; i < local_side_sets.size(); ++i) {
            const auto & lss = local_side_sets[i];
            ss_names[i] = const_cast<char *>(global_info.ss_names[i].c_str());
            if (lss.elems.empty()) {
                EXODUS_CHECK(ex_put_set_param(exoid, EX_SIDE_SET, lss.id, 0, 0));
            }
            else {
                EXODUS_CHECK(ex_put_set_param(exoid, EX_SIDE_SET, lss.id, lss.elems.size(), 0));
                EXODUS_CHECK(
                    ex_put_set(exoid, EX_SIDE_SET, lss.id, lss.elems.data(), lss.sides.data()));
            }
        }
        EXODUS_CHECK(ex_put_names(exoid, EX_SIDE_SET, ss_names.data()));
    }

    // 9. Global Exodus ID maps
    if (num_nodes_local > 0)
        EXODUS_CHECK(ex_put_id_map(exoid, EX_NODE_MAP, exo_node_id_map.data()));
    if (num_elems_local > 0)
        EXODUS_CHECK(ex_put_id_map(exoid, EX_ELEM_MAP, exo_elem_id_map.data()));

    EXODUS_CHECK(ex_close(exoid));
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
    catch (Exception & e) {
        fmt::println("{}", e.what());
        return -1;
    }
}
