#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/graphics.hpp>
#include "Util.hpp"
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
            std::vector<Point3> hyperPoints;
            std::vector<Point3> vertices;
            for (size_t i = 0; i < criticalPoints->size(); i++) {
                auto point = criticalPoints->operator[](i).first;
                Point3 hyperPoint(point[0], point[1], point[0]*point[0] + point[1]*point[1]);
                hyperPoints.push_back(hyperPoint);

                vertices.push_back(Point3(point[0], point[1], 0));
            }

            auto convexHull = getConvexHull(hyperPoints);

            for (size_t i = 0; i < convexHull.size(); i++) {
                convexHull[i] = Point3(convexHull[i][0], convexHull[i][1], 0);
            }

            //Kanten der konvexen Hülle zeichnen
            std::vector<Point3> edges;
            for (size_t i = 0; i < convexHull.size() - 1; i++) {
                edges.push_back(convexHull[i]);
                edges.push_back(convexHull[i + 1]);
            }
            edges.push_back(convexHull[convexHull.size() - 1]);
            edges.push_back(convexHull[0]);

            mGlyphs->add(Primitive::LINES).setColor(Color(1,0,0)).setVertices(edges);
            mGlyphs->add(Primitive::POINTS).setPointSize(4).setColor(Color(0,1,0)).setVertices(vertices);

            mGlyphs->add(Primitive::POINTS).setPointSize(8).setColor(Color(0,0,1)).setVertices(std::vector<Point3>({minimalPoint, Point3(0,0,0)}));
        }

        double angleBetween(Point3 point) {
            //Winkel zwischen den Geraden MINIMAL-point und der x-Achse
            Vector3 xAxis(-1,0,0);
            Vector3 p = minimalPoint - point;
            double scalarProduct =  p * xAxis;
            double lengthProduct = norm(p) * norm(xAxis);
            double value = acos(scalarProduct/lengthProduct) * 180/PI;
            return round( value * 1000.0 ) / 1000.0;
        }

        /**
         * @brief getConvexHull
         * Berechnet die konvexe Hülle einer Punktmenge mithilfe des Graham-Scan-Algorithmus
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
                if (p == minimalPoint) continue;
                pointsWithAngles.push_back(std::pair<Point3, double>(p, angleBetween(p)));
            }
            std::sort(pointsWithAngles.begin(), pointsWithAngles.end(), [](std::pair<Point3, double> p1, std::pair<Point3, double> p2) -> bool {
                return p1.second < p2.second;
            });
            std::vector<Point3> sorted;
            sorted.push_back(minimalPoint);
            double previousAngle = std::numeric_limits<double>::min();
            for (std::pair<Point3, double> p : pointsWithAngles) {
                if (previousAngle != p.second) { //keine Winkeldopplungen
                    sorted.push_back(p.first);
                    previousAngle = p.second;
                }
            }

            //Graham-Scan
            size_t i = 1;
            while (i < sorted.size() - 1) {
                Point3 current, previous, next;
                current = sorted[i];
                if (i == 0) {
                    previous = sorted[sorted.size()];
                } else {
                    previous = sorted[i - 1];
                }
                if (i == sorted.size()) {
                    next = sorted[0];
                } else {
                    next = sorted[i + 1];
                }

                //if (isRight(current, previous, next)) {
                if (isRight(previous, next, current)) {
                    i++;
                } else {
                    debugLog() << "Current:" << current << " Previous:" << previous << " Next:" << next;
                    debugLog() << " Erased " << sorted[i] << std::endl;
                    sorted.erase(sorted.begin() + i);
                    i--;
                }
            }

            return sorted;
        }

        bool isRight(Point3 a, Point3 b, Point3 c) {
            //gibt an, ob Punkt c rechts der Gerade ab liegt
            return 0 > ((b[0] - a[0])*(c[1] - a[1])) - ((c[0] - a[0])*(b[1] - a[1]));
        }

    };

    AlgorithmRegister<VoronoiAlgorithm> reg("Hauptaufgabe/Voronoi-2", "Berechne Voronoizellen");
}


