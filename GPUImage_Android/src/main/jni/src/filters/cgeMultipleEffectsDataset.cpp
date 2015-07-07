/*
* cgeMultipleEffectsDataset.cpp
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*/

#include "cgeMultipleEffectsDataset.h"

/*
By Wang Yang:

简要介绍：
ParsingEngine的目的是让完全不懂GLSL的人也知道如何快速添加新的特效。


EffectString解析规则和常用方法说明:

1. 每一步不同的处理均以'@'开头，后面跟处理方式。中间可以有空格或者没有
例: "@ method" 或者 "@method" 均为正确格式。method后面可以跟任意个字符，以空格隔开。
method后面的所有字符(直到下一个"@"符号或者字符串结束)将作为参数传递给该method对应的Parser.

2. curve方法参数解释： @curve方法格式为 "@curve <arg1> <arg2> ... <argN>"
<argN> 的格式有两种： "RGB (x1,y1) (xn, yn)", xn和yn分别表示0~255之间的数字
或者 "R (rx1,ry1) ... (rxn, ryn) G (gx1,gy1) ... (gxn,gyn) B (bx1,by1)...(bxn,byn)"
其中R，G，B分别表示对应通道，后面跟的点即为该通道下的点。
括号与参数之间可以有空格也可以没有。括号中的x与y之间可以使用任意间隔符如空格，逗号等.

例： 
曲线A: RGB 通道调整点为 (0, 0) (100, 200) (255, 255)
格式： "@curve RGB(0,0) (100, 200) (255, 255)"

曲线B: R通道(0, 0) (50, 25) (255, 255),
G通道(0, 0) (100, 150) (255, 255),
RGB通道(0, 0) (200, 150) (255, 255)
格式： "@curve R(0,0) (50, 25) (255, 255) G(0, 0) (100,150) (255, 255) RGB(0, 0) (200, 150) (255, 255)". PS(WangYang):为了简化我们的工作，我编写了曲线格式生成工具。请到tool目录下获取。

注： 当有连续多次curve调整时，可只写一次"@curve"，以上面的例子为例，如果曲线A，B中间没有其他操作，那么可以将A，B连接为一条curve指令，这将节省开销加快程序执行速度。
例: "@curve RGB(0,0) (100, 200) (255, 255) R(0,0) (50, 25) (255, 255) G(0, 0) (100,150) (255, 255) RGB(0, 0) (200, 150) (255, 255)" (与上例先后执行曲线A，曲线B结果相同)

3. blend方法参数解释： @blend方法格式为 "@blend <function> <texture> <intensity>"
<function>值目前有

正常: mix
溶解: dissolve[dsv]

变暗:     darken[dk]
正片叠底: multiply[mp]
颜色加深: colorburn[cb]
线性加深: linearburn[lb]
深色:     darkercolor[dc]

变亮:     lighten[lt]
滤色:     screen[sr]
颜色减淡: colordodge[cd]
线性减淡: lineardodge[ld]
浅色:     lightercolor[lc]

叠加:     overlay[ol]
柔光:     softlight[sl]
强光:     hardlight[hl]
亮光:     vividlight[vvl]
线性光:   linearlight[ll]
点光:     pinlight[pl]
实色混合: hardmix[hm]

差值:     difference[dif]
排除:     exclude[ec]
减去:     subtract[sub]
划分:     divide[div]

色相:     hue
饱和度:   saturation[sat]
颜色:     color[cl]
明度:     luminosity[lum]

相加:     add
反向加:   addrev
黑白:     colorbw  //此方法仅依据texture的alpha通道将src color置换为黑白.

注: [] 中的表示该值的缩写，可以使用缩写代替原本过长的参数值。

<texture> 参数表示所用到的资源文件名，包含后缀! 

<intensity>表示叠加强度(不透明度)，为(0, 100] 之间的整数。

例：使用资源图 src.jpg 进行叠加，强度为80%
格式： "@blend overlay src.jpg 80" 或者 "@blend ol src.jpg 80"

4. krblend方法参数解释： @krblend方法格式与blend方法一样。参见@blend方法。
 另, krblend所有参数以及用法均与blend方法一致。区别在于krblend方法进行纹理处理时，
 将固定纹理的比例以及对纹理进行等比例缩放以使最终覆盖全图。

5. pixblend方法参数解释： @pixblend方法格式为 "@pixblend <function> <color> <intensity>"
<function>参数与blend方法相同，请直接参考blend方法。
<intensity>参数含义与blend方法相同，请直接参考blend方法。
<color>参数包含四个浮点数，分别表示这个颜色的r,g,b,a，取值范围为 [0, 1] 或者 [0, 255]
例: 使用纯红色进行叠加，强度为90%
格式： "@pixblend overlay 1 0 0 0 90"  -->注意，中间的颜色值可以写为小数。当alpha值大于1时，所有颜色参数值域范围将被认为是[0, 255] 否则被认为是[0, 1]

6. selfblend方法参数解释： @selfblend方法格式为 "@selfblend <function> <intensity>"
注： 此方法中对应的参数与blend方法相同，区别在于没有<texture>参数。本方法将使用待处理图片自身颜色进行混合。

7. adjust方法参数解释： @adjust方法格式为 "@adjust <function> <arg1>...<argN>"
<function>值目前有
brightness (亮度): 后接一个参数 intensity, 范围 [-1, 1]

contrast (对比度): 后接一个参数 intensity, 范围 intensity > 0, 当 intensity = 0 时为灰色图像, intensity = 1 时为无效果, intensity > 1 时加强对比度.

saturation (饱和度): 后接一个参数 intensity, 范围 intensity > 0, 当 intensity = 0 时为黑白图像, intensity = 1 时为无效果， intensity > 1 时加强饱和度

monochrome (黑白): 后接六个参数, 范围 [-2, 3], 与photoshop一致。参数顺序分别为: red, green, blue, cyan, magenta, yellow

sharpen (锐化): 后接一个参数 intensity, 范围 [0, 10], 当intensity为0时无效果
blur (模糊): 后接一个参数 intensity, 范围 [0, 1], 当 intensity 为0时无效果

whitebalance (白平衡): 后接两个参数 temperature (范围：[-1, 1], 0为无效果) 和 tint (范围: [0, 5], 1 为无效果)

shadowhighlight[shl] (阴影&高光): 后接两个参数 shadow(范围: [-200, 100], 0为无效果) 和 highlight(范围: [-100, 200], 0为无效果)

hsv : 后接六个参数red, green, blue, magenta, yellow, cyan. 六个参数范围均为 [-1, 1]
hsl : 后接三个参数hue, saturation, luminance， 三个参数范围均为 [-1, 1]

level (色阶): 后接三个参数 dark, light, gamma, 范围均为[0, 1], 其中 dark < light

exposure (曝光) : 后接一个参数 intensity, 范围 [-10, 10]

colorbalance (色彩平衡): 后接三个参数 redShift [-1, 1], greenShift [-1, 1], blueShift [-1, 1]. (添加日期: 2015-3-30)

注: [] 中的表示该值的缩写，可以使用缩写代替原本过长的参数值。
<arg*> 表示该方法所需的参数，具体范围请参考相关class。 <arg*>的个数与具体<function>有关，

8. cvlomo方法参数解释： @cvlomo方法包含了子方法curve。格式 "@cvlomo <vignetteStart> <vignetteEnd> <colorScaleLow> <colorScaleRange> <saturation> <curve>"
<vignetteStart>和<vignetteEnd>均为大于0的小数，一般小于1。
<colorScaleLow> <colorScaleRange> 均为大于0的小数，一般小于1。 用于调整图像
<saturation> 0~1之间的小数， 设置图像饱和度。
参数<curve> 为一个完整的curve方法，但是不添加@curve 标记。
例： "@cvlomo 0.2 0.8 0.1 0.2 1 RGB(0, 0) (255, 255)"

9. colorscale方法参数解释: @colorscale方法格式为 "@colorscale <low> <range> <saturation>"
注: colorscale方法需要进行CPU计算，较影响速度。

10. vignette 方法参数解释： @vignette方法格式为 "@vignette <low> <range> <centerX> <centerY>
注: 其中low和range是必须选项，centerX和centerY是可选项，若不填，则默认为0.5。 centerX和centerY必须同时存在才生效。
例： "@vignette 0.1 0.9" , "@vignette 0.1 0.9 0.5 0.5" 

11. colormul 方法参数解释： @colormul方法格式为 "@colormul <function> <arg1> ...<argN>"
参数<function>值目前有 "flt", "vec" 和 "mat"。
当<function>为flt时， 后面跟一个参数 <arg>，将对所有像素执行乘法.
当<function>为vec时，后面跟三个参数<arg1> <arg2> <arg3>，将对所有像素分别执行rgb分量各自相乘。
当<function>为mat时，后面跟九个参数<arg1>...<argN>，将对所有像素分别执行矩阵的rgb分量进行矩阵乘法。

12. special方法参数解释: @special方法格式为 "@special <N>"
其中参数<N> 为该特效的编号。
此类用于处理所有不具备通用性的特效。直接重新编写一个processor以解决。

13. lomo方法参数解释：格式 "@lomo <vignetteStart> <vignetteEnd> <colorScaleLow> <colorScaleRange> <saturation> <isLinear>"
<vignetteStart>和<vignetteEnd>均为大于0的小数，一般小于1。
<colorScaleLow> <colorScaleRange> 均为大于0的小数，一般小于1。 用于调整图像
<saturation> 0~1之间的小数， 设置图像饱和度。
<isLinear> 0或1， 设置所使用的vignette方法是否为线性增长，不写此参数则默认为0
例： "@lomo 0.2 0.8 0.1 0.2 1 0"

======编号13 以前使用到的特效依赖库版本： 0.2.1.x =========

14. vigblend 方法参数解释: @vigblend方法格式为 "@vigblend <function> <color> <intensity> <low> <range> <centerX> <centerY> [kind]"
[isLinear] 参数为可选参数, 默认为0
0: 线性增强，vignette本身不包含alpha通道（alpha通道为1）
1: 线性增强，vignette本身以alpha通道形成渐变
2: 二次增强，vignette本身不包含alpha通道（alpha通道为1）
3: 二次增强，vignette本身以alpha通道形成渐变
例："@vigblend ol 0 0 0 1 50 0.02 0.45 0.5 0.5 0"
	"@vigblend mix 10 10 30 255 100 0 1.5 0.5 0.5 0",
	"@vigblend mix 10 10 30 255 100 0 1.5 0.5 0.5 1",
	"@vigblend mix 10 10 30 255 100 0 1.5 0.5 0.5 2",
	"@vigblend mix 10 10 30 255 100 0 1.5 0.5 0.5 3",
注：其他参数含义以及用法参考 pixblend 方法以及 vignette 方法。


======↑此注释以上编号使用的特效库依赖版本 0.3.2.1

15. selcolor (selective color) 方法参数解释: @selcolor方法格式："@selcolor <color1> <colorargs1> ...<colorN> <colorargsN>"
其中<colorN>为选择的颜色， 有效参数包括：
red, green, blue, cyan, magenta, yellow, white, gray, black.

<colorargsN> 为对选择颜色所做出的调整， 格式为
(cyan, magenta, yellow, key)   范围： [-1, 1]
每一个<colorargsN> 为使用括号括起来的四个参数， 如果该参数未作调整， 则写0

======↑新增加 2014-11-12

16. style 方法参数解释: @style 方法格式为 "@style <function> <arg1> ... <argN>"
<function>值目前有

crosshatch (交叉阴影): 后接两个参数 spacing 范围[0, 0.1] 和 lineWidth 范围(0, 0.01]

edge (sobel查找边缘): 后接两个参数 mix 范围[0, 1] 和 stride 范围(0, 5]

emboss (浮雕): 后接三个参数 mix 范围[0, 1], stride 范围[1, 5] 和 angle 范围[0, 2π]

halftone (半调): 后接一个参数 dotSize 范围 >= 1

haze (雾): 后接三个参数 distance 范围[-0.5, 0.5], slope 范围 [-0.5, 0.5] 和 color (参数 color包含三个分量， 分别表示r, g, b， 范围均为[0, 1] )

polkadot (圆点花样): 后接一个参数 dotScaling 范围 (0, 1]


======↑新增加 2015-2-5

17. beautify 方法参数解释: @beautify 方法格式为 "@beautify <function> <arg1>...<argN>"

<function>值 目前有

bilateral (双边滤波): 后接 三个参数 模糊半径(blurScale) 范围[-100, 100], 色彩容差(distanceFactor) 范围[1, 20] 和 重复次数(repeat times) 范围 >= 1
                      其中 重复次数为可选参数， 如果不填， 则默认为 1

======↑新增加 2015-3-19

*/

