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
			file >> tag >> textData.size;
			file >> tag >> textData.textEnd;

			std::string color;
			file >> tag >> color;

			try {
				textData.color = hexToSDLColor(color);
			}
			catch (std::string s) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, s.c_str());
				textData.color = { 0, 0, 0, 255 };
			}

			file >> tag >> textData.uv;
			file >> tag >> textData.revealNoteIndex;

			// Eliminamos el primer espacio en blanco
			textData.text.erase(0, 1);

			data[i][j] = textData;
		}
	}
}

SDL_Color LoadTexts::hexToSDLColor(const std::string& hex) {
	std::string hex_cleaned = hex;

	// 1. Eliminar el '#' inicial si existe
	if (!hex_cleaned.empty() && hex_cleaned[0] == '#') {
		hex_cleaned.erase(0, 1);
	}

	// 2. Validar la longitud (6 para RGB, 8 para RGBA)
	if (hex_cleaned.length() != 6 && hex_cleaned.length() != 8) {
		throw std::string("Formato hexadecimal inválido. Se esperaba RRGGBB o AARRGGBB.");
	}

	// 3. Extraer y convertir los componentes
	unsigned int r = 0, g = 0, b = 0, a = 255; // Alfa por defecto es opaco (255)

	try {
		if (hex_cleaned.length() == 8) {
			// Formato AARRGGBB
			a = std::stoul(hex_cleaned.substr(0, 2), nullptr, 16);
			r = std::stoul(hex_cleaned.substr(2, 2), nullptr, 16);
			g = std::stoul(hex_cleaned.substr(4, 2), nullptr, 16);
			b = std::stoul(hex_cleaned.substr(6, 2), nullptr, 16);
		}
		else {
			// Formato RRGGBB
			r = std::stoul(hex_cleaned.substr(0, 2), nullptr, 16);
			g = std::stoul(hex_cleaned.substr(2, 2), nullptr, 16);
			b = std::stoul(hex_cleaned.substr(4, 2), nullptr, 16);
		}
	}
	catch (const std::exception& e) {
		throw std::string("Caracter inválido encontrado en la cadena hexadecimal.");
	}

	// 4. Crear y devolver la estructura SDL_Color
	SDL_Color color;
	color.r = static_cast<Uint8>(r);
	color.g = static_cast<Uint8>(g);
	color.b = static_cast<Uint8>(b);
	color.a = static_cast<Uint8>(a);

	return color;
}

TextData LoadTexts::getTextData(size_t pagina, size_t index)
{
	return data[pagina][index];
}

int LoadTexts::getTextsCount(size_t pagina)
{
	return data[pagina].size();
}