//
// Created by arthur wesley on 12/12/21.
//

#ifndef SENTE_RESPONDER_H
#define SENTE_RESPONDER_H

#include "Parser.h"

#include <string>
#include <variant>
#include <memory>

#include "../../Game/GoGame.h"
#include "PythonBindings.h"

#include "Tokens/Literal.h"

namespace sente::GTP {

    class Responder;

    typedef std::pair<bool, std::string> Response;
    typedef std::pair<std::string, LiteralType> ArgumentPattern;
    typedef std::function<Response (Responder * self, const std::vector<std::shared_ptr<Token>>& arguments)> CommandMethod;

    class Responder {
    public:

        GoGame masterGame; // the game object that the GTP edits

        Responder(const std::string& engineName, const std::string& engineVersion);

        // GTP interpreter
        std::string interpret(std::string text);

        // Custom GTP command Registration
        py::function& registerCommand(py::function& function, const py::module_& inspect, const py::module_& typing);
        py::function& registerGenMove(py::function& function, const py::module_& inspect, const py::module_& typing);

        ///
        /// Getter and Setter Methods
        ///

        [[nodiscard]] std::string getEngineName() const;
        void setEngineName(std::string name);

        [[nodiscard]] std::string getEngineVersion() const;
        void setEngineVersion(std::string version);

        [[nodiscard]] std::unordered_map<std::string, std::vector<std::pair<CommandMethod,
                                                              std::vector<ArgumentPattern>>>> getCommands() const;

        [[nodiscard]] bool isActive() const;
        void setActive(bool set);

        ///
        /// Miscellaneous methods
        ///

        // for resting the game
        void setGTPDisplayFlags();

    private:

        bool active = true;

        std::string engineName;
        std::string engineVersion;

        std::unordered_map<std::string, std::vector<std::pair<CommandMethod, std::vector<ArgumentPattern>>>> commands;

        void registerCommand(const std::string& commandName, CommandMethod method,
                             std::vector<ArgumentPattern> argumentPattern);

        Response execute(const std::string& command, const std::vector<std::shared_ptr<Token>>& arguments);

        static std::string errorMessage(const std::string& message) ;
        static std::string errorMessage(const std::string& message, unsigned i) ;
        static std::string statusMessage(const std::string& message) ;
        static std::string statusMessage(const std::string& message, unsigned i) ;

        static bool argumentsMatch(const std::vector<ArgumentPattern>& expectedTypes,
                                   const std::vector<std::shared_ptr<Token>>& arguments);
        static Response invalidArgumentsErrorMessage(const std::vector<std::vector<ArgumentPattern>>& argumentPatterns,
                                                 const std::vector<std::shared_ptr<Token>>& arguments);

        static std::vector<ArgumentPattern> getArgumentPattern(py::function& function, const py::module_& inspect);
        static py::tuple gtpArgsToPyArgs(const std::vector<std::shared_ptr<Token>>& arguments, unsigned boardSize);

    };
}


#endif//SENTE_RESPONDER_H
