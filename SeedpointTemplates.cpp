#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/datastructures/DataObjectBundle.hpp>
#include "Util.hpp"
#include <math.h>

using namespace fantom;

namespace
{
/**
     * @brief The SeedpointTemplateAlgorithm class
     * Calculates the seedpoints from critical points and Voronoi cells
     */
    class SeedpointTemplateAlgorithm : public DataAlgorithm
    {
    public:
        struct Options : public DataAlgorithm::Options
        {
            Options( Control& control ) : DataAlgorithm::Options( control )
            {
                add<DefaultValueArray<std::pair<Point2, CriticalPointType>>>( "Critical Points", "List of critical points");
                //TODO voronoi-zellen
                add<double>("Delta Seed", "Dichte der Seedpoints", 2.5);
                add<DefaultValueArray<Point2>>("Random Points", "Zufällig verteilte Punkte");
                add<TensorFieldContinuous<2, Vector2>>("Field", "Feld mit Input" );
            }
        };

        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
            DataOutputs( DataOutputs::Control& control) : DataAlgorithm::DataOutputs( control )
            {
               add<DefaultValueArray<Point3>>( "Seedpoints" );
            }
        };

        SeedpointTemplateAlgorithm( InitData & data) : DataAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            double deltaSeeds = options.get<double>("Delta Seed");
            auto criticalPoints = options.get<DefaultValueArray<std::pair<Point2, CriticalPointType>>>("Critical Points");
            if (!criticalPoints) return;
            auto randomPointsValueArray = options.get<DefaultValueArray<Point2>>("Random Points");
            if (!randomPointsValueArray) return;
            auto field = options.get<TensorFieldContinuous<2, Vector2>>("Field");
            if(!field) return;
            auto evaluator = field->makeEvaluator();

            std::vector<Point2> randomPoints;
            for (size_t i = 0; i < randomPointsValueArray->size(); i++) {
                randomPoints.push_back(randomPointsValueArray->operator[](i));
            }

            std::vector<Point3> seedpoints;

            std::vector<Point2> pointList;
            for (size_t i = 0; i < criticalPoints->size(); i++) {
                pointList.push_back(criticalPoints->operator[](i).first);
            }

            //Pattern berechnen
            for (size_t i = 0; i < criticalPoints->size(); i++) {
                auto criticalPoint = criticalPoints->operator[](i);
                Point2 nextMiddlePoint = getNextMiddlePoint(criticalPoint.first, pointList);

                Point2 center = criticalPoint.first;
                double r = norm(center - nextMiddlePoint);

                if (criticalPoint.second == CriticalPointType::SINK || criticalPoint.second == CriticalPointType::SOURCE) {                    
                    unsigned int numberPointsOnCircle = abs(deltaSeeds * 10 * r);
                    for (unsigned int i = 0; i < numberPointsOnCircle; i++) {
                        double x = center[0] + r * cos(2 * M_PI * i / numberPointsOnCircle) / 2;
                        double y = center[1] + r * sin(2 * M_PI * i / numberPointsOnCircle) / 2;
                        seedpoints.push_back(to3D(Point2(x, y)));
                    }
                }

                if (criticalPoint.second == CriticalPointType::SADDLE) {
                    std::vector<Vector2> eigenvectors = getEigenvectors(center, *evaluator);

                    unsigned int numberPointsOnLines= deltaSeeds * 3 * r;
                    for (size_t i = 0; i < eigenvectors.size(); i++) {
                        Vector2 normalizedEigenvector = normalized(eigenvectors[i]);
                        Vector2 bisector(normalizedEigenvector[1], -normalizedEigenvector[0]); //orthogonal zur Eigenvektorrichtung
                        for (unsigned int j = 1; j <= numberPointsOnLines; j++) { //nicht durch 0 dividieren
                            seedpoints.push_back(to3D(center + j*r/numberPointsOnLines * bisector)); //Punkte gleichverteilt auf Orthogonalen zeichnen
                        }
                        seedpoints.push_back(to3D(center - r/2 * bisector)); //ein zusätzlicher Punkt in der entgegengesetzten Richtung
                    }
                }

                //alle zufälligen Punkte im Radius entfernen
                std::vector<Point2> pointsToDeleteList;

                for (size_t i = 0; i < randomPoints.size(); i++) {
                    if (norm(randomPoints[i] - center) < r) {
                        pointsToDeleteList.push_back(randomPoints[i]);
                    }
                }

                for (size_t i = 0; i < pointsToDeleteList.size(); i++) {
                    randomPoints.erase(std::remove(randomPoints.begin(), randomPoints.end(), pointsToDeleteList[i]), randomPoints.end());
                }
            }

            //zufällige Punkte
            for (size_t i = 0; i < randomPoints.size(); i++) {
                seedpoints.push_back(to3D(randomPoints[i]));
            }

            DefaultValueArray<Point3> valueArray(seedpoints, Precision::UINT64);
            auto result = std::make_shared<DefaultValueArray<Point3>>(valueArray);
            setResult("Seedpoints", result);
        }

        Point2 getNextMiddlePoint(Point2 center, std::vector<Point2> pointList) {
            Point2 nearestPoint = pointList[0];
            for (Point2& point : pointList) {
                if (center == point) continue;
                if (norm(point - center) < norm(nearestPoint - center)) {
                    nearestPoint = point;
                }
            }
            Point2 middlePoint = center + (nearestPoint - center)/2;
            return middlePoint;
        }
    };

    AlgorithmRegister<SeedpointTemplateAlgorithm> reg("Hauptaufgabe/SeedpointTemplates", "Berechnet SeedpointTemplates aus kritischen Punkten");
}
