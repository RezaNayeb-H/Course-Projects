#ifndef _MATH_HELP
#define _MATH_HELP

#include <cmath>
#include <iostream>

#define PI 3.14159265

#define EPSILON 10.0f



/* here we create a mathematical 2D vector that will be used for things like position and velocity in the game */ 
struct vector2D
{
	double x;
	double y;
	
	inline vector2D operator+(vector2D v)
	{
		vector2D res;
		res.x = x + v.x;
		res.y = y + v.y;
		return res;
	}

	inline vector2D operator-(vector2D v)
	{
		vector2D res;
		res.x = x - v.x;
		res.y = y - v.y;
		return res;
	}

	inline void operator+=(vector2D v)
	{
		x += v.x;
		y += v.y;
	}
	
	inline void operator-=(vector2D v)
	{
		x -= v.x;
		y -= v.y;
	}

	inline bool operator==(vector2D v)
	{
		if (v.x == x && v.y == y)
			return true;
		else
			return false;
	}


	inline vector2D operator/(double m)
	{
		vector2D res;
		res.x = x / m;
		res.y = y / m;
		return res;
	}

	double magnitude()
	{
		return (x*x) + (y*y); 
	}

	double magnitudeSqr()
	{
		return x*x + y*y;
	}

	vector2D norm()
	{
		vector2D Norm;
		double mag = sqrt(x*x + y*y);
		if (mag == 0)
			mag = 1;
		Norm.x = x / mag;
		Norm.y = y / mag;
		return Norm;
	}

	double angle()
	{
		return atan2(y, x) * 180.0 / PI;
	}
};


vector2D v2D(int x, int y)
{
	vector2D v;
	v.x = x;
	v.y = y;
	return v;
}

inline vector2D operator*(const double& lhs, const vector2D& rhs)
{
	return v2D(lhs * rhs.x, lhs * rhs.y);
}

inline vector2D operator*(const vector2D& lhs, const double& rhs)
{
	return v2D(lhs.x * rhs, lhs.y * rhs);
}

inline bool operator<=(const vector2D& lhs, const vector2D& rhs)
{
	if (lhs.x <= rhs.x && lhs.y <= rhs.y)
		return 1;
	else
		return 0;
}

std::ostream& operator<< (std::ostream& stream, vector2D const& v)
{
	stream << "{ " << v.x << ", " << v.y << " }";
	return stream;
}

vector2D Norm(vector2D v, double r = 1.0f)
{
	vector2D res;
	double mag = sqrt(v.x*v.x + v.y*v.y);
	if (mag == 0)
		mag = 1;
	res.x = r * v.x / mag;
	res.y = r * v.y / mag;
	return res;
}

vector2D angleNorm(double angle)
{
	vector2D v;
	angle = angle * PI / 180.0;
	v.x = cos(angle);
	v.y = sin(angle);
	return v;
}

vector2D round(vector2D v)
{
	vector2D r;
	r = {round(v.x), round(v.y)};
	return r;
}


bool CheckIntersection(vector2D v1, vector2D v2, vector2D u1, vector2D u2)		// check if line v1v2 intersects with u1u2
{
	double a1 = v2.y - v1.y;
	double b1 = v1.x - v2.x;
	double c1 = a1*(v1.x) + b1*(v1.y);

	double a2 = u2.y - u1.y;
	double b2 = u1.x - u2.x;
	double c2 = a2*(u1.x) + b2*(u1.y);

	double determinant = a1*b2 - a2*b1;

	if(determinant == 0)
	{
			return 0;
	}

	double x = (b2*c1 - b1*c2) / determinant;
	double y = (a1*c2 - a2*c1) / determinant;
	if(x <= std::max(v1.x, v2.x) && x >= std::min(v1.x, v2.x) && y <= std::max(v1.y, v2.y) && y >= std::min(v1.y, v2.y) && x <= std::max(u1.x, u2.x) && x >= std::min(u1.x, u2.x) && y <= std::max(u1.y, u2.y) && y >= std::min(u1.y, u2.y))
		return 1;
	return 0;
	
}

bool CheckCircleCollision(vector2D O1, double r1, vector2D O2, double r2)
{
	double rr = r1 + r2;
	if ((O1 - O2).magnitudeSqr() <= rr * rr)
		return true;
	return false;
}


/* the bezier curve is inspired by galaga SDL tutorials */
struct BezierCurve
{
	vector2D p1;
	vector2D c1;
	vector2D c2;
	vector2D p2;

	vector2D CalculateCurvePoint(float t)
	{
		float tt = t * t;
		float ttt = tt * t;
		float u = 1.0f - t;
		float uu = u * u;
		float uuu = uu * u;

		vector2D point = (uuu * p1) + (3 * uu * t * c1) + (3 * u * tt * c2) + (ttt * p2);
		point.x = round(point.x);
		point.y = round(point.y);
		return point;
	}
};

#endif
