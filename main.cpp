#include "main.h"
#include "pros/misc.h"
#include "pros/motor_group.hpp"
#include "pros/motors.h"
#include "pros/rtos.hpp"

    
//Define all systems here

    //adi::DigitalOut
    //MotorGroup/Motor is for motor/s
    
    pros::Controller master(pros::E_CONTROLLER_MASTER);
    pros::MotorGroup left_mg({-4, 10, -9});    
    pros::MotorGroup right_mg({1, -3, 5}); 
    pros::MotorGroup intake({20, 2});
    pros::MotorGroup lady_brown({6});
    pros::adi::DigitalOut tilter('A');     
    
    bool tilter_toggle = false;

    bool lb_macro_ran = false;

    bool lb_macro_active = false;  // Flag to track macro activation

   

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {

}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    {
    //move to first mogo
       left_mg.move_voltage(-10000);
       right_mg.move_voltage(-8800);
       pros::delay(465);                          


       left_mg.move_voltage(0);
       right_mg.move_voltage(0);
       pros::delay(300);


   //clamp first mogo
       tilter.set_value(true);
       pros::delay(200);


   //Score preload
       intake.move_voltage(-12000);
       pros::delay(800);


   //turn to second ring
       left_mg.move_voltage(-5000);
       right_mg.move_voltage(5000);
       pros::delay(490);


       left_mg.move_voltage(0);
       right_mg.move_voltage(0);
       pros::delay(300);


   //Move towards second ring
       left_mg.move_voltage(7000);
       right_mg.move_voltage(7000);
       pros::delay(460);

       left_mg.move_voltage(0);
       right_mg.move_voltage(0);
       pros::delay(300);


   //score second ring
       intake.move_voltage(-12000);
       pros::delay(2000);


       intake.move_voltage(0);
       pros::delay(200);


   //turn to next ring
       left_mg.move_voltage(-6000);
       right_mg.move_voltage(6000);
       pros::delay(390);
      
       left_mg.move_voltage(0);
       right_mg.move_voltage(0);
       pros::delay(300);

        // move to ring
        left_mg.move_voltage(6000);
        right_mg.move_voltage(6000);
        pros::delay(300);

        left_mg.move_voltage(0);
        right_mg.move_voltage(0);
        pros::delay(300);

        //intake ring
        intake.move_voltage(-12000);
        pros::delay(3000);

        //turn to next ring
        left_mg.move_voltage(-6000);
        right_mg.move_voltage(6000);
        pros::delay(200);

        left_mg.move_voltage(0);
        right_mg.move_voltage(0);
        pros::delay(300);

         //Go forward
         left_mg.move_voltage(5000);
         right_mg.move_voltage(5000);
         pros::delay(100);
 
         left_mg.move_voltage(0);
         right_mg.move_voltage(0);
         pros::delay(300);
     }
}

void opcontrol() {

    bool toggle = false;

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		// Arcade control scheme
		int dir = master.get_analog(ANALOG_LEFT_Y);    // Gets amount forward/backward from left joystick
		int turn = master.get_analog(ANALOG_RIGHT_X);  // Gets the turn left/right from right joystick
		left_mg.move(dir + turn);                      // Sets left motor voltage
		right_mg.move(dir - turn);                     // Sets right motor voltage
		

//Code for intake

    //Reverse intake
	if (master.get_digital(DIGITAL_L1)) {
		intake.move_velocity(12000);

	//Score
	} else if (master.get_digital(DIGITAL_L2)) {
		intake.move_velocity(-12000);
	 
    //Do nothing
	} else {
		intake.move_velocity(0);
	}


// Code for LadyBrown

    // Move LB Up
    if (master.get_digital(pros::E_CONTROLLER_DIGITAL_X)) { 
        lady_brown.move_velocity(9000);
        lady_brown.set_brake_mode(MOTOR_BRAKE_COAST);
        lb_macro_active = false;  // Disable macro when manually moving
        lb_macro_ran = true; 

    // Move LB Down
    } else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_A)) { 
        lady_brown.move_voltage(-9000);
        lady_brown.set_brake_mode(MOTOR_BRAKE_COAST);
        lb_macro_active = false;  // Disable macro when manually moving
        lb_macro_ran = true;

    // LB Macro - Moves up slightly and holds position
    } else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
        lady_brown.move_velocity(12000);  // Moves up at a slow speed
        pros::delay(200);                // Delay for controlled movement
        lady_brown.move_velocity(0);      // Stop movement
        intake.move_velocity(12000);
        pros::delay(200);
        lady_brown.move_velocity(12000);
        pros::delay(75);
        lady_brown.set_brake_mode(MOTOR_BRAKE_HOLD);  // Hold position
        lb_macro_active = true;           // Set macro as active

    // If macro is active, keep hold mode
    } else if (lb_macro_active) { 
        lady_brown.move_velocity(0);
        lady_brown.set_brake_mode(MOTOR_BRAKE_HOLD);

    // Default state when no buttons are pressed
    } else { 
        lady_brown.move_velocity(0);  
        lady_brown.set_brake_mode(MOTOR_BRAKE_HOLD);
        lb_macro_ran = false;
    }


//Code for tilter

    //Press to activite/deactivate
	if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)){
		tilter.set_value(!toggle);
		toggle = !toggle;
	}

    pros::delay(10);
	}
}