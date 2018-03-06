/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "gradient-descent-solver.hpp"
#include "cmath"

const double COS_PRECISION = 1e-6;

//////////////////////
// GradientDescentSolver

GradientDescentSolver::GradientDescentSolver() :
    m_limitsUsage(false),
    m_xyPrecision(1e-7),
    m_initialStep(1),
    m_startX(0),
    m_startY(0),
    m_leftX(0),
    m_rightX(0),
    m_leftY(0),
    m_rightY(0),
    m_resultX(0),
    m_resultY(0),
    m_resultVal(0),
    m_derivativeStep(1e-10),
    m_stepDecreasingCoefficient(0.1)
{
}

void GradientDescentSolver::setInitialStep(double step)
{
    m_initialStep = step;
}

void GradientDescentSolver::setXYPrecision(double xyPrecision)
{
    m_xyPrecision = xyPrecision;
}

void GradientDescentSolver::setFunction(const Function2D& function)
{
    m_function = function;
}

void GradientDescentSolver::setDerivativeStep(double step)
{
    m_derivativeStep = step;
}

void GradientDescentSolver::setStartPosition(double x, double y)
{
    m_startX = x; m_startY = y;
}

void GradientDescentSolver::setLimits(double leftX, double rightX, double leftY, double rightY)
{
    m_leftX = leftX; m_rightX = rightX;
    m_leftY = leftY; m_rightY = rightY;
}

void GradientDescentSolver::setLimitsUsage(bool limitsUsage)
{
    m_limitsUsage = limitsUsage;
}

double GradientDescentSolver::getX()
{
    return m_resultX;
}

double GradientDescentSolver::getY()
{
    return m_resultY;
}

double GradientDescentSolver::getValue()
{
    return m_resultVal;
}

void GradientDescentSolver::moveTo(double t)
{
    m_currentX += m_directionCosAx*t;
    m_currentY += m_directionCosAy*t;
}

void GradientDescentSolver::runVariableStep()
{
    if (m_limitsUsage) {
        throw std::runtime_error("Sorry, but limits usage are not implemented yet");
    }
    /// \todo [Low] Control escaping the area specified by limits
    m_currentX = m_startX;
    m_currentY = m_startY;
    
    m_curentStep = m_initialStep;
    
    double val = m_function(m_currentX, m_currentY);
    double prevVal;
    unsigned int itersDone = 0;
    while (itersDone <= 2 || !isReady(prevVal, val))
    {
        prevVal = val;/*
        double df_dx = (m_function(m_currentX + m_derivativeStep, m_currentY) - val);
        double df_dy = (m_function(m_currentX, m_currentY + m_derivativeStep) - val);*/
        
        // There is no need to devide by m_derivativeStep because we are normalizing gradient then
        double df_dx = m_function(m_currentX + m_derivativeStep, m_currentY)
                        - m_function(m_currentX - m_derivativeStep, m_currentY);
        double df_dy = m_function(m_currentX, m_currentY + m_derivativeStep)
                        - m_function(m_currentX, m_currentY - m_derivativeStep);
        
        double gradModule = sqrt(sqr(df_dx) + sqr(df_dy));
        
        m_directionCosAx = - df_dx / gradModule;
        m_directionCosAy = - df_dy / gradModule;
        
        if (std::isnan(m_directionCosAx) || std::isnan(m_directionCosAy))
        {
            // This case mean that gradModule == 0. Probably this point is solution
            break;
        }
        
        m_currentX += m_curentStep * m_directionCosAx;
        m_currentY += m_curentStep * m_directionCosAy;
        
        val = m_function(m_currentX, m_currentY);
        
        itersDone++;
    }
    m_resultX = m_currentX;
    m_resultY = m_currentY;
}

bool GradientDescentSolver::isReady(double prevVal, double val)
{
    if (prevVal > val)
        return false;
    
    if (m_curentStep < m_xyPrecision)
        return true;
    
    m_curentStep *= m_stepDecreasingCoefficient;
    return false;
}
