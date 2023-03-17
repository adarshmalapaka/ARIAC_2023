/**
 * @copyright Copyright (c) 2023
 * @file ariac_competition.hpp
 * @author Sanchit Kedia (sanchit@terpmail.umd.edu)
 * @author Adarsh Malapaka (amalapak@terpmail.umd.edu)
 * @author Tanmay Haldankar (tanmayh@terpmail.umd.edu)
 * @author Sahruday Patti (sahruday@umd.edu)
 * @author Kshitij Karnawat (kshitij@umd.edu)
 * @brief Implementation of RWA2 for ARIAC 2023 (Group 3)
 * @version 0.2
 * @date 2023-03-04
 * 
 * 
 */

#pragma once
#include <unistd.h>

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <typeinfo>
#include <vector>
#include <chrono>
#include <map>
#include <utility>
#include <algorithm>
#include <set>

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <ariac_msgs/msg/assembly_part.hpp>
#include <ariac_msgs/msg/assembly_task.hpp>
#include <ariac_msgs/msg/combined_task.hpp>
#include <ariac_msgs/msg/competition_state.hpp>
#include <ariac_msgs/msg/kitting_part.hpp>
#include <ariac_msgs/msg/kitting_task.hpp>
#include <ariac_msgs/msg/order.hpp>
#include <ariac_msgs/msg/part.hpp>
#include <ariac_msgs/srv/submit_order.hpp>
#include <ariac_msgs/msg/bin_parts.hpp>
#include <ariac_msgs/msg/conveyor_parts.hpp>
#include <rclcpp/qos.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/vector3.hpp>

#include "ceiling_robot.hpp"
#include "floor_robot.hpp"

class Orders;

/**
 * @brief Class definition for ARIAC Competition
 * 
 */
class AriacCompetition : public rclcpp::Node, public FloorRobot, public CeilingRobot {
    public:
        FloorRobot floor;
        CeilingRobot ceil;

        bool conveyor_parts_flag_{false};
        bool submit_orders_{false};
        int competition_state_ = -1;
        bool start_competition_flag_{false};

        std::vector<Orders> orders;
        std::vector<Orders> incomplete_orders;
        std::vector<Orders> current_order;
        std::vector<Orders> submitted_orders;
        
        std::vector<int> available_agv{1, 2, 3, 4};

        struct BinQuadrant {
            int part_type_clr = -1;
            geometry_msgs::msg::PoseStamped part_pose;
        };

        std::vector<int> conveyor_parts;
        std::map<int, BinQuadrant> bin_map;    // Holds part information in 72 possible bin locations (8 bins x 9 locations)

        /**
        * @brief Construct a new Ariac Competition object
        * 
        * @param node_name Name of the node
        */
        AriacCompetition(std::string);

        /**
        * @brief Callback function for competition state subscriber and to start competition
        * 
        * @param msg CompetitionState message 
        */
        void competition_state_cb(
            const ariac_msgs::msg::CompetitionState::ConstSharedPtr);

        /**
        * @brief Callback function to end the competition
        * 
        */
        void end_competition_timer_callback();

        /**
        * @brief Callback function to store the orders
        * 
        * @param msg Order
        */
        void order_callback(const ariac_msgs::msg::Order::SharedPtr);

            /**
        * @brief Callback function to retrieve bin part information
        * 
        * @param msg 
        */
        void bin_parts_callback(const ariac_msgs::msg::BinParts::SharedPtr);

        /**
        * @brief  Callback function to retrieve conveyor part information
        * 
        * @param msg 
        */
        void conveyor_parts_callback(const ariac_msgs::msg::ConveyorParts::SharedPtr);

        /**
        * @brief Method to submit the orders
        * 
        * @param order_id Order ID
        */
        void submit_order(std::string order_id);

        /**
        * @brief Method to process the order
        * 
        */
        void process_order();

        /**
        * @brief Method to do the kitting task
        * 
        */
        void do_kitting(std::vector<Orders>);

