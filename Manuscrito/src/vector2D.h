#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <iostream>

/**
 * Vector bidimensional genérico.
 */
template<typename T = float>
class Vector2D
{
public:
	T x, y;

	Vector2D(T x, T y) : x(x), y(y) { }
	Vector2D() : Vector2D(0, 0) { }

	// Operadores
	Vector2D operator+(const Vector2D& otro) const {
		return {x + otro.x, y + otro.y};
	}

	// TODO: completar

	// Operadores de entrada/salida
	friend std::ostream& operator<<(std::ostream& out, const Vector2D& v) {
		return out << '{' << v.x << ", " << v.y << '}';
	}

	bool operator > (const Vector2D& otro) const {
		return (x > otro.x && y > otro.y);
	}

	bool operator < (const Vector2D& otro) const {
		return (x < otro.x && y < otro.y);
	}
};

#endif // VECTOR2D_H
