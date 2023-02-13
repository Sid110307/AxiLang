#include "include/parser.h"

void Parser::checkInteractive(std::string functionName)
{
	if (!isModeSet)Log(Log::Type::Error, "No mode specified. Please set a mode first.\nUsage: MODE <I|P>", fileState);
	if (axiDraw.getMode() != "interactive")
		Log(Log::Type::Error, functionName + " can only be used in interactive mode.", fileState);

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

	if (redirectLevel > MAX_REDIRECTS) Log(Log::Type::Fatal, "Too many redirects.");

	Log(Log::Type::Debug, std::string(redirectLevel * 2, ' ') + "Downloading file from \"" + cleanText(url) + "\".");
	Log(Log::Type::Debug, std::string(redirectLevel * 2, ' ') + "Resolving URL.");

	CURL* curl = curl_easy_init();
	if (!curl) Log(Log::Type::Fatal, "Could not initialize cURL.");

	std::string buffer;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, MAX_REDIRECTS);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) Log(Log::Type::Fatal, "Could not download file from \"" + cleanText(url) + "\".");

	long responseCode;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
	if (responseCode == 301 || responseCode == 302)
	{
		char* redirectUrl;
		curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &redirectUrl);

		Log(Log::Type::Debug,
			std::string(redirectLevel * 2, ' ') + "Redirecting to \"" + cleanText(redirectUrl) + "\".");
		curl_easy_cleanup(curl);

		return downloadFile(redirectUrl, redirectLevel + 1);
	}

	curl_easy_cleanup(curl);
	Log(Log::Type::Debug, std::string(redirectLevel * 2, ' ') + "Creating temporary file.");

	boost::filesystem::path temp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
	std::ofstream file;
	file.open(temp.string(), std::ios::out | std::ios::binary);

	if (!file.is_open())
		Log(Log::Type::Fatal, std::string(redirectLevel * 2, ' ') + "Could not create temporary file for plot.");

	file << buffer;
	file.close();

	Log(Log::Type::Debug, std::string(redirectLevel * 2, ' ') + "Saved to: " + temp.string());
	return temp.string();
}

