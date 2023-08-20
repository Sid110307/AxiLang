#include "include/parser.h"

void Parser::checkInteractive(const std::string &functionName)
{
    if (!isModeSet)
    {
        Log(Log::Type::ERROR, "No mode specified. Please set a mode first.\nUsage: MODE <I|P>", fileState,
            shouldExitOnError);
        return;
    }
    if (axiDraw.getMode() != "interactive")
    {
        Log(Log::Type::ERROR, functionName + " can only be used in interactive mode.", fileState, shouldExitOnError);
        return;
    }
}

static std::string downloadFile(const std::string &url, int redirectLevel = 1)
{
    auto writeCallback = [](char *contents, size_t size, size_t nmemb, std::string *buffer)
    {
        size_t realSize = size * nmemb;
        buffer->append(contents, realSize);

        return realSize;
    };

    if (redirectLevel > MAX_REDIRECTS) Log(Log::Type::FATAL, "Too many redirects.");

    Log(Log::Type::DEBUG, std::string(redirectLevel * 2, ' ') + "Downloading file from \"" + sanitize(url) + "\".");
    Log(Log::Type::DEBUG, std::string(redirectLevel * 2, ' ') + "Resolving URL.");

    CURL *curl = curl_easy_init();
    if (!curl) Log(Log::Type::FATAL, "Could not initialize cURL.");

    std::string userAgent = "AxiLang/" + std::string(PROJECT_VERSION);
    std::string buffer;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, MAX_REDIRECTS);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) Log(Log::Type::FATAL, "Could not download file from \"" + sanitize(url) + "\".");

    long responseCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    if (responseCode == 301 || responseCode == 302)
    {
        char *redirectUrl;
        curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &redirectUrl);

        Log(Log::Type::DEBUG,
            std::string(redirectLevel * 2, ' ') + "Redirecting to \"" + sanitize(redirectUrl) + "\".");
        curl_easy_cleanup(curl);

        return downloadFile(redirectUrl, redirectLevel + 1);
    }

    curl_easy_cleanup(curl);
    Log(Log::Type::DEBUG, std::string(redirectLevel * 2, ' ') + "Creating temporary file.");

    boost::filesystem::path temp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    std::ofstream file;
    file.open(temp.string(), std::ios::out | std::ios::binary);

    if (!file.is_open())
        Log(Log::Type::FATAL, std::string(redirectLevel * 2, ' ') + "Could not create temporary file for plot.");

    file << buffer;
    file.close();

    Log(Log::Type::DEBUG, std::string(redirectLevel * 2, ' ') + "Saved to: " + temp.string());
    return temp.string();
}

