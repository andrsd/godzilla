#include "WeakForm.h"
#include "Error.h"
#include "CallStack.h"

namespace godzilla {

WeakForm::WeakForm() : neq(0)
{
    _F_;
}

WeakForm::~WeakForm()
{
    _F_;
}

void
WeakForm::add_jacobian_block(uint i, uint j, jacform_val_t fn, SymFlag sym, uint area, uint nx, ...)
{
    _F_;
    assert(i < this->neq);
    assert(j < this->neq);
    assert(sym == ANTISYM || sym == UNSYM || sym == SYM);
    if (sym == ANTISYM && i == j)
        error("Only off-diagonal forms can be antisymmetric.");
    // if (this->jfvol.size() > 100)
    //     warning("Large number of forms (> 100). Is this the intent?");

    // va_list ap;
    // va_start(ap, nx);
    // JacFormVol form(i, j, sym, area, fn, va_list);
    // this->jfvol.push_back(form);
    // va_end(ap);
}

void
WeakForm::add_jacobian_block_surf(uint i, uint j, jacform_val_t fn, uint area, uint nx, ...)
{
    _F_;
    assert(i < this->neq);
    assert(j < this->neq);

    // JacFormSurf form = { i, j, area, fn, ord };
    // init_ext_fns;
    // this->jfsurf.push_back(form);
}

void
WeakForm::add_residual_block(uint i, resform_val_t fn, uint area, uint nx, ...)
{
    _F_;
    assert(i < this->neq);

    // ResFormVol form = { i, area, fn, ord };
    // init_ext_fns;
    // this->rfvol.push_back(form);
}

void
WeakForm::add_residual_block_surf(uint i, resform_val_t fn, uint area, uint nx, ...)
{
    _F_;
    assert(i < this->neq);

    // ResFormSurf form = { i, area, fn, ord };
    // init_ext_fns;
    // this->rfsurf.push_back(form);
}

void
WeakForm::set_ext_fns(void * fn, uint nx, ...)
{
    error("Not implemented");
}

//// stages ////////////////////////////////////////////////////////////////////////////////////////

void
WeakForm::get_stages(Space ** spaces, std::vector<WeakForm::Stage> & stages, bool rhsonly)
{
    _F_;
    stages.clear();

    if (!rhsonly) {
        // process volume jacobian forms
        for (std::size_t i = 0; i < this->jfvol.size(); i++) {
            uint ii = this->jfvol[i].i;
            uint jj = this->jfvol[i].j;
            // Mesh * m1 = spaces[ii]->get_mesh();
            // Mesh * m2 = spaces[jj]->get_mesh();
            // Stage * s = find_stage(stages, ii, jj, m1, m2, this->jfvol[i].ext);
            // s->jfvol.push_back(&this->jfvol[i]);
        }

        // process surface jacobian forms
        for (std::size_t i = 0; i < this->jfsurf.size(); i++) {
            uint ii = this->jfsurf[i].i;
            uint jj = this->jfsurf[i].j;
            // Mesh * m1 = spaces[ii]->get_mesh();
            // Mesh * m2 = spaces[jj]->get_mesh();
            // Stage * s = find_stage(stages, ii, jj, m1, m2, this->jfsurf[i].ext);
            // s->jfsurf.push_back(&this->jfsurf[i]);
        }
    }

    // process volume residual forms
    for (std::size_t i = 0; i < this->rfvol.size(); i++) {
        uint ii = this->rfvol[i].i;
        // Mesh * m = spaces[ii]->get_mesh();
        // Stage * s = find_stage(stages, ii, ii, m, m, this->rfvol[i].ext);
        // s->rfvol.push_back(&this->rfvol[i]);
    }

    // process surface residual forms
    for (std::size_t i = 0; i < this->rfsurf.size(); i++) {
        uint ii = this->rfsurf[i].i;
        // Mesh * m = spaces[ii]->get_mesh();
        // Stage * s = find_stage(stages, ii, ii, m, m, this->rfsurf[i].ext);
        // s->rfsurf.push_back(&this->rfsurf[i]);
    }

    // initialize the arrays meshes and fns needed by Traverse for each stage
    for (std::size_t i = 0; i < stages.size(); i++) {
        // Stage * s = &stages[i];
        // for (auto & it = s->idx_set.begin(); it != s->idx_set.end(); it++)
        // {
        //     s->idx.push_back(*it);
        //     s->meshes.push_back(spaces[*it]->get_mesh());
        //     s->fns.push_back(nullptr);
        // }
        // for (auto & it = s->ext_set.begin(); it != s->ext_set.end(); it++)
        // {
        //     s->ext.push_back(*it);
        //     s->meshes.push_back((*it)->get_mesh());
        //     s->fns.push_back(*it);
        // }
        // s->idx_set.clear();
        // s->seq_set.clear();
        // s->ext_set.clear();
    }
}

WeakForm::Stage *
WeakForm::find_stage(std::vector<WeakForm::Stage> & stages,
                     uint ii,
                     uint jj,
                     Mesh * m1,
                     Mesh * m2,
                     std::vector<MeshFunction1D *> & ext)
{
    _F_;
    // first create a list of meshes the form uses
    // std::set<uint> seq;
    // seq.insert(m1->get_seq());
    // seq.insert(m2->get_seq());
    // for (std::size_t i = 0; i < ext.size(); i++)
    //     seq.insert(ext[i]->get_mesh()->get_seq());

    // find a suitable existing stage for the form
    Stage * s = nullptr;
    // for (std::size_t i = 0; i < stages.size(); i++)
    //     if (seq.size() == stages[i].seq_set.size() &&
    //         equal(seq.begin(), seq.end(), stages[i].seq_set.begin())) {
    //         s = &stages[i];
    //         break;
    //     }

    // create a new stage if not found
    if (s == nullptr) {
        Stage newstage;
        stages.push_back(newstage);
        s = &stages.back();
        // s->seq_set = seq;
    }

    // update and return the stage
    // for (std::size_t i = 0; i < ext.size(); i++)
    //     s->ext_set.insert(ext[i]);
    // s->idx_set.insert(ii);
    // s->idx_set.insert(jj);

    return s;
}

DenseMatrix<bool>
WeakForm::get_blocks()
{
    _F_;
    DenseMatrix<bool> blocks(this->neq, this->neq);
    for (uint i = 0; i < this->neq; i++)
        for (uint j = 0; j < this->neq; j++)
            blocks[i][j] = false;

    for (std::size_t i = 0; i < this->jfvol.size(); i++) {
        blocks[this->jfvol[i].i][this->jfvol[i].j] = true;
        if (this->jfvol[i].sym)
            blocks[this->jfvol[i].j][this->jfvol[i].i] = true;
    }

    for (std::size_t i = 0; i < this->jfsurf.size(); i++)
        blocks[this->jfsurf[i].i][this->jfsurf[i].j] = true;

    return blocks;
}

} // namespace godzilla
