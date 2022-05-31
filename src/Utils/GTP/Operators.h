//
// Created by arthur wesley on 12/18/21.
//

#ifndef SENTE_OPERATORS_H
#define SENTE_OPERATORS_H

#include "Responder.h"

namespace sente::GTP {

    Response protocolVersion(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response name(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response version(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response knownCommand(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response listCommands(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response quit(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response boardSize(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response clearBoard(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response komi(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response play(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response genMove(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response showBoard(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);

    Response undoOnce(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response undoMultiple(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response loadSGF1(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);
    Response loadSGF2(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments);

}

#endif //SENTE_OPERATORS_H
