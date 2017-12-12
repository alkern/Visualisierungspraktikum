#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/graphics.hpp>
#include "Util.h"
#include <math.h>

#define PI 3.14159265

using namespace fantom;

namespace
{
/**
     * @brief The VoronoiAlgorithm class
     *
     * Berechne Voronoizellen für eine Menge von kritischen Punkten.
     * https://de.wikipedia.org/wiki/Voronoi-Diagramm#Pseudocode
     */
    class VoronoiAlgorithm : public VisAlgorithm
    {
        std::unique_ptr< Primitive > mGlyphs;
        Point3 minimalPoint;

    public:
        struct Options : public VisAlgorithm::Options
        {
            Options( Control& control ) : VisAlgorithm::Options( control )
            {
                add<DefaultValueArray<std::pair<Point2, CriticalPointType>>>( "Critical Points", "List of critical points");
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs( fantom::VisOutputs::Control& control ) : VisAlgorithm::VisOutputs( control )
            {
                addGraphics( "Voronoi-Zellen" );
            }
        };

        VoronoiAlgorithm( InitData & data) : VisAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            mGlyphs = getGraphics("Voronoi-Zellen").makePrimitive();

            auto criticalPoints = options.get<DefaultValueArray<std::pair<Point2, CriticalPointType>>>("Critical Points");
            if (!criticalPoints) return;

            //Hyperpoints erstellen
//            std::vector<Point3> hyperPoints;
//            std::vector<Point3> vertices;
//            for (size_t i = 0; i < criticalPoints->size(); i++) {
//                auto point = criticalPoints->operator[](i).first;
//                Point3 hyperPoint(point[0], point[1], point[0]*point[0] + point[1]*point[1]);
//                hyperPoints.push_back(hyperPoint);

//                vertices.push_back(Point3(point[0], point[1], 0));
//            }

            //----------------------------------------------------------------------------------------------------------
            std::vector<Point3> test;
            test.push_back(Point3(0,2,0));
            test.push_back(Point3(1,1,0));
            test.push_back(Point3(2,1,0));
            test.push_back(Point3(3,2,0));
            test.push_back(Point3(1.5,1.5,0));

            auto convexHull = getConvexHull(test);
            debugLog() << "Testing" << std::endl;
            debugLog() << convexHull.size() << std::endl;
            //----------------------------------------------------------------------------------------------------------

//            auto convexHull = getConvexHull(hyperPoints);

            //Kanten der konvexen Hülle zeichnen
            std::vector<Point3> edges;
            for (size_t i = 0; i < convexHull.size() - 1; i++) {
                edges.push_back(convexHull[i]);
                edges.push_back(convexHull[i + 1]);
            }
            edges.push_back(convexHull[edges.size()]);
            edges.push_back(convexHull[0]);

            mGlyphs->add(Primitive::LINES).setColor(Color(1,0,0)).setVertices(edges);
            mGlyphs->add(Primitive::POINTS).setPointSize(4).setColor(Color(0,1,0)).setVertices(test);
        }

        double angleBetween(Point3 p0, Point3 p) {
            double scalarProduct = p0 * p;
            double lengthProduct = norm(p0) * norm(p);
            double value = acos(scalarProduct/lengthProduct) * 180/PI;
            return round( value * 1000.0 ) / 1000.0;
        }

        bool angleCompare(Point3 p1, Point3 p2) {
            return angleBetween(minimalPoint, p1) < angleBetween(minimalPoint, p2);
        }

        /**
         * @brief getConvexHull
         * Berechnet die konvexe Hülle einer Punktmenge mithilfe des Gift-Wrapping-Algorithmus
         * https://de.wikipedia.org/wiki/Gift-Wrapping-Algorithmus
         * @param points
         * @return Randpunkte der konvexen Hülle
         */
        std::vector<Point3> getConvexHull(std::vector<Point3> points) {
            //finde Punkt mit minimaler Ordinate (y-Wert)
            Point3 minOrdinate(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
            for (Point3 p : points) {
                if (p[1] < minOrdinate[1]) {
                    minOrdinate = p;
                }
                if (p[1] == minOrdinate[1] && p[0] < minOrdinate[0]) { //bei gleicher Ordinate kleinste Abszisse
                    minOrdinate = p;
                }
            }
            minimalPoint = minOrdinate;

            //Punkte nach Winkel sortieren
            std::vector<std::pair<Point3, double>> pointsWithAngles;
            for (Point3 p : points) {
                pointsWithAngles.push_back(std::pair<Point3, double>(p, angleBetween(minOrdinate, p)));
            }
            std::sort(pointsWithAngles.begin(), pointsWithAngles.end(), [](std::pair<Point3, double> p1, std::pair<Point3, double> p2) -> bool {
                return p1.second < p2.second;
            });
            std::vector<Point3> sorted;
            for (std::pair<Point3, double> p : pointsWithAngles) {
                sorted.push_back(p.first);
            }

            size_t i = 1;
            while (i < sorted.size()) {
                if (!isLeft(sorted[i], sorted[i-1], sorted[i+1])) {
                    i++;
                } else {
                    sorted.erase(sorted.begin() + i);
                    i--;
                }
            }

            return sorted;

//            std::vector<Point3> output; //P in pseudocode
//            auto startPoint = minOrdinate;
//            output.push_back(startPoint);
//            Point3 endPoint(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
//            size_t i = 0;
//            while (endPoint != output[0] && i < points.size()) {
//                output[i] = startPoint;
//                endPoint = points[0];

//                for (size_t j = 1; j < points.size(); j++) {
//                    if (endPoint == startPoint || isLeft(points[j], startPoint, endPoint)) {
//                        endPoint = points[j];
//                    }
//                }
//                startPoint = endPoint;
//                i++;
//            }
//            return output;
        }

//        /**
//         * @brief isLeft
//         * Prüfe, ob p links von der Gerade zwischen start und end
//         * @param p
//         * @param start
//         * @param end
//         * @return
//         */
        bool isLeft(Point3 si, Point3 start, Point3 end) {
            double scalarProduct = (end - start) * (si - start);
            double lengthProduct = norm(end - start) * norm(si - start);
            double acosValue = acos(scalarProduct/lengthProduct) * 180/PI;
            return acosValue > 180;
        }

    };

    AlgorithmRegister<VoronoiAlgorithm> reg("Hauptaufgabe/Voronoi", "Berechne Voronoizellen");
}


