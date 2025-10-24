#pragma once
#include <vector>
#include <string>

class Text;
class Book;
class GameObject;

class CodeTest
{
private:
	std::vector<Text*> codeTexts;
	std::string correctSolution;
	std::string currentText;

	Book* book = nullptr;
	GameObject* page = nullptr;
	int pageIndex = -1;

	int selectedTextIndex = 0;
	int codeLength = 0;

public:
	CodeTest(std::vector<Text*> texts, std::string solution, Book& book, GameObject* page, int pageIndex);
	void render() const;

	void setInputCode(char c);
};

