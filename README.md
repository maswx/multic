
### What is `multic`
`multic` 是一个计算变量乘以常数的工具。可以用最少的计算表达式拟合 out = ax ,其中a是常数，x是一个固定位宽的未知数。

在Verilog定点设计中通常将一个固定位宽的量分割为3个部分去表示一个数。
例如，浮点数 x=1.25，需要用8bit定点位宽表示时，需要先自己定义符号位位宽、整数位宽以及小数位宽.这里定义为`1 1 6` 也就是1个符号位，1个整数位，6个小数位.那么a = (0_1_010000)
此时如果需要计算 out = ax, 其中a = 0.1678901, 那么首先要将a二进制化。0.167890 =  + 2^-3 + 2^-5 + 2^-7 + 2^-8 - 2^-13 + 2^-15 + 2^-17 + 2^-18...；然后将x按a的值做响应的移位叠加即可。
需要注意的是，为了保证精度, 需要指定输出数据的位宽。例如指定输入x的数据类型为`1 1 6`, 输出数据类型为 `1 1 14` 那么运行

```shell
	./multic 0.1678901 -wi '1 1 6' -wo '1 1 14' 
```
即可获得所需要的verilog代码
```
const[0] = 0.167890 =  + 2^-3 + 2^-5 + 2^-7 + 2^-8 - 2^-13 + 2^-15 + 2^-17 + 2^-18 + 2^-20 + 2^-21 + 2^-25 - 2^-29 + 2^-33 + 2^-34 + 2^-36 + 2^-39 + 2^-41 - 2^-45 + 2^-48 + 2^-49 + 2^-51 + 2^-52 + 2^-54
wire [16-1:0]OUT = //(1+1+6)->(1+1+14)//multic 0.167890.
                 + {{ 3{IN0[8-1]}}, IN0[8-1: 0],  5'd0}//+2^-3
                 + {{ 5{IN0[8-1]}}, IN0[8-1: 0],  3'd0}//+2^-5
                 + {{ 7{IN0[8-1]}}, IN0[8-1: 0],  1'd0}//+2^-7
                 + {{ 8{IN0[8-1]}}, IN0[8-1: 0]       }//+2^-8
                 - {{13{IN0[8-1]}}, IN0[8-1: 5]       }//-2^-13
                 + {{15{IN0[8-1]}}, IN0[8-1: 7]       }//+2^-15
                 ;

```

### Build 
`multic` license by GPLv3。运行以下代码即可获取multic。
```shell
git clone https://github.com/maswell/multic.git
cd multic 
mkdir build
cd build
cmake ..
make
./multic -h
```



### Example
这里有一些其他常用的例子
```shell	
multic                       #get help. the same as `multic -h` / `multic -help` / `multic --help`
multic -v                    #version info and current version feature. the same as `multic --version` / `multic -V` / `multic -Version` / `multic -version`
multic 0.1678901             #multiplied by the constant 0.1678901. -wi 默认为 `1 1 6`  -wo 默认为 `1 1 8`
multic 0.1678901 -wi '1 1 6' -wo '1 1 8'     #指定输入输出数据位宽
multic 13.025234589 -f './out.txt'  #write the output to file. 
multic '0.12 -1.4'            # 支持多个数据, 计算 0.12*IN1 - 1.4*IN2 .
multic '-1e2 1e-1'            # 支持科学计数法
```









