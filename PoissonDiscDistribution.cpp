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
     * Create random points in 2D
     */
    class PoissonDiscDistributionAlgorithm : public DataAlgorithm
    {
    public:
        struct Options : public DataAlgorithm::Options
        {
            Options( Control& control ) : DataAlgorithm::Options( control )
            {
                add< Grid < 2 > >("Grid", "Grid" );
                add<double>("Delta Seed", "Distanz der Zufallspunkte", 0.2);
                add<int>("Grow Rate", "Groesse der Iterationsschritte", 30);
                add<int>("Number Start Points", "Anzahl Startpunkte", 1);
            }
        };

        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
            DataOutputs( DataOutputs::Control& control) : DataAlgorithm::DataOutputs( control )
            {
               add<DefaultValueArray<Point2>>( "Seedpoints" );
            }
        };

        PoissonDiscDistributionAlgorithm( InitData & data) : DataAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            auto grid = options.get<Grid<2>>("Grid");
            if (!grid) return;

            std::vector<Point2> seedpoints;
            size_t newPointCount = options.get<int>("Grow Rate");
            double deltaSeed = options.get<double>("Delta Seed");

            std::queue<Point2> processQueue;
            for (int i = 0; i < options.get<int>("Number Start Points"); i++) {
                processQueue.push(generateRandomStartPoint(grid->points()));
            }

            while (!processQueue.empty()) {
                Point2 currentPoint = processQueue.front();
                processQueue.pop();

                for (size_t i = 0; i < newPointCount; i++) {
                    Point2 newPoint = generateNewPoint(currentPoint, deltaSeed);
                    if (!grid->locate(newPoint)) continue;
                    if (!inNeighbourhood(newPoint, seedpoints, deltaSeed)) {
                        processQueue.push(newPoint);
                        seedpoints.push_back(newPoint);
                    }
                }
            }

            DefaultValueArray<Point2> valueArray(seedpoints, Precision::UINT64);
            auto result = std::make_shared<DefaultValueArray<Point2>>(valueArray);
            setResult("Seedpoints", result);
        }

        Point2 generateRandomStartPoint(const ValueArray<Point2>& points) {
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
            return Point2(fRand(minX, maxX), fRand(minY, maxY));
        }

        double fRand(double fMin, double fMax) {
            double f = (double)rand() / RAND_MAX;
            return fMin + f * (fMax - fMin);
        }

        Point2 generateNewPoint(Point2 current, double distance) {
            double radius = distance * (fRand(1,2));
            double angle = 2 * M_PI * fRand(0,1);

            double newX = current[0] + radius * cos(angle);
            double newY = current[1] + radius * sin(angle);
            return Point2(newX, newY);
        }

        bool inNeighbourhood(Point2 current, std::vector<Point2> points, double min_distance) {
            for (size_t i = 0; i < points.size(); i++) {
                double distance = norm(points[i] - current);
                if (distance > 0 && distance < min_distance) {
                    return true;
                }
            }
            return false;
        }
    };

    AlgorithmRegister<PoissonDiscDistributionAlgorithm> reg("Hauptaufgabe/PoissonDiscDistribution", "Berechnet zufällige Punkte");
}
