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

class Euler
{
    public:
        Euler(double initialStepWidth) {
            this->stepWidth = initialStepWidth;
            this->reset();
        }
        ~Euler() {}

        Point2 nextStep(Point2 startPoint, TensorFieldContinuous<2, Point2>::Evaluator& evaluator) {
            if (evaluator.reset(startPoint)) {
                auto changeRate = evaluator.value(); //Wert an der Stelle xn
                auto nextPoint = startPoint + this->stepWidth * changeRate; //Addition und Multiplikation auf Tensoren ist definiert
                if (startPoint == nextPoint) this->setHasNext(false);
                return nextPoint;
            }
            this->setHasNext(false);
            return startPoint;
        }

        void reset() {setHasNext(true);}
        bool hasNext() {return dhn;}
    protected:
        void setHasNext(bool b) {dhn = b;}
        double stepWidth;
    private:
        bool dhn;
    };

    class VisThresholdAlgorithm : public VisAlgorithm
    {
        std::unique_ptr< Primitive > mGlyphs;

    public:
        struct Options : public VisAlgorithm::Options
        {
            Options( Control& control ) : VisAlgorithm::Options( control )
            {
                add< TensorFieldContinuous< 2, Vector2 > >("Field", "Feld mit Input" );
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

            Eigen::MatrixXd m(2,2);
            m(0,0) = 3;
            m(1,0) = 2.5;
            m(0,1) = -1;
            m(1,1) = m(1,0) + m(0,1);

            debugLog() << m << std::endl;


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
