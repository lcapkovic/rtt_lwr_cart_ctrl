// rtt_lwr_cart_imp_ctrl - ISIR Thu 02 Jul 2015 06:16:26 PM CEST
// Copyright (c) Antoine Hoarau, All rights reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3.0 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library.

#ifndef __RTT_LWR_CART_IMP_CTRL_HPP__
#define __RTT_LWR_CART_IMP_CTRL_HPP__

#include "rtt_lwr_abstract/rtt_lwr_abstract.hpp"
#include <eigen_conversions/eigen_msg.h>
#include <kdl/frames.hpp>

#include <kdl/frames_io.hpp>
#include <kdl/framevel_io.hpp>
#include <kdl/kinfam_io.hpp>
#include <kdl/frameacc_io.hpp>

#include <kdl/trajectory.hpp>
#include <kdl/trajectory_segment.hpp>
#include <kdl/trajectory_stationary.hpp>
#include <kdl/trajectory_composite.hpp>
#include <kdl/trajectory_composite.hpp>
#include <kdl/velocityprofile_trap.hpp>
#include <kdl/path_roundedcomposite.hpp>
#include <kdl/rotational_interpolation_sa.hpp>
#include <kdl/utilities/error.h>
#include <kdl/trajectory_composite.hpp>

#include <lwr_flex_models/flex_model_12.h>
#include <lwr_flex_models/flex_model_23.h>
#include <lwr_flex_models/flex_model_42.h>

#include <nav_msgs/Path.h>
#include <rtt_ros_kdl_tools/chainjnttojacdotsolver.hpp>
#include <kdl/chainiksolvervel_pinv.hpp>
#include <kdl/chainiksolvervel_wdls.hpp>

#include <unsupported/Eigen/MatrixFunctions>

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}

namespace KDL{
    inline Vector asDiag(const Vector& a,const Vector& b)
    {
         return Vector(a.x() * b.x() ,a.y() * b.y() , a.z() * b.z());
    }
}


namespace lwr{
  static const int PINV_SOLVER  =0;
  static const int WDL_SOLVER   =1;
   
  class RttLwrCartImpCtrl : public RTTLWRAbstract{
    public:
      RttLwrCartImpCtrl(const std::string& name);
      virtual ~RttLwrCartImpCtrl(){
          delete ctraject;
    };
      bool computeTrajectory(const double radius,const double eqradius);
      void updateHook();
      bool configureHook();
      void updateCartesianGains(const double kp_lin,const double kp_ang,const double kd_lin,const double kd_ang);
      RTT::OutputPort<geometry_msgs::PoseStamped> port_X_curr;
      RTT::OutputPort<geometry_msgs::PoseStamped> port_X_des;
      RTT::OutputPort<geometry_msgs::PoseStamped> port_X_tmp;
      RTT::OutputPort<nav_msgs::Path> port_path_ros;

    protected:
      geometry_msgs::PoseStamped X_curr_msg,X_des_msg,X_tmp_msg;
      bool ready_to_start_;
      double gain_;
      KDL::Frame frame_des_kdl;
      Eigen::Matrix<double,6,1> X_err,Xd_err;
      KDL::Frame frame_kdl;
      KDL::FrameVel frame_vel_des_kdl;
      tf::Pose cart_pos_tf,cart_pos_tf_des;
      KDL::Wrench wrench_kdl;
      KDL::JntArray jnt_acc_kdl;
      Eigen::Matrix<double,6,1> F_ext;
      KDL::Twist cart_twist_des_kdl;
      
      flexmod12 flex_model_12;
      flexmod23 flex_model_23;
      flexmod42 flex_model_42;
      
      KDL::Path_RoundedComposite* path;
      KDL::VelocityProfile* velpref;
      KDL::Trajectory* traject;
      KDL::Trajectory_Composite* ctraject;
      void publishTrajectory();
      boost::scoped_ptr<KDL::ChainJntToJacDotSolver> jdot_solver;
      boost::scoped_ptr<KDL::ChainIkSolverVel_wdls> wdls_solver;
      KDL::Jacobian jdot;
      KDL::Twist jdot_qdot;
      double t_traj_curr;
      
      Eigen::Matrix<double,7,1> jnt_pos_eigen;
      Eigen::Matrix<double,7,1> corr_cart;
      
      boost::scoped_ptr<KDL::ChainIkSolverVel_pinv> pinv_solver;
      
      double kp_lin,kd_lin,kp_ang,kd_ang;
      KDL::JntArray qdd_des_kdl;
      Eigen::VectorXd qdd_des,coriolis;
      KDL::JntSpaceInertiaMatrix mass_kdl;
      bool traj_computed;
      double dx_ang_;
      bool debug_mode_;
      const bool isReadyToStart()const{return ready_to_start_;};
      KDL::Twist d_err_last;
      bool use_jdot_qdot_,use_coriolis_,use_f_ext_,use_xdd_des_;
      int jacobian_solver_type_;
      Eigen::MatrixXd mass_inv;
      double elapsed;
      bool use_mass_sqrt_;
      
  };
}
ORO_CREATE_COMPONENT(lwr::RttLwrCartImpCtrl)
#endif
