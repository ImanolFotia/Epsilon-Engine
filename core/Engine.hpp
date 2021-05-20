#pragma once

#include <Driver/API/Context.hpp>

#include <ProgramData.h>

namespace Epsilon
{
    class Engine
    {
    public:
        static Engine &Get()
        {
            return instance;
        }

        Engine(Engine&&) = delete;
        Engine(const Engine&) = delete;

        API::CONTEXT_TYPE ContextType() {
            return mContextType;
        }

        void ContextType(API::CONTEXT_TYPE type) {
            mContextType = type;
        }

        unsigned Width() { return WindowWidth; }
        unsigned Height() { return WindowHeight; }
        
        void Width(unsigned w) { WindowWidth = w; }
        void Height(unsigned h) { WindowHeight = h; }

        const ProgramData & Settings() {
            return mProgramData;
        }

        const std::string & ExecutableName() {
            return mExecutableName;
        }

        int NumArguments() const {
            return mNumArguments;
        }
        
        void ExecutableName(const std::string & name) {
            mExecutableName = name;
        }

        void NumArguments(int n) {
            mNumArguments = n;
        }

        void addArgument(const std::string & key, const std::string & value) {
            mArguments[key] = value;
        }

        bool hasArgument(const std::string & x) {
            return mArguments.contains(x);
        }

        struct arguments_names_t {
        const char* width = "-width";
        const char* height = "-height";
        
        const char* renderer = "-renderer";
    } ArgumentNames;

    void ParseArguments(int argc, char **argv)
    {
        auto & ref = Engine::Get();
        ref.ExecutableName(std::string(argv[0]));
        ref.NumArguments(argc);

        for(int i = 1; i < argc; i++) {
            if(std::strcmp(ArgumentNames.width, argv[i]) == 0) {
                ref.addArgument(argv[i], argv[i+1]);
                int w = std::atoi(argv[i+1]);
                ref.Width(w);
                i++;
            } else if(std::strcmp(ArgumentNames.height, argv[i]) == 0) {
                ref.addArgument(argv[i], argv[i+1]);
                int h = std::atoi(argv[i+1]);
                ref.Height(h);
                i++;

            } else if(std::strcmp(ArgumentNames.renderer, argv[i]) == 0) {
                ref.addArgument(argv[i], argv[i+1]);
                i++;

            } else {
                std::cerr << "Invalid argument: " << argv[i] << std::endl;
            }
        }
    }

    private:
        Engine() = default;

        API::CONTEXT_TYPE mContextType;
        static Engine instance;

        unsigned WindowWidth = 800;
        unsigned WindowHeight = 600;

        ProgramData mProgramData;

        std::string mExecutableName;
        int mNumArguments = 0;

        std::unordered_map<std::string, std::string> mArguments;
    };
} // namespace Epsilon