namespace CGEEffectsDataSet
{
	const char* g_cgeEffectString[] =
	{

		"@cvlomo 0 0.85 0.1 0.1 1 R(0, 0)(89, 43)(178, 188)(221, 255)(255,255)G(0, 0)(63, 63)(184, 211)(255, 255)B(0, 22)(255, 216)",//0
		"@cvlomo 0 0.85 0.1 0.1 1 R (0, 0) (127, 145) (255, 229) G (0, 0) (63, 66) (127, 144) (255, 223) B (0, 0) (127, 117) (255, 195)",//1
		"@cvlomo 0 0.85 0.1 0.2 1 R (0, 0) (127, 91) (255, 214) G (0, 0) (127, 102) (255, 255) B (0, 0) (127, 108) (255, 255) ",//2
		"@cvlomo 0 0.93 0 0.08 1 R(0, 8)(63, 75)(191, 168)(255, 234)G(0, 0)(63, 50)(180, 208)(255, 255)B(0, 0)(50, 18)(150, 130)(255, 235) @pixblend softlight 190 150 28 255 30",//3
		"@cvlomo 0 0.85 0.1 0.1 0.3 R(0, 14)(63, 66)(129, 151)(208, 255)G(0, 38)(63, 72)(127, 127)(196, 255)B(0, 49)(63, 75)(127, 121)(202, 233)(255, 255)",//4

		"@curve R(0, 0)(117, 95)(155, 171)(179, 225)(255, 255)G(0, 0)(94, 66)(155, 176)(255, 255)B(0, 0)(48, 59)(141, 130)(255, 224)",//5
		"@curve R(0, 0)(69, 63)(105, 138)(151, 222)(255, 255)G(0, 0)(67, 51)(135, 191)(255, 255)B(0, 0)(86, 76)(150, 212)(255, 255)",//6
		"@curve R(0, 0)(43, 77)(56, 104)(100, 166)(255, 255)G(0, 0)(35, 53)(255, 255)B(0, 0)(110, 123)(255, 212)",//7
		"@curve R(0, 0)(35, 71)(153, 197)(255, 255)G(0, 15)(16, 36)(109, 132)(255, 255)B(0, 23)(181, 194)(255, 230)",//8
		"@curve R(15, 0)(92, 133)(255, 234)G(0, 20)(105, 128)(255, 255)B(0, 0)(120, 132)(255, 214)",//9
		"@curve R(0, 4)(255, 244)G(0, 0)(255, 255)B(0, 84)(255, 194)",//10
		"@curve R(48, 56)(82, 129)(130, 206)(214, 255)G(7, 37)(64, 111)(140, 190)(232, 220)B(2, 97)(114, 153)(229, 172)",//11
		"@curve R(39, 0)(93, 61)(130, 136)(162, 193)(208, 255)G(41, 0)(92, 61)(128, 133)(164, 197)(200, 250)B(0, 23)(125, 127)(255, 230)",//12
		"@curve R(40, 162)(108, 186)(142, 208)(193, 227)(239, 249)G(13, 7)(72, 87)(124, 150)(197, 206)(255, 255)B(8, 22)(57, 97)(112, 147)(184, 204)(255, 222)",//13
		"@curve R(18, 0)(67, 63)(104, 152)(128, 255)G(23, 4)(87, 106)(132, 251)B(17, 0)(67, 63)(108, 174)(128, 251)",//14
		"@curve R(5, 49)(85, 173)(184, 249)G(23, 35)(65, 76)(129, 145)(255, 199)B(74, 69)(158, 107)(255, 126)",//15

		//Note: @colorscale 0.01 0.01 1 should be used in 16~20
		"@special 16",//16
		"@adjust hsv -0.7 -0.7 0.5 -0.7 -0.7 0.5 @pixblend ol 0.243 0.07059 0.59215 1 25",//17
		"@adjust hsv -0.7 0.5 -0.7 -0.7 -0.7 0.5 @pixblend ol 0.07059 0.60391 0.57254 1 25",//18
		"@adjust hsv -0.7 0.5 -0.7 -0.7 0 0 @pixblend ol 0.2941 0.55292 0.06665 1 25",//19
		"@adjust hsv -0.8 0 -0.8 -0.8 0.5 -0.8 @pixblend ol 0.78036 0.70978 0.09018 1 28",//20

		"@special 21",//21
		"@adjust hsv -0.4 -0.64 -1.0 -0.4 -0.88 -0.88 @curve R(0, 0)(119, 160)(255, 255)G(0, 0)(83, 65)(163, 170)(255, 255)B(0, 0)(147, 131)(255, 255)",//22
		"@adjust hsv -0.5 -0.5 -0.5 -0.5 -0.5 -0.5 @curve R(0, 0)(129, 148)(255, 255)G(0, 0)(92, 77)(175, 189)(255, 255)B(0, 0)(163, 144)(255, 255)",//23
		"@adjust hsv 0.3 -0.5 -0.3 0 0.35 -0.2 @curve R(0, 0)(111, 163)(255, 255)G(0, 0)(72, 56)(155, 190)(255, 255)B(0, 0)(103, 70)(212, 244)(255, 255)",//24
		"@curve R(40, 40)(86, 148)(255, 255)G(0, 28)(67, 140)(142, 214)(255, 255)B(0, 100)(103, 176)(195, 174)(255, 255) @adjust hsv 0.32 0 -0.5 -0.2 0 -0.4",//25
		"@curve R(4, 35)(65, 82)(117, 148)(153, 208)(206, 255)G(13, 5)(74, 78)(109, 144)(156, 201)(250, 250)B(6, 37)(93, 104)(163, 184)(238, 222)(255, 237) @adjust hsv -0.2 -0.2 -0.44 -0.2 -0.2 -0.2",//26
		"@adjust hsv -1 -1 -1 -1 -1 -1",//27

		"@special 28",//28

		"@special 29",//29
		"@special 30",//30
		"@special 31",//31
		"@special 32",//32

		"@unavailable",//33
		"@unavailable",//34
		"@unavailable",//35
		"@unavailable",//36
		"@unavailable",//37
		"@unavailable",//38
		"@special 39",//39
		"@unavailable",//40
		"@unavailable",//41

		"@krblend ol e42_t.jpg 100",//42
		"@krblend ol e43_t.jpg 50",//43
		"@krblend sr e44_t.jpg 100",//44
		"@krblend ol e45_t.jpg 100",//45
		"@krblend ol e46_t.jpg 65",//46
		"@krblend lt e47_t.jpg 50",//47
		"@krblend sr e48_t.jpg 65",//48
		"@krblend sr e49_t.jpg 100",//49
		"@krblend ol e50_t.jpg 50",//50
		"@krblend ol e51_t.jpg 100",//51
		"@krblend ol e52_t.jpg 80",//52
		"@krblend ol e53_t.jpg 30",//53
		"@krblend ol e54_t.jpg 30",//54
		"@krblend ol e55_t.jpg 30",//55
		"@krblend ol e56_t.jpg 40",//56
		"@krblend ol e57_t.jpg 40",//57
		"@krblend ol e58_t.jpg 20",//58
		"@krblend hl e59_t.jpg 30",//59
		"@krblend sr e60_t.jpg 80",//60
		"@krblend ol e61_t.jpg 20",//61
		"@krblend mp e62_t.jpg 40",//62
		"@krblend ol e63_t.jpg 30",//63
		"@krblend cd e64_t.jpg 75",//64
		"@krblend lt e65_t.jpg 80",//65
		"@krblend lt e66_t.jpg 60",//66
		"@krblend lt e67_t.jpg 45",//67
		"@krblend lt e68_t.jpg 55",//68
		"@krblend lt e69_t.jpg 30",//69
		"@krblend lt e70_t.jpg 40",//70
		"@krblend lt e71_t.jpg 40",//71
		"@krblend lt e72_t.jpg 50",//72


		"@special 73", //73
		"@special 74", //74
		"@special 75", //75
		"@special 76", //76
		"@special 77", //77
		"@special 78", //78
		"@special 79", //79
		"@special 80", //80

		"@colorscale 0.01 0.01 0 @blend ol e81_t.jpg 60",//81
		"@colorscale 0.01 0.01 0 @blend ol e82_t.jpg 60",//82

		"@colorscale 0.1 0.01 1 @blend hl Muti_texture1-3.jpg 56 @blend ol Muti_texture1-2.png 100 @blend sl Muti_texture1-1.png 100",//83
		"@unavailable",//84
		"@unavailable",//85
		"@unavailable",//86

		"@special 87",//87
		"@special 88",//88
		"@special 89",//89
		"@special 90",//90

		"@vignette 0.19 1 @adjust hsl -0.04 -0.52 0.06 ", //91

		"@special 92",//92
		"@cvlomo 0 0.95 0 0 1 R(0, 0)(69, 55)(179, 189)(239, 255)G(0, 0)(64, 64)(183, 198)(255, 255)B(0, 14)(255, 233) @krblend softlight e93_t.jpg 20 @selcolor gray(20, -5, 0, 0) ",//93
		"@lomo 0 0.95 -1 -1 1  @adjust whitebalance 0.15 1.28 @krblend softlight e94_t.jpg 15",//94
		"@adjust whitebalance 0.001 0.5",//95
		//"@special 96",//96
		"@krblend color e96_t.jpg 33",//96
		"@adjust whitebalance -0.3577 0.767",//97
		"@lomo 0 0.95 -1 -1 1 @krblend multiply e98_t.jpg 70 @adjust whitebalance -0.23 1.12 @adjust brightness 0.26 @adjust contrast 1.12",//98
		"@special 99",//99
		"@adjust saturation 0 @curve RGB(48, 23)(67, 95)(100, 184)(164, 244)(255, 255) @blend cb vtg.png 100",//100
		"@adjust saturation 0 @curve RGB(14, 0)(127, 137)(239, 255) @krblend ol Texturize2_32.jpg 30",//101

		"@curve R(33, 0)(104, 76)(167, 168)(218, 255)G(30, 0)(112, 71)(174, 189)(234, 255)B(8, 30)(160, 122)(233, 255)",//102
		"@curve R(35, 43)(159, 194)(255, 255)G(0, 26)(141, 157)(255, 218)B(0, 81)(255, 145)",//103
		"@lomo 0 1 0.01 0.01 0",//104
		"@cvlomo 0 1.8 0.1 0.1 0.6 R(0, 23)(105, 147)(255, 255)G(14, 18)(76, 84)(170, 186)(255, 230)B(26, 25)(98, 101)(255, 204)",//105
		"@curve R(14, 0)(175, 191)(255, 255)G(17, 0)(77, 70)(160, 166)(255, 239)B(12, 0)(91, 79)(171, 176)(255, 255)  @selcolor red(0, 0, 0, 50) white(-30, 0, 10, 0) gray(0, 0, -5, 0) black(-17, 54, -18, 5) @adjust contrast 1.20",//106
		"@cvlomo 0 1.8 0.01 0.01 1.3 R(0, 36)(87, 100)(163, 179)(255, 255)G(0, 0)(77, 74)(163, 186)(255, 255)B(21, 30)(255, 235)",//107
		"@cvlomo 0 1.8 0.01 0.01 1.3 R(33, 10)(255, 255)G(0, 15)(86, 94)(171, 194)(255, 255)B(0, 20)(255, 235)",//108
		"@cvlomo 0 1.8 0.01 0.01 1.3 R(0, 31)(106, 121)(255, 196)G(0, 11)(64, 95)(132, 140)(255, 229)B(0, 0)(157, 156)(255, 210)",//109
		"@krblend softlight e98_t.jpg 20 @cvlomo 0 1.5 0 0 1.2 R(8, 0)(69, 57)(142, 117)(255, 246)G(0, 6)(97, 92)(165, 170)(255, 255)B(0,5)(247, 255) @selcolor white(-50, 0, -10, -20) gray(0,-5, 0, 0) black(0, 10, 0, -20) @adjust contrast 1.2",//110

		"@colorscale 0.01 0.01 0",//111
		"@lomo 0 1.8 0.01 0.01 0",//112
		"@blend ol e113_t.jpg 100",//113
		"@colorscale 0.01 0.1 1 @krblend ec e114_t.png 51",//114
		"@blend ol e113_t.jpg 50",//115
		"@colorscale 0.01 0.01 0.5 @selfblend sr 100 @blend mix e116_t.png 51 @pixblend cl 0.4274 0.2353 0.08234 1 34",//116
		"@curve R(0, 23)(105, 147)(255, 255)G(14, 18)(76, 84)(170, 186)(255, 230)B(26, 25)(98, 101)(255, 204)",//117
		"@special 118", //118
		"@colormul mat 0.34 0.48 0.22 0.34 0.48 0.22 0.34 0.48 0.22 @curve R(0, 29)(20, 48)(83, 103)(164, 166)(255, 239)G(0, 30)(30, 61)(66, 94)(151, 160)(255, 241)B(2, 48)(82, 93)(166, 143)(255, 199)",//119
		"@colormul mat 0.34 0.48 0.22 0.34 0.48 0.22 0.34 0.48 0.22 @curve R(0, 0)(9, 10)(47, 38)(87, 69)(114, 92)(134, 116)(175, 167)(218, 218)(255, 255)G(40, 0)(45, 14)(58, 34)(74, 55)(125, 118)(192, 205)(255, 255)B(0, 0)(15, 16)(37, 31)(71, 55)(108, 88)(159, 151)(204, 201)(255, 255)",//120
		//############################################
		"@blend mix Glimmer_1.png 100 @curve R(0, 6)(102, 127)(157, 189)(237, 255)G(0, 7)(47, 60)(102, 156)(168, 222)(244, 255)B(4, 48)(79, 135)(179, 174)(255, 244) @blend ol Glimmer_2.jpg  10 @curve R(0, 39)(26, 50)(72, 71)(120, 108)(170, 159)(241, 232)G(0, 45)(29, 62)(113, 104)(168, 164)(217, 214)(255, 253)B(0, 42)(118, 133)(193, 174)(255, 220) @blend lb Glimmer_Gina.jpg 25",//121
		"@blend mix Glimmer_1.png 100 @curve R(0, 6)(102, 127)(157, 189)(237, 255)G(0, 7)(47, 60)(102, 156)(168, 222)(244, 255)B(4, 48)(79, 135)(179, 174)(255, 244) @blend ol Glimmer_2.jpg 10 @curve R(0, 39)(26, 50)(72, 71)(120, 108)(170, 159)(241, 232)G(0, 45)(29, 62)(113, 104)(168, 164)(217, 214)(255, 253)B(0, 42)(118, 133)(193, 174)(255, 220) @curve R(0, 0)(79, 79)(183, 199)(250, 255)G(1, 0)(125, 132)(169, 186)(255, 255)B(5, 4)(120, 127)(187, 206)(247, 255)",//122
		"@unavailable",//123
		"@unavailable",//124
		"@unavailable",//125
		"@unavailable",//126

		"@unavailable",//127
		"@unavailable",//128
		"@unavailable",//129
		"@unavailable",//130

		"@unavailable",//131
		"@unavailable",//132
		"@unavailable",//133
		"@unavailable",//134
		"@unavailable",//135
		"@unavailable",//136
		"@unavailable",//137
		//##########################################################################
		"@unavailable",//138
		"@unavailable",//139
		"@unavailable",//140
		"@unavailable",//141
		"@unavailable",//142
		"@unavailable",//143
		"@unavailable",//144
		"@unavailable",//145
		"@unavailable",//146
		"@unavailable",//147
		"@unavailable",//148
		"@unavailable",//149
		"@unavailable",//150
		"@unavailable",//151
		"@unavailable",//152
		"@unavailable",//153
		"@unavailable",//154
		"@unavailable",//155
		"@unavailable",//156
		"@unavailable",//157
		"@unavailable",//158
		"@unavailable",//159
		"@curve R(3, 0)(23, 29)(83, 116)(167, 206)(255, 255)G(5, 0)(56, 64)(160, 189)(255, 255)B(3, 0)(48, 49)(142, 167)(248, 255)",//160
		"@curve R(15, 0)(45, 37)(92, 103)(230, 255)G(19, 0)(34, 22)(138, 158)(228, 252)B(19, 0)(74, 63)(159, 166)(230, 255)",//161
		"@curve R(0, 4)(39, 103)(134, 223)(242, 255)G(0, 3)(31, 85)(68, 155)(131, 255)(219, 255)B(0, 3)(42, 110)(114, 207)(255, 255)",//162
		"@curve R(17, 0)(37, 18)(75, 52)(238, 255)G(16, 0)(53, 32)(113, 92)(236, 255)B(16, 0)(80, 57)(171, 164)(235, 255)",//163
		"@curve R(33, 0)(70, 32)(146, 143)(185, 204)(255, 255)G(22, 0)(103, 71)(189, 219)(255, 252)B(10, 0)(54, 29)(93, 66)(205, 220)(255, 255)",//164
		"@curve R(4, 4)(38, 38)(146, 146)(201, 202)(255, 255)G(0, 0)(80, 74)(192, 187)(255, 255)B(0, 0)(58, 58)(183, 184)(255, 255)",//165
		"@curve R(5, 8)(36, 51)(115, 145)(201, 220)(255, 255)G(6, 9)(67, 83)(169, 190)(255, 255)B(3, 3)(55, 60)(177, 190)(255, 255)",//166
		"@curve R(14, 0)(51, 42)(135, 138)(191, 202)(234, 255)G(11, 6)(78, 77)(178, 185)(242, 250)B(11, 0)(22, 10)(72, 60)(171, 162)(217, 209)(255, 255)",//167
		"@curve R(9, 0)(26, 7)(155, 108)(194, 159)(255, 253)G(9, 0)(50, 19)(218, 194)(255, 255)B(0, 0)(29, 9)(162, 116)(218, 194)(255, 255)",//168
		"@curve R(0, 0)(69, 93)(126, 160)(210, 232)(255, 255)G(0, 0)(36, 47)(135, 169)(250, 254)B(0, 0)(28, 30)(107, 137)(147, 206)(255, 255)",//169
		"@curve R(2, 2)(16, 30)(72, 112)(135, 185)(252, 255)G(2, 1)(30, 42)(55, 84)(157, 207)(238, 249)B(1, 0)(26, 17)(67, 106)(114, 165)(231, 250)",//170
		"@curve R(16, 0)(60, 45)(124, 124)(214, 255)G(18, 2)(91, 81)(156, 169)(213, 255)B(16, 0)(85, 74)(158, 171)(211, 255) @curve R(17, 0)(144, 150)(214, 255)G(16, 0)(61, 47)(160, 172)(215, 255)B(21, 2)(131, 135)(213, 255)",//171
		"@curve R(0, 0)(120, 96)(165, 255)G(90, 0)(131, 145)(172, 255)B(77, 0)(165, 167)(255, 255)",//172
		"@curve R(9, 0)(49, 62)(124, 155)(218, 255)G(10, 0)(30, 33)(137, 169)(223, 255)B(10, 0)(37, 45)(96, 122)(150, 182)(221, 255)",//173
		"@curve R(81, 3)(161, 129)(232, 253)G(91, 0)(164, 136)(255, 225)B(76, 0)(196, 162)(255, 225)",//174
		"@blend mp e175_t.png 80",//175
		"@blend lb e176_t.png 80",//176
		"@blend mp e177_t.png 50",//177
		"@blend mp e178_t.jpg 80",//178
		"@krblend sr e179_t.png 80",//179
		"@krblend sr e180_t.png 100",//180
		"@krblend ld e181_t.png 80",//181
		"@krblend mp e182_t.png 60",//182
		"@krblend mp e183_t.jpg 100",//183
		"@blend ol e184_t.png 100",//184
		"@blend ol e185_t.png 100",//185
		"@blend ol e186_t.png 80",//186
		"@blend mp e187_t.png 70",//187
		"@krblend ol e188_t.png 100",//188
		"@blend mp e189_t.png 70",//189
		"@blend ol e190_t.png 50",//190
		"@unavailable",//191
		"@unavailable",//192
		"@unavailable",//193
		"@unavailable",//194
		"@unavailable",//195
		"@unavailable",//196
		"@unavailable",//197
		"@unavailable",//198
		"@unavailable",//199
		"@unavailable",//200
		"@unavailable",//201
		"@unavailable",//202
		"@unavailable",//203
		"@unavailable",//204
		"@unavailable",//205
		"@unavailable",//206
		"@unavailable",//207
		"@unavailable",//208
		"@unavailable",//209
		"@unavailable",//210
		"@unavailable",//211
		"@unavailable",//212
		"@unavailable",//213
		"@unavailable",//214
		"@unavailable",//215
		"@unavailable",//216
		"@unavailable",//217
		"@unavailable",//218
		"@unavailable",//219
		"@unavailable",//220
		"@unavailable",//221
		"@unavailable",//222
		"@unavailable",//223
		"@unavailable",//224
		"@unavailable",//225
		"@unavailable",//226
		"@unavailable",//227
		"@unavailable",//228
		"@unavailable",//229
		"@unavailable",//230
		"@unavailable",//231
		"@unavailable",//232
		"@unavailable",//233
		"@unavailable",//234
		"@unavailable",//235
		"@unavailable",//236
		"@unavailable",//237
		"@unavailable",//238
		"@unavailable",//239
		"@unavailable",//240
		"@unavailable",//241
		"@unavailable",//242
		"@unavailable",//243
		"@unavailable",//244
		"@unavailable",//245
		"@unavailable",//246
		"@unavailable",//247
		"@unavailable",//248
		"@unavailable",//249
		"@unavailable",//250
		"@unavailable",//251
		"@unavailable",//252
		"@unavailable",//253
		"@unavailable",//254
		"@unavailable",//255
		"@unavailable",//256
		"@unavailable",//257
		"@unavailable",//258
		"@unavailable",//259
		"@unavailable",//260
		"@unavailable",//261
		"@unavailable",//262
		"@unavailable",//263
		"@unavailable",//264
		"@unavailable",//265
		"@unavailable",//266
		"@unavailable",//267
		"@unavailable",//268
		"@unavailable",//269
		"@unavailable",//270
		"@unavailable",//271
		"@unavailable",//272
		"@unavailable",//273
		"@unavailable",//274
		"@unavailable",//275
		"@unavailable",//276
		"@unavailable",//277
		"@unavailable",//278
		"@unavailable",//279
		"@unavailable",//280
		"@unavailable",//281
		"@unavailable",//282
		"@unavailable",//283
		"@unavailable",//284
		"@unavailable",//285
		"@unavailable",//286
		"@unavailable",//287
		"@unavailable",//288
		"@unavailable",//289
		"@unavailable",//290
		"@unavailable",//291
		"@unavailable",//292
		"@unavailable",//293
		"@unavailable",//294
		"@unavailable",//295
		"@unavailable",//296
		"@unavailable",//297
		"@unavailable",//298
		"@unavailable",//299
		"@curve R(0, 0)(135, 147)(255, 255)G(0, 0)(135, 147)(255, 255)B(0, 0)(135, 147)(255, 255)  @adjust saturation 0.71 @adjust brightness -0.05 @curve R(19, 0)(45, 36)(88, 90)(130, 125)(200, 170)(255, 255)G(18, 0)(39, 26)(71, 74)(147, 160)(255, 255)B(0, 0)(77, 58)(136, 132)(255, 204)",//300
		"@adjust saturation 0 @curve R(9, 13)(37, 13)(63, 23)(81, 43)(91, 58)(103, 103)(159, 239)(252, 242)G(3, 20)(29, 20)(56, 19)(77, 37)(107, 108)(126, 184)(137, 217)(150, 248)(182, 284)(255, 255)B(45, 17)(78, 51)(96, 103)(131, 202)(255, 255)",//301
		"@unavailable",//302
		"@curve R(42, 2)(53, 52)(80, 102)(100, 123)(189, 196)(255, 255)G(55, 74)(75, 98)(95, 114)(177, 197)(203, 212)(221, 220)(229, 234)(240, 249)B(0, 132)(81, 188)(180, 251)",//303
		"@adjust saturation 0 @curve R(0, 68)(10, 72)(42, 135)(72, 177)(98, 201)(220, 255)G(0, 29)(12, 30)(57, 127)(119, 203)(212, 255)(254, 239)B(0, 36)(54, 118)(66, 141)(119, 197)(155, 215)(255, 254)",//304
		"@unavailable",//305
		"@curve R(0, 64)(16, 13)(58, 128)(108, 109)(162, 223)(255, 255)G(0, 30)(22, 35)(42, 58)(56, 86)(70, 119)(130, 184)(189, 212)B(6, 36)(76, 157)(107, 192)(173, 229)(255, 255)", //306
		"@unavailable",//307
		"@krblend sl e308_t1.png 50 @krblend ol e308_t2.png 100",//308
		"@unavailable",//309
		"@curve R(0, 48)(89, 69)(145, 150)(195, 209)(255, 233)G(0, 47)(63, 53)(102, 82)(141, 146)(199, 212)(255, 238)B(0, 50)(48, 47)(93, 71)(133, 133)(201, 215)(255, 232) @adjust saturation 0.47  @blend mp e310_t.png 41",//310
		"@unavailable",//311
		"@unavailable",//312
		"@vigblend mix 10 10 30 255 91 0 1.0 0.5 0.5 3 @curve R(0, 31)(35, 75)(81, 139)(109, 174)(148, 207)(255, 255)G(0, 24)(59, 88)(105, 146)(130, 171)(145, 187)(180, 214)(255, 255)B(0, 96)(63, 130)(103, 157)(169, 194)(255, 255)",//313 //"@blend mix vtg313.png 91 @curve R(0, 31)(35, 75)(81, 139)(109, 174)(148, 207)(255, 255)G(0, 24)(59, 88)(105, 146)(130, 171)(145, 187)(180, 214)(255, 255)B(0, 96)(63, 130)(103, 157)(169, 194)(255, 255)",//313  
		"@unavailable",//314
		"@curve R(0, 47)(51, 70)(99, 127)(178, 221)(255, 255)G(0, 29)(27, 35)(59, 61)(113, 141)(169, 219)(255, 255)B(0, 8)(33, 15)(93, 83)(123, 152)(172, 222)(255, 225) @adjust hsl -0.033 -0.64 -0.05 @blend mix e315_t.png 100",//315
		"@adjust saturation 0 @curve R(0, 49)(16, 44)(34, 56)(74, 120)(120, 185)(151, 223)(255, 255)G(0, 46)(34, 73)(85, 129)(111, 164)(138, 192)(170, 215)(255, 255)B(0, 77)(51, 101)(105, 143)(165, 182)(210, 213)(250, 229)", //316
		"@unavailable",//317
		"@curve R(0, 29)(35, 81)(63, 141)(101, 214)(168, 243)(255, 255)G(0, 16)(25, 33)(89, 152)(118, 196)(164, 219)B(0, 0)(31, 5)(73, 27)(116, 51)(174, 69)(255, 82)", //318
		"@adjust saturation 0 @curve R(74, 0)(104, 54)(125, 130)(191, 226)(255, 255)G(0, 110)(28, 109)(98, 124)(132, 166)(176, 196)(255, 203)B(0, 127)(89, 138)(189, 198)(255, 195)",//319
		"@unavailable",//320
		"@adjust saturation 0 @adjust level 0 0.83921 0.8772",//321
		"@adjust level 0.0392 0.9255 1 @adjust level 0.2235 0.9412 0.763 @adjust hsl 0 -1 0.22",//322
		"@adjust hsl -0.02 -1 0.02",//323
		"@adjust saturation 0 @curve R(0, 0)(54, 83)(99, 167)(255, 255)G(0, 0)(54, 83)(99, 167)(255, 255)B(0, 0)(54, 83)(99, 167)(255, 255) @adjust level 0.1882 1 0.909", //324
		"@adjust brightness 0.19 @adjust saturation 0 @adjust level 0.2039 0.969 0.909 @blend mp e325_t.png 41",//325
		"@adjust saturation 0 @curve R(1, 0)(124, 126)(255, 255)R(0, 0)(132, 150)(255, 255)G(0, 0)(132, 150)(255, 255)B(0, 0)(132, 150)(255, 255)", //326
		"@unavailable",//327
		"@unavailable",//328
		"@krblend mix e329_t1.png 100 @curve R(0, 37)(39, 73)(53, 92)(89, 133)(129, 174)(204, 219)(255, 225)G(0, 36)(47, 86)(105, 164)(155, 198)(243, 235)B(0, 48)(22, 65)(46, 98)(84, 153)(160, 216)(207, 236)(255, 246) @adjust saturation 0.54 @curve R(47, 0)(255, 255)G(53, 3)(255, 255)B(51, 0)(255, 255)", //329
		"@adjust saturation 0.54 @curve R(0, 0)(71, 88)(130, 175)(255, 255)G(0, 0)(79, 81)(128, 164)(255, 255) @krblend mix e329_t1.png 100 @krblend ol e330_t2.png 35 @krblend mix e330_t3.png 35 @krblend mp e330_t4.png 80 @krblend mix e330_t5.png 10 @krblend mp e330_t6.png 10 @krblend mp e330_t7.png 15",  //330
		"@unavailable",//331
		"@unavailable",//332
		"@unavailable",//333
		"@curve R(0, 0)(113, 185)(145, 230)(255, 255)G(0, 0)(116, 152)(145, 179)(172, 212)(255, 255) @blend mix e334_t1.png 50 @blend sl e334_t2.png 40 @blend sl e334_t3.png 50 @blend mix e334_t4.png 100 @blend sl e334_t5.png 25 @blend mix e334_t6.png 15", //334
		"@adjust saturation 0.62 @krblend lt e335_t1.png 12 @krblend mix e335_t2.png 35 @krblend lt e335_t3.png 80 @krblend lt e335_t4.png 10", //335
		"@blend mix e336_t1.png 100 @adjust hsl 0.04 -0.41 0 @curve R(0, 34)(72, 143)(118, 189)(196, 218)(255, 228)G(0, 68)(102, 207)(254, 247)B(0, 65)(39, 165)(87, 227)(187, 248)(255, 255)", //336
		"@unavailable",//337
		"@curve R(0, 31)(48, 48)(173, 138)(252, 207)G(0, 23)(58, 49)(190, 206)B(0, 29)(67, 55)(115, 85)(181, 194)(255, 213) @adjust saturation 0.6 @blend sl e338_t1.png 100 @blend mix e338_t2.png 100 @blend mix e338_t3.png 30",//338
		"@curve R(0, 0)(122, 121)(255, 255)G(0, 0)(108, 143)(255, 255)B(0, 0)(114, 174)(255, 255) @blend mix e339_t1.png 50 @blend mix e339_t2.png 25 @blend hl e339_t3.png 80 @adjust saturation 0.6",  //339
		"@unavailable",//340
		"@adjust hsl 0 -0.29 -0.17 @adjust level 0.08235 1 0.97 @curve R(0, 74)(42, 96)(116, 155)(255, 255)G(0, 17)(21, 34)(124, 150)(255, 255)B(0, 12)(126, 128)(255, 252)", //341
		"@adjust hsl 0 -0.45 -0.4 @curve R(0, 81)(50, 114)(91, 183)(133, 210)(194, 234)(255, 255)G(0, 21)(50, 88)(112, 192)(255, 255)B(0, 0)(30, 28)(80, 145)(112, 185)(170, 216)(255, 255)", //342
		"@unavailable",//343
		"@adjust hsl 0 -0.11 -0.23 @adjust level 0.0588 1 1.0526 @curve R(10, 0)(62, 60)(143, 172)(255, 255)G(0, 7)(22, 33)(35, 45)(54, 73)(124, 147)(201, 220)B(5, 57)(71, 101)(155, 159)(255, 255)", //344
		"@adjust hsl 0 -0.14 -0.33 @curve R(0, 5)(42, 72)(137, 191)(255, 255)G(0, 26)(23, 68)(114, 181)(255, 241)B(0, 4)(22, 71)(150, 205)(255, 255)", //345
		"@unavailable",//346
		"@adjust hsl 0.02 -0.31 -0.17 @curve R(0, 28)(23, 45)(117, 148)(135, 162)G(0, 8)(131, 152)(255, 255)B(0, 17)(58, 80)(132, 131)(127, 131)(255, 225)", //347
		"@blend mix e348_t.png 5 @selfblend sl 15 @adjust level 0.0588 1 1 @adjust hsl 0 -0.2 -0.17 @curve R(0, 0)(74, 64)(183, 161)(255, 255)G(0, 0)(128, 131)(255, 255)B(0, 9)(57, 85)(76, 100)(127, 131)(255, 194)",//348
		////////////////////////////////
		"@blend ol e349_t.png 100",    //349
		"@blend ol e350_t.png 100",    //350
		"@blend ol e351_t.png 90",    //351
		"@blend ol e352_t.png 90",    //352
		"@krblend sr e353_t.png 80",    //353
		"@krblend sr e354_t.png 90",    //354
		"@krblend ld e355_t.png 80",    //355
		"@blend ol e356_t.png 80",    //356
		"@blend lb e357_t.png 100",    //357
		"@blend ol e358_t.png 100",    //358
		//////////////////////////////
		"@adjust brightness 0.79 @blend sr e359_t.png 100",//359
		"@blend cd e360_t1.png 78 @blend sr e360_t2.png 100", //360
		"@adjust brightness 0.75 @blend ld e361_t.png 84",//361
		"@blend sr e362_t.png 100",//362
		"@blend ld e363_t.png 70",//363
		"@adjust hsl -0.5 0 0.05 @blend ld e364_t.png 100",//364
		//////////////
		"@blend screen e365_t1.jpg 100",//365
		"@blend screen e366_t1.jpg 90 @pixblend lighten 0.6039 0.47843 0.396 1 60", //366
		"@blend screen e367_t1.png 100 @blend softlight e367_t2.jpg 100", //367
		"@blend screen e368_t1.jpg 80",//368
		"@blend overlay e369_t1.jpg 60 @blend screen e369_t2.jpg 80", //369
		"@blend overlay e370_t1.jpg 80 @pixblend multiply 1 0.37647 0 1 20", //370
		"@blend linearlight e371_t1.jpg 35 @blend screen e371_t2.jpg 90", //371
		"@blend screen e372_t1.jpg 90 @pixblend overlay 0.251 0.22353 0.73333 1 40", //372
		"@blend overlay e373_t1.jpg 80 @blend lighten e373_t2.jpg 100", //373
		"@blend overlay e374_t1.jpg 100 @blend overlay e374_t2.jpg 100",  //374
		"@blend overlay e375_t1.jpg 50 @pixblend overlay 0.439216 0.00784 0.996 1 40",//375
		"@blend softlight e376_t1.jpg 40 @blend lighten e376_t2.jpg 50",//376
		"@blend overlay e377_t1.jpg 100 @blend hardlight e377_t2.jpg 40",//377
		"@blend softlight e378_t1.jpg 100 @blend lighten e378_t2.jpg 60",//378
		"@blend screen e379_t1.jpg 80",//379
		"@blend screen e380_t1.jpg 100",//380
		"@blend screen e381_t1.jpg 80 @blend screen e381_t2.jpg 80", //381
		"@blend screen e382_t1.jpg 80 @pixblend overlay 0.1529 0.6196 0.839216 1 100", //382
		"@blend softlight e383_t2.jpg 80", //383
		"@blend hardlight e384_t1.jpg 55 @blend multiply e384_t2.jpg 40", //384
		"@blend mix e385_t1.jpg 60 @blend softlight e385_t2.jpg 45", //385
		"@blend lighten e386_t1.png 55 @blend screen e386_t2.jpg 70", //386
		"@blend lighten e387_t1.jpg 60 @blend screen e387_t2.png 85", //387
		"@blend overlay e388_t1.jpg 80 @blend screen e388_t2.jpg 40", //388
		"@blend screen e389_t1.png 100 @blend softlight e389_t2.jpg 100", //389
		"@blend screen e390_t1.jpg 80 @blend softlight e390_t2.jpg 100", //390
		"@blend mp e391_t1.png 100 @blend overlay e391_t2.jpg 80 @pixblend softlight 0.84314 0.28235 0.15686 1 60",//391
		"@pixblend mix 1 1 1 1 20 @blend screen e392_t2.png 100", //392
		"@blend colordodge e393_t1.png 80",//393
		"@pixblend screen 1 0 0 0 50 @blend lineardodge e394_t2.png 80", //394
		"@blend screen e395_t1.png 100 @blend softlight e395_t2.jpg 100", //395
		"@blend lineardodge e396_t1.png 80",//396
		"@blend colordodge e397_t1.png 100",//397
		"@curve R(0, 0)(129, 159)(255, 255)G(0, 0)(129, 159)(255, 255)B(0, 0)(129, 159)(255, 255) R(0, 0)(107, 109)(131, 158)(255, 255)G(0, 0)(54, 2)(121, 108)(169, 183)(255, 255)B(0, 0)(255, 255)" ,//398
		"@curve B(0, 48)(117, 125)(255, 168) R(0, 0)(170, 183)(255, 255)G(0, 0)(170, 183)(255, 255)B(0, 0)(170, 183)(255, 255)" ,//399
		"@curve R(0, 0)(165, 157)(255, 255)G(0, 0)(165, 157)(255, 255)B(0, 0)(165, 157)(255, 255)B(0, 0)(155, 208)(255, 255)" ,//400
		"@adjust hsl 0.053 -0.32 0.05 @curve R(0, 0)(209, 225)(255, 255)G(0, 0)(107, 128)(255, 255)B(0, 0)(56, 97)(181, 185)(255, 255)" ,//401

		//New "Black&White fx" -- from 402 to 414.
		"@adjust saturation 0 @curve R(0, 0)(108, 85)(163, 162)(255, 255)G(0, 0)(108, 85)(163, 162)(255, 255)B(0, 0)(108, 85)(163, 162)(255, 255) @blend overlay e402_t.jpg 80",//402
		"@adjust saturation 0 @curve R(0, 0)(87, 66)(142, 160)(255, 255)G(0, 0)(87, 66)(142, 160)(255, 255)B(0, 0)(87, 66)(142, 160)(255, 255) @blend overlay e403_t.jpg 100",//403
		"@adjust saturation 0 @curve R(0, 0)(75, 114)(145, 218)(255, 255)G(0, 0)(75, 114)(145, 218)(255, 255)B(0, 0)(75, 114)(145, 218)(255, 255) @blend overlay e404_t.jpg 100",//404
		"@adjust saturation 0 @curve R(0, 0)(66, 89)(188, 231)(255, 255)G(0, 0)(66, 89)(188, 231)(255, 255)B(0, 0)(66, 89)(188, 231)(255, 255) @blend colorburn e405_t.jpg 30",//405
		"@adjust saturation 0 @curve R(63, 0)(98, 139)(167, 255)G(63, 0)(98, 139)(167, 255)B(63, 0)(98, 139)(167, 255)",//406
		"@adjust saturation 0 @curve R(113, 0)(117, 255)G(113, 0)(117, 255)B(113, 0)(117, 255) @blend screen e407_t.jpg 50",//407
		"@adjust saturation 0 @curve R(0, 0)(69, 11)(132, 81)(194, 196)(255, 255)G(0, 0)(69, 11)(132, 81)(194, 196)(255, 255)B(0, 0)(69, 11)(132, 81)(194, 196)(255, 255) @blend overlay e408_t.jpg 50",//408
		"@adjust saturation 0 @curve R(0, 0)(113, 133)(255, 255)G(0, 0)(113, 133)(255, 255)B(0, 0)(113, 133)(255, 255) @blend overlay e403_t.jpg 100",//409
		"@adjust saturation 0 @curve R(0, 0)(37, 19)(70, 89)(150, 189)(255, 255)G(0, 0)(37, 19)(70, 89)(150, 189)(255, 255)B(0, 0)(37, 19)(70, 89)(150, 189)(255, 255) @blend overlay e410_t.jpg 80",//410
		"@adjust saturation 0 @curve R(0, 0)(78, 83)(187, 199)(255, 255)G(0, 0)(42, 33)(119, 130)(255, 255)B(0, 0)(126, 125)(255, 255) @blend overlay e405_t.jpg 30",//411
		"@adjust saturation 0 @curve R(0, 0)(55, 42)(124, 144)(197, 185)(255, 255) @blend overlay e405_t.jpg 30",//412
		"@adjust saturation 0 @curve R(0, 0)(82, 91)(255, 255)G(0, 0)(119, 129)(255, 255)B(0, 0)(122, 102)(255, 255) @blend overlay e403_t.jpg 100",//413
		"@adjust saturation 0 @curve R(0, 0)(69, 78)(150, 202)(255, 255)G(0, 0)(98, 143)(255, 255) @blend overlay e402_t.jpg 80",//414

		//Face effect -- from 415 to 438

		"@curve R(0, 0)(71, 74)(164, 165)(255, 255) @pixblend screen 0.94118 0.29 0.29 1 20"   ,//415
		"@curve G(0, 0)(144, 166)(255, 255) @pixblend screen 0.94118 0.29 0.29 1 20"   ,//416
		"@curve B(0, 0)(68, 72)(149, 184)(255, 255) @pixblend screen 0.94118 0.29 0.29 1 20"   ,//417
		"@curve R(0, 0)(71, 74)(164, 165)(255, 255) @pixblend overlay 0.357 0.863 0.882 1 40"   ,//418
		"@curve R(0, 0)(96, 61)(154, 177)(255, 255) @pixblend overlay 0.357 0.863 0.882 1 40"   ,//419
		"@curve R(0, 0)(152, 183)(255, 255)G(0, 0)(161, 133)(255, 255) @pixblend overlay 0.357 0.863 0.882 1 40"   ,//420
		"@curve R(0, 0)(149, 145)(255, 255)G(0, 0)(149, 145)(255, 255)B(0, 0)(149, 145)(255, 255) @pixblend colordodge 0.937 0.482 0.835 1 20",//421
		"@curve G(0, 0)(101, 127)(255, 255) @pixblend colordodge 0.937 0.482 0.835 1 20"   ,//422
		"@curve B(0, 0)(70, 87)(140, 191)(255, 255) @pixblend pinlight 0.247 0.49 0.894 1 20"   ,//423
		"@curve R(0, 0)(53, 28)(172, 203)(255, 255) @blend softlight e424_t.jpg 20"   ,//424
		"@adjust saturation 0.7 @pixblend screen 0.8112 0.243 1 1 40"   ,//425
		"@adjust saturation 0.7 @pixblend screen 1 0.243 0.69 1 30"   ,//426

		//////////////////////////////////////////////////////////////////////////
		"@blend vvl e427_t.jpg 30",//427
		"@blend screen e428_t.jpg 60",//428
		"@blend overlay e429_t.jpg 30",//429
		"@blend screen e430_t.jpg 60",//430
		"@blend hardlight e431_t.jpg 40",//431
		"@blend softlight e432_t.jpg 60",//432
		"@adjust saturation 0 @pixblend overlay 0.957 0.58 1 1 30",//433
		"@adjust saturation 0 @pixblend overlay 0.933 0.518 0.239 1 40",//434
		"@adjust saturation 0.2 @curve R(0, 0)(87, 66)(142, 160)(255, 255)G(0, 0)(87, 66)(142, 160)(255, 255)B(0, 0)(87, 66)(142, 160)(255, 255)",//435
		"@adjust saturation 0 @curve R(0, 0)(75, 46)(155, 179)(255, 255)G(0, 0)(75, 46)(155, 179)(255, 255)B(0, 0)(75, 46)(155, 179)(255, 255)",//436
		"@adjust saturation 0 @curve R(0, 0)(108, 85)(163, 162)(255, 255)G(0, 0)(108, 85)(163, 162)(255, 255)B(0, 0)(108, 85)(163, 162)(255, 255)",//437
		"@adjust saturation 0 @curve R(0, 0)(113, 133)(255, 255)G(0, 0)(113, 133)(255, 255)B(0, 0)(113, 133)(255, 255)",//438

		//PES effect -- from 439 to 458
		"@curve R(0, 0)(73, 100)(201, 207)(255, 255)G(0, 0)(73, 100)(201, 207)(255, 255)B(0, 0)(73, 100)(201, 207)(255, 255)@blend overlay e439_t1.jpg 100 @pixblend softlight 0.682353 0.64314 0.2 1 100",//439
		"@blend overlay e440_t1.jpg 100 @pixblend multiply 0.48235 0.8117647 0.615686 1 39",//440
		"@pixblend multiply 0.68235 0.64314 0.2 1 39 @blend softlight e441_t1.jpg 100 @curve R(0, 0)(67, 90)(255, 255)G(0, 0)(67, 90)(255, 255)B(0, 0)(67, 90)(255, 255)",//441
		"@curve R(0, 0)(87, 56)(255, 255)G(0, 0)(87, 56)(255, 255)B(0, 0)(87, 56)(255, 255) @pixblend screen 0.44314 0.11765 0.4235 1 52 @blend softlight e442_t1.jpg 100",//442
		"@curve R(0, 0)(49, 72)(177, 170)(255, 255)G(0, 0)(49, 72)(177, 170)(255, 255)B(0, 0)(49, 72)(177, 170)(255, 255) @pixblend multiply 0.61569 0.862745 0.2 1 39 @blend softlight e443_t1.jpg 78",//443
		"@curve R(0, 0)(48, 89)(157, 184)(255, 255)G(0, 0)(48, 89)(157, 184)(255, 255)B(0, 0)(48, 89)(157, 184)(255, 255) @blend softlight e444_t1.jpg 100 @pixblend softlight 0.82353 0.2784 0.047 1 39",//444
		"@curve R(0, 0)(54, 81)(179, 196)(255, 255)G(0, 0)(54, 81)(179, 196)(255, 255)B(0, 0)(54, 81)(179, 196)(255, 255) @pixblend color 0.01176 0.23529 0.57647 1 19 @blend multiply e445_t1.jpg 50",//445
		"@curve R(0, 0)(84, 109)(255, 255)G(0, 0)(84, 109)(255, 255)B(0, 0)(84, 109)(255, 255) @pixblend colorburn 0.99216 0.3647 0.02745 1 28",//446
		"@curve R(0, 0)(45, 66)(187, 190)(255, 255)G(0, 0)(45, 66)(187, 190)(255, 255)B(0, 0)(45, 66)(187, 190)(255, 255) @blend color e447_t1.jpg 100 @pixblend lighten 0.02353 0.945 0.95686 1 39",//447
		"@curve R(0, 0)(74, 89)(178, 182)(255, 255)G(0, 0)(74, 89)(178, 182)(255, 255)B(0, 0)(74, 89)(178, 182)(255, 255) @blend softlight e448_t1.jpg 100 @pixblend linearburn 0.76078 0.68235 0.41176 1 39",//448
		//////////////////////////////////////////////////////////////////////////
		"@blend softlight e449_t1.jpg 100 @pixblend hardlight 0.6784 0.82745 0.4235 1 49",//449
		"@curve R(0, 0)(57, 62)(255, 255)G(0, 0)(57, 62)(255, 255)B(0, 0)(57, 62)(255, 255) @blend multiply e449_t1.jpg 52 @pixblend softlight 1 1 0 1 38 ",//450
		"@pixblend softlight 0.89 0.1647 0.77647 1 39 @blend softlight e451_t1.jpg 100 @curve R(0, 0)(199, 179)(255, 255)G(0, 0)(199, 179)(255, 255)B(0, 0)(199, 179)(255, 255)",//451
		"@curve R(0, 0)(92, 99)(188, 167)(255, 255)G(0, 0)(92, 99)(188, 167)(255, 255)B(0, 0)(92, 99)(188, 167)(255, 255) @blend overlay e452_t1.jpg 61 @pixblend screen 0 0.937255 0.9882 1 34 ",//452
		"@curve R(0, 0)(97, 107)(203, 190)(255, 255)G(0, 0)(97, 107)(203, 190)(255, 255)B(0, 0)(97, 107)(203, 190)(255, 255) @blend softlight e453_t1.jpg 100 @pixblend pinlight 0.9882 0.847 0.172549 1 29 ",//453
		"@curve R(0, 0)(77, 113)(202, 194)(255, 255)G(0, 0)(77, 113)(202, 194)(255, 255)B(0, 0)(77, 113)(202, 194)(255, 255) @pixblend softlight 0 0.4 1 1 39 @blend softlight e454_t1.jpg 56 ",//454
		"@curve R(0, 0)(81, 108)(177, 190)(255, 255)G(0, 0)(81, 108)(177, 190)(255, 255)B(0, 0)(81, 108)(177, 190)(255, 255) @pixblend multiply 0.6117647 0.988235 0.15686 1 32 @blend softlight e455_t1.jpg 100 ",//455
		"@curve R(0, 0)(69, 84)(173, 152)(255, 255)G(0, 0)(69, 84)(173, 152)(255, 255)B(0, 0)(69, 84)(173, 152)(255, 255) @pixblend colordodge 0.39216 0.3451 0.00784 1 54 @blend softlight e456_t1.jpg 100 ",//456
		"@curve R(0, 0)(59, 85)(179, 203)(255, 255)G(0, 0)(59, 85)(179, 203)(255, 255)B(0, 0)(59, 85)(179, 203)(255, 255) @pixblend softlight 0.8353 0.43137 0.0706 1 75 @blend softlight e457_t1.jpg 81 ",//457
		"@pixblend softlight 0.7294 0.772549 0.0588 1 100 @blend softlight e458_t1.jpg 76 ",//458
		"@adjust saturation 0 @curve R(0, 0)(80, 54)(138, 175)(255, 255) @blend overlay e402_t.jpg 80 " ,//459

		"@adjust saturation 0 @curve R(0, 0)(37, 138)(200, 124)(255, 255) @blend overlay e460_t.jpg 80 ", //460
		"@adjust saturation 0 @curve R(0, 0)(255, 255)G(0, 0)(71, 133)(163, 82)(255, 255)B(0, 0)(181, 146)(255, 255) @blend overlay e461_t.jpg 100 ", //461
		"@adjust saturation 0 @curve G(0, 0)(50, 105)(201, 138)(255, 255)B(0, 0)(142, 209)(255, 255) @blend overlay e462_t.jpg 30 ", //462
		"@adjust saturation 0 @curve B(0, 0)(34, 170)(187, 88)(255, 255) @blend overlay e463_t.jpg 100 ", //463
		"@adjust saturation 0@curve R(0, 0)(106, 191)(255, 255)G(0, 0)(255, 255)B(0, 0)(34, 170)(189, 76)(255, 255) @blend overlay e464_t.jpg 100 ", //464
		"@adjust saturation 0 @curve R(0, 0)(124, 58)(255, 255)G(0, 0)(91, 125)(255, 255)B(0, 0)(34, 170)(189, 76)(255, 255) @blend overlay e465_t.jpg 60 ", //465
		"@adjust saturation 0 @curve R(0, 0)(35, 117)(100, 255)(255, 255)G(0, 0)(255, 255)B(0, 0)(65, 101)(199, 40)(255, 255) @blend overlay e460_t.jpg 80 ", //466
		"@adjust saturation 0 @curve R(0, 0)(69, 217)(255, 255)G(0, 0)(255, 255)B(0, 0)(57, 200)(199, 89)(255, 255) @blend overlay e461_t.jpg 100 ", //467
		"@adjust saturation 0 @curve R(0, 0)(101, 36)(152, 244)(255, 255)G(0, 0)(255, 255)B(0, 0)(57, 200)(199, 89)(255, 255) @blend overlay e465_t.jpg 60 ", //468
		"@adjust saturation 0 @curve R(0, 0)(48, 172)(171, 89)(255, 255)G(0, 0)(255, 255)B(0, 0)(82, 34)(117, 237)(255, 255) @blend overlay e462_t.jpg 30 ", //469
		"@adjust saturation 0 @curve R(0, 0)(48, 172)(171, 89)(255, 255)G(0, 0)(89, 160)(181, 224)(255, 255)B(0, 0)(82, 34)(117, 237)(255, 255) @blend overlay e460_t.jpg 80 ", //470
		"@adjust saturation 0 @curve R(0, 0)(35, 109)(179, 206)(255, 255)G(0, 0)(58, 111)(194, 157)(255, 255)B(0, 0)(105, 19)(140, 192)(255, 255) @blend overlay e460_t.jpg 80 ", //471
		"@blend overlay e472_t.png 100 ",    //472
		"@blend overlay e473_t.png 100",    //473
		"@blend overlay e474_t.png 90",    //474
		"@blend overlay e475_t.png 90",    //475
		"@blend screen e476_t.png 80",    //476
		"@krblend screen e477_t.png 90",    //477
		"@krblend lineardodge e478_t.png 80",    //478
		"@blend overlay e479_t.png 80",    //479
		"@blend linearburn e480_t.png 100",    //480
		"@blend overlay e481_t.png 100",    //481
		"@unavailable", //482
		"@unavailable", //483
		"@unavailable", //484
		"@unavailable", //485
		"@unavailable", //486
		"@unavailable", //487
		"@unavailable", //488
		"@unavailable", //489
		"@unavailable", //490
		"@unavailable", //491
		"@unavailable", //492
		"@unavailable", //493
		"@krblend overlay e494_t.jpg 80", //494
		"@krblend screen e495_t.jpg 80", //495
		"@krblend overlay e496_t.jpg 60", //496
		"@krblend overlay e497_t.jpg 60", //497
		"@krblend overlay e498_t.jpg 100", //498
		"@krblend overlay e499_t.jpg 100", //499

		////////////////  新添加 2014-4-10   /////////////////////

		"@adjust saturation 0 @blend ol e500_t.jpg 100", //500
		"@adjust saturation 0 @blend ol e501_t.jpg 100", //501
		"@adjust saturation 0 @blend ol e502_t.jpg 100", //502
		"@adjust saturation 0 @curve RGB(0, 0)(108, 85)(163,162)(255,255) @blend ol e460_t.jpg 80", // 503
		"@adjust saturation 0 @curve RGB(0,0)(66,89)(188,231)(255,255) @blend ol e462_t.jpg 30", // 504
		"@adjust saturation 0 @curve RGB(63,0)(98,139)(167,255)", //505

		"@adjust saturation 0 @curve R(0, 0)(44, 25)(83, 73)(108, 114)(132, 159)(150, 196)(171, 288)(255, 255)G(0, 0)(42, 18)(69, 37)(114, 98)(145, 164)(197, 231)(255, 255)B(79, 0)(150, 109)(206, 199)(227, 233)(255, 255) @pixblend color 0.9137 0.823529 0.6157 1 100 @blend sl e506_t.jpg 60", //506
		"@curve R(4, 22)(18, 47)(45, 91)(62, 118)(107, 167)(185, 200)(255, 255)G(10, 10)(57, 60)(93, 105)(123, 151)(167, 206)(227, 243)B(0, 32)(67, 73)(161, 133)(211, 187)(237, 233)(253, 255) @blend ol e507_t.jpg 60", //507
		"@adjust saturation 0 @curve R(0, 0)(79, 31)(122, 75)(161, 140)(207, 222)(243, 255)G(0, 0)(74, 29)(119, 78)(154, 131)(162, 146)(208, 220)(216, 230)(255, 255)B(0, 0)(16, 4)(65, 32)(106, 84)(149, 127)(187, 165)(213, 213)(241, 255) @pixblend ol 0.8588 0.85 0.796 1 60 @blend ol e508_t.jpg 80", //508
		"@pixblend color 0.4196 0.3686 0.294 1 100 @vigblend ol 0 0 0 1 20 0 0.5 0.5 0.5 3 @ blend ol e509_t.jpg 60", //509 //"@pixblend color 0.4196 0.3686 0.294 1 100 @krblend ol vtg410.png 20 @ blend ol e509_t.jpg 60", //509
		"@adjust saturation 1.05 @blend ol e510_t.jpg 100", //510
		"@pixblend ol 0.74118 0.6549 0.549 1 100 @blend ol e511_t.jpg 80", //511
		"@curve R(0, 0)(71, 74)(164, 165)(255, 255) @pixblend ol 0.3569 0.862745 0.88235 1 40", //512
		"@curve G(0, 0)(101, 127)(255, 255) @pixblend cd 0.937 0.48235 0.835 1 40", //513
		"@adjust saturation 0.7 @pixblend sl 0.81176 0.243 1 1 60", //514
		"@blend ol e429_t.jpg 30", //515
		"@blend screen e430_t.jpg 60", //516
		"@blend sl e432_t.jpg 40", //517
		"@pixblend ol 0.17255 0.596 0.1843 1 60 @blend lt e518_t.png 100", //518
		"@pixblend ol 0.00784 0.82745 0.76 1 60 @blend hl e519_t.png 60", //519
		"@pixblend ol 0.588 0.37647 0.7647 1 60 @blend cl e520_t.png 100", //520
		"@pixblend ol 0.54118 0.6549 0.252745 1 100 @blend cd e521_t.jpg 100", //521
		"@pixblend ol 0.12549 0.66667 0.7451 1 100 @blend sr e522_t.jpg 100", //522
		"@pixblend ol 0.90196 0.8549 0.3098 1 60 @blend ol e523_t.png 100", //523
		"@pixblend sl 0.9176 0.702 0.36863 1 100 @blend sl e524_t.jpg 100 @vigblend ol 0 0 0 1 100 0 0.5 0.5 0.5 3", //524 //"@pixblend sl 0.9176 0.702 0.36863 1 100 @blend sl e524_t.jpg 100 @krblend ol vtg410.png 100", //524
		"@pixblend sl 77 103 234 255 100 @blend sl e524_t.jpg 100 @vigblend ol 0 0 0 1 100 0 0.5 0.5 0.5 3", //525 //"@pixblend sl 77 103 234 255 100 @blend sl e524_t.jpg 100 @krblend ol vtg410.png 100", //525
		"@pixblend sl 100 212 225 255 100 @blend sl e526_t.jpg 100 @vigblend ol 0 0 0 1 100 0 0.5 0.5 0.5 3", //526 //"@pixblend sl 100 212 225 255 100 @blend sl e526_t.jpg 100 @krblend ol vtg410.png 100", //526
		"@pixblend sl 246 99 241 255 100 @blend sl e526_t.jpg 100 @vigblend ol 0 0 0 1 100 0 0.5 0.5 0.5 3", //527 //"@pixblend sl 246 99 241 255 100 @blend sl e526_t.jpg 100 @krblend ol vtg410.png 100", //527
		"@pixblend sl 241 226 95 255 100 @blend sl e528_t.jpg 100 @vigblend ol 0 0 0 1 40 0 0.5 0.5 0.5 3", //528 //"@pixblend sl 241 226 95 255 100 @blend sl e528_t.jpg 100 @krblend ol vtg410.png 40", //528
		"@pixblend sl 241 164 95 255 100 @blend sl e529_t.jpg 100 @vigblend ol 0 0 0 1 60 0 0.5 0.5 0.5 3", //529 //"@pixblend sl 241 164 95 255 100 @blend sl e529_t.jpg 100 @krblend ol vtg410.png 60", //529

		"@adjust saturation 0.2 @blend ol e530_t.jpg 100 @vigblend ol 0 0 0 1 60 0 0.5 0.5 0.5 3", //530 //"@adjust saturation 0.2 @blend ol e530_t.jpg 100 @krblend ol vtg410.png 60", //530
		"@adjust saturation 0 @pixblend pl 152 142 175 255 100 @blend ol e531_t.jpg 100 @vigblend ol 0 0 0 1 60 0 0.5 0.5 0.5 3", //531 //"@adjust saturation 0 @pixblend pl 152 142 175 255 100 @blend ol e531_t.jpg 100 @krblend ol vtg410.png 60", //531
		"@adjust saturation 0 @pixblend mp 28 108 131 255 40 @blend ol e532_t.jpg 100", //532
		"@adjust saturation 0 @pixblend ol 115 130 100 255 80 @blend mp e533_t.jpg 100", //533
		"@adjust saturation 0.2 @blend ol e534_t.jpg 100 @vigblend ol 0 0 0 1 40 0 0.5 0.5 0.5 3", //534 //"@adjust saturation 0.2 @blend ol e534_t.jpg 100 @krblend ol vtg410.png 40", //534
		"@adjust saturation 0 @blend ol e535_t.jpg 80@vigblend ol 0 0 0 1 60 0 0.5 0.5 0.5 3", //535 //"@adjust saturation 0 @blend ol e535_t.jpg 80 @krblend ol vtg410.png 60", //535

		"@pixblend sl 85 180 16 255 100 @vigblend ol 0 0 0 1 20 0 0.5 0.5 0.5 3", //536 //"@pixblend sl 85 180 16 255 100 @krblend ol vtg410.png 20", //536
		"@pixblend sl 255 90 0 255 100 @vigblend ol 0 0 0 1 20 0 0.5 0.5 0.5 3", //537 //"@pixblend sl 255 90 0 255 100 @krblend ol vtg410.png 20", //537
		"@adjust saturation 0.4 @pixblend sl 234 201 45 255 100 @vigblend ol 0 0 0 1 20 0 0.5 0.5 0.5 3", //538 //"@adjust saturation 0.4 @pixblend sl 234 201 45 255 100 @krblend ol vtg410.png 20", //538
		"@adjust saturation 0.4 @pixblend sl 106 173 215 255 100 @vigblend ol 0 0 0 1 20 0 0.5 0.5 0.5 3", //539 //"@adjust saturation 0.4 @pixblend sl 106 173 215 255 100 @krblend ol vtg410.png 20", //539

		////////////////  新添加 2014-4-10   /////////////////////
		////////////////  ↓ 新添加 2014-11-13  /////////////////////

		// Film

		"@selcolor yellow(-87, 15, 34, 16) green(-45, 0, -47, 33) cyan(39, -26, 60, 0) blue(75, 23, 39, 0) white(39, -25, 28, 19) gray(2, 0, 0, -17) black(2, 2, 2, -6) @curve rgb(0, 0)(31, 33)(69, 64)(116, 91)(172, 153)(213, 202)(255, 255) @tileblend ol film_tile.jpg 100 @adjust saturation 0 @blend ol e540_t.jpg 60", //540

		"@blend sl e540_t.jpg 80 @selcolor yellow(-51, 15, 0, 38) green(-45, 0, -47, 33) cyan(-16, -20, 67, 0) white(38, 5, 36, -18) gray(25, -4, 0, -3) black(27, 12, 8, 5) @tileblend sl film_tile.jpg 60", //541

		"@blend ol e540_t.jpg 60 @curve rgb(0, 13)(38, 36)(217, 211)(255, 255)g(0, 14)(135, 134)(205, 207)(255, 255)b(21, 0)(205, 203)(255, 255) @selcolor red(-12, -32, 7, 20) yellow(-22 20 -26 0) cyan(-22 -21 40 40) white(0 -19 12 12) gray(3 4 -1 2) black(-10 0 -12 1) @adjust brightness 0.05 @adjust contrast 1.08 @adjust hsl -0.02 -0.27 0 @selcolor white(-5 -26 -8 -36) gray(6 -2 -8 -10) black(0 0 0 -0.01) @pixblend ol 138 126 100 255 73 @tileblend ol film_tile.jpg 75", //542

		"@adjust brightness 0.05 @adjust contrast 1.07 @selcolor red(-20 0 0 0) cyan(-86 -29 38 33) white(38 -8 -55 33) gray(0 0 0 -13) black(-6 7 18 -5) @curve r(0, 0)(108, 106)(197, 210)(255, 255)g(0, 0)(197, 196)(255, 255)b(0, 0)(207, 191)(255, 255) @tileblend sl film_tile.jpg 70", //543

		"@pixblend sl 235 203 94 255 30 @selcolor white(11 8 13 0)  gray(21 15 16 -13) black(6 -12 23 23) @adjust brightness  0.11 @adjust contrast 1.18 @tileblend sl film_tile.jpg 60  @blend ol e554_t.jpg 60 @pixblend lighten 39 61 64 255 60", //544

		"@selcolor white(-46 32 75 -15) cyan(-32 0 38 0) black(0 0 0 5) @adjust brightness 0.04 @adjust contrast 1.05 @tileblend sl film_tile.jpg 100 @blend ol e540_t.jpg 60", //545

		"@selcolor white(-9 4 22 60) black(34 -4 28 0) @adjust brightness 0.03 @adjust contrast 0.85 @tileblend sl film_tile.jpg 50 @blend ol e540_t.jpg 60", //546

		"@adjust saturation 0.35 @selcolor yellow(-41 25 10 0) cyan(28 0 61 61) white(0 -20 -23 -15) gray(0 0 0 -11) black(8 0 2 8) @adjust brightness -0.11 @adjust contrast 1.13 @curve rgb(0, 52)(255, 255) @tileblend sl film_tile.jpg 20 @blend ol e540_t.jpg 50", //547

		"@blend ol e540_t.jpg 50 @curve rgb(0, 6)(49, 44)(75, 72)(217, 211)(255, 255)g(0, 14)(255, 255)b(5, 0)(255, 255) @selcolor red(-12 -32 7 20) yellow(-20 35 80 0) white(-30 -23 100 -42) gray(-20 -9 8 4) black(-12 9 85 10) @adjust brightness 0.07 @adjust contrast 1.03 @adjust saturation 0.67 @tileblend sl film_tile.jpg 50", //548

		"@blend ol e554_t.jpg 50 @curve rgb(0, 6)(49, 44)(75, 72)(217, 211)(255, 255)g(0, 14)(12, 22)(158, 150)(205, 193)(255, 255)b(21, 0)(205, 203)(249, 253) @selcolor red(-21 -46 30 -8) yellow(-22 80 -26 0) cyan(-22 -21 40 40) white(0 20 10 13) gray(-5 -3 8 -13) black(0 10 -10 -5) @adjust brightness 0.06 @adjust contrast 1.11 @adjust saturation 0.84 @tileblend sl film_tile.jpg 50", //549

		"@blend ol e540_t.jpg 50 @selcolor white(-40 19 36 -18) gray(25 0 0 0) black(-12 -2 -4 0) @tileblend sl film_tile.jpg 50", //550

		"@blend ol e540_t.jpg 40 @curve rgb(0, 0)(38, 36)(125, 133)(211, 213)(255, 255)g(0, 14)(12, 22)(135, 134)(255, 255)b(21, 0)(205, 203)(249, 253) @selcolor red(-12 -32 7 20) yellow(-22 20 -26 0) cyan(-22 -21 40 40) white(38 -22 49 15) gray(2 8 -7 -1) black(-4 8 -21 4) @adjust brightness 0.07 @adjust contrast 1.19 @adjust hsl -0.02 -0.27 0 @selcolor yellow(0 0 26 48) white(-8 -23 -44 -7) gray(6 -2 -8 -10) @pixblend ol 138 126 100 255 73 @tileblend sl film_tile.jpg 20", //551

		"@blend ol e540_t.jpg 50 @curve rgb(0, 0)(38, 36)(125, 133)(211, 213)(255, 255)g(0, 14)(12, 22)(135, 134)(255, 255)b(21, 0)(205, 203)(249, 253) @selcolor red(-12 -32 7 20) yellow(-22 20 -26 0) cyan(-22 -21 40 40) white(38 -22 49 15) gray(2 8 -7 -1) black(-4 8 -21 4) @adjust brightness 0.08 @adjust contrast 1.06 @adjust hsl -0.02 -0.27 0 @selcolor yellow(0 0 26 48) white(-8 -23 44 -7) gray(6 -2 -8 -10) @tileblend sl film_tile.jpg 50", //552

		"@blend ol e540_t.jpg 20 @tileblend sl film_tile.jpg 20 @selcolor red(-20, -21, -19, 10) white(-9, 0, 0, 27) gray(0, 0, 0, 12) black(13, -8, 0, 34) @adjust brightness 0.13 @adjust contrast 1.2 @selcolor black(0, -7, 0, 0) @pixblend lighten 23 40 34 255 60", //553

		"@curve rgb(0, 8)(30, 38)(63, 63)(151, 159)(208, 203)(255, 255)r(0, 0)(64, 66)(133, 133)(192, 194)(255, 255)g(0, 0)(11, 22)(47, 48)(71, 71)(122, 123)(181, 184)(255, 255)b(0, 0)(18, 17)(39, 41)(67, 66)(118, 118)(171, 169)(191, 192)(203, 212) @pixblend ol 123 115 105 255 100 @blend sl e554_t.jpg 68", //554

		//Christmas
		"@curve RGB(0, 0)(197, 218)(255, 255)R(0, 25)(142, 144)(239, 255)B(0, 0)(185, 176)(255, 255) @blend ol e555_t2.png 40 @blend sr e555_t1.jpg 100", //555
		"@curve RGB(0, 0)(47, 75)(111, 155)(197, 218)(255, 255)R(0, 25)(142, 144)(239, 255)B(0, 0)(185, 176)(255, 255) @blend ol e555_t2.png 40 @blend sr e556_t1.png 100", //556
		"@blend sr e557_t1.jpg 100 @blend ol e555_t2.png 40 @adjust saturation 1.12 @selcolor red(0, 4, 0, 0) gray(-5, -9, 4, 0) black(0, 4, -5, 0) @adjust brightness -0.09 @adjust contrast 1.24", //557
		"@pixblend sr 178 110 13 255 30 @blend ol e555_t2.png 40 @blend sr e558_t1.png 100", //558
		"@blend lt e559_t1.jpg 100 @selcolor red(-30, 15, 46, 0) yellow(-25, 25, 13, 0) gray(-10, -12, 8, -2) black(-1, -3, -3, 0) @blend ol e555_t2.png 40", //559
		"@selcolor blue(90, -52, 59, 16) white(-65, -41, -28, 3) black(-19, -5, 20, 0) @blend sr e560_t1.jpg 100 @blend ol e555_t2.png 40", //560
		"@selcolor yellow(-6, 40, 0, 0) white(71, -39, 24, 0) black(14, 14, -1, 0) @blend sr e561_t.png 100 @pixblend sl 191 87 26 255 30", //561

		////////////////  ↑ 新添加 2014-11-13  /////////////////////
		////////////////  ↓ film补充 2014-11-18  /////////////////////

		"@blend ol e562_t.jpg 48 @curve rgb(0, 13)(12, 22)(44, 38)(64, 56)(119, 110)(168, 158)(255, 255)r(0, 0)(34, 51)(127, 128)(255, 255) @adjust contrast 1.09 @adjust saturation 0.74", //562

		"@adjust saturation 0.61 @adjust level 0.1137 0.9451 1.22 @blend sl e554_t.jpg 100 @blend sl e563_t.jpg 100 @adjust contrast 1.1 @curve rgb(0, 0)(12, 19)(63, 63)(92, 92)(189, 183)(255, 255)", //563

		////////////////  ↑ film补充 2014-11-18  一共两个/////////////////////
		////////////////  ↓ 新添加 bw 2014-12-3 从564到582一共十九个 /////////////////////

		"@krblend softlight e564To582_t1.jpg 75 @adjust saturation 0 @vigblend lighten 59 59 59 255 100 0 0 0.5 0.5 3 @krblend multiply e564To582_t2.jpg 100", // 564
		"@krblend overlay e564To582_t1.jpg 100 @adjust contrast 1.12 @adjust saturation 0 ", //565
		"@krblend softlight e564To582_t3.jpg 100 @adjust saturation 0 @vigblend lighten 53 47 49 255 100 0 0 0.5 0.5 3 @vigblend pinlight 53 47 49 255 12 0 0 0.5 0.5 3 ", //566
		"@adjust saturation 0 @selcolor white(-14, -9, 8, -16) black(-7, 1, 3, 0) @vigblend softlight 171 171 171 255 100 0 0 0.5 0.5 3 ", //567
		"@adjust saturation 0 @curve RGB(14, 0)(255, 240) @adjust contrast 1.24 @krblend overlay e564To582_t1.jpg 54 @vigblend overlay 223 191 152 255 20 0 0 0.5 0.5 3 ", //568
		"@adjust saturation 0 @adjust contrast 1.24 @selcolor white(-19, 4, 11, 15) gray(2, -1, -4, 0) black(0, 6, -1, 0) @krblend overlay e564To582_t3.jpg 54 ", //569
		"@selcolor white(39, -25, 28, 19) gray(2, 0, 0, -17) black(2, 2, 2, -6) @curve RGB(0, 0)(31, 33)(69, 64)(118, 89)(172, 153)(213, 202)(255, 255) @adjust saturation 0 @krblend multiply e564To582_t2.jpg 100 @krblend softlight e564To582_t1.jpg 60 ", //570
		"@krblend overlay e564To582_t4.jpg 100 @adjust saturation 0 @selcolor white(-12, -2, -1, 0) gray(2, -2, 0, 20) black(6, -6, 0, 3) ", //571
		"@adjust contrast 1.08 @selcolor black(0, 3, 5, 1) @adjust saturation 0 ", //572
		"@adjust saturation 0 @selcolor white(-10, -14, 11, 5) gray(4, 5, 10, 13) black(4, 6, 7, 2) ", //573
		"@curve R(0, 0)(160, 92)(255, 255)B(0, 0)(149, 92)(255, 255) @adjust saturation 0 ", //574
		"@adjust saturation 0 @selcolor white(0, -11, 11, 5) gray(0, 6, 6, 5) black(-1, 2, -5, 0) ", //575
		"@selcolor red(0, 0, 0, -100) yellow(0, 0, 0, -100) @adjust saturation 0 @adjust contrast 1.08 ", //576
		"@selcolor green(100, 100, 100, 100) blue(100, 100, 100, 100) cyan(100, 100, 100, 100) magenta(100, 100, 100, 100) @adjust saturation 0 ", //577
		"@krblend multiply e564To582_t5.jpg 100 @adjust saturation 0 @adjust contrast 1.2 @vigblend color 232 217 186 255 33 0 0 0.5 0.5 3 ", //578
		"@vigblend color 255 0 0 255 100 0 0 0.5 0.5 3 @adjust saturation 0 @adjust contrast 1.84 ", //579
		"@adjust saturation 0 @vigblend multiply 149 90 50 255 20 0 0 0.5 0.5 3 ", //580
		"@krblend multiply e564To582_t2.jpg 100 @adjust saturation 0 @selcolor black(0, 0, 0, -8) @adjust contrast 1.4 ", //581
		"@adjust saturation 0 @vigblend multiply 0 0 0 255 100 0.24 0.97 0.5 0.5 3 @selcolor black(0, 0, 0, 8) ", //582

		//////////////// ↑ 新添加 bw 2014-12-3 从564到582 一共十九个 /////////////////////
		//////////////// ↓ 新添加 Xmas 2014-12-3 从583到593 一共十一个 ///////////////////

		"@krblend screen e583To593_t17.jpg 52 @selcolor red(0, 0, 0, 5) white(22, -52, 0, 0) @vigblend softlight 0 0 0 255 100 0.5 0.32 0.5 0.5 3 @adjust contrast 1.12 ", //583
		"@krblend softlight e583To593_t16.jpg 52 @vigblend softlight 0 0 0 255 100 0.42 0.33 0.5 0.5 3 @adjust contrast 1.24 ", //584
		"@krblend softlight e583To593_t15.jpg 100 @vigblend softlight 0 0 0 255 100 0.51 0.26 0.5 0.5 3 @adjust contrast 1.24 ", //585
		"@vigblend softlight 0 0 0 255 100 0.52 0.29 0.5 0.5 3 @krblend softlight e583To593_t14.jpg 61 @selcolor white(57, 0, 25, 12) gray(-1, -9, 0, 0) black(-4, -7, 29, 13) @adjust contrast 1.24 ", //586
		"@krblend softlight e583To593_t13.jpg 100 @selcolor red(-5, 0, 0, 0) gray(0, 0, 0, -19) black(-4, 9, -13, -10) @vigblend softlight 0 0 0 255 50 0.49 0.21 0.5 0.5 3 @adjust contrast 1.2 ", //587
		"@krblend hardlight e583To593_t12.jpg 100 @selcolor red(10, 0, 0, 0) green(-19, 0, -31, 0) white(15, -35, 49, 0) gray(-8, -18, 0, 0) black(1, -27, 0, 0) @vigblend softlight 0 0 0 255 100 0.5 0.42 0.5 0.5 3 @tileblend screen e583To593_t9_tile.jpg 60", //588
		"@krblend linearlight e583To593_t10.jpg 60 @selcolor black(-13, -2, -7, 12) @adjust brightness -0.12 @adjust contrast 1.12 ", //589
		"@vigblend softlight 203 148 0 255 30 0 0 0.5 0.5 3 @selcolor red(17, -19, 11, 13) yellow(36, 5, -7, 12) white(0, 0, -8, 0) gray(0, -2, -12, -29) black(-38, 29, -17, 20) @adjust contrast 1.08 @adjust brightness 0.07 @tileblend lighten e583To593_t11_tile.jpg 60", //590
		"@krblend softlight e583To593_t8.jpg 100 @adjust brightness 0.19 @adjust contrast 1.08 ", //591
		"@krblend softlight e583To593_t7.jpg 100 @adjust brightness -0.11 @adjust contrast 1.04 @vignette 0.54 1 ", //592
		"@krblend softlight e583To593_t6.jpg 100 ", //593

		//////////////// ↑ 新添加 Xmas 2014-12-3 从583到593 一共十一个 ///////////////////

		//////////////// ↓ 新添加 Tokyo 2014-12-3 从594到608 一共十一个 ///////////////////
		"@selcolor red(8, -42, 4, 0) white(34, 11, 30, -12) gray(-5, -8, -3, -18) black(-12, -20, -9, -8) @adjust contrast 1.24 @pixblend softlight 234 197 102 255 24 @selcolor gray(0, 0, 0, 20) ", //594
		"@selcolor red(-27, -42, -17, -16) green(-6, -58, 0, -13) yellow(1, 5, -11, 4) white(34, 11, 30, -12) gray(-5, -8, -3, -10) black(-22, -22, -20, -10) @adjust contrast 1.04 ", //595
		"@selcolor white(0, 0, 0, 67) @adjust contrast 1.08 @pixblend screen 80 56 52 255 67 ", //596
		"@selcolor yellow(-51, 15, 0, 38) white(38, 5, 36, -18) gray(25, -4, 0, -3) black(27, 12, 8, 5) @pixblend softlight 226 226 226 255 70 ", //597
		"@krblend screen e594To608_t1.jpg 100 ", //598
		"@pixblend screen 27 42 63 255 100 ", //599
		"@krblend screen e594To608_t2.jpg 100 @selcolor white(26, -6, 0, 0) gray(0, 0, 0, 25) black(0, 0, 0, 80) ", //600
		"@pixblend screen 26 35 34 255 26 @pixblend screen 127 131 104 255 35 @pixblend linearburn 229 225 216 255 100 ", //601
		"@selcolor red(54, 0, 1, -25) green(-43, 28, -87, -23) yellow(-20, 44, -61, -12) white(84, -51, 9, 0) black(-30, 30, -31, 11) ", //602
		"@adjust saturation 0.8 @selcolor red(81, 34, 30, 60) green(64, 63, 56, 54) yellow(54, 69, 64, 57) white(22, -12, 28, 40) gray(9, 7, 6, 6) black(0, 7, 13, -10) @adjust brightness -0.11 @adjust contrast 1.13 ", //603
		"@adjust saturation 0.8 @adjust brightness -0.11 @adjust contrast 1.12 @curve RGB(0, 53)(255, 232)R(0, 0)(144, 91)(255, 255)G(0, 0)(136, 108)(255, 255)B(0, 28)(255, 245) ", //604
		"@adjust saturation 0.8 @adjust brightness -0.11 @adjust contrast 1.12 ", //605
		"@pixblend overlay 110 72 125 255 51 @adjust saturation 0.71 @selcolor yellow(0, 0, 20, 0) white(66, -10, 0, 18) gray(5, 5, -5, 0) black(-15, 25, 25, 0) ", //606
		"@krblend lighten e594To608_t2.jpg 74 @selcolor gray(0, 0, 0, 4) black(0, 0, 0, 68) ", //607
		"@krblend lighten e594To608_t2.jpg 74 @pixblend overlay 82 103 78 255 65 @selcolor gray(0, 0, 0, 4) black(0, 0, 0, 68) ", //608

		//////////////// ↑ 新添加 Tokyo 2014-12-3 从594到608 一共十一个 ///////////////////

		//////////////// ↓ 新添加 Cinematic 2014-12-3 从609到610一共两个 ///////////////////

		"@curve R(1, 0)(51, 47)(134, 143)(185, 193)(255, 239)G(0, 0)(58, 48)(132, 139)(174, 187)(200, 210)(255, 255)B(0, 2)(64, 57)(108, 103)(199, 191)(255, 255) RGB(0, 0)(28, 52)(63, 112)(112, 169)(141, 191)(183, 206)(221, 238)(255, 255)   @selcolor red(10, -35, 39, 96) green(79, 96, -40, 58) blue(-2, -28, 73, 26) cyan(6, -37, -60, 0) yellow(99, 31, -5, 40) white(10, 60, 60, -70) gray(-4, 7, 20, -2) black(16, 20, -11, 62) @pixblend multiply 170 160 150 255 30", //609
		"@selcolor red(20, -10, 5, 4) green(-36, 40, -50, 80) blue(50, -42, 28, 4) cyan(68, 30, -34, -9) yellow(-80, -80, -80, 30) magenta(-30, -40, 2, -60) white(-50, -80, 11, -20) gray(-20, 15, -7, 10) black(0, 0, 0, 11)  @pixblend lighten 19 32 15 255 60 @pixblend mix 85 153 172 255 23 @pixblend multiply 222 199 223 255 25 @pixblend darken 199 217 203 255 20 @selcolor red(-20, 10, -2, 27)   yellow(-50, 60, -0, 0) magenta(78, -57, 53, 0) @tileblend sl film_tile.jpg 40", //610
		
		//////////////// ↑ 新添加 Cinematic 2014-12-3 从609到610一共两个 ///////////////////

		//////////////// ↓ 2015-2-15 占位 611-629 ///////////////////
		"", //611
		"", //612
		"", //613
		"", //614
		"", //615
		"", //616
		"", //617
		"", //618
		"", //619
		"", //620
		"", //621
		"", //622
		"", //623
		"", //624
		"", //625
		"", //626
		"", //627
		"", //628
		"", //629
		//////////////// ↑ 2015-2-15 占位 611-629 ///////////////////

		//////////////// ↓ 2015-2-15 Disposable 630-637 ///////////////////

		"@adjust brightness 0.03 @adjust saturation 0.55@curve RGB(0, 0)(73, 73)(138, 153)(255, 255)R(0, 0)(63,66)(189,172)(255, 255)G(0, 0)(61,58)(119,116)(255, 255)B(0, 0)(77,56)(169, 171)(154, 168)(255, 255) @blend overlay e630To637_t1.jpg 12 ", //630
		"@adjust saturation 0.89 @blend overlay e630To637_t1.jpg 30 @adjust contrast 0.98 @curve R(0, 16)(65, 100)(173, 178)(255, 255)G(0, 0)(56, 75)(255, 255)B(0, 0)(60, 60)(148, 196)(255, 255)RGB(0,10)(30, 28)(255, 255) @adjust brightness -0.09", //631
		"@curve R(0, 9)(255, 246)G(0, 4)(239, 255)B(0, 30)(37, 44)(136, 141)(230, 241)(255, 255) @adjust contrast 1.12 @adjust brightness 0.14 @adjust brightness 0.11 ", //632
		"", //633
		"@curve R(0, 0)(80, 74)(163, 161)(255, 255)G(0, 0)(86, 62)(177, 170)(255, 255)B(0, 0)(87, 59)(171, 182)(255, 255)RGB(0, 28)(88, 76)(185, 192)(255, 255) @blend overlay e630To637_t5.jpg 76@adjust contrast 1@adjust saturation 0.89 @adjust brightness 0.06", //634
		"@curve R(0, 0)(48, 50)(141, 153)(255, 255)G(0, 39)(38, 60)(109, 120)(239, 246)B(0, 9)(58, 35)(79, 106)(168, 210)(207, 255)RGB(0, 31)(165, 182)(255, 241) @adjust brightness -0.40 @adjust contrast 0.9 @adjust saturation 0.75 @blend overlay e630To637_t6.jpg 50 ", //635
		"@blend colordodge e630To637_t7.jpg 15 @curve RGB(0, 0)(58, 66)(106, 125)(255, 255)  @adjust saturation 1.18@adjust brightness 0.09 @adjust contrast 0.89@blend overlay e630To637_t1.jpg 30@adjust brightness -0.18", //636
		"", //637

		//////////////// ↑ 2015-2-15 Disposable 630-637 ///////////////////

		//////////////// ↓ 2015-2-15 Solar 638-643 ///////////////////

		"@krblend overlay e638To647_t16.jpg 80  @selcolor red(20, -1, 30, -9) yellow(23, 20, 20, 56) white(0, -80, 10, 0) gray(-10, 10, -10, 0) black(-10, 10, 10, 20) @blend screen e638To647_t3.jpg 100 ", //638
		"@adjust contrast 1.08 @selcolor red(0, 0, 0, 10) green(92, 0, 0, 100) blue(0, 0, 100, 100) cyan(0, 100, 100, 100) yellow(15, -20, 50, 16) white(-48, 15, 6, 90) gray(-3, -9, 0, 0) black(-11, 9, -3, 11) @krblend overlay e638To647_t16.jpg 100 @blend screen e638To647_t5.jpg 100 ", //639
		"@krblend overlay e638To647_t16.jpg 80 @pixblend multiply 244 232 190 255 100 @adjust hsl 0 0.28 0.25 @adjust shadowhighlight -137 -28 @blend screen e638To647_t6.jpg 100 ", //640
		"@blend overlay e638To647_t16.jpg 80 @selcolor blue(39, -47, 79, 5) yellow(-60, -20, 2, 9) white(-37, -62, 57, -40) black(4, 2, -6, 10) @pixblend darken 108 199 170 255 18 @pixblend overlay 108 199 170 255 18 @blend screen e638To647_t13.jpg 100 ", //641
		"@selcolor green(7, 0, 0, 0) cyan(41, 0, 0, 0) yellow(-45, 10, -14, -17) white(62, -17, -45, 38) black(1, 5, -3, 0) @pixblend darken 176 232 250 255 48 @pixblend overlay 176 232 250 255 29 @krblend screen e638To647_t9.jpg 100 @krblend overlay e638To647_t16.jpg 80", //642
		"@krblend overlay e638To647_t16.jpg 100 @pixblend multiply 249 236 196 255 70 @selcolor red(-20, -2, 10, 30) yellow(12, -10, -20, -50) black(4, 10, -20, 13) @blend screen e638To647_t14.jpg 100 ", //643

		//////////////// ↑ 2015-2-15 Solar 638-643 ///////////////////

		//////////////// ↓ 2015-2-15 Grayscale 645-653 ///////////////////

		"", //644
		"@colormul mat 1 0 0 1 0 0 1 0 0 @blend softlight e645To654_t4.jpg 15 @blend overlay e645To654_t2.jpg 30 @blend multiply e645To654_t6.jpg 100 @blend overlay e645To654_t8.jpg 80", //645
		"@colormul mat 0 1 0 0 1 0 0 1 0 @blend softlight e645To654_t4.jpg 15 @blend overlay e645To654_t3.jpg 30 @blend multiply e645To654_t11.jpg 40 @blend overlay e645To654_t6.jpg 50", //646
		"@colormul mat 0 0 1 0 0 1 0 0 1 @blend softlight e645To654_t4.jpg 15 @blend overlay e645To654_t3.jpg 30  @blend overlay e645To654_t12.jpg 30 @blend multiply e645To654_t6.jpg 30", //647
		"@colormul vec 1 1.4 0.7@adjust saturation 0  @blend overlay e645To654_t9.jpg 40 @blend overlay e645To654_t3.jpg 60 @tileblend softlight e645To654_t13.jpg 30 ", //648
		"", //649
		"", //650
		"@krblend overlay e645To654_t4.jpg 50 @adjust saturation 0 @selcolor white(-12, -2, -1, 0) gray(2, -2, 0, 20) black(6, -6, 0, 3) @adjust saturation 0.4  @krblend overlay e645To654_t2.jpg 20", //651
		"@adjust contrast 1.2 @adjust saturation 0 @selcolor white(4, -8, -30, -20) gray(2, 3, 1, 8) black(2, 1, 3, 8)  @krblend darken e645To654_t6.jpg 100 @krblend overlay e645To654_t6.jpg 50 ", //652
		"@krblend multiply e645To654_t5.jpg 60 @adjust saturation 0 @adjust contrast 1.2 @vigblend color 232 217 186 255 33 0 0 0.5 0.5 3 @adjust saturation 0.5 @blend overlay e645To654_t6.jpg 80", //653

		//////////////// ↑ 2015-2-15 Grayscale 645-653 ///////////////////

		"@beautify bilateral 100 5.1 4 @adjust saturation 0 @style edge 1 1.3 @curve RGB(0, 255)(255, 0) ", // for test
	};

	const int g_cgeEffectNum = sizeof(g_cgeEffectString) / sizeof(*g_cgeEffectString);

	const char* cgeGetEffectStringByID(int effectID)
	{
		if(effectID < 0 || effectID >= g_cgeEffectNum)
			return (const char*)(0);
		return g_cgeEffectString[effectID];
	}

}

//保存特效文件辅助代码
// #include <fstream>
// using namespace std;
// bool save2File()
// {
// 	for(int i = 583; i <= 593; ++i)
// 	{
// 		char buffer[1024];
// 		sprintf(buffer, "C:/Users/Administrator/Desktop/test1/effect.%d.v0_5_3_0.algorithm", i);
// 		fstream file(buffer, fstream::binary | fstream::out);
// 		file << CGEEffectsDataSet::cgeGetEffectStringByID(i);
// 		file.close();
// 	}
// 	return true;
// }
// 
// bool b = save2File();