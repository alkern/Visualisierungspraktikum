#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include "Util.hpp"

using namespace fantom;

namespace
{
/**
     * @brief The CriticalPointsAlgorithm class
     *
     * Findet Zellen, die kritische Punkte beinhalten und markiert sie
     */
    class CriticalPointsAlgorithm : public DataAlgorithm
    {
        std::vector<std::pair<Point2, CriticalPointType>> criticalPoints;

    public:
        struct Options : public DataAlgorithm::Options
        {
            Options( Control& control ) : DataAlgorithm::Options( control )
            {
                add<TensorFieldContinuous<2, Vector2>>("Field", "Feld mit Input" );
            }
        };

        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
            DataOutputs( DataOutputs::Control& control ) : DataAlgorithm::DataOutputs( control )
            {
                add< DefaultValueArray<std::pair<Point2, CriticalPointType>>>( "Critical Points" );
            }
        };

        CriticalPointsAlgorithm( InitData & data) : DataAlgorithm( data )
        {
        }

        void execute( const Algorithm::Options& options, const volatile bool& /* abortFlag */ ) override
        {
            std::shared_ptr<const TensorFieldContinuous<2, Vector2>> field = options.get<TensorFieldContinuous<2, Vector2>>("Field");
            if (!field) return;

            std::shared_ptr<const Grid<2>> grid = std::dynamic_pointer_cast<const Grid<2>>(field->domain());
            if (!grid) return;

            const ValueArray<Point2>& gridPoints = grid->points();
            auto evaluator = field->makeEvaluator();

            for (size_t i = 0; i < grid->numCells(); i++) {
                auto cell = grid->cell(i);
                findCriticalPoint(cell, gridPoints, *evaluator);
            }

            DefaultValueArray<std::pair<Point2, CriticalPointType>> valueArray(criticalPoints, Precision::UINT64);
            auto result = std::make_shared<DefaultValueArray<std::pair<Point2, CriticalPointType>>>(valueArray);
            setResult("Critical Points", result);
        }

        void findCriticalPoint(Cell cell, const ValueArray<Point2>& points, TensorFieldContinuous<2, Vector2>::Evaluator& evaluator) {
            evaluator.reset(points[cell.index(0)]);
            Vector2 v0 = evaluator.value();
            evaluator.reset(points[cell.index(1)]);
            Vector2 v1 = evaluator.value();
            evaluator.reset(points[cell.index(2)]);
            Vector2 v2 = evaluator.value();
            evaluator.reset(points[cell.index(3)]);
            Vector2 v3 = evaluator.value();

            //Kategorisierung eigentlich mit categorizeCriticalPoint(getEigenvalues(point.first, evaluator))

            //quellen
            if (v0[0] < 0 && v0[1] > 0 && v1[0] < 0 && v1[1] < 0 && v2[0] > 0 && v2[1] < 0 && v3[0] > 0 && v3[1] > 0) {
                auto point = std::make_pair(getCenterPoint(cell, points), CriticalPointType::SOURCE);
                criticalPoints.push_back(point);
            }

            //senken
            if (v0[0] > 0 && v0[1] < 0 && v1[0] > 0 && v1[1] > 0 && v2[0] < 0 && v2[1] > 0 && v3[0] < 0 && v3[1] < 0) {
                auto point = std::make_pair(getCenterPoint(cell, points), CriticalPointType::SINK);
                criticalPoints.push_back(point);
            }

            //sattel
            if (v0[0] > 0 && v0[1] < 0 && v1[0] < 0 && v1[1] < 0 && v2[0] < 0 && v2[1] > 0 && v3[0] > 0 && v3[1] > 0) {
                auto point = std::make_pair(getCenterPoint(cell, points), CriticalPointType::SADDLE);
                criticalPoints.push_back(point);
            }
            if (v0[0] < 0 && v0[1] > 0 && v1[0] > 0 && v1[1] > 0 && v2[0] > 0 && v2[1] < 0 && v3[0] < 0 && v3[1] < 0) {
                auto point = std::make_pair(getCenterPoint(cell, points), CriticalPointType::SADDLE);
                criticalPoints.push_back(point);
            }
        }

        Point2 getCenterPoint(Cell cell, const ValueArray<Point2>& points) {
            auto p0 = points[cell.index(0)];
            auto p2 = points[cell.index(2)];
            Point2 middle((p0[0] + p2[0])/2, (p0[1] + p2[1])/2);
            return middle;
        }

        /**
         * @brief categorizeCriticalPoint
         * @param eigenvalues: Absteigend sortierte Liste an Eigenwerten (Realteile)
         * @return Typ des kritischen Punkts
         */
        CriticalPointType categorizeCriticalPoint(std::vector<double> eigenvalues) {
            double EPSILON = 0.000000001; //Grenze, um zwei doubles als gleich anzusehen @TODO sinnvollen Wert finden
            double lambda1 = eigenvalues[0];
            double lambda2 = eigenvalues[1];
            if (abs(lambda1 - lambda2) < EPSILON && abs(lambda1 - 0) < EPSILON) return CriticalPointType::CENTER;
            if (lambda1 > 0 && 0 > lambda2) return CriticalPointType::SADDLE;
            if ((lambda1 > lambda2 && lambda2 > 0) || (abs(lambda1 - lambda2) < EPSILON && lambda1 > 0)) return CriticalPointType::SINK;
            if ((lambda2 > lambda1 && 0 > lambda2) || (abs(lambda1 - lambda2) < EPSILON && 0 > lambda1)) return CriticalPointType::SOURCE;
            return CriticalPointType::NONE; //default
        }

    };

    AlgorithmRegister< CriticalPointsAlgorithm > reg("Hauptaufgabe/CriticalPoints", "Markiert Zellen mit kritischen Punkten");
}