void Parser::parse()
{
	assert(Lexer::Token::Type::EndOfFile == 36);

	auto unknownToken = std::find_if(fileState.tokens.begin(), fileState.tokens.end(), [](Token token)
	{
		return token.type == Token::Type::Unknown;
	});

	if (unknownToken != fileState.tokens.end())
		Log(Log::Type::Error, "Unknown token \"" + unknownToken->value + "\".", fileState);

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
						Log(Log::Type::Error, "Invalid mode specified.\nUsage: MODE <I|P>", fileState);
				}
				break;
			}
			case Token::Type::Opts:
			{
				if (!isModeSet)
					Log(Log::Type::Error, "No mode specified. Please set a mode first.\nUsage: MODE <I|P>", fileState);

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
						else Log(Log::Type::Error, "UNITS can only be set in interactive mode.", fileState);
						break;
					case Token::Type::EndOpts:
						break;
					default:
						Log(Log::Type::Error, std::string(
								"Invalid option specified.\nUsage: OPTS <OPTIONS> <VALUE>\nOptions: ACCEL, PENU_POS, "
								"PEND_POS, PENU_DELAY, PEND_DELAY, PENU_SPEED, PEND_SPEED, PENU_RATE, PEND_RATE, MODEL, "
								"PORT") + (axiDraw.getMode() == "interactive" ? ", UNITS" : ""), fileState);
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
						else Log(Log::Type::Error, "Invalid acceleration specified.\nUsage: ACCEL <VALUE>", fileState);

						break;
					}
					case Token::Type::PenUpPosition:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenUpPosition(std::stoi(next.value));
						else
							Log(Log::Type::Error, "Invalid raised pen position specified.\nUsage: PENU_POS <VALUE>",
								fileState);

						break;
					}
					case Token::Type::PenDownPosition:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenDownPosition(std::stoi(next.value));
						else
							Log(Log::Type::Error, "Invalid lowered pen position specified.\nUsage: PEND_POS <VALUE>",
								fileState);

						break;
					}
					case Token::Type::PenUpDelay:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenUpDelay(std::stoi(next.value));
						else
							Log(Log::Type::Error, "Invalid pen raise delay specified.\nUsage: PENU_DELAY <VALUE>",
								fileState);

						break;
					}
					case Token::Type::PenDownDelay:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenDownDelay(std::stoi(next.value));
						else
							Log(Log::Type::Error, "Invalid pen lower delay specified.\nUsage: PEND_DELAY <VALUE>",
								fileState);

						break;
					}
					case Token::Type::PenUpSpeed:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenUpSpeed(std::stoi(next.value));
						else
							Log(Log::Type::Error, "Invalid pen raise speed specified.\nUsage: PENU_SPEED <VALUE>",
								fileState);

						break;
					}
					case Token::Type::PenDownSpeed:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenDownSpeed(std::stoi(next.value));
						else
							Log(Log::Type::Error, "Invalid pen lower speed specified.\nUsage: PEND_SPEED <VALUE>",
								fileState);

						break;
					}
					case Token::Type::PenUpRate:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenUpRate(std::stoi(next.value));
						else
							Log(Log::Type::Error, "Invalid pen raise rate specified.\nUsage: PENU_RATE <VALUE>",
								fileState);

						break;
					}
					case Token::Type::PenDownRate:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setPenDownRate(std::stoi(next.value));
						else
							Log(Log::Type::Error, "Invalid pen lower rate specified.\nUsage: PEND_RATE <VALUE>",
								fileState);

						break;
					}
					case Token::Type::Model:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setModel(std::stoi(next.value));
						else Log(Log::Type::Error, "Invalid model specified.\nUsage: MODEL <VALUE>", fileState);

						break;
					}
					case Token::Type::Port:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::String) axiDraw.setPort(next.value);
						else Log(Log::Type::Error, "Invalid port specified.\nUsage: PORT \"<VALUE>\"", fileState);

						break;
					}
					case Token::Type::Units:
					{
						Token next = fileState.tokens[token.index + 1];
						if (next.type == Token::Type::Number) axiDraw.setUnits(std::stoi(next.value));
						else Log(Log::Type::Error, "Invalid units specified.\nUsage: UNITS <VALUE>", fileState);

						break;
					}
					case Token::Type::EndUOpts:
						break;
					default:
						Log(Log::Type::Error, "Invalid option specified.\nUsage: UOPTS <OPTIONS> <VALUE>\n"
											  "Options: ACCEL, PENU_POS, PEND_POS, PENU_DELAY, PEND_DELAY, PENU_SPEED, "
											  "PEND_SPEED, PENU_RATE, PEND_RATE, MODEL, PORT, UNITS", fileState);
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
				} else Log(Log::Type::Error, "Invalid X coordinate specified.\nUsage: GOTO <X> <Y>", fileState);

				if (nextToken.type == Token::Type::Number) point.second = std::stod(nextToken.value);
				else Log(Log::Type::Error, "Invalid Y coordinate specified.\nUsage: GOTO <X> <Y>", fileState);

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
				} else Log(Log::Type::Error, "Invalid X coordinate specified.\nUsage: GOTO_REL <X> <Y>", fileState);

				if (nextToken.type == Token::Type::Number) point.second = std::stod(nextToken.value);
				else Log(Log::Type::Error, "Invalid Y coordinate specified.\nUsage: GOTO_REL <X> <Y>", fileState);

				axiDraw.goToRelative(point.first, point.second);
				break;
			}
			case Token::Type::Draw:
			{
				checkInteractive("DRAW");

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

								points.push_back(std::make_pair(x, y));
							}
							Log(Log::Type::Error, "Invalid Y coordinate specified.\nUsage: DRAW <X> <Y> <X> <Y> ...",
								fileState);
						} else
							Log(Log::Type::Error,
								"Invalid number of coordinates specified.\nUsage: DRAW <X> <Y> <X> <Y> ...", fileState);
					} else
						Log(Log::Type::Error, "Invalid X coordinate specified.\nUsage: DRAW <X> <Y> <X> <Y> ...",
							fileState);
				}

				axiDraw.draw(points);
				break;
			}
			case Token::Type::Wait:
			{
				checkInteractive("WAIT");
				Token nextToken = fileState.tokens[token.index + 1];

				if (nextToken.type == Token::Type::Number) axiDraw.wait(std::stod(nextToken.value));
				else Log(Log::Type::Error, "Invalid wait time specified.\nUsage: WAIT <MS>", fileState);

				break;
			}
			case Token::Type::GetPos:
			{
				checkInteractive("GETPOS");
				std::pair<double, double> pos = axiDraw.getPosition();

				Log(Log::Type::Info, "X: " + std::to_string(pos.first) + "Y: " + std::to_string(pos.second));
				break;
			}
			case Token::Type::GetPen:
			{
				checkInteractive("GETPEN");

				Log(Log::Type::Info, std::string("Pen is ") + (axiDraw.getPen() ? "down" : "up") + ".");
				break;
			}
			case Token::Type::SetPlot:
			{
				if (!isModePlot) Log(Log::Type::Error, "SETPLOT can only be used in plot mode.", fileState);

				std::string filePath = fileState.tokens[token.index + 1].value;
				if (filePath.empty()) Log(Log::Type::Error, "No file path/internet URL specified.", fileState);

				if (std::regex_match(filePath, std::regex("https?://.*")))
					filePath = downloadFile(filePath);

				std::ifstream file(filePath);
				if (!file) Log(Log::Type::Error, "Could not open file \"" + filePath + "\".", fileState);
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
				Log(Log::Type::Error, "Unknown token: " + token.item.value, fileState);
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
				Log(Log::Type::Error, "Unexpected token: " + token.item.value, fileState);
			}
		}
	}
}
