#pragma once
#include <vector>
#include <string>

class Text;

class CodeTest
{
private:
	std::vector<Text*> codeTexts;
	std::string correctSolution;
	std::string currentText;

	int selectedTextIndex = 0;
	int codeLength = 0;

public:
	CodeTest(std::vector<Text*> texts, std::string solution);
	void render() const;

	void setInputCode(char c);
};

