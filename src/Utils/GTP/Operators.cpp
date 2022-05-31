//
// Created by arthur wesley on 12/18/21.
//

#include <iostream>


#include "Operators.h"
#include "../SGF/SGF.h"

namespace sente::GTP {

    Response protocolVersion(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) self;
        (void) arguments;
        return {true, "2"};
    }

    Response name(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) arguments;
        return {true, self->getEngineName()};
    }
    Response version(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) arguments;
        return {true, self->getEngineVersion()};
    }
    Response knownCommand(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        auto commands = self->getCommands();
        if (commands.find(arguments[1]->getText()) == commands.end()){
            return {true, "false"};
        }
        else {
            return {true, "true"};
        }
    }
    Response listCommands(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) arguments;
        std::stringstream commands;

        auto registeredCommands = self->getCommands();

        for (auto command = registeredCommands.begin(); command != registeredCommands.end();){
            commands << command->first;

            // only add the newline if we need to
            if (++command != registeredCommands.end()){
                commands << std::endl;
            }
        }

        return {true, commands.str()};

    }
    Response quit(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) arguments;
        self->setActive(false);
        return {true, ""};
    }
    Response boardSize(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        // reset the board
        auto* size = (Integer*) arguments[1].get();
        if (size->getValue() == 9 or size->getValue() == 13 or size->getValue() == 19){
            self->masterGame = GoGame(size->getValue(), self->masterGame.getRules(), self->masterGame.getKomi());
            self->setGTPDisplayFlags();
            return {true, ""};
        }
        else {
            return {false, "unacceptable size"};
        }
    }
    Response clearBoard(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) arguments;
        // reset the board
        self->masterGame = GoGame(self->masterGame.getSide(), self->masterGame.getRules(), self->masterGame.getKomi());
        self->setGTPDisplayFlags();
        return {true, ""};
    }
    Response komi(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        auto* newKomi = (Float*) arguments[1].get();
        self->masterGame.setKomi(newKomi->getValue());
        return {true, ""};
    }
    Response play(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){

        // generate a move from the arguments
        sente::Move move = ((Move*) arguments[1].get())->getMove(self->masterGame.getSide());;

        if (self->masterGame.isAddLegal(move)){
            if (self->masterGame.isLegal(move)){
                // play the stone
                self->masterGame.playStone(move);
                return {true, ""};
            }
            else {
                self->masterGame.addStone(move);
                return {true, ""};
            }
        }
        else {
            // if the move is illegal, report it
            return {false, "illegal move"};
        }
    }
    Response genMove(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) self;
        (void) arguments;
        throw std::runtime_error("genmove has not been implemented by this engine, please register a valid function");
    }
    Response showBoard(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) arguments;
        return {true, "\n" + std::string(self->masterGame)};
    }
    Response undoOnce(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        (void) arguments;
        if (not self->masterGame.isAtRoot()){
            self->masterGame.stepUp(1);
            self->setGTPDisplayFlags();
            return {true, ""};
        }
        else {
            return {false, "cannot undo"};
        }
    }
    Response undoMultiple(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        auto* steps = (Integer*) arguments[1].get();
        if (self->masterGame.getMoveSequence().size() >= steps->getValue()){
            self->masterGame.stepUp(steps->getValue());
            self->setGTPDisplayFlags();
            return {true, ""};
        }
        else {
            return {false, "cannot undo"};
        }
    }

    Response baseLoadSGF(Responder * self, const std::string& filePath){

        // load the text from the file
        std::ifstream filePointer(filePath);

        if (not filePointer.good()){
            return {false, "cannot load file"};
        }

        std::string SGFText = std::string((std::istreambuf_iterator<char>(filePointer)),
                                          std::istreambuf_iterator<char>());

        // generate the move tree
        auto tree = sente::SGF::loadSGF(SGFText, false, true, true);

        // set the engine's game to be the move tree
        self->masterGame = GoGame(tree);
        self->setGTPDisplayFlags();

        return {true, ""};

    }

    Response loadSGF1(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        auto* pathStr = (String*) arguments[1].get();
        auto response = baseLoadSGF(self, pathStr->getText());

        self->masterGame.playDefaultSequence();

        return response;
    }

    Response loadSGF2(Responder * self, const std::vector<std::shared_ptr<Token>>& arguments){
        // load the board
        auto* pathStr = (String*) arguments[1].get();
        auto* moves = (Integer*) arguments[2].get();
        auto response = baseLoadSGF(self, pathStr->getText());

        // play out the sequence
        auto moveSequence = self->masterGame.getDefaultSequence();

        unsigned movesAdvanced = std::min(moves->getValue(), unsigned(moveSequence.size()));

        moveSequence = std::vector<sente::Move>(moveSequence.begin(), moveSequence.begin() + movesAdvanced);

        self->masterGame.playMoveSequence(moveSequence);

        return response;
    }

}
