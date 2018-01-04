#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/datastructures/DataObjectBundle.hpp>
#include "Util.h"
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
                add<double>("Delta Seed", "Dichte der Seedpoints", 1);
                add<DefaultValueArray<Point2>>("Random Points", "Zufällig verteilte Punkte");
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

                if (criticalPoint.second == CriticalPointType::SINK || criticalPoint.second == CriticalPointType::SOURCE) {
                    Point2 center = criticalPoint.first;
                    double r = norm(center - nextMiddlePoint);
                    unsigned int numberPointsOnCircle = abs(deltaSeeds * 10 * r);
                    for (unsigned int i = 0; i < numberPointsOnCircle; i++) {
                        double x = center[0] + r * cos(2 * M_PI * i / numberPointsOnCircle) / 2;
                        double y = center[1] + r * sin(2 * M_PI * i / numberPointsOnCircle) / 2;
                        seedpoints.push_back(to3D(Point2(x, y)));
                    }

                    //alle zufälligen Punkte im Radius entfernen
                    for (size_t i = 0; i < randomPoints.size(); i++) {
                        if (norm(randomPoints[i] - center) < r) {
                            randomPoints.erase(randomPoints.begin() + i);
                        }
                    }
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
            return nearestPoint;
        }

        Point3 to3D(Point2 p) {
            return Point3(p[0], p[1], 0);
        }
    };

    AlgorithmRegister<SeedpointTemplateAlgorithm> reg("Hauptaufgabe/SeedpointTemplates", "Berechnet SeedpointTemplates aus kritischen Punkten");
}
