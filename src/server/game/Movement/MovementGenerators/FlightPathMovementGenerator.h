/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FlightPathMovementGenerator_h__
#define FlightPathMovementGenerator_h__

#include "MovementGenerator.h"
#include "PathMovementBase.h"

class Player;

/**
* FlightPathMovementGenerator generates movement of the player for the paths
* and hence generates ground and activities for the player.
*/
class FlightPathMovementGenerator : public MovementGeneratorMedium<Player, FlightPathMovementGenerator>, public PathMovementBase<Player, TaxiPathNodeList>
{
    public:
        explicit FlightPathMovementGenerator()
        {
            _currentNode = 0;
            _endGridX = 0.0f;
            _endGridY = 0.0f;
            _endGridZ = 0.0f;
            _endMapId = 0;
            _preloadTargetNode = 0;
        }
        void LoadPath(Player& player, uint32 startNode = 0);
        void DoInitialize(Player& );
        void DoReset(Player& );
        void DoFinalize(Player& );
        bool DoUpdate(Player& , uint32);
        MovementGeneratorType GetMovementGeneratorType() const override { return FLIGHT_MOTION_TYPE; }

        TaxiPathNodeList const& GetPath() { return _path; }
        uint32 GetPathAtMapEnd() const;
        bool HasArrived() const { return (_currentNode >= _path.size()); }
        void SetCurrentNodeAfterTeleport();
        void SkipCurrentNode() { ++_currentNode; }
        void DoEventIfAny(Player& player, TaxiPathNodeEntry const& node, bool departure);

        bool GetResetPosition(Unit&, float& x, float& y, float& z) override;
        void InitEndGridInfo();
        void PreloadEndGrid();

    private:

        float _endGridX;                            //! X coord of last node location
        float _endGridY;                            //! Y coord of last node location
        float _endGridZ;                            //! Z coord of last node location
        uint32 _endMapId;                           //! map Id of last node location
        uint32 _preloadTargetNode;                  //! node index where preloading starts

        struct TaxiNodeChangeInfo
        {
                uint32 PathIndex;
                int32 Cost;
        };

        std::deque<TaxiNodeChangeInfo> _pointsForPathSwitch;    //! node indexes and costs where TaxiPath changes
};

#endif // FlightPathMovementGenerator_h__