        /**
        * @brief Method to perform the assembly task
        * 
        */
        void do_assembly(std::vector<Orders>);

        /**
        * @brief Method to carry out the combined task
        * 
        */
        void do_combined(std::vector<Orders>);

        /**
        * @brief Method to search the bin for the part
        * 
        * @return int 
        */
        int search_bin(int);

        /**
        * @brief Method to check if the conveyor has the part
        * 
        * @return int 
        */
        int search_conveyor(int);

        /**
        * @brief Set the up map object to store the bin part information
        * 
        */
        void setup_map();

        /**
        * @brief Method to convert the part type to string
        * 
        * @param int Part type from ariac_msgs::msg:Part
        * @return std::string Part type as string
        */
        std::string ConvertPartTypeToString(int);

        /**
        * @brief Method to convert the part color to string
        * 
        * @param int Part color from ariac_msgs::msg:Part
        * @return std::string Part color as string
        */
        std::string ConvertPartColorToString(int);
        
        /**
        * @brief Method to convert the destination to string
        * 
        * @param int Destination from ariac_msgs
        * @return std::string Destination as string
        */
        std::string ConvertDestinationToString(int, int);
        
        /**
        * @brief Method to convert the assembly station to string
        * 
        * @param int Assembly station from ariac_msgs
        * @return std::string Assembly station as string
        */
        std::string ConvertAssemblyStationToString(int);

        /**
        * @brief Method to lock the AGV
        * 
        * @param int AGV number
        */
        void lock_agv(int);

        /**
        * @brief Method to move the AGV
        * 
        * @param int  AGV number
        * @param std::string AGV Destination
        *
        */
        void move_agv(int, std::string);

        /**
        * @brief Method to choose the AGV for Combined task
        * 
        * @param int Station number
        */
        int determine_agv(int);

    private:
        rclcpp::Subscription<ariac_msgs::msg::CompetitionState>::SharedPtr
            competition_state_sub_;

        rclcpp::TimerBase::SharedPtr end_competition_timer_;

        ariac_msgs::msg::Order order_;

        rclcpp::Subscription<ariac_msgs::msg::Order>::SharedPtr order_subscriber_;
        rclcpp::Subscription<ariac_msgs::msg::BinParts>::SharedPtr bin_parts_subscriber_;
        rclcpp::Subscription<ariac_msgs::msg::ConveyorParts>::SharedPtr conveyor_parts_subscriber_;
};

/**
 * @brief Class to store Kitting Order
 * 
 */
class Kitting {
    public:
        /**
            * @brief Construct a new Kitting object
            * 
            * @param agv_number 
            * @param tray_id 
            * @param destination 
            * @param _parts_kit 
            */
        Kitting(unsigned int agv_number,
                        unsigned int tray_id,
                        unsigned int destination,
                        const std::vector<std::array<int, 3>> _parts_kit) : agv_id_(agv_number),
                                                                    tray_id_(tray_id),
                                                                    destination_(destination),
                                                                    parts_kit_(_parts_kit) {}

        /**
            * @brief Get the Agv Id object
            * 
            * @return unsigned int 
            */
        unsigned int GetAgvId() const { return agv_id_; }

        /**
            * @brief Get the Tray Id object
            * 
            * @return unsigned int 
            */
        unsigned int GetTrayId() const { return tray_id_; }

        /**
            * @brief Get the Destination object
            * 
            * @return unsigned int 
            */
        unsigned int GetDestination() const { return destination_; }

        /**
            * @brief Get the Parts object
            * 
            * @return const std::vector<std::array<int, 3>> 
            */
        const std::vector<std::array<int, 3>> GetParts() const { return parts_kit_; }

    private:
        unsigned int agv_id_;
        unsigned int tray_id_;
        unsigned int destination_;
        std::vector<std::array<int, 3>> parts_kit_;
};

/**
 * @brief Struct of type Part used in Assembly and Combined Order
 * 
 */
struct Part {
      int type;
      int color;
      geometry_msgs::msg::PoseStamped assembled_pose;
      geometry_msgs::msg::Vector3 install_direction;
};

