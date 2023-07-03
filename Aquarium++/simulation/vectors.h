//
// Created by paulo on 25/06/2023.
//

#ifndef SDL_PLAYGROUND_VECTORS_H
#define SDL_PLAYGROUND_VECTORS_H

#include <stdint.h>
#include <math.h>

class Vector2 {
public:
	double x;
	double y;
	
	Vector2(double x, double y) {
		this->x = x;
		this->y = y;
	}
	
	inline double length_sqr() {
		return (x * x) + (y * y);
	}
	
	inline double length() {
		return sqrt(length_sqr());
	}
	
	inline double dot(Vector2 vec) {
		return (x * vec.x) + (y * vec.y);
	}
	
	Vector2 scale(double factor) {
		this->x *= factor;
		this->y *= factor;
		
		return *this;
	}
	
	Vector2 clamp(double max_length) {
		
		double l = length();
		
		if (l <= max_length) {
			return *this;
		}
		
		return scale(max_length / l);
	}
	
	Vector2 operator-(Vector2 vec) {
		return {this->x - vec.x, this->y - vec.y};
	}
	
	Vector2 operator+(Vector2 vec) {
		return {x + vec.x, y + vec.y};
	}
	
	Vector2 normalize() {
		return scale(1 / length());
	}
	
	Vector2 perpendicular_norm() {
		double dot_h = dot(Vector2(1, 0));
		double dot_v = dot(Vector2(0, 1));
		//TODO
		return {0, 0};
	}
	
};

double inline rag_to_deg(double x) {
	return ((x) > 0 ? (x) : (2 * M_PI + (x))) * 360 / (2 * M_PI);
}

#endif //SDL_PLAYGROUND_VECTORS_H