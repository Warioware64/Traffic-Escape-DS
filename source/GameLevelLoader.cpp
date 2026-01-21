#include "GameLevelLoader.hpp"
#include "GameLevels.hpp"

void GameLevelLoader::LoadLevel(int level)
{
    lev_data = Levels.at(level);

    std::for_each(lev_data.begin(), lev_data.end(), [](CarsStates& n){
        if (n.true_car == 1)
        {
            n.basepose = PosVehicules::BasePoses.at(n.orientation);
            PosVehicules::LoadVehicule_Mesh(n.ptrMesh, n.carID, n.orientation);
            PosVehicules::LoadVehicule_Texture(&n.texGLptr, n.tex);
        }

    });
}