/**
 * @brief Class to store Assembly Order
 * 
 */
class Assembly {
    public:
        /**
        * @brief Construct a new Assembly object
        * 
        * @param agv_numbers 
        * @param station 
        * @param parts_assm 
        */
        Assembly(std::vector<unsigned int> agv_numbers, unsigned int station, std::vector<Part> parts_assm) : agv_numbers_(agv_numbers),
                                                                                            station_(station),
                                                                                            parts_assm_(parts_assm) {}

        /**
        * @brief Get the Agv Numbers object
        * 
        * @return const std::vector<unsigned int> 
        */
        const std::vector<unsigned int> GetAgvNumbers() const { return agv_numbers_; }

        /**
        * @brief Get the Station object
        * 
        * @return unsigned int 
        */
        unsigned int GetStation() const { return station_; }

        /**
        * @brief Get the Parts object
        * 
        * @return const std::vector<Part> 
        */
        const std::vector<Part> GetParts() const { return parts_assm_; }

    private:
        std::vector<unsigned int> agv_numbers_;
        unsigned int station_;
        std::vector<Part> parts_assm_;
};

/**
 * @brief Class to store Combined Order
 * 
 */
class Combined {
    public:
        /**
        * @brief Construct a new Combined object
        * 
        * @param _station 
        * @param parts_comb 
        */
        Combined(unsigned int _station, std::vector<Part> parts_comb) : station_(_station),
                                                                parts_comb_(parts_comb) {}

        /**
        * @brief Get the Station object
        * 
        * @return unsigned int 
        */
        unsigned int GetStation() const { return station_; }

        /**
        * @brief Get the Parts object
        * 
        * @return const std::vector<Part> 
        */
        const std::vector<Part> GetParts() const { return parts_comb_; }

    private:
        unsigned int station_;
        std::vector<Part> parts_comb_;
};

class Orders {
    protected:
        std::string id_;
        unsigned int type_;
        bool priority_;
        std::shared_ptr<Kitting> kitting_ = nullptr;
        std::shared_ptr<Assembly> assembly_ = nullptr;
        std::shared_ptr<Combined> combined_ = nullptr;

    public:
        /**
        * @brief Construct a new Orders object
        * 
        * @param id 
        * @param type 
        * @param priority 
        */
        Orders(std::string id,
                unsigned int type,
                bool priority) : id_(id),
                                    type_(type),
                                    priority_(priority) {}
        ~Orders() = default;
        
        /**
        * @brief Get the Id object
        * 
        * @return std::string 
        */
        std::string GetId() const { return id_; }

        /**
        * @brief Get the Type object
        * 
        * @return unsigned int 
        */
        unsigned int GetType() const { return type_; }
        
        /**
        * @brief Get the Priority of the object
        * 
        * @return true 
        * @return false 
        */
        bool IsPriority() const { return priority_; }

        /**
        * @brief Get the Kitting object
        * 
        * @return std::shared_ptr<Kitting> 
        */
        std::shared_ptr<Kitting> GetKitting() const { return kitting_; }

        /**
        * @brief Set the Kitting object
        * 
        * @param _kitting 
        */
        virtual void SetKitting(std::shared_ptr<Kitting> _kitting) { kitting_ = _kitting; }

        /**
        * @brief Get the Assembly object
        * 
        * @return std::shared_ptr<Assembly> 
        */
        std::shared_ptr<Assembly> GetAssembly() const { return assembly_; }

        /**
        * @brief Set the Assembly object
        * 
        * @param _assembly 
        */
        virtual void SetAssembly(std::shared_ptr<Assembly> _assembly) { assembly_ = _assembly; }

        /**
        * @brief Get the Combined object
        * 
        * @return std::shared_ptr<Combined> 
        */
        std::shared_ptr<Combined> GetCombined() const { return combined_; }

        /**
        * @brief Set the Combined object
        * 
        * @param _combined 
        */
        virtual void SetCombined(std::shared_ptr<Combined> _combined) { combined_ = _combined; }
};
