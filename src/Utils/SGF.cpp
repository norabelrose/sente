//
// Created by arthur wesley on 7/2/21.
//

#include <stack>
#include <regex>
#include <sstream>

#include <pybind11/pybind11.h>

#include "../Include/Utils/SGF.h"
#include "../Include/Utils/SenteExceptions.h"

namespace py = pybind11;

std::string strip(const std::string &input)
{

    // py::print("entering strip with \"" + input + "\"");
    // py::print("size: ", input.size());

    auto start_it = input.begin();
    auto end_it = input.end();
    while (std::isspace(*start_it) and start_it != end_it)
        start_it++;
    while (std::isspace(*end_it) and start_it != end_it){
        end_it--;
    }

    /*
    py::print("\"", *start_it, *end_it, "\"");
    py::print("\"" + std::string(start_it, end_it) + "\"");
    py::print("leaving strip");
     */

    return {start_it, end_it};
}

namespace sente {
    namespace utils {

        SGFNode nodeFromText(const std::string& SGFText){

            // py::print("entering nodeFromText with \"" + SGFText + "\"");

            SGFNode node;
            std::string temp;

            SGFProperty lastProperty;

            auto cursor = SGFText.begin();
            auto previousSlice = cursor;

            bool inBrackets = false;

            // initialize the tree from the first item
            for (; cursor < SGFText.end(); cursor++) {
                switch (*cursor){
                    case '[':

                        if (not inBrackets){
                            // slice out the property
                            temp = strip(std::string(previousSlice, cursor));

                            // only make a new property if a new property exists
                            if (not temp.empty()){
                                if (isProperty(temp)){
                                    lastProperty = fromStr(strip(temp));
                                }
                                else {
                                    throw InvalidSGFException("Unknown SGF Property: \"" + temp + "\"");
                                }
                            }

                            // update the index of the previous slice
                            previousSlice = cursor + 1;
                        }

                        inBrackets = true;

                        break;
                    case ']':

                        // slice out the argument of the property
                        temp = strip(std::string(previousSlice, cursor));

                        // add the property
                        if (lastProperty == NONE){
                            throw InvalidSGFException("No Command listed: " +
                            std::string(SGFText.begin(), cursor));
                        }
                        else {
                            // py::print("putting in \"" + temp + "\" for \"" + toStr(lastCommand) + "\"");
                            node.appendProperty(lastProperty, temp);
                        }

                        inBrackets = false;

                        // update the index of the previous slice
                        previousSlice = cursor + 1;
                        break;
                    case '\\':
                        // skip the next character
                        cursor++;
                        break;
                    default:
                        break;
                }
            }

            if (*cursor != ']'){
                // TODO: throw an exception if the last item in the sequence is not a closing bracket
            }

            return node;

        }

