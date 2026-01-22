#include "GameLevelLoader.hpp"
#include "GameLevels.hpp"


inline int conditional_temp_grid(const CarsStates& car, const CarsStates& exclude, int minus)
{
    if (minus == 2)
    {
        return 0;
    }
    
    if (PosVehicules::OrientationRULESpreset.at(car.orientation) == PosVehicules::OrientationRULESpreset.at(exclude.orientation))
    {
        return minus;
    }
    else
    {
        return 0;
    }
}

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

bool GameLevelLoader::CollisionCheck(Grid2D grid2d, size_t excludeIndex, int operator_sign)
{
    size_t i = 0;
    int x_minus = 0;
    int y_minus = 0;
    if (PosVehicules::OrientationRULESpreset.at(lev_data.at(excludeIndex).orientation) == OrientationRULES::LEFT_RIGHT)
    {
        x_minus = operator_sign;
    }
    else
    {
        y_minus = operator_sign;
    }

    auto it = std::find_if(lev_data.begin(), lev_data.end(), [&](const CarsStates& car){
        size_t currentIndex = i++;
        Grid2D temp_grid = {.x = car.grid2d.x + conditional_temp_grid(car, lev_data.at(excludeIndex), x_minus), .y = car.grid2d.y + conditional_temp_grid(car, lev_data.at(excludeIndex), y_minus)};
        return currentIndex != excludeIndex && car.true_car != 0 && temp_grid == grid2d;
    });

    return it != lev_data.end();
}