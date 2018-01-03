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
                 add< Color >("Color", "Farbe VoronoLinien", Color(0.7, 0.9, 0.9));
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
            std::vector<std::vector<Point3>> closestTripletList;
            closestTripletList = threeNeighbourPointsList(criticalPoints3D);

            //find the cutting points between triangles of closest pairs
            std::vector<std::vector<Point3>> middlePoints;
            for(size_t i = 0; i < closestTripletList.size(); i++){
                Point3 p1 = closestTripletList[i][0];
                Point3 p2 = closestTripletList[i][1];
                Point3 p3 = closestTripletList[i][2];
                std::vector<Point3> thisMiddlePoints;
                thisMiddlePoints.push_back(getMiddle(p1,p2));
                thisMiddlePoints.push_back(getMiddle(p1,p3));
                thisMiddlePoints.push_back(getMiddle(p2,p3));
                middlePoints.push_back(thisMiddlePoints);

                //uncomment to check middle Points
                debugLog()<< "m1" << getMiddle(p1,p2) <<std::endl;
                debugLog()<< "m2" << getMiddle(p1,p3) << std::endl;
                debugLog()<< "m3" << getMiddle(p2,p3) << std::endl;
            }

            // draw the Voronoi lines
            std::vector<Point3> circleMiddlePoints;
            for(size_t i = 0; i < closestTripletList.size(); i++){
                Point3 p1 = closestTripletList[i][0];
                Point3 p2 = closestTripletList[i][1];
                Point3 p3 = closestTripletList[i][2];
                Point3 m1 = middlePoints[i][0]; //p1,p2
                Point3 m2 = middlePoints[i][1]; //p1,p3

                Vector3 criticalPointToMiddlePointM1 = m1-p1;
                Vector3 criticalPointToMiddlePointM2 = m2-p1;
                Vector3 orthogonalVectorM1(-criticalPointToMiddlePointM1[1], criticalPointToMiddlePointM1[0], 0);
                Vector3 orthogonalVectorM2(-criticalPointToMiddlePointM2[1], criticalPointToMiddlePointM2[0], 0);

                //calculating intersect of m1 + a*Ortho1 = m2 + b*Ortho2
                double a1 = m1[0];
                double b1 = m2[0];
                double a2 = m1[1];
                double b2 = m2[1];
                double r1 = orthogonalVectorM1[0];
                double r2 = orthogonalVectorM1[1];
                double s1 = orthogonalVectorM2[0];
                double s2 = orthogonalVectorM2[1];

                double v = (r2*(a1-b1) - r1*(a2-b2)) / (r2*s1 - r1*s2);
                Point3 circleMiddle(b1 + v * s1, b2 + v * s2, 0);

//                circleMiddlePoints.push_back(circleMiddle);
//                circleMiddlePoints.push_back(m1);
//                circleMiddlePoints.push_back(m2);
                circleMiddlePoints.push_back(p1);
                circleMiddlePoints.push_back(p2);
                circleMiddlePoints.push_back(p3);
           }

           mGlyphs->add(Primitive::TRIANGLES).setColor(options.get< Color >("Color")).setVertices(circleMiddlePoints);




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
                    std::tuple<Point3,double> pointDistanceTuple(pList[j], norm(currentPointToFindNeighbour - pList[j]));
                    pointDistanceTupleList.push_back(pointDistanceTuple);
                }

                // find closest one
                double shortestDistance = std::numeric_limits<double>::max();
                Point3 closestPoint = Point3(0,0,0);
                for(size_t j = 0; j < pointDistanceTupleList.size(); j++){
                    double distance = std::get<1>(pointDistanceTupleList[j]);
                    if((distance != 0) && (distance < shortestDistance)){
                        shortestDistance = distance;
                        closestPoint = std::get<0>(pointDistanceTupleList[j]);
                    }
                }

                // find second closest one ; refactor later
                double secondShortestDistance = std::numeric_limits<double>::max();
                Point3 secondClosestPoint = Point3(0,0,0);
                for(size_t j = 0; j < pointDistanceTupleList.size(); j++){
                    double secondDistance = std::get<1>(pointDistanceTupleList[j]);
                    if((secondDistance != 0) && (secondDistance != shortestDistance) && (secondDistance < secondShortestDistance)){
                        secondShortestDistance = secondDistance;
                        secondClosestPoint = std::get<0>(pointDistanceTupleList[j]);
                    }
                }

                // uncomment to check points
                debugLog()<< "p1" << currentPointToFindNeighbour<<std::endl;
                debugLog()<< "p2" << closestPoint<< "; distance: " << shortestDistance << std::endl;
                debugLog()<< "p3" << secondClosestPoint<< "; distance: " << secondShortestDistance << std::endl;

                //add 3 points to list and add that array to result
                threeNeighbourPoints.push_back(currentPointToFindNeighbour);
                threeNeighbourPoints.push_back(closestPoint);
                threeNeighbourPoints.push_back(secondClosestPoint);

                result.push_back(threeNeighbourPoints);
            }
            return result;
        }

        //calculates the middle between 2 points
        Point3 getMiddle(Point3 p1, Point3 p2){
            Point3 result = p1 + ((p2-p1)/2) ;
            return result;
        }

        //take 2D Point and returns it in 3D
        Point3 to3D(Point2 p) {
            return Point3(p[0], p[1], 0);
        }
    };

    AlgorithmRegister< VoronoiAlgorithm > reg("Hauptaufgabe/Voronoi", "Vornoi linien");
}


