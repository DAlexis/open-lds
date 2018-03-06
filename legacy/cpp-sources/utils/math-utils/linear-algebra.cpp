/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "linear-algebra.hpp"
#include "math-basics.hpp"
#include <math.h>

///////////////////////
// 2d algebra

LinearRegression2DFinder::LinearRegression2DFinder() :
    m_hasResult(false)
{
}

void LinearRegression2DFinder::setArrays(size_t length, double* x, double* y)
{
    m_px = x;
    m_py = y;
    m_length = length;
    m_hasResult = false;
}
 
double LinearRegression2DFinder::getAngle()
{
    if (not m_hasResult) calculate();
    return m_angle;
}

double LinearRegression2DFinder::getDistance()
{
    if (not m_hasResult) calculate();
    return m_distance;
}

double LinearRegression2DFinder::getError()
{
    if (not m_hasResult) calculate();
    return m_error;
}

double LinearRegression2DFinder::getNormedError()
{
    if (not m_hasResult) calculate();
    return m_normedError;
}

void LinearRegression2DFinder::calculate()
{
    double xCenter = 0, yCenter = 0;
    for (size_t i=0; i<m_length; i++)
    {
        xCenter += m_px[i]; yCenter += m_py[i]; 
    }
    xCenter /= m_length; yCenter /= m_length;
    
    /*
     * sin (2 alpha + ksi) = 0
     * 
     * cos ksi ~ sum(y_i^2) - sum(x_i^2)
     * sin ksi ~ 2sum(x_i y_i)
     * 
     * where center of mass of x and y is (0, 0)!
     */
     
    double cosKsi = 0, sinKsi = 0;
    double totalSquareDistFromCenter = 0;
    for (size_t i=0; i<m_length; i++)
    {
        double x = m_px[i] - xCenter;
        double y = m_py[i] - yCenter;
        totalSquareDistFromCenter += x*x+y*y; 
        cosKsi += y*y - x*x;
        sinKsi += x*y;
    }
    sinKsi *= 2.0;
    
    double ksi = atan2(sinKsi, cosKsi);
    m_angle = -ksi / 2.0;
    
    /*
     * d + cos(alpha)sum(x_i)/N + sin(alpha)sum(y_i)/N = 0
     */
    m_distance = -(cos(m_angle)*xCenter + sin(m_angle)*yCenter);
    
    // Calculating absolute error
    m_error = 0;
    for (size_t i=0; i<m_length; i++)
    {
        m_error += sqr(cos(m_angle)*m_px[i] + sin(m_angle)*m_py[i]+m_distance);
    }
    
    // Calculating normed error
    m_normedError = m_error / totalSquareDistFromCenter;
    m_hasResult = true;
}

void makeXPositive(Vector2d vec, const double tanDeltaAlpha)
{
    if (vec[0] < 0) {
        double len=vectorLength(vec);
        if (fabs(vec[0]/len) > tanDeltaAlpha) {
            vec[0] = -vec[0];
            vec[1] = -vec[1];
        }
    }
}

double vectorLength(const Vector2d vec)
{
    return sqrt(sqr(vec[0]) + sqr(vec[1]));
}

///////////////////////
// 3d linear algebra
void vectorProduct(Vector3d result, const Vector3d vec1, const Vector3d vec2)
{
    result[0] =   vec1[1] * vec2[2] - vec1[2] * vec2[1];
    result[1] = - vec1[0] * vec2[2] + vec1[2] * vec2[0];
    result[2] =   vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

double scalarProduct(const Vector3d vec1, const Vector3d vec2)
{
    return vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];
}

double normal(const Vector3d vec)
{
    return sqrt(sqr(vec[0]) + sqr(vec[1]) + sqr(vec[2]));
}

void normalize(Vector3d vec)
{
    double norm = normal(vec);
    vec[0] /= norm;
    vec[1] /= norm;
    vec[2] /= norm;
}

void inverse(Vector3d vec)
{
    vec[0] = -vec[0];
    vec[1] = -vec[1];
    vec[2] = -vec[2];
}

void applyTransformation(const Matrix3x3 matrix, Vector3d vec)
{
    Vector3d tmp;
    tmp[0] = vec[0]; tmp[1] = vec[1]; tmp[2] = vec[2];
    vec[0] = matrix[0+0] * tmp[0] + matrix[0+1] * tmp[1] + matrix[0+2] * tmp[2];
    vec[1] = matrix[3+0] * tmp[0] + matrix[3+1] * tmp[1] + matrix[3+2] * tmp[2];
    vec[2] = matrix[6+0] * tmp[0] + matrix[6+1] * tmp[1] + matrix[6+2] * tmp[2];
}

void createRotationMatrix(Matrix3x3 matrix, const Vector3d axis, double angle)
{
    createRotationMatrix(matrix, axis, cos(angle), sin(angle));
}

void createRotationMatrix(Matrix3x3 matrix, const Vector3d axis, double cos, double sin)
{
    const int x = 0, y = 1, z = 2;
    matrix[0+0] = cos + (1-cos)*sqr(axis[x]);            matrix[0+1] = (1-cos)*axis[x]*axis[y] - sin*axis[z]; matrix[0+2] = (1-cos)*axis[x]*axis[z] + sin*axis[y];
    matrix[3+0] = (1-cos)*axis[y]*axis[x] + sin*axis[z]; matrix[3+1] = cos + (1-cos)*sqr(axis[y]);            matrix[3+2] = (1-cos)*axis[y]*axis[z] - sin*axis[x];
    matrix[6+0] = (1-cos)*axis[z]*axis[x] - sin*axis[y]; matrix[6+1] = (1-cos)*axis[z]*axis[y] + sin*axis[x]; matrix[6+2] = cos + (1-cos)*sqr(axis[z]);
}

