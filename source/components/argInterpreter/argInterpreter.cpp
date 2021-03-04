#include "argInterpreter.hpp"

#include "../../providers/logger/logger.hpp"
#include "../../utilities/runtimeError.hpp"
#include "../../utilities/utility/utility.hpp"
#include "helpOption.hpp"
#include "../../providers/config/config.hpp"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

namespace ArgInterpreter
{

    class ArgEnumerator;

    static void interpretArgument(const std::string &rawOptionKey, std::shared_ptr<ArgEnumerator> args);

    class ArgEnumerator
    {
    private:
        std::vector<std::string> data;
        std::vector<std::string>::iterator iter;

    public:
        ArgEnumerator(int argc, char *argv[])
        {
            for (int i = 0; i < argc; i++)
                data.push_back(argv[i]);

            this->iter = this->data.begin();
        }

        void start() { this->iter = this->data.begin(); }
        bool isEnded() const { return this->iter == this->data.end(); }

        const std::string &pop()
        {
            const std::string &tmpStr = peek();
            (this->iter)++;
            return tmpStr;
        }

        const std::string &peek() const
        {
            if (isEnded())
                throw RunntimeError("Argument Interpreter: not enought arguments");
            else
                return *(this->iter);
        }

        bool isNextKey()
        {
            const std::string &nextArg = peek();
            return nextArg[0] == '-';
        }
    };

    class ArgOption
    {
    private:
        void (*action)(std::shared_ptr<ArgEnumerator>);

    public:
        ArgOption(void (*_action)(std::shared_ptr<ArgEnumerator>)) : action(_action) {}

        void operator()(std::shared_ptr<ArgEnumerator> args) const { this->action(args); }
    };

    static const ArgOption helpOption([](std::shared_ptr<ArgEnumerator> args) {
        (void)args;
        config.unsetExecuteFlag();
        printHelp();
    });

    static const ArgOption validateConfigOption([](std::shared_ptr<ArgEnumerator> args) {
        config.unsetExecuteFlag();
        std::string configFile;
        if (args->isEnded() || args->isNextKey())
            configFile = CONFIG_DEFAULT_FILE;
        else
            configFile = args->pop();

        try
        {
            logger.information("Validating: " + configFile);
            Config::validateFile(configFile);
            logger.success("Validation check was successful");
        }
        catch (const RunntimeError &err)
        {
            (void)err;
            logger.error("Validation check was unsucessful");
        }
    });

    static const ArgOption setConfigFileOption([](std::shared_ptr<ArgEnumerator> args) {
        std::string configFilePath = args->pop();

        config.setConfigFilePath(configFilePath);
    });

    static const std::unordered_map<std::string, ArgOption> argOptions = {
        {"--help", helpOption},
        {"-h", helpOption},
        {"--validate-config", validateConfigOption},
        {"-vc", validateConfigOption},
        {"--set-configFile", setConfigFileOption},
        {"-scf", setConfigFileOption},
    };

    void interpret(int argc, char *argv[])
    {
        std::shared_ptr<ArgEnumerator> argsEnumerator =
            std::make_shared<ArgEnumerator>(ArgEnumerator(argc, argv));

        while (!argsEnumerator->isEnded())
        {
            if (argsEnumerator->isNextKey())
                interpretArgument(std::string(argsEnumerator->pop()), argsEnumerator);
            else
                throw RunntimeError("Argument Interpreter: unexpected argument (" + argsEnumerator->pop() + ")");
        }
    }

    static void interpretArgument(const std::string &rawOptionKey, std::shared_ptr<ArgEnumerator> args)
    {
        std::string optionKey = toLowerCase(rawOptionKey);

        std::unordered_map<std::string, ArgOption>::const_iterator optionPoz = argOptions.find(optionKey);

        if (optionPoz == argOptions.end())
            throw RunntimeError("Argument Interpreter: option not found (" + optionKey + ")");
        else
            optionPoz->second(args);
    }
} // namespace ArgInterpreter
