// Copyright (c) 2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TEB_CONTROLLER_HPP_
#define TEB_CONTROLLER_HPP_

#include <memory>
#include <string>
#include <thread>

#include <dwb_controller/progress_checker.hpp>
#include <dwb_core/common_types.hpp>
#include <dwb_core/dwb_local_planner.hpp>
#include <nav_2d_msgs/msg/pose2_d_stamped.hpp>
#include <nav_2d_utils/odom_subscriber.hpp>
#include <nav2_util/lifecycle_node.hpp>
#include <nav2_msgs/action/follow_path.hpp>
#include <nav2_util/simple_action_server.hpp>

#include "teb_local_planner/teb_local_planner_ros.h"

namespace teb_local_planner
{

class TebController : public nav2_util::LifecycleNode
{
public:
  TebController();
  ~TebController();

protected:
  // The lifecycle interface
  nav2_util::CallbackReturn on_configure(const rclcpp_lifecycle::State & state) override;
  nav2_util::CallbackReturn on_activate(const rclcpp_lifecycle::State & state) override;
  nav2_util::CallbackReturn on_deactivate(const rclcpp_lifecycle::State & state) override;
  nav2_util::CallbackReturn on_cleanup(const rclcpp_lifecycle::State & state) override;
  nav2_util::CallbackReturn on_shutdown(const rclcpp_lifecycle::State & state) override;
  nav2_util::CallbackReturn on_error(const rclcpp_lifecycle::State & state) override;

  using GoalHandle = rclcpp_action::ServerGoalHandle<nav2_msgs::action::FollowPath>;
  using ActionServer = nav2_util::SimpleActionServer<nav2_msgs::action::FollowPath>;

  // Our action server implements the FollowPath action
  std::unique_ptr<ActionServer> action_server_;

  // The action server callback
  void followPath();

  bool isGoalReached();
  void publishVelocity(const nav_2d_msgs::msg::Twist2DStamped & velocity);
  void publishZeroVelocity();
  bool getRobotPose(nav_2d_msgs::msg::Pose2DStamped & pose2d);
  std::vector<geometry_msgs::msg::PoseStamped> pathToPoseVec(const nav2_msgs::msg::Path & path);
  void setPlannerPath(const nav2_msgs::msg::Path & path);
  void computeAndPublishVelocity();
  void updateGlobalPath();

  // The TebController contains a costmap node
  std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros_;
  std::unique_ptr<std::thread> costmap_thread_;

  // Publishers and subscribers
  std::shared_ptr<nav_2d_utils::OdomSubscriber> odom_sub_;
  rclcpp_lifecycle::LifecyclePublisher<geometry_msgs::msg::Twist>::SharedPtr vel_pub_;

  // The local planner
  std::unique_ptr<TebLocalPlannerROS> planner_;
  
  std::unique_ptr<rclcpp::executors::SingleThreadedExecutor> costmap_executor_;
  
  std::unique_ptr<dwb_controller::ProgressChecker> progress_checker_;
};

}  // namespace dwb_controller

#endif