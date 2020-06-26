#ifndef HUBO_PLANNER_FOOTSTEP_PLANNER_H
#define HUBO_PLANNER_FOOTSTEP_PLANNER_H

#include <quadmap/quadmap.h>
#include <footstep_planner/footstep_planner_node.h>
#include <collision_detector/collision_detector.h>

#include <random>

#define USE_ROTATION

class FootstepPlanner {
public:
    // OPTIONS =========================================================================================================
    const double D = 0.09; // Displacement between center of body and center of foot
    const double SUPPORT_REGION_WIDTH  = 0.35;   // maximum range of next footstep in forward direction [m]
    const double SUPPORT_REGION_HEIGHT = 0.3;   // maximum range of next footstep in side direction [m]
    const double SUPPORT_REGION_BIAS   = 0.05;  //

    const double SUPPORT_REGION_MIN_X  = FOOTSTEP_WIDTH + 0.05;
    const double SUPPORT_REGION_MAX_X  = SUPPORT_REGION_WIDTH;
    const double SUPPORT_REGION_MIN_Y  = SUPPORT_REGION_BIAS;
    const double SUPPORT_REGION_MAX_Y  = SUPPORT_REGION_HEIGHT;

#ifdef USE_ROTATION
    const double SUPPORT_REGION_ROTATION = 0.174533; // maximum range of next footstep in rotation [rad]
#endif
    const unsigned int NUMBER_OF_TRIALS = 1000;

    enum {
        FOOT_LEFT  = 0,
        FOOT_RIGHT = 1
    };

public:
    FootstepPlanner(const double _footstep_width, const double _footstep_height)
    : environment(nullptr), stepping_stones(nullptr), FOOTSTEP_WIDTH(_footstep_width), FOOTSTEP_HEIGHT(_footstep_height)
    {
        footstep_size = quadmap::point2d((float)_footstep_width, (float)_footstep_height);
    }

    /*
     *
     */
    bool planning(const Configuration& _start_conf, const Configuration& _goal_conf);

    /*
     *
     */
    bool planning(const Configuration& _goal_conf);

    /*
     *
     */
    void set_environment(const quadmap::QuadTree* _environment) { this->environment = _environment; }

    /*
     *
     */
    void set_stepping_stones(const quadmap::QuadTree* _stepping_stones) { this->stepping_stones = _stepping_stones; }

    /*
     *
     */
    const std::vector<Configuration>& get_footsteps() const { return footsteps; }
    /*
     *
     */

    const Configuration& get_next_footstep() const { return footsteps[footsteps.size()-2]; }


    /*
     *
     */
    bool is_next_footstep_right() const { return !is_current_footstep_right; }

    /*
     *
     */
    void set_current_footstep_configuration(const Configuration& _configuration, bool _is_right) {
        current_footstep_configuration = _configuration;
        is_current_footstep_right = _is_right;
    }
    FootstepNode* get_current_footstep_node() {
        return new FootstepNode(is_current_footstep_right, current_footstep_configuration);
    }
    void set_current_footstep_configuration(const Configuration& _configuration) {
        current_footstep_configuration = _configuration;
        is_current_footstep_right = !is_current_footstep_right;
    }
    void set_current_footstep_configuration() {
        if(footsteps.size() < 2)
            return;
        current_footstep_configuration = footsteps[footsteps.size()-2];
        is_current_footstep_right = !is_current_footstep_right;
    }



protected:
    const quadmap::QuadTree* environment;
    const quadmap::QuadTree* stepping_stones;

    Configuration current_footstep_configuration;
    bool          is_current_footstep_right = false;

    /*
     *
     */
    void set_initial_footsteps_from_pose(const Configuration& _configuration, FootstepNode& _left_footstep, FootstepNode& _right_footstep);

    /*
     *
     */
    FootstepNode* make_new_sample(const std::vector<FootstepNode*>& _footstep_nodes);

    /*
     * Check whether the footstep is on the stepping stones.
     * The function checks the center of footstep and four vertices.
     * This computation can be inaccurate but computationally efficient.
     */
    inline bool isOnSteppingStones(const OBB& _footstep_model);

    /*
     *
     */
    inline bool isLinkToGoalStep(const FootstepNode* _A, const FootstepNode* _B);

    /*
     *
     */
    bool isAvailableFootStep(const FootstepNode* _footstep_node);

    const double        FOOTSTEP_WIDTH;
    const double        FOOTSTEP_HEIGHT;
    quadmap::point2d    footstep_size;

    std::vector<Configuration> footsteps;   // 0-index: goal footstep, last index: start(previous) footstep
};

#endif //HUBO_PLANNER_FOOTSTEP_PLANNER_H