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

            std::vector<Point2> seedpoints;

            //Pattern berechnen
            //mit zufälligen Punkten auffüllen

            DefaultValueArray<Point2> valueArray(seedpoints, Precision::UINT64);
            auto result = std::make_shared<DefaultValueArray<Point2>>(valueArray);
            setResult("Seedpoints", result);
        }
    };

    AlgorithmRegister<SeedpointTemplateAlgorithm> reg("Hauptaufgabe/SeedpointTemplates", "Berechnet SeedpointTemplates aus kritischen Punkten und Voronoi-Zellen");
}