        Tree<SGFNode> loadSGF(const std::string& SGFText){

            // py::print("entering SGFText");
            // py::print(SGFText.size());
            // py::print("the first character is", *SGFText.begin());

            if (SGFText.empty()){
                throw InvalidSGFException("File is Empty or unreadable");
            }

            bool inBrackets = false;
            bool firstNode = true;

            // temporary string variable
            std::string temp;

            auto cursor = SGFText.begin();
            auto previousSlice = cursor;

            unsigned FFVersion;

            std::stack<unsigned> branchDepths{};

            Tree<SGFNode> SGFTree;
            SGFNode tempNode;

            // go through the rest of the tree

            for (; cursor < SGFText.end(); cursor++){
                // py::print("slice is currently \"" + std::string(previousSlice, cursor) + "\"");
                // py::print(SGFTree.getDepth());
                switch (*cursor){
                    case '[':
                        // enter brackets
                        inBrackets = true;
                        break;
                    case ']':
                        // leave brackets
                        if (inBrackets){
                            inBrackets = false;
                        }
                        else {
                            throw InvalidSGFException("Extra Closing Bracket");
                        }
                        break;
                    case '\\':
                        cursor++;
                        break;
                    case '(':
                        if (not inBrackets){
                            temp = strip(std::string(previousSlice, cursor));

                            if (not temp.empty()) {
                                // add the property prior to this one
                                tempNode = nodeFromText(temp);

                                if (firstNode){
                                    SGFTree = Tree<SGFNode>(tempNode);
                                    firstNode = false;
                                    if (SGFTree.get().hasProperty(FF)){
                                        FFVersion = std::stoi(SGFTree.get().getProperty(FF)[0]);
                                    }
                                    else {
                                        // the file format must be FF[1] because it's not specified
                                        FFVersion = 1;
                                    }
                                }
                                else {
                                    SGFTree.insert(tempNode);
                                }
                                // validate the result with the file format version
                                if (not SGFTree.get().getInvalidProperties(FFVersion).empty()){
                                    throw InvalidSGFException("The Property \"" +
                                          toStr(SGFTree.get().getInvalidProperties(FFVersion)[0]) +
                                          "\" is not supported on this version of SGF (FF[" +
                                          std::to_string(FFVersion) + "])");
                                }
                            }

                            // with the property added to the tree, the push the depth of the current node onto the stack
                            branchDepths.push(SGFTree.getDepth());

                            // update the previousSlice
                            previousSlice = cursor + 1;
                        }
                        break;
                    case ')':
                        if (not inBrackets){

                            temp = strip(std::string(previousSlice, cursor));

                            if (not temp.empty()) {
                                // add the property prior to this one
                                tempNode = nodeFromText(temp);
                                if (firstNode){
                                    SGFTree = Tree<SGFNode>(tempNode);
                                    firstNode = false;
                                    if (SGFTree.get().hasProperty(FF)){
                                        FFVersion = std::stoi(SGFTree.get().getProperty(FF)[0]);
                                    }
                                    else {
                                        // the file format must be FF[1] because it's not specified
                                        FFVersion = 1;
                                    }
                                }
                                else {
                                    SGFTree.insert(tempNode);
                                }
                                // validate the result with the file format version
                                if (not SGFTree.get().getInvalidProperties(FFVersion).empty()){
                                    throw InvalidSGFException("The Property \"" +
                                          toStr(SGFTree.get().getInvalidProperties(FFVersion)[0]) +
                                          "\" is not supported on this version of SGF (FF[" +
                                          std::to_string(FFVersion) + "])");
                                }
                            }

                            // update the previousSlice
                            previousSlice = cursor + 1;

                            // update the depth
                            if (not branchDepths.empty()){
                                // step up until we reach the previous branch depth
                                while (SGFTree.getDepth() > branchDepths.top()){
                                    SGFTree.stepUp();
                                }
                                branchDepths.pop();
                            }
                            else {
                                throw InvalidSGFException("extra closing parentheses");
                            }

                        }
                        break;
                    case ';':
                        if (not inBrackets){
                            if (previousSlice + 1 < cursor){
                                // get the node from the text
                                tempNode = nodeFromText(strip(std::string(previousSlice, cursor)));
                                if (firstNode){
                                    SGFTree = Tree<SGFNode>(tempNode);
                                    firstNode = false;
                                    if (SGFTree.get().hasProperty(FF)){
                                        FFVersion = std::stoi(SGFTree.get().getProperty(FF)[0]);
                                    }
                                    else {
                                        // the file format must be FF[1] because it's not specified
                                        FFVersion = 1;
                                    }
                                }
                                else {
                                    SGFTree.insert(tempNode);
                                }
                                // validate the result with the file format version
                                if (not SGFTree.get().getInvalidProperties(FFVersion).empty()){
                                    throw InvalidSGFException("The Property \"" +
                                          toStr(SGFTree.get().getInvalidProperties(FFVersion)[0]) +
                                          "\" is not supported on this version of SGF (FF[" +
                                          std::to_string(FFVersion) + "])");
                                }
                            }

                            // update the previousSlice
                            previousSlice = cursor + 1;
                            break;
                        }
                }
            }

            if (firstNode){
                throw InvalidSGFException("Unable to find any SGF nodes in file");
            }

            if (SGFTree.getDepth() != 0){
                throw InvalidSGFException("Missing Closing parentheses");
            }

            // make sure that the game we loaded is a go game
            if (SGFTree.get().hasProperty(GM)){
                if (SGFTree.get().getProperty(GM)[0] != "1"){
                    throw InvalidSGFException("Game is not a Go Game (Sente only parses Go Games)");
                }
            }

            return SGFTree;

        }

        void insertIntoSGF(Tree<SGFNode>& moves, std::stringstream& SGF){

            // insert the current node
            SGF << ";" << std::string(moves.get());

            if (moves.getDepth() == 0){
                SGF << std::endl;
            }

            for (auto& child : moves.getChildren()){
                if (moves.getChildren().size() != 1){
                    SGF << "\n(";
                }
                if (not child.getMove().isResign()){

                    // step to the child
                    moves.stepTo(child);
                    // insert the child into the SGF
                    insertIntoSGF(moves, SGF);
                    // step up
                    moves.stepUp();
                }
                if (moves.getChildren().size() != 1){
                    SGF << ")";
                }
            }

        }

        std::string dumpSGF(const GoGame& game){

            std::stringstream ss;

            // get the tree and advance to the root
            auto tree = game.getMoveTree();
            tree.advanceToRoot();

            ss << "(";

            insertIntoSGF(tree, ss);

            ss << ")";

            return ss.str();
        }
    }
}
