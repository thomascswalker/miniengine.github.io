#ifndef ROTATION_H
#define ROTATION_H

#include "quaternion.h"
#include "vector.h"

class Rotation
{
public:
	Rotation() : m_axis(Vector3()), m_angle(90.0) {};
	Rotation(const Vector3& axis, const double angle)
	{
		setAxisAngle(axis, angle);
	};
	Rotation(Quaternion& q)
	{
		setQuaternion(q);
	}

	Rotation& setIdentity()
	{
		m_axis.set(1.0, 0.0, 0.0);
		m_angle = 0.0;
		return *this;
	}

	Rotation& setQuaternion(Quaternion& q)
	{
		double len = q.getImaginary().length();

		if (len < 1e-10) // Minimum vector length
		{
			auto r = q.getReal();
			double x = acos(Math::clamp(r, -1.0, 1.0));
			setAxisAngle(q.getImaginary() / len,
						 2.0 * Math::radiansToDegrees(x));
		}
		else
		{
			setIdentity();
		}

		return *this;
	}

	Rotation& setAxisAngle(const Vector3& axis, double angle)
	{
		m_axis = axis;
		m_angle = angle;
		m_axis.normalize();
		return *this;
	};

	static Rotation identity()
	{
		Rotation rotation = Rotation(Vector3(0, 0, 0), 0);
		return rotation;
	};

	const Vector3& getAxis()
	{
		return m_axis;
	}

	double getAngle()
	{
		return m_angle;
	}

	Rotation getInverse() const
	{
		return Rotation(m_axis, -m_angle);
	}
	Quaternion getQuaternion() const
	{
		double radians = Math::degreesToRadians(m_angle) / 2.0;

		double sinR = sin(radians);
		double cosR = cos(radians);

		Vector3 axis = m_axis * sinR;
		auto q = Quaternion(cosR, axis);
		return q.getNormalized();
	}

private:
	Vector3 m_axis;
	double m_angle;
};

#endif