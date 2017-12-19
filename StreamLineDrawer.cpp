#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/fields.hpp>
#include <fantom/graphics.hpp>
#include "Util.h"

using namespace fantom;

namespace
{
    class Integrator
    {
    public:
        virtual ~Integrator() {}
        virtual Point2 nextStep(Point2 xn, TensorFieldContinuous<2, Point2>::Evaluator& evaluator) = 0;
        void reset() {setHasNext(true); numSteps = 0;}
        bool hasNext() {return dhn && this->numSteps < this->maxSteps;}
        void setHasNext(bool b) {dhn = b;}
    protected:
        Integrator(double stepWidth, int maxSteps) {
            this->stepWidth = stepWidth;
            this->maxSteps = maxSteps;
            this->reset();
        }
        double stepWidth;
        double numSteps = 0;
        double maxSteps = 0;
    private:
        bool dhn;
    };

    class Euler : public Integrator
    {
        double epsilon;

    public:
        Euler(double initialStepWidth, int maxSteps, double eps) : Integrator(initialStepWidth, maxSteps) {
            epsilon = eps;
        }
        ~Euler() {}

        Point2 nextStep(Point2 startPoint, TensorFieldContinuous<2, Point2>::Evaluator& evaluator) override {
            this->numSteps++;
            if (evaluator.reset(startPoint)) {
                auto changeRate = evaluator.value(); //Wert an der Stelle xn
                auto nextPoint = startPoint + this->stepWidth * changeRate;

                //Adaptive Schrittweitenanpassung
                auto supportStep = startPoint + this->stepWidth * 0.5 * changeRate; //Wert in halber Entfernung zum Vergleich
                if (norm(supportStep - nextPoint) * 10 < epsilon) { //Schrittweite bei sehr kleiner Differenz verdoppeln
                    this->stepWidth *= 2;
                    return nextStep(startPoint, evaluator);
                }
                if (norm(supportStep - nextPoint) > epsilon) { //Schrittweite bei großer Differenz halbieren
                    this->stepWidth /= 2;
                    return nextStep(startPoint, evaluator);
                }

                if (startPoint == nextPoint) this->setHasNext(false);
                return nextPoint;
            }
            this->setHasNext(false);
            return startPoint;
        }
    };

    class RungeKutta : public Integrator {
    public:
        RungeKutta(double initialStepWidth, int maxSteps): Integrator(initialStepWidth, maxSteps) {}
        ~RungeKutta() {}

        Point2 nextStep(Point2 startPoint, TensorFieldContinuous<2, Point2>::Evaluator& evaluator) override {
            this->numSteps++;
            Point2 q1 = getTensor(startPoint, evaluator);
            Point2 q2 = getTensor(startPoint + this->stepWidth * 0.5 * q1, evaluator);
            Point2 q3 = getTensor(startPoint + this->stepWidth * 0.5 * q2, evaluator);
            Point2 q4 = getTensor(startPoint + q3, evaluator);
            Point2 nextPoint = startPoint + this->stepWidth / 6 * (q1 + q2 + q3 + q4);
            if (startPoint == nextPoint) this->setHasNext(false);
            if (!this->hasNext()) return startPoint;
            return nextPoint;
        }

    private:
        Point2 getTensor(Point2 point, TensorFieldContinuous<2, Point2>::Evaluator& evaluator) {
            if (evaluator.reset(point)) {
                return evaluator.value();
            } else {
                this->setHasNext(false);
                return point;
            }
        }
    };

    /**
     * @brief The VisStreamLinesAlgorithm class
     *
     * Zeichnet Stromlinien in einem Vektorfeld. Startpunkte können gleichverteilt auf einer Gerade oder Fläche bestimmt werden.
     * Die Berechnung der Stromlinien erfolg wahlweise über das explizite Euler- oder das Runge-Kutta-Verfahren vierter Ordnung.
     */
    class VisStreamLinesAlgorithm : public VisAlgorithm
    {
        std::unique_ptr< Primitive > mGlyphs;

