# 逸仙狮考核期任务

## gitlab的使用

​	队里代码管理都是基于gitlab平台的，希望大家能够自行学习gitl的基础知识，如果不熟悉可以参考[教程](https://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000)。要求大家能掌握git的基本功能。

## RM2018规则的熟悉与理解

​	请各位自行阅读规则RM2018机甲大师赛比赛规则手册V2.0.pdf, 也可观看加比赛视频加深对比赛的理解。

## 单片机基础功能的实现

​	本项任务是想让大家熟悉单片机的基础功能的配置，我们使用的单片机型号为[stm32f407vet6 mini版](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.5ff62e8dBSj8zy&id=523361737493&_u=e1kvag5v70be)，完成下面任务也可以使用其他型号的单片机，不必苛求使用一样型号的单片机。另外，还需要一个[stlink](https://detail.tmall.com/item.htm?id=558866168716&spm=a1z09.2.0.0.5ff62e8dBSj8zy&_u=e1kvag5va880)将程序烧录进单片机中，以及一个[usb转串口模块](https://detail.tmall.com/item.htm?id=41299186436&spm=a1z09.2.0.0.5ff62e8dBSj8zy&_u=e1kvag5v4af7)。还需要少量杜邦线和LED发光二极管。



* 开发环境配置，安装包都在所给的[资料](https://share.weiyun.com/5H49dc2)里面。
  * Keil μVision 5 安装和配置
  * STLink 驱动安装
  * 串口驱动
  * 串口助手软件的安装


* GPIO的配置与使用（led.c）

  ​	配置引脚 PB9，并且使与之相连的 LED 灯能够固定一段时间闪烁一次。具体配置看文件 led.c。配置完 led.c 之后，在 main 中调用该初始化函数并通过 for 循环来改变 PB9 引脚高低电平的转化来实现板载 LED 灯的亮灭。

* USART  的 配置与使用(usart3.c)

  ​	完成文件usart3.c中的USART3_ Configuration()函数以及中断处理函数 USART3_IRQHandler()中对接收到的字符进行处理，通过电脑的串口助手软件实现发送数字给单片机，单片机收到数字后加 1 并发送回电脑。

* 时钟中断(timer.c)

  ​	完成文件 timer.c 中的函数 TIM6_ Configuration() 以及中断处理函数TIM6_DAC_IRQHandler()，利用时钟中断来实现 led 灯(PB9) 1s 闪烁一次。

* PWM的配置(pwm.c)

  ​	完成文件 pwm.c, 将PA0复用为PWM输出，并通过发光二极管实现呼吸灯功能。

* 时钟编码器模式（encoder.c）

  ​	完成 encoder 文件，利用时钟1的编码器输入模式，实现检测 上一任务的 pwm 的脉冲数。









