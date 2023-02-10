#include "include/parser.h"

#ifndef MAX_REDIRECTS
#define MAX_REDIRECTS 5
#endif

void Parser::checkInteractive(std::string functionName)
{
	if (!isModeSet)
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: No mode specified. Please set a mode first.\n"
				  << "Usage: MODE <I|P>" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (axiDraw.getMode() != "interactive")
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: " << functionName << " can only be used in interactive mode."
				  << std::endl;
		exit(EXIT_FAILURE);
	}
}

static size_t writeCallback(char* contents, size_t size, size_t nmemb, std::string* buffer)
{
	size_t realSize = size * nmemb;
	buffer->append(contents, realSize);
	return realSize;
}

static std::string downloadFile(const std::string &url, int redirectLevel = 1)
{
	auto cleanText = [](std::string text)
	{
		std::string cleanedText;
		for (char c: text)
		{
			if (c == '\r') cleanedText += "\\r";
			else if (c == '\n') cleanedText += "\\n";
			else if (c == '\t') cleanedText += "\\t";
			else if (c == '\v') cleanedText += "\\v";
			else if (c == '\f') cleanedText += "\\f";
			else if (c == '\a') cleanedText += "\\a";
			else if (c == '\b') cleanedText += "\\b";
			else if (c == '\0') cleanedText += "\\0";
			else if (c == '\033') cleanedText += "\\033";
			else cleanedText += c;
		}

		return cleanedText;
	};

	if (redirectLevel > MAX_REDIRECTS)
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Too many redirects." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "[\033[1;30mDEBUG\033[0m]: " << std::string(redirectLevel * 2, ' ') << "Downloading file from \""
			  << cleanText(url) << "\"." << std::endl;
	std::cout << "[\033[1;30mDEBUG\033[0m]: " << std::string(redirectLevel * 2, ' ') << "Resolving URL." << std::endl;

	CURL* curl = curl_easy_init();
	if (!curl)
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Could not initialize cURL." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string buffer;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, MAX_REDIRECTS);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Could not download file from \"" << cleanText(url) << "\"."
				  << std::endl;
		exit(EXIT_FAILURE);
	}

	long responseCode;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
	if (responseCode == 301 || responseCode == 302)
	{
		char* redirectUrl;
		curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &redirectUrl);

		std::cout << "[\033[1;30mDEBUG\033[0m]: " << std::string(redirectLevel * 2, ' ') << "Redirecting to \""
				  << cleanText(redirectUrl) << "\"." << std::endl;
		curl_easy_cleanup(curl);

		return downloadFile(redirectUrl, redirectLevel + 1);
	}

	curl_easy_cleanup(curl);
	std::cout << "[\033[1;30mDEBUG\033[0m]: " << std::string(redirectLevel * 2, ' ') << "Creating temporary file."
			  << std::endl;

	fs::path temp = fs::temp_directory_path() / fs::unique_path();
	std::ofstream file;
	file.open(temp.string(), std::ios::out | std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "[\033[1;31mERROR\033[0m]: Could not create temporary file for plot." << std::endl;
		exit(EXIT_FAILURE);
	}

	file << buffer;
	file.close();

	std::cout << "[\033[1;30mDEBUG\033[0m]: " << std::string(redirectLevel * 2, ' ') << "Saved to: " << temp.string()
			  << std::endl;
	return temp.string();
}

