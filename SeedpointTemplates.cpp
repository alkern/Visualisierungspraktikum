#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/datastructures/DataObjectBundle.hpp>
#include "Util.h"

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
            }
        };

        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
            DataOutputs( DataOutputs::Control& control) : DataAlgorithm::DataOutputs( control )
            {
               add<DefaultValueArray<Point2>>( "Seedpoints" );
            }
        };

        SeedpointTemplateAlgorithm( InitData & data) : DataAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            double deltaSeeds = options.get<double>("Delta Seed");

            std::vector<Point3> seedpoints;

            //Pattern berechnen
            //mit zufälligen Punkten auffüllen

            auto myCriticalPoints = options.get< DefaultValueArray<std::pair<Point2, CriticalPointType>>>("Critical Points");
            if (!myCriticalPoints) return;

            std::vector<Point2> onlyCriticalPoints;

            for(size_t i = 0; i < myCriticalPoints -> size(); i++){
                onlyCriticalPoints.push_back(myCriticalPoints->operator[](i).first);
            }

            for(size_t i = 0; i < onlyCriticalPoints.size(); i++){
                Point2 test = getMiddlePoint(onlyCriticalPoints, onlyCriticalPoints[i]);
                //HERE CONTINUE
                debugLog() <<  test << std::endl;
            }

            DefaultValueArray<Point3> valueArray(seedpoints, Precision::UINT64);
            auto result = std::make_shared<DefaultValueArray<Point3>>(valueArray);
            setResult("Seedpoints", result);
        }

        //FUNCTIONS
        Point2 getMiddlePoint(std::vector<Point2> criticalPoints, Point2 point){
            //calculate distances
            std::vector<std::tuple<Point2,double>> pointDistanceTupleList;
            for(size_t i = 0; i < criticalPoints.size(); i++){
                std::tuple<Point2,double> pointDistanceTuple(criticalPoints[i], norm(point - criticalPoints[i]));
                pointDistanceTupleList.push_back(pointDistanceTuple);
            }

            // find closest one
            double shortestDistance = std::numeric_limits<double>::max();
            Point2 closestPoint = Point2(0,0);
            for(size_t i = 0; i < pointDistanceTupleList.size(); i++){
                double distance = std::get<1>(pointDistanceTupleList[i]);
                if((distance != 0) && (distance < shortestDistance)){
                    shortestDistance = distance;
                    closestPoint = std::get<0>(pointDistanceTupleList[i]);
                }
            }
            Point2 result = closestPoint;
            return result;
        }

};

    AlgorithmRegister<SeedpointTemplateAlgorithm> reg("Hauptaufgabe/SeedpointTemplates", "Berechnet SeedpointTemplates aus kritischen Punkten und Voronoi-Zellen");

}
