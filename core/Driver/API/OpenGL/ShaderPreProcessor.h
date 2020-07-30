#pragma once

#include <pch.hpp>

namespace GLSLPreProcessor {

    const char* MACROS[2] = {"include", "message"};

    struct sDirective {
        std::string macro;
        std::string directive;
    };

    static std::string PreProcessGLSL(std::string pInputShaderString) {
        std::string OutputShaderString;
        std::vector<sDirective> Directives;

        //std::stringstream InputShaderStringStream(pInputShaderString);
        std::ifstream InputShaderStringStream(pInputShaderString, std::ios::in);

        std::string Line = "";
        bool excludeLine = false;
		while(getline(InputShaderStringStream, Line))
        {
            for(int i = 0; i < 2; i++)
            {
                if(Line.find(MACROS[i]) != std::string::npos)
                {
                    sDirective tmpDir;
                    tmpDir.macro = MACROS[i];
                    unsigned first = Line.find("<");
                    unsigned last = Line.find(">");
                    std::string strNew = Line.substr(first+1,last-(first+1));
                    tmpDir.directive = strNew;
                    Directives.push_back(tmpDir);
                    //std::cout << "Found macro: " << tmpDir.macro << " with directive: " << tmpDir.directive << std::endl;

                    if(Directives.at(i).macro == "include") {
                            excludeLine = true;
                            //std::cout << "Attaching file" << std::endl;
                        OutputShaderString += PreProcessGLSL("shaders/"+tmpDir.directive);
                    }

                }
            }
            if(excludeLine){

                OutputShaderString += "\n";
                excludeLine = false;
            }
            else{
                OutputShaderString += "\n" + Line;}
        }
		InputShaderStringStream.close();

        for(unsigned int i = 0; i < Directives.size(); i++) {
             if(Directives.at(i).macro == "message") {

            } else {
                //std::cout << "MACRO: " << Directives.at(i).macro << " is not recognized by this preprocessor, ignoring..." << std::endl;
                continue;
            }
        }

        return OutputShaderString;
    }

    static std::string openAndAttachFile(std::string path) {
        return nullptr;
    }

}
