#include "cmsis_os.h"
#include "spi.h"
#include "AttitudeTask.h"

#include "irobotec.hpp"
#include "irobotec/modules/algorithm/mahony_ahrs.h"

using irobot_ec::device::BMI088;
using irobot_ec::modules::algorithm::MahonyAhrs;

// 注意！！：不要在全局作用域中创建传感器对象，因为这样会导致对象的构造函数在main函数之前调用，而HAL库此时还未初始化
// 如果需要全局访问，可以将对象指针声明为全局变量，然后在任务函数中new一个对象绑定到指针上
// 像这样：
BMI088 *bmi088;
// void task() {
//   bmi088 = new BMI088(...);
// }

void AttitudeTask(void const *unused) {
  // 对于BMI088，这个传感器由两个SPI设备组成，一个是加速度计，一个是陀螺仪
  // 使用时分别传入SPI总线指针、加速度计的CS引脚和陀螺仪的CS引脚
  bmi088 = new BMI088(hspi1, CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, CS1_GYRO_GPIO_Port, CS1_GYRO_Pin);
  // IST8310是一个I2C设备，传入I2C总线指针和RESET引脚
  //   IST8310 ist8310(hi2c3, GPIOG, GPIO_PIN_6);
  // 创建Mahony姿态解算器对象，传入采样率
  MahonyAhrs mahony(1000.0f);

  for (;;) {
    osDelay(1);

    // 更新传感器数据
    bmi088->Update();
    // ist8310.Update();

    // // 更新姿态解算器
    // mahony.Update(bmi088.gyro_x(), bmi088.gyro_y(), bmi088.gyro_z(),
    //               bmi088.accel_x(), bmi088.accel_y(), bmi088.accel_z(),
    //               ist8310.mag_x(), ist8310.mag_y(), ist8310.mag_z());
    // 更新姿态解算器(禁用磁力计)
    mahony.UpdateImu(bmi088->gyro_x(), bmi088->gyro_y(), bmi088->gyro_z(), bmi088->accel_x(), bmi088->accel_y(),
                     bmi088->accel_z());

    // 获取姿态数据(欧拉角，弧度)
    mahony.euler_pitch();
    mahony.euler_roll();
    mahony.euler_yaw();
    // 或者获取四元数
    mahony.quat_w();
    mahony.quat_x();
    mahony.quat_y();
    mahony.quat_z();
  }
}