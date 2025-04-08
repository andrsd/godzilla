// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/BasicTSAdapt.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/ConstantFunction.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/CSVOutput.h"
#include "godzilla/DirichletBC.h"
#include "godzilla/ExodusIIMesh.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/FunctionAuxiliaryField.h"
#include "godzilla/FunctionInitialCondition.h"
#include "godzilla/GmshMesh.h"
#include "godzilla/L2Diff.h"
#include "godzilla/L2FieldDiff.h"
#include "godzilla/LineMesh.h"
#include "godzilla/MeshPartitioningOutput.h"
#include "godzilla/ParsedFunction.h"
#include "godzilla/PiecewiseConstant.h"
#include "godzilla/PiecewiseLinear.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/RZSymmetry.h"
#include "godzilla/RestartOutput.h"
#include "godzilla/TecplotOutput.h"
#include "godzilla/VTKOutput.h"

namespace godzilla {

void
App::register_objects(Registry & r)
{
    REGISTER_OBJECT(r, BasicTSAdapt);
    REGISTER_OBJECT(r, BoxMesh);
    REGISTER_OBJECT(r, ConstantAuxiliaryField);
    REGISTER_OBJECT(r, ConstantFunction);
    REGISTER_OBJECT(r, ConstantInitialCondition);
    REGISTER_OBJECT(r, CSVOutput);
    REGISTER_OBJECT(r, DirichletBC);
    REGISTER_OBJECT(r, ExodusIIMesh);
    REGISTER_OBJECT(r, ExodusIIOutput);
    REGISTER_OBJECT(r, FileMesh);
    REGISTER_OBJECT(r, FunctionAuxiliaryField);
    REGISTER_OBJECT(r, FunctionInitialCondition);
    REGISTER_OBJECT(r, GmshMesh);
    REGISTER_OBJECT(r, L2Diff);
    REGISTER_OBJECT(r, L2FieldDiff);
    REGISTER_OBJECT(r, LineMesh);
    REGISTER_OBJECT(r, MeshPartitioningOutput);
    REGISTER_OBJECT(r, ParsedFunction);
    REGISTER_OBJECT(r, PiecewiseConstant);
    REGISTER_OBJECT(r, PiecewiseLinear);
    REGISTER_OBJECT(r, RectangleMesh);
    REGISTER_OBJECT(r, RestartOutput);
    REGISTER_OBJECT(r, RZSymmetry);
    REGISTER_OBJECT(r, TecplotOutput);
    REGISTER_OBJECT(r, VTKOutput);
}

} // namespace godzilla
