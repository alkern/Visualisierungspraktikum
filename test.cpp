#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/dataset/TensorFieldBase.hpp>
#include <fantom/datastructures/DataObjectBundle.hpp>
#include <fantom/graphics.hpp>
#include <math.h>
#include <Eigen/Dense>

using namespace fantom;

namespace
{
    class VisThresholdAlgorithm : public VisAlgorithm
    {
        std::unique_ptr< Primitive > mGlyphs;

    public:
        struct Options : public VisAlgorithm::Options
        {
            Options( Control& control ) : VisAlgorithm::Options( control )
            {
                add< TensorFieldContinuous< 2, Vector2 > >("Field", "Feld mit Input" );
                add< Grid < 2 > >("Grid", "Grid" );
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs( fantom::VisOutputs::Control& control ) : VisAlgorithm::VisOutputs( control )
            {
                addGraphics( "Streamlines" );
            }
        };

        VisThresholdAlgorithm( InitData & data) : VisAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            mGlyphs = getGraphics("Streamlines").makePrimitive();

            auto grid = options.get<Grid<2>>("Grid");
            if (!grid) return;

            Point2 p(10000,100000);

            Cell c = grid->locate(p);

            if (!c) {
                debugLog() << "Not a cell yay" << std::endl;
                return;
            }

            if (grid->contains(c, p)) {
                debugLog() << "In booh" << std::endl;
            } else {
                debugLog() << "Out yeah" << std::endl;
            }
        }

        double getDistance(Point3 p1, Point3 p2){
            double result = sqrt(pow((p2[0]-p1[0]), 2.0)+pow((p2[1]-p1[1]), 2.0));
            return result;
        }

        Point2 to2D(Point3 p) {
            return Point2(p[0], p[1]);
        }

        Point3 to3D(Point2 p) {
            return Point3(p[0], p[1], 0);
        }

    };

    AlgorithmRegister< VisThresholdAlgorithm > reg("VisPraktikum/Test", "Markiert Schwellwerte");
}
