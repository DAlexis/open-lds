/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef LINEAR_ALGEBRA_H_INCLUDED
#define LINEAR_ALGEBRA_H_INCLUDED

#include <stddef.h>

typedef double Vector2d[2];
typedef double Vector3d[3];
typedef double Matrix3x3[9];

///////////////////////
// 2d algebra

/** @brief Find best linear regression for set of points minimizing 2d-distance between point and line
 * The line equation is cos(Angle)*x + sin(Angle)*y + Distance = 0
 */
class LinearRegression2DFinder
{
public:
    LinearRegression2DFinder();
    void setArrays(size_t length, double* x, double* y);
    
    double getAngle();
    double getDistance();
    /// Returns sum(r_i^2)
    double getError();
    /// Returns average(r_i^2)/average( (r_i-center)^2 )
    double getNormedError();
private:
    void calculate();
    
    bool m_hasResult;
    double *m_px, *m_py;
    double m_angle;
    double m_distance;
    double m_error;
    double m_normedError;
    size_t m_length;
};

/** @brief Reflect vector from the point (0,0) if it is in II or III quadrant,
 * but only if x/y<tan(deltaAlpha)
 * @todo Think about another method...
 */
void makeXPositive(Vector2d vec, const double tanDeltaAlpha = 0.05);
double vectorLength(const Vector2d vec);

///////////////////////
// 3d linear algebra
void vectorProduct(Vector3d result, const Vector3d vec1, const Vector3d vec2);
double scalarProduct(const Vector3d vec1, const Vector3d vec2);
double normal(const Vector3d vec);
void normalize(Vector3d vec);
void inverse(Vector3d vec);
void applyTransformation(const Matrix3x3 matrix, Vector3d vec);

void createRotationMatrix(Matrix3x3 matrix, const Vector3d axis, double angle);
void createRotationMatrix(Matrix3x3 matrix, const Vector3d axis, double cos, double sin);

#endif // LINEAR_ALGEBRA_H_INCLUDED
