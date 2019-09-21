#include "aabb.h"

void AABB::Upload(Buffer& Dest)
{
	GLfloat Data[4] = {
		GLfloat(X),
		GLfloat(Y),
		GLfloat(X + Width),
		GLfloat(Y + Height)
	};
	Dest.Upload((void*)& Data, sizeof(Data));
}
