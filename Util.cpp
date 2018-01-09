#include "Util.hpp"

Point3 to3D(Point2 p) {
    return Point3(p[0], p[1], 0);
}

Point2 to2D(Point3 p) {
    return Point2(p[0], p[1]);
}

std::vector<Vector2> getEigenvectors(Point2 point, TensorFieldContinuous<2, Point2>::Evaluator& evaluator) {
    std::vector<Vector2> eigenvectors;

    Eigen::Matrix2d jacobi = getJacobiMatrix(point, evaluator);

    Eigen::EigenSolver<Eigen::MatrixXd> es(jacobi);
    for (int i = 0; i < 2; i++) {
        auto ev = es.eigenvectors();
        eigenvectors.push_back(Vector2(ev.col(i)(0).real(), ev.col(i)(1).real()));
    }

    return eigenvectors;
}

std::vector<double> getEigenvalues(Point2 point, TensorFieldContinuous<2, Point2>::Evaluator& evaluator) {
    std::vector<double> eigenvalues;

    Eigen::Matrix2d jacobi = getJacobiMatrix(point, evaluator);

    Eigen::EigenSolver<Eigen::MatrixXd> es(jacobi);
    for (int i = 0; i < 3; i++) {
        eigenvalues.push_back(es.eigenvalues()(i).real());
    }

    std::sort(eigenvalues.begin(), eigenvalues.end());
    return eigenvalues;
}

Eigen::Matrix2d getJacobiMatrix(Point2 point, TensorFieldContinuous<2, Point2>::Evaluator& evaluator) {
    Eigen::Matrix2d m(2,2);
    double h = 0.1;

    //Vektor am Punkt und Vektoren in X- bzw. Y-Nachbarschaft bestimmen
    evaluator.reset(point);
    Vector2 vectorAtPoint = evaluator.value();

    evaluator.reset(point + Vector2(h,0));
    Vector2 vectorDeltaX = evaluator.value();
    evaluator.reset(point + Vector2(0,h));
    Vector2 vectorDeltaY = evaluator.value();

    m(0,0) = differentialQuotient(vectorDeltaX[0], vectorAtPoint[0], h);
    m(0,1) = differentialQuotient(vectorDeltaY[0], vectorAtPoint[0], h);
    m(1,0) = differentialQuotient(vectorDeltaX[1], vectorAtPoint[1], h);
    m(1,1) = differentialQuotient(vectorDeltaY[1], vectorAtPoint[1], h);

    return m;
}

/**
 * @brief differentialQuotient
 * @param deltaU: Funktionswert an der nächsten Stelle u+h
 * @param u: Funktionswert an der Stelle
 * @param h: Differenz
 * @return Änderung am Punkt u
 */
double differentialQuotient(double deltaU, double u, double h) {
    return (deltaU - u) / h;
}
