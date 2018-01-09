#ifndef HAUPTAUFGABE_UTIL
#define HAUPTAUFGABE_UTIL

#include <fantom/fields.hpp>
#include <Eigen/Dense>

using namespace fantom;

enum CriticalPointType {SINK, SOURCE, SADDLE};

Point3 to3D(Point2 point);
Point2 to2D(Point3 point);
std::vector<Vector2> getEigenvectors(Point2 point, TensorFieldContinuous<2, Point2>::Evaluator& evaluator);
std::vector<double> getEigenvalues(Point2 point, TensorFieldContinuous<2, Point2>::Evaluator& evaluator);
Eigen::Matrix2d getJacobiMatrix(Point2 point, TensorFieldContinuous<2, Point2>::Evaluator& evaluator);
double differentialQuotient(double deltaU, double u, double h);

//typedef DefaultValueArray<std::pair<Point2, CriticalPointType>> CriticalPoints;

#endif