void Parser::parse()
{
    assert(Token::Type::EndOfFile == 36);

    auto unknownToken = std::find_if(fileState.tokens.begin(), fileState.tokens.end(), [](const Token &token)
    {
        return token.type == Token::Type::Unknown;
    });

    if (unknownToken != fileState.tokens.end())
    {
        Log(Log::Type::ERROR, "Unknown token \"" + unknownToken->value + "\".", fileState, shouldExitOnError);
        return;
    }

    for (auto token: enumerate(fileState.tokens))
    {
        switch (token.item.type)
        {
            case Token::Type::Mode:
            {
                if (fileState.tokens.size() < token.index + 2)
                {
                    Log(Log::Type::ERROR, "No mode specified.\nUsage: MODE <I|P>", fileState,
                        shouldExitOnError);
                    break;
                }

                Token nextToken = fileState.tokens[token.index + 1];

                switch (nextToken.type)
                {
                    case Token::Type::PlotMode:
                        isModeSet = true;
                        isModePlot = true;

                        break;
                    case Token::Type::InteractiveMode:
                        axiDraw.modeInteractive();
                        isModeSet = true;

                        break;
                    default:
                        Log(Log::Type::ERROR, "Invalid mode specified.\nUsage: MODE <I|P>", fileState,
                            shouldExitOnError);
                        break;
                }
                break;
            }
            case Token::Type::Opts:
            {
                if (!isModeSet)
                {
                    Log(Log::Type::ERROR, "No mode specified. Please set a mode first.\nUsage: MODE <I|P>",
                        fileState, shouldExitOnError);
                    break;
                }

                if (fileState.tokens.size() < token.index + 3)
                {
                    Log(Log::Type::ERROR, "No option specified.\nUsage: OPTS\n\t<option> <value>\n\t...\nEND_OPTS",
                        fileState, shouldExitOnError);
                    break;
                }

                Token optionName = fileState.tokens[token.index + 1];
                Token optionValue = fileState.tokens[token.index + 2];

                switch (optionName.type)
                {
                    case Token::Type::Acceleration:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setAcceleration(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid acceleration specified.\nUsage: ACCEL <VALUE>", fileState,
                                shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenUpPosition:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenUpPosition(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid raised pen position specified.\nUsage: PENU_POS <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenDownPosition:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenDownPosition(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid lowered pen position specified.\nUsage: PEND_POS <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenUpDelay:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenUpDelay(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen raise delay specified.\nUsage: PENU_DELAY <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenDownDelay:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenDownDelay(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen lower delay specified.\nUsage: PEND_DELAY <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenUpSpeed:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenUpSpeed(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen raise speed specified.\nUsage: PENU_SPEED <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenDownSpeed:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenDownSpeed(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen lower speed specified.\nUsage: PEND_SPEED <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenUpRate:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenUpRate(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen raise rate specified.\nUsage: PENU_RATE <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenDownRate:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenDownRate(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen lower rate specified.\nUsage: PEND_RATE <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::Model:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setModel(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid model specified.\nUsage: MODEL <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::Port:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::String) axiDraw.setPort(next.value);
                        else
                            Log(Log::Type::ERROR, "Invalid port specified.\nUsage: PORT \"<VALUE>\"",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::Units:
                        checkInteractive("UNITS");
                        axiDraw.setUnits(std::stoi(optionValue.value));

                        break;
                    case Token::Type::EndOpts:
                        break;
                    default:
                        Log(Log::Type::ERROR, std::string(
                                    "Invalid option specified.\nUsage: OPTS <OPTIONS> <VALUE>\nOptions: ACCEL, PENU_POS, "
                                    "PEND_POS, PENU_DELAY, PEND_DELAY, PENU_SPEED, PEND_SPEED, PENU_RATE, PEND_RATE, MODEL, "
                                    "PORT") + (axiDraw.getMode() == "interactive" ? ", UNITS" : ""),
                            fileState, shouldExitOnError);
                        break;
                }
                break;
            }
            case Token::Type::UOpts:
            {
                checkInteractive("UOPTS");

                if (fileState.tokens.size() < token.index + 3)
                {
                    Log(Log::Type::ERROR, "No option specified.\nUsage: UOPTS\n\t<option> <value>\n\t...\nEND_UOPTS",
                        fileState, shouldExitOnError);
                    break;
                }

                Token optionName = fileState.tokens[token.index + 1];
                Token optionValue = fileState.tokens[token.index + 2];

                switch (optionName.type)
                {
                    case Token::Type::Acceleration:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setAcceleration(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid acceleration specified.\nUsage: ACCEL <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenUpPosition:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenUpPosition(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid raised pen position specified.\nUsage: PENU_POS <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenDownPosition:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenDownPosition(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid lowered pen position specified.\nUsage: PEND_POS <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenUpDelay:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenUpDelay(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen raise delay specified.\nUsage: PENU_DELAY <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenDownDelay:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenDownDelay(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen lower delay specified.\nUsage: PEND_DELAY <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenUpSpeed:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenUpSpeed(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen raise speed specified.\nUsage: PENU_SPEED <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenDownSpeed:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenDownSpeed(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen lower speed specified.\nUsage: PEND_SPEED <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenUpRate:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenUpRate(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen raise rate specified.\nUsage: PENU_RATE <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::PenDownRate:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setPenDownRate(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid pen lower rate specified.\nUsage: PEND_RATE <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::Model:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setModel(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid model specified.\nUsage: MODEL <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::Port:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::String) axiDraw.setPort(next.value);
                        else
                            Log(Log::Type::ERROR, "Invalid port specified.\nUsage: PORT \"<VALUE>\"",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::Units:
                    {
                        Token next = optionValue;
                        if (next.type == Token::Type::Number) axiDraw.setUnits(std::stoi(next.value));
                        else
                            Log(Log::Type::ERROR, "Invalid units specified.\nUsage: UNITS <VALUE>",
                                fileState, shouldExitOnError);

                        break;
                    }
                    case Token::Type::EndUOpts:
                        break;
                    default:
                        Log(Log::Type::ERROR, "Invalid option specified.\nUsage: UOPTS <OPTIONS> <VALUE>\n"
                                              "Options: ACCEL, PENU_POS, PEND_POS, PENU_DELAY, PEND_DELAY, PENU_SPEED, "
                                              "PEND_SPEED, PENU_RATE, PEND_RATE, MODEL, PORT, UNITS",
                            fileState, shouldExitOnError);
                        break;
                }

                axiDraw.updateOptions();
                break;
            }
            case Token::Type::Connect:
            {
                checkInteractive("CONNECT");
                axiDraw.connect();

                break;
            }
            case Token::Type::Disconnect:
            {
                checkInteractive("DISCONNECT");
                axiDraw.disconnect();

                break;
            }
            case Token::Type::PenUp:
            {
                checkInteractive("PENUP");
                axiDraw.penUp();

                break;
            }
            case Token::Type::PenDown:
            {
                checkInteractive("PENDOWN");
                axiDraw.penDown();

                break;
            }
            case Token::Type::PenToggle:
            {
                checkInteractive("PENTOGGLE");
                axiDraw.penToggle();

                break;
            }
            case Token::Type::Home:
            {
                checkInteractive("HOME");
                axiDraw.home();

                break;
            }
            case Token::Type::GoTo:
            {
                checkInteractive("GOTO");

                if (fileState.tokens.size() < token.index + 2)
                {
                    Log(Log::Type::ERROR, "Invalid coordinates specified.\nUsage: GOTO <X> <Y>", fileState,
                        shouldExitOnError);
                    break;
                }

                std::pair<double, double> point;
                Token nextToken = fileState.tokens[token.index + 1];

                if (nextToken.type == Token::Type::Number)
                {
                    point.first = std::stod(nextToken.value);
                    nextToken = fileState.tokens[token.index + 1];
                } else
                {
                    Log(Log::Type::ERROR, "Invalid X coordinate specified.\nUsage: GOTO <X> <Y>", fileState,
                        shouldExitOnError);
                    break;
                }

                if (nextToken.type == Token::Type::Number) point.second = std::stod(nextToken.value);
                else
                {
                    Log(Log::Type::ERROR, "Invalid Y coordinate specified.\nUsage: GOTO <X> <Y>", fileState,
                        shouldExitOnError);
                    break;
                }

                axiDraw.goTo(point.first, point.second);
                break;
            }
            case Token::Type::GoToRelative:
            {
                checkInteractive("GOTO_REL");

                if (fileState.tokens.size() < token.index + 2)
                {
                    Log(Log::Type::ERROR, "Invalid coordinates specified.\nUsage: GOTO_REL <X> <Y>", fileState,
                        shouldExitOnError);
                    break;
                }

                std::pair<double, double> point;
                Token nextToken = fileState.tokens[token.index + 1];

                if (nextToken.type == Token::Type::Number)
                {
                    point.first = std::stod(nextToken.value);
                    nextToken = fileState.tokens[token.index + 1];
                } else
                {
                    Log(Log::Type::ERROR, "Invalid X coordinate specified.\nUsage: GOTO_REL <X> <Y>", fileState,
                        shouldExitOnError);
                    break;
                }

                if (nextToken.type == Token::Type::Number) point.second = std::stod(nextToken.value);
                else
                {
                    Log(Log::Type::ERROR, "Invalid Y coordinate specified.\nUsage: GOTO_REL <X> <Y>", fileState,
                        shouldExitOnError);
                    break;
                }

                axiDraw.goToRelative(point.first, point.second);
                break;
            }
            case Token::Type::Draw:
            {
                checkInteractive("DRAW");

                if (fileState.tokens.size() < token.index + 2)
                {
                    Log(Log::Type::ERROR, "Invalid coordinates specified.\nUsage: DRAW <X> <Y> <X> <Y> ...",
                        fileState, shouldExitOnError);
                    break;
                }

                std::vector<std::pair<double, double>> points;
                size_t index = token.index + 1;

                while (index < fileState.tokens.size())
                {
                    Token nextToken = fileState.tokens[index];
                    if (nextToken.type == Token::Type::Number)
                    {
                        double x = std::stod(nextToken.value);
                        index++;

                        if (index < fileState.tokens.size())
                        {
                            nextToken = fileState.tokens[index];
                            if (nextToken.type == Token::Type::Number)
                            {
                                double y = std::stod(nextToken.value);
                                index++;

                                points.emplace_back(x, y);
                            } else
                            {
                                Log(Log::Type::ERROR,
                                    "Invalid Y coordinate specified.\nUsage: DRAW <X> <Y> <X> <Y> ...",
                                    fileState, shouldExitOnError);
                                break;
                            }
                        } else
                        {
                            Log(Log::Type::ERROR,
                                "Invalid number of coordinates specified.\nUsage: DRAW <X> <Y> <X> <Y> ...",
                                fileState, shouldExitOnError);
                            break;
                        }
                    } else
                    {
                        Log(Log::Type::ERROR, "Invalid X coordinate specified.\nUsage: DRAW <X> <Y> <X> <Y> ...",
                            fileState, shouldExitOnError);
                        break;
                    }
                }

                axiDraw.draw(points);
                break;
            }
            case Token::Type::Wait:
            {
                checkInteractive("WAIT");

                if (fileState.tokens.size() < token.index + 2)
                {
                    Log(Log::Type::ERROR, "Invalid wait time specified.\nUsage: WAIT <MS>", fileState,
                        shouldExitOnError);
                    break;
                }

                Token nextToken = fileState.tokens[token.index + 1];

                if (nextToken.type == Token::Type::Number) axiDraw.wait(std::stod(nextToken.value));
                else
                {
                    Log(Log::Type::ERROR, "Invalid wait time specified.\nUsage: WAIT <MS>", fileState,
                        shouldExitOnError);
                    break;
                }

                break;
            }
            case Token::Type::GetPos:
            {
                checkInteractive("GETPOS");
                std::pair<double, double> pos = axiDraw.getPosition();

                Log(Log::Type::INFO, "X: " + std::to_string(pos.first) + "Y: " + std::to_string(pos.second));
                break;
            }
            case Token::Type::GetPen:
            {
                checkInteractive("GETPEN");

                Log(Log::Type::INFO, std::string("Pen is ") + (axiDraw.getPen() ? "down" : "up") + ".");
                break;
            }
            case Token::Type::SetPlot:
            {
                if (!isModePlot)
                {
                    Log(Log::Type::ERROR, "SETPLOT can only be used in plot mode.", fileState, shouldExitOnError);
                    break;
                }
                if (fileState.tokens.size() < token.index + 2)
                {
                    Log(Log::Type::ERROR, "No file path/internet URL specified.", fileState, shouldExitOnError);
                    break;
                }

                std::string filePath = fileState.tokens[token.index + 1].value;
                if (filePath.empty())
                {
                    Log(Log::Type::ERROR, "No file path/internet URL specified.", fileState, shouldExitOnError);
                    break;
                }

                if (std::regex_match(filePath, std::regex("https?://.*")))
                    filePath = downloadFile(filePath);

                std::ifstream file(filePath);
                if (!file)
                {
                    Log(Log::Type::ERROR, "Could not open file \"" + filePath + "\".", fileState, shouldExitOnError);
                    break;
                }
                file.close();

                axiDraw.modePlot(filePath);
                break;
            }
            case Token::Type::Plot:
            {
                axiDraw.runPlot();
                break;
            }
            case Token::Type::Unknown:
            {
                Log(Log::Type::ERROR, "Unknown token: " + token.item.value, fileState, shouldExitOnError);
                break;
            }
            case Token::Type::PlotMode:
            case Token::Type::InteractiveMode:
            case Token::EndOpts:
            case Token::EndUOpts:
            case Token::Acceleration:
            case Token::PenUpPosition:
            case Token::PenDownPosition:
            case Token::PenUpDelay:
            case Token::PenDownDelay:
            case Token::PenUpSpeed:
            case Token::PenDownSpeed:
            case Token::PenUpRate:
            case Token::PenDownRate:
            case Token::Model:
            case Token::Port:
            case Token::Units:
            case Token::Number:
            case Token::String:
                break;
            case Token::Type::EndOfFile:
            {
                return;
            }
            default:
            {
                Log(Log::Type::ERROR, "Unexpected token: " + token.item.value, fileState, shouldExitOnError);
                break;
            }
        }
    }
}
