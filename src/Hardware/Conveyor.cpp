/**
 * @file Conveyor.cpp
 * @brief Implementation of the Conveyor class.
 */
#include "Hardware/Conveyor.h"

Conveyor::Conveyor(int speed_pin, int in1_pin, int in2_pin)
    : speed_pin_(speed_pin), in1_pin_(in1_pin), in2_pin_(in2_pin), is_running_(false) {}

void Conveyor::begin() {
  pinMode(speed_pin_, OUTPUT);
  pinMode(in1_pin_, OUTPUT);
  pinMode(in2_pin_, OUTPUT);
  stop(); // Ensure it's stopped initially
}

void Conveyor::run() {
  // IN1=HIGH, IN2=LOW 일 때 전진
  digitalWrite(in1_pin_, HIGH);
  digitalWrite(in2_pin_, LOW);
  analogWrite(speed_pin_, 128); // Half speed
  is_running_ = true;
}

void Conveyor::stop() {
  digitalWrite(in1_pin_, LOW);
  digitalWrite(in2_pin_, LOW);
  analogWrite(speed_pin_, 0);
  is_running_ = false;
}

bool Conveyor::isRunning() const {
  return is_running_;
} 