void Parser::parse()
{
	assert(Token::Type::EndOfFile == 36);

	for (auto token: enumerate(fileState.tokens))
	{
		switch (token.item.type)
		{
			case Token::Type::Mode:
			{
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
						std::cerr << "[\033[1;31mERROR\033[0m]: Invalid mode specified.\n"
								  << "Usage: MODE <I|P>" << std::endl;
						exit(EXIT_FAILURE);
				}
				break;
			}
			case Token::Type::Opts:
			{
				if (!isModeSet)
				{
					std::cerr << "[\033[1;31mERROR\033[0m]: No mode specified. Please set a mode first.\n"
							  << "Usage: MODE <I|P>" << std::endl;
					exit(EXIT_FAILURE);
				}

				switch (fileState.tokens[token.index + 1].type)
				{
					case Token::Type::Acceleration:
						axiDraw.setAcceleration(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::PenUpPosition:
						axiDraw.setPenUpPosition(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::PenDownPosition:
						axiDraw.setPenDownPosition(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::PenUpDelay:
						axiDraw.setPenUpDelay(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::PenDownDelay:
						axiDraw.setPenDownDelay(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::PenUpSpeed:
						axiDraw.setPenUpSpeed(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::PenDownSpeed:
						axiDraw.setPenDownSpeed(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::PenUpRate:
						axiDraw.setPenUpRate(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::PenDownRate:
						axiDraw.setPenDownRate(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::Model:
						axiDraw.setModel(std::stoi(fileState.tokens[token.index + 1].value));
						break;
					case Token::Type::Port:
						axiDraw.setPort(fileState.tokens[token.index + 1].value);
						break;
					case Token::Type::Units:
						if (axiDraw.getMode() == "interactive")
							axiDraw.setUnits(std::stoi(fileState.tokens[token.index + 1].value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: UNITS can only be set in interactive mode."
									  << std::endl;
							exit(EXIT_FAILURE);
						}
						break;
					case Token::Type::EndOpts:
						break;
					default:
						std::cerr << "[\033[1;31mERROR\033[0m]: Invalid option specified.\n"
								  << "Usage: OPTS <OPTIONS> <VALUE>\n"
								  << "Options: ACCEL, PENU_POS, PEND_POS, PENU_DELAY, PEND_DELAY, PENU_SPEED, PEND_SPEED, PENU_RATE, PEND_RATE, MODEL, PORT"
								  << (axiDraw.getMode() == "interactive" ? ", UNITS" : "") << std::endl;
						return;
				}
				break;
			}
			case Token::Type::UOpts:
			{
				checkInteractive("UOPTS");

				switch (fileState.tokens[token.index + 1].type)
				{
					case Token::Type::Acceleration:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setAcceleration(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid acceleration specified.\n"
									  << "Usage: ACCEL <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::PenUpPosition:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenUpPosition(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid raised pen position specified.\n"
									  << "Usage: PENU_POS <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::PenDownPosition:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenDownPosition(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid lowered pen position specified.\n"
									  << "Usage: PEND_POS <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::PenUpDelay:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenUpDelay(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid pen raise delay specified.\n"
									  << "Usage: PENU_DELAY <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::PenDownDelay:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenDownDelay(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid pen lower delay specified.\n"
									  << "Usage: PEND_DELAY <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::PenUpSpeed:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenUpSpeed(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid pen raise speed specified.\n"
									  << "Usage: PENU_SPEED <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::PenDownSpeed:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenDownSpeed(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid pen lower speed specified.\n"
									  << "Usage: PEND_SPEED <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::PenUpRate:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenUpRate(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid pen raise rate specified.\n"
									  << "Usage: PENU_RATE <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::PenDownRate:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenDownRate(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid pen lower rate specified.\n"
									  << "Usage: PEND_RATE <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::Model:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setModel(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid model specified.\n"
									  << "Usage: MODEL <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::Port:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::String) axiDraw.setPort(next.value);
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid port specified.\n"
									  << "Usage: PORT \"<VALUE>\"" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::Units:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setUnits(std::stoi(next.value));
						else
						{
							std::cerr << "[\033[1;31mERROR\033[0m]: Invalid units specified.\n"
									  << "Usage: UNITS <VALUE>" << std::endl;
							exit(EXIT_FAILURE);
						}

						break;
					}
					case Token::Type::EndUOpts:
						break;
					default:
						std::cerr << "[\033[1;31mERROR\033[0m]: Invalid option specified.\n"
								  << "Usage: UOPTS <OPTIONS> <VALUE>\n"
								  << "Options: ACCEL, PENU_POS, PEND_POS, PENU_DELAY, PEND_DELAY, PENU_SPEED, PEND_SPEED, PENU_RATE, PEND_RATE, MODEL, PORT, UNITS"
								  << std::endl;
						exit(EXIT_FAILURE);
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

				std::pair<double, double> point;
				Token nextToken = fileState.tokens[token.index + 1];

				if (nextToken.type == Token::Type::Number)
				{
					point.first = std::stod(nextToken.value);
					nextToken = fileState.tokens[token.index + 1];
				} else
				{
					std::cerr << "[\033[1;31mERROR\033[0m]: Invalid X coordinate specified.\n"
							  << "Usage: GOTO <X> <Y>" << std::endl;
					exit(EXIT_FAILURE);
				}

				if (nextToken.type == Token::Type::Number) point.second = std::stod(nextToken.value);
				else
				{
					std::cerr << "[\033[1;31mERROR\033[0m]: Invalid Y coordinate specified.\n"
							  << "Usage: GOTO <X> <Y>" << std::endl;
					exit(EXIT_FAILURE);
				}

				axiDraw.goTo(point.first, point.second);
				break;
			}
			case Token::Type::GoToRelative:
			{
				checkInteractive("GOTO_REL");

				std::pair<double, double> point;
				Token nextToken = fileState.tokens[token.index + 1];

				if (nextToken.type == Token::Type::Number)
				{
					point.first = std::stod(nextToken.value);
					nextToken = fileState.tokens[token.index + 1];
				} else
				{
					std::cerr << "[\033[1;31mERROR\033[0m]: Invalid X coordinate specified.\n"
							  << "Usage: GOTO_REL <X> <Y>" << std::endl;
					exit(EXIT_FAILURE);
				}

				if (nextToken.type == Token::Type::Number) point.second = std::stod(nextToken.value);
				else
				{
					std::cerr << "[\033[1;31mERROR\033[0m]: Invalid Y coordinate specified.\n"
							  << "Usage: GOTO_REL <X> <Y>" << std::endl;
					exit(EXIT_FAILURE);
				}

				axiDraw.goToRelative(point.first, point.second);
				break;
			}
			case Token::Type::Draw:
			{
//				TODO: Fix `DRAW` command
//				checkInteractive("DRAW");
//
//				std::vector<std::pair<double, double>> points;
//				Token nextToken = fileState.tokens[token.index + 1];
//
//				while (nextToken.type == Token::Type::Number)
//				{
//					std::pair<double, double> point;
//					point.first = std::stod(nextToken.value);
//					nextToken = fileState.tokens[token.index + 1];
//
//					if (nextToken.type == Token::Type::Number)
//					{
//						point.second = std::stod(nextToken.value);
//						nextToken = fileState.tokens[token.index + 1];
//					} else
//					{
//						std::cerr << "[\033[1;31mERROR\033[0m]: Invalid Y coordinate specified.\n"
//								  << "Usage: DRAW <X> <Y> <X> <Y> ..." << std::endl;
//						exit(EXIT_FAILURE);
//					}
//
//					points.push_back(point);
//				}
//
//				axiDraw.draw(points);

				std::cout << "[\033[1;33mWARNING\033[0m]: The `DRAW` command is not yet implemented." << std::endl;
				break;
			}
			case Token::Type::Wait:
			{
				checkInteractive("WAIT");
				Token nextToken = fileState.tokens[token.index + 1];

				if (nextToken.type == Token::Type::Number) axiDraw.wait(std::stod(nextToken.value));
				else
				{
					std::cerr << "[\033[1;31mERROR\033[0m]: Invalid wait time specified.\n"
							  << "Usage: WAIT <MS>" << std::endl;
					exit(EXIT_FAILURE);
				}

				break;
			}
			case Token::Type::GetPos:
			{
				checkInteractive("GETPOS");
				std::pair<double, double> pos = axiDraw.getPosition();
				std::cout << "X: " << pos.first << " Y: " << pos.second << std::endl;

				break;
			}
			case Token::Type::GetPen:
			{
				checkInteractive("GETPEN");
				std::cout << "Pen is " << (axiDraw.getPen() ? "down" : "up") << "." << std::endl;

				break;
			}
			case Token::Type::SetPlot:
			{
				std::string filePath = fileState.tokens[token.index + 1].value;
				if (filePath.empty())
				{
					std::cerr << "[\033[1;31mERROR\033[0m]: No file path/internet URL specified."
							  << std::endl;
					exit(EXIT_FAILURE);
				}

				if (std::regex_match(filePath, std::regex("https?://.*")))
					filePath = downloadFile(filePath);

				std::ifstream file(filePath);
				if (!file)
				{
					std::cerr << "[\033[1;31mERROR\033[0m]: Could not open file '" << filePath << "'."
							  << std::endl;
					exit(EXIT_FAILURE);
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
				std::cerr << "[\033[1;31mERROR\033[0m]: Unknown token '" << token.item.value << "' on line "
						  << fileState.lineNums[token.index] << ".\n  " << fileState.lines[token.index] << "\n  "
						  << std::string(fileState.linePositions[token.index] - token.item.value.length(), ' ')
						  << "\033[1;31m" << std::string(token.item.value.length(), '^') << "\033[0m" << std::endl;
				exit(EXIT_FAILURE);
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
				std::cerr << "[\033[1;31mERROR\033[0m]: Unexpected token '" << token.item.value << "' on line "
						  << fileState.lineNums[token.index] << ".\n  " << fileState.lines[token.index] << "\n  "
						  << std::string(fileState.linePositions[token.index] - token.item.value.length(), ' ')
						  << "\033[1;31m" << std::string(token.item.value.length(), '^') << "\033[0m" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}
}
