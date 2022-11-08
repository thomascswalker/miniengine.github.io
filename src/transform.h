#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix.h"

MINI_NAMESPACE_OPEN
MINI_USING_DIRECTIVE

class Transform
{
public:
	Transform() {};

	Transform&		identity();

	Matrix4			getMatrix() const;
	void			setMatrix(Matrix4& m)
	{	
		m_translation = m.getTranslation();
		m_rotation = m.getRotation();
	}

	void			setTranslation(const Vector3& t);
	const Vector3&	getTranslation() { return m_translation; }
	void			setRotation(const Rotation& r);
	const Rotation& getRotation() { return m_rotation; }
	void			setScale(const Vector3& s);
	const Vector3&	getScale() { return m_scale; }

	const Vector3&  getForward();
	const Vector3&  getRight();
	const Vector3&  getUp();


private:
	Vector3 m_translation;
	Rotation m_rotation;
	Vector3 m_scale;
};

MINI_NAMESPACE_CLOSE

#endif