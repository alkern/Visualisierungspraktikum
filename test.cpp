#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include "Util.hpp"

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
                //addGraphics( "Streamlines" );
            }
        };

        VisThresholdAlgorithm( InitData & data) : VisAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            //mGlyphs = getGraphics("Streamlines").makePrimitive();

            Eigen::Matrix3d m = Eigen::Matrix3d::Ones(3,3);
            std::vector<double> eigenvalues;
            Eigen::EigenSolver<Eigen::Matrix3d> es(m);
            for (int i = 0; i < 3; i++) {
                eigenvalues.push_back(es.eigenvalues()(i).real());
            }
            std::sort(eigenvalues.begin(), eigenvalues.end());
            std::reverse(eigenvalues.begin(), eigenvalues.end());
            for (int i = 0; i < 3; i++) {
                debugLog() << eigenvalues[i] << std::endl;
            }
        }

    };

    AlgorithmRegister< VisThresholdAlgorithm > reg("VisPraktikum/Test", "Markiert Schwellwerte");
}
