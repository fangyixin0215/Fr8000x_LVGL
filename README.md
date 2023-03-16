# lvgl_knob_open_demo

#### 介绍
lvgl旋钮屏基础例程

#### 软件架构
软件架构说明


#### 安装教程

1.  ui转换资源工具详见https://gitee.com/http1520/lh_img_conver_tools.git
2.  模拟器工程代码详见https://gitee.com/http1520/lvgl_knob_visual_studio.git

#### 使用说明

1. 拉取工程所有代码到本地

2. 使用烧录工具freq_download添加ui资源目录下的测试固件all_test.bin即可测试功能

   ![image-20230228112507118](D:\WorkFiles\开发笔记\image-20230228112507118.png)

3. 如果需要更改开机动画文件，将gif文件名后缀改成.bin文件，然后使用freq_download工具中的选项框里面打开指定文件烧录，再添加动画文件的bin,将flash地址框填入0x100000烧录即可，如果需要更新ui图片文件参考转换工具里面的说明文档，将转换出来的UI_ALL.bin文件烧录到0x200000的地址。更改烧录指定文件时也要添加程序固件ble_simple_peripheral.bin文件。

   ![image-20230228113248959](D:\WorkFiles\开发笔记\image-20230228113248959.png)

   ![image-20230228113203584](D:\WorkFiles\开发笔记\image-20230228113203584.png)

   

4. 整个工程中涉及三个文件分别是程序固件ble_simple_peripheral.bin对应flash起始地址0x00，gif.bin文件对应flash地址0x100000，UI_ALL文件对应flash地址0x200000，也可以用工具将三个文件打包成一个文件方便烧录使用。https://gitee.com/http1520/file_hander_tools.git

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request

