# CAD_halfedge_Euler_sweep
Euler Operations and Sweeping Based on Half Edge Data Structure
在VS2017中x86下编译，系统中需要预先配置#include <glut.h>。
main函数中已经给出了一个含四个通孔正确的实体样例，可以直接运行查看结果。
可以使用上下左右方向键来改变观察角度，鼠标右击空白处可以开关深度测试和背面剔除效果。
可以在main函数中改变out_loop的点的坐标改变面的外环，定义多个正确的in_loop增加内环，并且需要相应的添加增加内环的环节。
改变d数组的值可以改变sweep的方向，d数组的三个值分别对应x,y,z方向。
