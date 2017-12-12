#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/graphics.hpp>
#include "Util.h"
#include <math.h>


using namespace fantom;

namespace
{
// shows voronoi lines
    class VoronoiAlgorithm : public VisAlgorithm
    {
        std::unique_ptr< Primitive > mGlyphs;

    public:
        struct Options : public VisAlgorithm::Options
        {
            Options( Control& control ) : VisAlgorithm::Options( control )
            {
                 add<TensorFieldContinuous<2, Vector2>>("Field", "Feld mit Input" );
                 add< DefaultValueArray<std::pair<Point2, CriticalPointType>>>( "Critical Points", "List of critical points");

            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs( fantom::VisOutputs::Control& control ) : VisAlgorithm::VisOutputs( control )
            {
                addGraphics( "Voronoi" );
            }
        };

        VoronoiAlgorithm( InitData & data) : VisAlgorithm( data )
        {
        }


        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            mGlyphs = getGraphics("Voronoi").makePrimitive();

            auto myCriticalPoints = options.get< DefaultValueArray<std::pair<Point2, CriticalPointType>>>("Critical Points");
            if (!myCriticalPoints) return;

            std::vector<Point3> criticalPoints3D;

            for (size_t i = 0; i < myCriticalPoints->size(); i++){
                Point3 myPoint = to3D(myCriticalPoints->operator[](i).first);
                criticalPoints3D.push_back(myPoint);
            }

            mGlyphs->add(Primitive::POINTS).setColor(Color(0.0,1.0,0.0)).setPointSize(3).setVertices(criticalPoints3D);

            // figuring out 3 closest points to each other
            std::vector<std::vector<Point3>> closestPairList;
            closestPairList = threeNeighbourPointsList(criticalPoints3D);
        }


        //// FUNCTIONS

        //returns array of 3 neighbouring point arrays
        std::vector<std::vector<Point3>> threeNeighbourPointsList(std::vector<Point3> pList){
            std::vector<std::vector<Point3>> result;
            for (size_t i = 0;  i < pList.size(); i++){
                std::vector<Point3> threeNeighbourPoints;

                //firstPoint
                Point3 currentPointToFindNeighbour = pList[i];

                std::vector<std::tuple<Point3,double>> pointDistanceTupleList;
                for(size_t j = 0; j < pList.size(); j++){
                    std::tuple<Point3,double> pointDistanceTuple(currentPointToFindNeighbour, getDistance(currentPointToFindNeighbour, pList[j]));
                    pointDistanceTupleList.push_back(pointDistanceTuple);
//                    debugLog()<<std::get<0>(pointDistanceTuple)<<std::endl;
//                    debugLog()<<std::get<1>(pointDistanceTuple)<<std::endl;
//not yet checked for correctness
                }
                // sort 2 closest ones
                double shortestDistance = std::numeric_limits<double>::max();
                Point3 closestPoint = Point3(0,0,0);
                for(size_t j = 0; j < pointDistanceTupleList.size(); j++){
                    double distance = std::get<1>(pointDistanceTupleList[j]);

                    if((distance < shortestDistance) && (distance != 0)){
                        shortestDistance = distance;
                        closestPoint = std::get<0>(pointDistanceTupleList[j]);
                        debugLog()<< "point" << closestPoint<<std::endl;
                        debugLog()<< "initialPoint" << currentPointToFindNeighbour<<std::endl;
                        debugLog()<< "distance" << shortestDistance<<std::endl;
                    }
                }
                debugLog()<< "p1" << closestPoint<<std::endl;
                debugLog()<< "p2" << currentPointToFindNeighbour<<std::endl;
                //add 3 points
                threeNeighbourPoints.push_back(currentPointToFindNeighbour);


                result.push_back(threeNeighbourPoints);

            }
            return result;
        }

        //takes 2 Point3s and returns the distance as double
        double getDistance(Point3 p1, Point3 p2){
            double result = sqrt(pow((p2[0]-p1[0]), 2.0)+pow((p2[1]-p1[1]), 2.0));
            return result;
        }

        //take 2D Point and returns it in 3D
        Point3 to3D(Point2 p) {
            return Point3(p[0], p[1], 0);
        }

    };

    AlgorithmRegister< VoronoiAlgorithm > reg("Hauptaufgabe/Voronoi", "Vornoi linien");
}


