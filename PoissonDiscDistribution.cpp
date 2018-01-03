#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/datastructures/DataObjectBundle.hpp>
#include "Util.h"
#include <queue>
#include <math.h>

using namespace fantom;

namespace
{
/**
     * @brief The PoissonDiscDistributionAlgorithm class
     * Create random points
     */
    class PoissonDiscDistributionAlgorithm : public DataAlgorithm
    {
    public:
        struct Options : public DataAlgorithm::Options
        {
            Options( Control& control ) : DataAlgorithm::Options( control )
            {
                add< Grid < 2 > >("Grid", "Grid" );
                add<double>("Delta Seed", "Dichte der Zufallspunkte", 5);
            }
        };

        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
            DataOutputs( DataOutputs::Control& control) : DataAlgorithm::DataOutputs( control )
            {
               add<DefaultValueArray<Point3>>( "Seedpoints" );
            }
        };

        PoissonDiscDistributionAlgorithm( InitData & data) : DataAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            auto grid = options.get<Grid<2>>("Grid");
            if (!grid) return;
            const ValueArray<Point2>& points = grid->points();

            std::vector<Point3> seedpoints;
            size_t newPointCount = 2;
            double deltaSeed = options.get<double>("Delta Seed");

            double minX = std::numeric_limits<double>::max();
            double minY = std::numeric_limits<double>::max();
            double maxX = std::numeric_limits<double>::min();
            double maxY = std::numeric_limits<double>::min();

            for (size_t i = 0; i < points.size(); i++) {
                Point2 point = points[i];
                if (point[0] < minX) {
                    minX = point[0];
                }
                if (point[0] > maxX) {
                    maxX = point[0];
                }
                if (point[1] < minY) {
                    minY = point[1];
                }
                if (point[1] > maxY) {
                    maxY = point[1];
                }
            }
            Point3 randomPoint(fRand(minX, maxX), fRand(minY, maxY), 0);

            std::queue<Point3> processQueue;
            processQueue.push(randomPoint);

            while (!processQueue.empty()) {
                Point3 currentPoint = processQueue.front();
                processQueue.pop();
                seedpoints.push_back(currentPoint);

                for (size_t i = 0; i < newPointCount; i++) {
                    Point3 newPoint = generateNewPoint(currentPoint, deltaSeed);
                    //TODO check if point is in grid
                    if (!inNeighbourhood(newPoint, seedpoints, deltaSeed)) {
                        processQueue.push(newPoint);
                    }
                }
            }

            seedpoints.push_back(Point3(minX, minY, 0));
            seedpoints.push_back(Point3(maxX, maxY, 0));

            DefaultValueArray<Point3> valueArray(seedpoints, Precision::UINT64);
            auto result = std::make_shared<DefaultValueArray<Point3>>(valueArray);
            setResult("Seedpoints", result);
        }

        double fRand(double fMin, double fMax) {
            double f = (double)rand() / RAND_MAX;
            return fMin + f * (fMax - fMin);
        }

        Point3 generateNewPoint(Point3 current, double distance) {
            double radius = distance * (fRand(1,2));
            double angle = 2 * M_PI * fRand(0,1);

            double newX = current[0] + radius * cos(angle);
            double newY = current[1] + radius * sin(angle);
            return Point3(newX, newY, 0);
        }

        bool inNeighbourhood(Point3 current, std::vector<Point3> points, double min_distance) {
            for (size_t i = 0; i < points.size(); i++) {
                double distance = norm(points[i] - current);
                if (distance > 0 && distance < min_distance) {
                    return true;
                }
            }
            return false;
        }

        Point3 to3D(Point2 p) {
            return Point3(p[0], p[1], 0);
        }
    };

    AlgorithmRegister<PoissonDiscDistributionAlgorithm> reg("Hauptaufgabe/PoissonDiscDistribution", "Berechnet zufällige Punkte");
}