    public:
        struct Options : public VisAlgorithm::Options
        {
            Options( Control& control ) : VisAlgorithm::Options( control )
            {
                add< TensorFieldContinuous<2, Vector2>>("Field", "Feld mit Input" ); ///home/visprak11/fantom/TestData/streamTest2.vtk
                add< InputChoices >("Method", "Integrationsverfahren", std::vector<std::string>({"Euler", "Runge-Kutta"}), "Euler");
                add< double >("Stepwidth", "Schrittweite fuer das Euler-Verfahren", 0.1);
                add< double >("Adaptiver Grenzwert", "Epsilon fuer die adaptive Schrittweitenanpassung bei Euler", 0.1);
                add< int >("Number of Steps", "Maximale Anzahl an Integrationsschritten", 100);
                add< Color >("Color", "Farbe der Stromlinien", Color(0.75, 0.75, 0.0));
                add< DefaultValueArray<Point3> >("Seedpoints", "Saatpunkte");
                add < double >("Delta Streamline", "Abbruchsdistanz fuer Streamlines", 0.01);
            }

            void optionChanged( const std::string& name )
            {
                if( name == "Method" )
                {
                    std::string value = get< std::string >( "Method" );
                    if (value.compare("Euler") == 0) {
                        setVisible( "Adaptiver Grenzwert", true );
                        return;
                    }
                    setVisible( "Adaptiver Grenzwert", false );
                }
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs( fantom::VisOutputs::Control& control ) : VisAlgorithm::VisOutputs( control )
            {
                addGraphics( "Streamlines" );
            }
        };

        VisStreamLinesAlgorithm( InitData & data) : VisAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            mGlyphs = getGraphics("Streamlines").makePrimitive();

            auto field = options.get<TensorFieldContinuous<2, Vector2>>("Field");
            if (!field) return;
            auto grid = std::dynamic_pointer_cast<const Grid<2>>(field->domain());
            if (!grid) return;

            //Speichert die Stützstellen der Streamlines für einzelne Zellen um Abbruchskriterium einzusetzen
            std::unordered_map<size_t, std::vector<Point3>> streamlinePointsInCell;
            for (size_t i = 0; i < grid->numCells(); i++) {
                streamlinePointsInCell.insert({i, std::vector<Point3>()});
            }
            double deltaStreamline = options.get<double>("Delta Streamline");

            auto startPointsValueArray = options.get< DefaultValueArray<Point3> >("Seedpoints");
            std::vector<Point3> startPoints;
            if (startPointsValueArray) {
                for (size_t i = 0; i < startPointsValueArray->size(); i++) {
                    startPoints.push_back(startPointsValueArray->operator[](i));
                }
            } else {
                startPoints.push_back(Point3(1.983000, 0, 0));
            }

            Integrator* integrator;
            std::string method = options.get< std::string >("Method");
            if (method.compare("Euler") == 0) integrator = new Euler(options.get< double >("Stepwidth"), options.get<int>("Number of Steps"), options.get<double>("Adaptiver Grenzwert"));
            if (method.compare("Runge-Kutta") == 0) integrator = new RungeKutta(options.get< double >("Stepwidth"), options.get<int>("Number of Steps"));

            mGlyphs->add(Primitive::POINTS).setColor(Color(1, 0, 0)).setPointSize(4).setVertices(startPoints);

            //Streamlines von Startpunkten aus zeichnen
            for (size_t i = 0; i < startPoints.size(); i++) {
                auto evaluator = field->makeEvaluator();
                auto startPoint = startPoints[i];
                std::vector<Point3> vertices;

                while (integrator->hasNext()) {
                    Vector3 nextPoint = to3D(integrator->nextStep(to2D(startPoint), *evaluator));
                    vertices.push_back(startPoint);
                    vertices.push_back(nextPoint);

                    //bisherige Stützstellen vergleichen, abbrechen wenn zu nah
                    size_t cellIndex = grid->index(grid->locate(to2D(nextPoint)));
                    for (auto& point : streamlinePointsInCell[cellIndex]) {
                        if (norm(point - nextPoint) < deltaStreamline && point != startPoint) {
                            integrator->setHasNext(false); //falls Punkt zu nah, abbrechen
                        }
                    }
                    streamlinePointsInCell[cellIndex].push_back(nextPoint);

                    startPoint = nextPoint;
                }
                integrator->reset();

                mGlyphs->add(Primitive::LINES).setColor(options.get< Color >("Color")).setVertices(vertices);
            }
        }

        Point2 to2D(Point3 p) {
            return Point2(p[0], p[1]);
        }

        Point3 to3D(Point2 p) {
            return Point3(p[0], p[1], 0);
        }

    };

    AlgorithmRegister< VisStreamLinesAlgorithm > reg("Hauptaufgabe/StreamLineDrawer", "Zeichnet Stromlinien");
}
