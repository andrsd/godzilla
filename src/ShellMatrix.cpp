// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ShellMatrix.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/Matrix.h"
#include <petscmat.h>

namespace godzilla {

PetscErrorCode
ShellMatrix::invoke_matmult_op_delegate(Mat matrix, Vec vector, Vec action)
{
    CALL_STACK_MSG();
    ShellMatrix * shell_matrix = nullptr;
    PETSC_CHECK(MatShellGetContext(matrix, &shell_matrix));
    Matrix A(matrix);
    A.inc_reference();
    Vector x(vector);
    x.inc_reference();
    Vector y(action);
    y.inc_reference();
    if (shell_matrix->mult_delegate)
        shell_matrix->mult_delegate.invoke(A, x, y);
    else
        throw Exception("Delegate not set for MatMult operation");
    return 0;
}

ShellMatrix::ShellMatrix() : Matrix() {}

ShellMatrix::ShellMatrix(Mat m) : Matrix(m)
{
    PETSC_CHECK(MatShellSetContext(m, this));
}

void
ShellMatrix::create(MPI_Comm comm, Int m, Int n, Int M, Int N)
{
    CALL_STACK_MSG();
    Mat mat;
    PETSC_CHECK(MatCreateShell(comm, m, n, M, N, this, &mat));
    this->obj = mat;
}

void
ShellMatrix::set_operation(MatOperation op, void (*g)(void))
{
    CALL_STACK_MSG();
    PETSC_CHECK(MatShellSetOperation(*this, op, g));
}

} // namespace godzilla
