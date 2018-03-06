/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef GRADIENT_DESCENT_SOLVER_H_INCLUDED
#define GRADIENT_DESCENT_SOLVER_H_INCLUDED

#include "math-basics.hpp"
#include <boost/function.hpp>

#include <functional>
#include <stdexcept>

typedef std::function<double (double x, double y)> Function2D;

class GradientDescentSolver
{
public:
    GradientDescentSolver();
    
    
    void setStartPosition(double x, double y);
    void setLimits(double leftX, double rightX, double leftY, double rightY);
    void setLimitsUsage(bool limitsUsage);
    void setFunction(const Function2D& function);
    
    /// Set x,y precision for variable step method
    void setXYPrecision(double xyPrecision);
    
    /// Set step for fluxion calculating
    void setDerivativeStep(double step);
    
    /// Set initial step for variable step method
    void setInitialStep(double step);
    /// Run solving using variable step method
    void runVariableStep();
    
    /// Get solved x
    double getX();
    /// Get solved y
    double getY();
    /// Get solved minimum value of function
    double getValue();

private:
    Function2D m_function;
    /// \todo Implement Limint usage
    bool m_limitsUsage;
    double m_xyPrecision;
    double m_initialStep;
    double m_startX, m_startY;
    double m_leftX, m_rightX, m_leftY, m_rightY;
    double m_resultX, m_resultY, m_resultVal;
    
    double m_currentX, m_currentY;
    
    double m_derivativeStep;
    
    double m_directionCosAx, m_directionCosAy;
    
    double m_stepDecreasingCoefficient;
    double m_curentStep;
    
    /// Decrease step or return false if step is enough small
    bool isReady(double prevVal, double val);
    
    void moveTo(double t);
};

#endif // GRADIENT_DESCENT_SOLVER_H_INCLUDED
