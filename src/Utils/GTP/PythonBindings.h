//
// Created by arthur wesley on 12/26/21.
//

#ifndef SENTE_PYTHONBINDINGS_H
#define SENTE_PYTHONBINDINGS_H

#include "Responder.h"

namespace sente::GTP {

    bool isGTPType(const py::type& type);

    void checkGTPCommand(py::function& function, const py::module_& inspect,
                         const py::module_& typing);
    bool isUnionInstance(const py::object& response, const py::object& type, const py::module_& typing);

}

#endif //SENTE_PYTHONBINDINGS_H
