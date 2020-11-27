#include "sobolev/constraints/total_volume.h"
#include "helpers.h"

namespace rsurfaces
{
    namespace Constraints
    {

        TotalVolumeConstraint::TotalVolumeConstraint(MeshPtr &mesh, GeomPtr &geom)
        {
            ResetFunction(mesh, geom);
        }

        void TotalVolumeConstraint::ResetFunction(MeshPtr &mesh, GeomPtr &geom)
        {
            initValue = totalVolume(geom, mesh);
        }

        size_t TotalVolumeConstraint::nRows()
        {
            return 1;
        }

        // Derivative of total volume is the area-weighted normal at each vertex.
        void TotalVolumeConstraint::addTriplets(std::vector<Triplet> &triplets, MeshPtr &mesh, GeomPtr &geom, int baseRow)
        {
            VertexIndices indices = mesh->getVertexIndices();
            for (GCVertex v : mesh->vertices())
            {
                Vector3 normal_v = areaWeightedNormal(geom, v);
                size_t i3 = 3 * indices[v];
                // Fill constraint row with area normal in each vertex's 3 entries
                triplets.push_back(Triplet(baseRow, i3, normal_v.x));
                triplets.push_back(Triplet(baseRow, i3 + 1, normal_v.y));
                triplets.push_back(Triplet(baseRow, i3 + 2, normal_v.z));
            }
        }

        void TotalVolumeConstraint::addEntries(Eigen::MatrixXd &M, MeshPtr &mesh, GeomPtr &geom, int baseRow)
        {
            VertexIndices indices = mesh->getVertexIndices();
            for (GCVertex v : mesh->vertices())
            {
                Vector3 normal_v = areaWeightedNormal(geom, v);
                size_t i3 = 3 * indices[v];
                // Fill constraint row with area normal in each vertex's 3 entries
                M(baseRow, i3) = normal_v.x;
                M(baseRow, i3 + 1) = normal_v.y;
                M(baseRow, i3 + 2) = normal_v.z;
            }
        }

        void TotalVolumeConstraint::addErrorValues(Eigen::VectorXd &V, MeshPtr &mesh, GeomPtr &geom, int baseRow)
        {
            double current = totalVolume(geom, mesh);
            V(baseRow) = (current - initValue);
        }

        double TotalVolumeConstraint::getTargetValue()
        {
            return initValue;
        }

        void TotalVolumeConstraint::incrementTargetValue(double incr)
        {
            initValue += incr;
        }

    } // namespace Constraints
} // namespace rsurfaces