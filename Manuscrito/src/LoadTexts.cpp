#include "LoadTexts.h"

#include <SDL3/SDL.h>
#include <string>
#include <fstream>

LoadTexts::LoadTexts(std::string filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open text data file: %s", filePath.c_str());
		return;
	}

	std::string tag;

	int pagesCount = 0;
	file >> tag >> pagesCount;

		for (int i = 0; i < pagesCount; i++) {
		//leemos PaginaX
		file >> tag;

		int textsCount = 0;
		file >> tag >> textsCount;
		for (int j = 0; j < textsCount; j++) {
			//leemos Tx
			file >> tag;

			TextData textData = TextData();
			file >> tag;
			std::getline(file, textData.text);
			file >> tag >> textData.position.x;
			file >> tag >> textData.position.y;
			file >> tag >> textData.textEnd;
			file >> tag >> textData.rect.x;
			file >> tag >> textData.rect.y;
			file >> tag >> textData.rect.w;
			file >> tag >> textData.rect.h;

			// Eliminamos el primer espacio en blanco
			textData.text.erase(0, 1);

			data[i][j] = textData;
		}
	}
}

TextData LoadTexts::getTextData(size_t pagina, size_t index)
{
	return data[pagina][index];
}

int LoadTexts::getTextsCount(size_t pagina)
{
	return data[pagina].size();
}