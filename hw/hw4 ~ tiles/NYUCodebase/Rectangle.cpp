#pragma once
#include <algorithm>
#include "ShaderProgram.h"

struct Rectangle {
	float vertices[12];
	int size = 12;
	float a;
	float r;
	float g;
	float b;

	Rectangle(float, float, float, float, float, float);
	Rectangle(float, float);
	Rectangle();
	void render(ShaderProgram&);
};


Rectangle::Rectangle(float w, float h, float r, float g, float b, float a) : a(a), r(r), g(g), b(b) {
	float temp[] = {
		-w/2, -h/2,
		w/2, -h/2,
		w/2, h/2,
		-w/2, -h/2,
		w/2, h/2,
		-w/2, h/2
	};

	for (int i = 0; i < Rectangle::size; i++) {
		Rectangle::vertices[i] = temp[i];
	}
}

Rectangle::Rectangle(float w, float h) : Rectangle::Rectangle(w, h, 1.0f, 1.0f, 1.0f, 1.0f) { }
Rectangle::Rectangle() : Rectangle::Rectangle(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f) { }


void Rectangle::render(ShaderProgram& sp) {
	sp.SetColor(Rectangle::r, Rectangle::g, Rectangle::b, Rectangle::a);
	glVertexAttribPointer(sp.positionAttribute, 2, GL_FLOAT, false, 0, Rectangle::vertices);
	glEnableVertexAttribArray(sp.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, Rectangle::size/2);
	glDisableVertexAttribArray(sp.positionAttribute);
}