#include "gif_decoder.h"
#include <string.h>
#include "stdbool.h"

#include "os_mem.h"
#include "os_timer.h"
#include "driver_flash.h"
#include "driver_gpio.h"
#include "co_printf.h"
#include "driver_system.h"
#include "os_timer.h"
#include "driver_timer.h"
#include "driver_st77903.h"
#include "sys_utils.h"

#define FREQCHIP_LOGO  1
#define KTXCHIP_LOGO   0

#if 1
#define GIF_MEM_SRC 1  //1 ram  2 psram

#if (GIF_MEM_SRC==1)
#define gif_free   os_free
#define gif_malloc os_malloc
#elif (GIF_MEM_SRC==2)
#define gif_free    lv_mem_free
#define gif_malloc  lv_mem_alloc
#endif

//#define   
//#define GIF_WIDTH_X 320
//#define GIF_HIGHT_Y 385
uint16_t GIF_WIDTH_X =400;
uint16_t GIF_HIGHT_Y =400;


#define GIF_PSRAM_ADDR  0x22180000

lv_img_dsc_t gif_img_param= { 
.header.always_zero = 0,
.header.reserved = 0,	
//.header.w = GIF_WIDTH_X,
//.header.h =GIF_HIGHT_Y,
//.data_size =(GIF_WIDTH_X*GIF_HIGHT_Y)*  LV_COLOR_SIZE / 8,
.header.cf = LV_IMG_CF_TRUE_COLOR, 
//.header.cf = LV_IMG_CF_INDEXED_1BIT, 
//.data =(void *)GIF_PSRAM_ADDR,
.data =(void *)GIF_PSRAM_ADDR,
};

lv_obj_t* gif_img_obj;
gif_handle_t my_gif_handle;


//#define GIF_LOGO     printf
//#define co_printf     printf

#define GIF_LOGO(...)   do {}while(0)
//#define co_printf(...)     do {}while(0)
	
#define LOG_UP    //gpio_portd_write(gpio_portd_read() | (1<<GPIO_BIT_1) )
#define LOG_DOWN    //gpio_portd_write(gpio_portd_read() & ~(1<<GPIO_BIT_1) )
#define LOG_LCD_UP   // gpio_portd_write(gpio_portd_read() | (1<<GPIO_BIT_6) )
#define LOG_LCD_DOWN    //gpio_portd_write(gpio_portd_read() & ~(1<<GPIO_BIT_6) )

#define GIF_RAM_BUFFER_SIZE 259200   //

void gif_display_defer(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e, uint16_t *buff);
static void gif_lcd_fill(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color);

//#define read_gif_filedata flash_read  //ssp_flash_read 
//#define read_gif_filedata ssp_flash_read 
//#define read_gif_filedata read_from_psram 

const uint16_t _aMaskTbl[16] =
{
	0x0000, 0x0001, 0x0003, 0x0007,
	0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff,
	0x0fff, 0x1fff, 0x3fff, 0x7fff,
};	

const uint8_t _aInterlaceOffset[]={8,8,4,2};
const uint8_t _aInterlaceYPos  []={0,4,2,1};

static uint8_t gifdecoding=0;//标记GIF正在解码.
static uint16_t *lcd_60x60_ram_buffer = (void *)GIF_PSRAM_ADDR;
static uint32_t lcd_60x60_ram_buffer_add =0;
static uint16_t ram_buffer_y = 0;
static uint16_t ram_buffer_high = 0;
static uint16_t gif_file_width = 0;
static uint16_t gif_file_height = 0;
static uint32_t gif_flash_addr = 0;

void gif_lcd_display(uint16_t Xstar,uint16_t Xend,uint16_t Y,uint16_t color);

//代码运行时偏移了0x10000000 所以对应0x10100000
#if (FREQCHIP_LOGO==1)
#define GIF_START_ADDR (uint8_t *)(0x10100000) //gif文件烧录到0x100000地址 代码运行时偏移了0x10000000 所以对应0x10100000
#endif
#if (KTXCHIP_LOGO==1)
#define GIF_START_ADDR (uint8_t *)(0x10100000) //gif文件烧录到0x100000地址 代码运行时偏移了0x10000000 所以对应0x10100000
#endif

//gif_img0   (uint8_t*)0x10100000
#define GIF_WORK1_ADDR  (uint8_t*)(0x10139638)

uint8_t *g_gif_paddr=GIF_START_ADDR;
//uint8_t GIF_FILE_ADDR[1024*1024*8] = { 0 };
void read_gif_filedata(uint32_t offset, uint32_t length, uint8_t *buffer)
{
	memcpy(buffer,(uint8_t*)&g_gif_paddr[offset],length);
}
//检测GIF头
//返回值:0,是GIF89a/87a;非零,非GIF89a/87a
ATTRIBUTE_GIF_MEM uint8_t gif_check_head(uint32_t *flash_addr)
{
	uint8_t gifversion[6];
	read_gif_filedata(*flash_addr,6,gifversion);
	*flash_addr += 6;
	if((gifversion[0]!='G')||(gifversion[1]!='I')||(gifversion[2]!='F')||
	(gifversion[3]!='8')||((gifversion[4]!='7')&&(gifversion[4]!='9'))||
	(gifversion[5]!='a'))return 2;
	else return 0;	
}
//将RGB888转为RGB565
//ctb:RGB888颜色数组首地址.
//返回值:RGB565颜色.
ATTRIBUTE_GIF_MEM uint16_t gif_getrgb565(uint8_t *ctb) 
{
	uint16_t r,g,b;
	r=(ctb[0]>>3)&0X1F;
	g=(ctb[1]>>2)&0X3F;
	b=(ctb[2]>>3)&0X1F;
	return b+(g<<5)+(r<<11);
}

//读取颜色表
//file:文件;
//gif:gif信息;
//num:tbl大小.
//返回值:0,OK;其他,失败;
ATTRIBUTE_GIF_MEM uint8_t gif_readcolortbl(uint32_t *flash_addr,gif89a * gif,uint16_t num)
{
	uint8_t rgb[3];
	uint16_t t;
//	uint32_t readed;
	GIF_LOGO("gif_readcolortbl flash_addr=0x%x,Num = %d\r\n",*flash_addr,num);
	for(t=0;t<num;t++)
	{
		read_gif_filedata(*flash_addr,3,rgb);
		*flash_addr += 3;
		gif->colortbl[t]=gif_getrgb565(rgb);
	}
	return 0;
} 

//得到逻辑屏幕描述,图像尺寸等
//file:文件;
//gif:gif信息;
//返回值:0,OK;其他,失败;
ATTRIBUTE_GIF_MEM uint8_t gif_getinfo(uint32_t *flash_addr,gif89a * gif)
{
	
	read_gif_filedata(*flash_addr,7,(uint8_t*)&gif->gifLSD);
	*flash_addr += 7;
	if(gif->gifLSD.flag&0x80)//存在全局颜色表
	{
		gif->numcolors=2<<(gif->gifLSD.flag&0x07);//得到颜色表大小
		if(gif_readcolortbl(flash_addr,gif,gif->numcolors))return 1;//读错误	
	}	   
	return 0;
}
//保存全局颜色表	 
//gif:gif信息;
ATTRIBUTE_GIF_MEM void gif_savegctbl(gif89a* gif)
{
	uint16_t i=0;
	for(i=0;i<256;i++)gif->bkpcolortbl[i]=gif->colortbl[i];//保存全局颜色.
}
//恢复全局颜色表	 
//gif:gif信息;
ATTRIBUTE_GIF_MEM void gif_recovergctbl(gif89a* gif)
{
	uint16_t i=0;
	for(i=0;i<256;i++)gif->colortbl[i]=gif->bkpcolortbl[i];//恢复全局颜色.
}
//初始化LZW相关参数	   
//gif:gif信息;
//codesize:lzw码长度
ATTRIBUTE_GIF_MEM void gif_initlzw(gif89a* gif,uint8_t codesize) 
{
 	memset((uint8_t *)gif->lzw, 0, sizeof(LZW_INFO));
	gif->lzw->SetCodeSize  = codesize;
	gif->lzw->CodeSize     = codesize + 1;
	gif->lzw->ClearCode    = (1 << codesize);
	gif->lzw->EndCode      = (1 << codesize) + 1;
	gif->lzw->MaxCode      = (1 << codesize) + 2;
	gif->lzw->MaxCodeSize  = (1 << codesize) << 1;
	gif->lzw->ReturnClear  = 1;
	gif->lzw->LastByte     = 2;
	gif->lzw->sp           = gif->lzw->aDecompBuffer;
}
//读取一个数据块
//gfile:gif文件;
//buf:数据缓存区
//maxnum:最大读写数据限制
ATTRIBUTE_GIF_MEM uint16_t gif_getdatablock(uint32_t *flash_addr,uint8_t *buf,uint16_t maxnum) 
{
	uint8_t cnt;
//	uint32_t readed;
//	uint32_t fpos;
	read_gif_filedata(*flash_addr,1,&cnt);//得到LZW长度	
	*flash_addr += 1;
	//GIF_LOGO("gif_getdatablock addr:0x%x cnt=%x  buf_addr=0x%x buff=%d maxnum =%d\r\n",*flash_addr,cnt,buf,*buf,maxnum);
	if(cnt) 
	{
		if (buf)//需要读取 
		{
			if(cnt>maxnum)
			{
			//	fpos=f_tell(gfile);
			//	f_lseek(gfile,fpos+cnt);//跳过
				*flash_addr += cnt;
				return cnt;//直接不读
			}
			read_gif_filedata(*flash_addr,cnt,buf);//得到LZW长度	
			*flash_addr += cnt;
		}else 	//直接跳过
		{
			//fpos=f_tell(gfile);
			//f_lseek(gfile,fpos+cnt);//跳过
			*flash_addr += cnt;
		}
	}
	return cnt;
}
//ReadExtension		 
//Purpose:
//Reads an extension block. One extension block can consist of several data blocks.
//If an unknown extension block occures, the routine failes.
//返回值:0,成功;
// 		 其他,失败
ATTRIBUTE_GIF_MEM uint8_t gif_readextension(uint32_t *flash_addr,gif89a* gif, int *pTransIndex,uint8_t *pDisposal)
{
	uint8_t temp;
//	uint32_t readed;	 
	uint8_t buf[4];  
	read_gif_filedata(*flash_addr,1,&temp);//得到长度
	*flash_addr += 1;
	GIF_LOGO("gif_readextension temp=%x!******************************\r\n",temp);
	switch(temp)
	{
		case GIF_PLAINTEXT:
		case GIF_APPLICATION:
		case GIF_COMMENT:
			while(gif_getdatablock(flash_addr,0,256)>0);			//获取数据块
			return 0;
		case GIF_GRAPHICCTL://图形控制扩展块
			if(gif_getdatablock(flash_addr,buf,4)!=4)return 1;	//图形控制扩展块的长度必须为4 
 		 	gif->delay=(buf[2]<<8)|buf[1];					//得到延时 
			*pDisposal=(buf[0]>>2)&0x7; 	    			//得到处理方法
			if((buf[0]&0x1)!=0)*pTransIndex=buf[3];			//透明色表 
			read_gif_filedata(*flash_addr,1,&temp);//得到长度
			*flash_addr += 1; 		//得到LZW长度	
 			if(temp!=0)return 1;							//读取数据块结束符错误.
			return 0;
	}
	return 1;//错误的数据
}

//从LZW缓存中得到下一个LZW码,每个码包含12位
//返回值:<0,错误.
//		 其他,正常.
ATTRIBUTE_GIF_MEM int gif_getnextcode(uint32_t *flash_addr,gif89a* gif) 
{
	int i,j,End;
	long Result;
	if(gif->lzw->ReturnClear)
	{
		//The first code should be a clearcode.
		gif->lzw->ReturnClear=0;
		return gif->lzw->ClearCode;
	}
	End=gif->lzw->CurBit+gif->lzw->CodeSize;
	if(End>=gif->lzw->LastBit)
	{
		int Count;
		if(gif->lzw->GetDone)return-1;//Error 
		gif->lzw->aBuffer[0]=gif->lzw->aBuffer[gif->lzw->LastByte-2];
		gif->lzw->aBuffer[1]=gif->lzw->aBuffer[gif->lzw->LastByte-1];
		if((Count=gif_getdatablock(flash_addr,&gif->lzw->aBuffer[2],300))==0)gif->lzw->GetDone=1;
		if(Count<0)return -1;//Error 
		gif->lzw->LastByte=2+Count;
		gif->lzw->CurBit=(gif->lzw->CurBit-gif->lzw->LastBit)+16;
		gif->lzw->LastBit=(2+Count)*8;
		End=gif->lzw->CurBit+gif->lzw->CodeSize;
	}
	j=End>>3;
	i=gif->lzw->CurBit>>3;
	if(i==j)Result=(long)gif->lzw->aBuffer[i];
	else if(i+1==j)Result=(long)gif->lzw->aBuffer[i]|((long)gif->lzw->aBuffer[i+1]<<8);
	else Result=(long)gif->lzw->aBuffer[i]|((long)gif->lzw->aBuffer[i+1]<<8)|((long)gif->lzw->aBuffer[i+2]<<16);
	Result=(Result>>(gif->lzw->CurBit&0x7))&_aMaskTbl[gif->lzw->CodeSize];
	gif->lzw->CurBit+=gif->lzw->CodeSize;
	return(int)Result;
}

//得到LZW的下一个码
//返回值:<0,错误(-1,不成功;-2,读到结束符了)
//		 >=0,OK.(LZW的第一个码)
ATTRIBUTE_GIF_MEM int gif_getnextbyte(uint32_t *flash_addr,gif89a* gif) 
{
	int i,Code,Incode;
	while((Code=gif_getnextcode(flash_addr,gif))>=0)
	{
		if(Code==gif->lzw->ClearCode)
		{
			//Corrupt GIFs can make this happen  
			if(gif->lzw->ClearCode>=(1<<MAX_NUM_LWZ_BITS))return -1;//Error 
			//Clear the tables 
			memset((uint8_t*)gif->lzw->aCode,0,sizeof(gif->lzw->aCode));
			for(i=0;i<gif->lzw->ClearCode;++i)gif->lzw->aPrefix[i]=i;
			//Calculate the'special codes' independence of the initial code size
			//and initialize the stack pointer 
			gif->lzw->CodeSize=gif->lzw->SetCodeSize+1;
			gif->lzw->MaxCodeSize=gif->lzw->ClearCode<<1;
			gif->lzw->MaxCode=gif->lzw->ClearCode+2;
			gif->lzw->sp=gif->lzw->aDecompBuffer;
			//Read the first code from the stack after clear ingand initializing*/
			do
			{
				gif->lzw->FirstCode=gif_getnextcode(flash_addr,gif);
			}while(gif->lzw->FirstCode==gif->lzw->ClearCode);
			gif->lzw->OldCode=gif->lzw->FirstCode;
			return gif->lzw->FirstCode;
		}
		if(Code==gif->lzw->EndCode)return -2;//End code
		Incode=Code;
		if(Code>=gif->lzw->MaxCode)
		{
			*(gif->lzw->sp)++=gif->lzw->FirstCode;
			Code=gif->lzw->OldCode;
		}
		while(Code>=gif->lzw->ClearCode)
		{
			*(gif->lzw->sp)++=gif->lzw->aPrefix[Code];
			if(Code==gif->lzw->aCode[Code])return Code;
			if((gif->lzw->sp-gif->lzw->aDecompBuffer)>=sizeof(gif->lzw->aDecompBuffer))return Code;
			Code=gif->lzw->aCode[Code];
		}
		*(gif->lzw->sp)++=gif->lzw->FirstCode=gif->lzw->aPrefix[Code];
		if((Code=gif->lzw->MaxCode)<(1<<MAX_NUM_LWZ_BITS))
		{
			gif->lzw->aCode[Code]=gif->lzw->OldCode;
			gif->lzw->aPrefix[Code]=gif->lzw->FirstCode;
			++gif->lzw->MaxCode;
			if((gif->lzw->MaxCode>=gif->lzw->MaxCodeSize)&&(gif->lzw->MaxCodeSize<(1<<MAX_NUM_LWZ_BITS)))
			{
				gif->lzw->MaxCodeSize<<=1;
				++gif->lzw->CodeSize;
			}
		}
		gif->lzw->OldCode=Incode;
		if(gif->lzw->sp>gif->lzw->aDecompBuffer)return *--(gif->lzw->sp);
	}
	return Code;
}

//DispGIFImage		 
//Purpose:
//   This routine draws a GIF image from the current pointer which should point to a
//   valid GIF data block. The size of the desired image is given in the image descriptor.
//Return value:
//  0 if succeed
//  1 if not succeed
//Parameters:
//  pDescriptor  - Points to a IMAGE_DESCRIPTOR structure, which contains infos about size, colors and interlacing.
//  x0, y0       - Obvious.
//  Transparency - Color index which should be treated as transparent.
//  Disposal     - Contains the disposal method of the previous image. If Disposal == 2, the transparent pixels
//                 of the image are rendered with the background color.
ATTRIBUTE_GIF_MEM uint8_t gif_dispimage(uint32_t *flash_addr,gif89a* gif,uint16_t x0,uint16_t y0,int Transparency, uint8_t Disposal) 
{
//	uint32_t readed;	
//	uint16_t color_data = 0; 
   	uint8_t lzwlen;
//	uint8_t i = 0;
	int Index,OldIndex,XPos,YPos,YCnt,Pass,Interlace,XEnd;
	int Width,Height,Cnt,ColorIndex;
	uint16_t bkcolor;
	uint16_t *pTrans;

	Width=gif->gifISD.width;
	Height=gif->gifISD.height;
	XEnd=Width+x0-1;
	bkcolor=gif->colortbl[gif->gifLSD.bkcindex];
	pTrans=(uint16_t*)gif->colortbl;
	read_gif_filedata(*flash_addr,1,&lzwlen);
	*flash_addr += 1;
	gif_initlzw(gif,lzwlen);//Initialize the LZW stack with the LZW code size 
	Interlace=gif->gifISD.flag&0x40;//是否交织编码
	for(YCnt=0,YPos=y0,Pass=0;YCnt<Height;YCnt++)//Y
	{
		Cnt=0;
		OldIndex=-1;
		for(XPos=x0;XPos<=XEnd;XPos++)//X
		{
			if(gif->lzw->sp>gif->lzw->aDecompBuffer)Index=*--(gif->lzw->sp);
		    else Index=gif_getnextbyte(flash_addr,gif);	   
			if(Index==-2)return 0;//Endcode     
			if((Index<0)||(Index>=gif->numcolors))
			{
				//IfIndex out of legal range stop decompressing
				return 1;//Error
			}
			//If current index equals old index increment counter
			if((Index==OldIndex)&&(XPos<=XEnd))
				Cnt++;
	 		else
			{
				if(Cnt)//Cnt>0
				{
					if(OldIndex!=Transparency)
					{									    
						gif_lcd_display(XPos-Cnt-1,XPos,YPos,*(pTrans+OldIndex));
					}else if(Disposal==2)
					{
						gif_lcd_display(XPos-Cnt-1,XPos,YPos,bkcolor);
					}
					Cnt=0;
				}else//Cnt = 0
				{
					if(OldIndex>=0)
					{
						if(OldIndex!=Transparency){
                            //LCD_DrawPoint(XPos-1,YPos,*(pTrans+OldIndex));
                            gif_lcd_display(XPos-1,XPos-1,YPos,*(pTrans+OldIndex));
						}
					 	else if(Disposal==2){
                           gif_lcd_display(XPos-1,XPos-1,YPos,bkcolor);
					 	}
					}
				}
			}
			OldIndex=Index;
		}
		if((OldIndex!=Transparency)||(Disposal==2))
		{
			if(OldIndex!=Transparency)ColorIndex=*(pTrans+OldIndex);
		    else ColorIndex=bkcolor;
			//color_data = (ColorIndex>>8)|(ColorIndex<<8);
	 		if(Cnt)
			{
				//LCD_DrawLine(XPos-Cnt-1,YPos,XPos,YPos,ColorIndex);
				gif_lcd_display(XPos-Cnt-1,XPos,YPos,ColorIndex);
			}else 
			//LCD_DrawPoint(XEnd,YPos,ColorIndex);
				gif_lcd_display(XEnd,XEnd,YPos,ColorIndex);
		}
		//Adjust YPos if image is interlaced 
		if(Interlace)//交织编码
		{
			YPos+=_aInterlaceOffset[Pass];
			if((YPos-y0)>=Height)
			{
				++Pass;
				YPos=_aInterlaceYPos[Pass]+y0;
			}
		}else YPos++;	    
	}
	return 0;
}  	

//恢复成背景色
//x,y:坐标
//gif:gif信息.
//pimge:图像描述块信息
ATTRIBUTE_GIF_MEM void gif_clear2bkcolor(uint16_t x,uint16_t y,gif89a* gif,ImageScreenDescriptor pimge)
{
	uint16_t x0,y0,x1,y1;
	uint16_t color=gif->colortbl[gif->gifLSD.bkcindex];
	if(pimge.width==0||pimge.height==0)return;//直接不用清除了,原来没有图像!!
	if(gif->gifISD.yoff>pimge.yoff)
	{
   		x0=x+pimge.xoff;
		y0=y+pimge.yoff;
		x1=x+pimge.xoff+pimge.width-1;;
		y1=y+gif->gifISD.yoff-1;
		if(x0<x1&&y0<y1&&x1<GIF_WIDTH_X&&y1<GIF_HIGHT_Y)
			gif_lcd_fill(x0,y0,x1,y1,color); //设定xy,的范围不能太大.2021-7-9
	}
	if(gif->gifISD.xoff>pimge.xoff)
	{
   		x0=x+pimge.xoff;
		y0=y+pimge.yoff;
		x1=x+gif->gifISD.xoff-1;;
		y1=y+pimge.yoff+pimge.height-1;
		if(x0<x1&&y0<y1&&x1<GIF_WIDTH_X&&y1<GIF_HIGHT_Y)
			gif_lcd_fill(x0,y0,x1,y1,color);
	}
	if((gif->gifISD.yoff+gif->gifISD.height)<(pimge.yoff+pimge.height))
	{
   		x0=x+pimge.xoff;
		y0=y+gif->gifISD.yoff+gif->gifISD.height-1;
		x1=x+pimge.xoff+pimge.width-1;;
		y1=y+pimge.yoff+pimge.height-1;
		if(x0<x1&&y0<y1&&x1<GIF_WIDTH_X&&y1<GIF_HIGHT_Y)
			gif_lcd_fill(x0,y0,x1,y1,color);
	}
 	if((gif->gifISD.xoff+gif->gifISD.width)<(pimge.xoff+pimge.width))
	{
   		x0=x+gif->gifISD.xoff+gif->gifISD.width-1;
		y0=y+pimge.yoff;
		x1=x+pimge.xoff+pimge.width-1;;
		y1=y+pimge.yoff+pimge.height-1;
		if(x0<x1&&y0<y1&&x1<GIF_WIDTH_X&&y1<GIF_HIGHT_Y)
			gif_lcd_fill(x0,y0,x1,y1,color);
	}   
}

//画GIF图像的一帧
//gfile:gif文件.
//x0,y0:开始显示的坐标
ATTRIBUTE_GIF_MEM uint8_t gif_drawimage(uint32_t *flash_addr,gif89a* gif,uint16_t x0,uint16_t y0)
{		  
//	uint32_t readed;
	uint8_t res=0,temp=0;    
    uint16_t numcolors = { 0 };
    ImageScreenDescriptor previmg = {0};

	uint8_t Disposal=0;
	int TransIndex=0;
	uint8_t Introducer=0;
	TransIndex=-1;
    GIF_LOGO("flash_addr=%x!\r\n",*flash_addr);
	do
	{
		read_gif_filedata(*flash_addr,1,&Introducer);
		*flash_addr += 1;
		GIF_LOGO("gif_drawimage Introducer=%x!\r\n",Introducer);
		switch(Introducer)
		{		 
			case GIF_INTRO_IMAGE://图像描述
				previmg.xoff=gif->gifISD.xoff;
 				previmg.yoff=gif->gifISD.yoff;
				previmg.width=gif->gifISD.width;
				previmg.height=gif->gifISD.height;

				read_gif_filedata(*flash_addr,9,(uint8_t*)&gif->gifISD);
				*flash_addr += 9;
				if(gif->gifISD.flag&0x80)//存在局部颜色表
				{							  
					gif_savegctbl(gif);//保存全局颜色表
					numcolors=2<<(gif->gifISD.flag&0X07);//得到局部颜色表大小
					if(gif_readcolortbl(flash_addr,gif,numcolors)){//读错误
						GIF_LOGO("gif_readcolortbl ERR!\r\n");
						return 1;
					}
				}
         GIF_LOGO("Disposal = %d\r\n",Disposal);
				if(Disposal==2)gif_clear2bkcolor(x0,y0,gif,previmg); 
				gif_dispimage(flash_addr,gif,x0+gif->gifISD.xoff,y0+gif->gifISD.yoff,TransIndex,Disposal);
                uint16_t count_0xff = 0;
 				while(1)
				{
					//f_read(gfile,&temp,1,(UINT*)&readed);//读取一个字节
					read_gif_filedata(*flash_addr,1,&temp);
					*flash_addr += 1;
					if(temp==0)break;
					if(temp == 0xff){
							count_0xff++;
							if(count_0xff>255)
									break;
					}else{
							count_0xff=0;
					} 
					*flash_addr += temp;
			  }
				if(temp!=0){
                    GIF_LOGO("read_gif_filedata data ERR!\r\n");
                    return 1;//Error 
                }
					

				if(lcd_60x60_ram_buffer_add !=0){
					
							LOG_LCD_UP;//刷屏
							//display(lcd_60x60_ram_buffer,0);
							GIF_LOGO("gc9c01_display0\r\n");

							lcd_60x60_ram_buffer_add =0;
							LOG_LCD_DOWN;
				}
				
				ram_buffer_y = 0;
				return 0;
			case GIF_INTRO_TERMINATOR://得到结束符了
				return 2;//代表图像解码完成了.
			case GIF_INTRO_EXTENSION:
				//Read image extension*/
				res=gif_readextension(flash_addr,gif,&TransIndex,&Disposal);//读取图像扩展块消息
				if(res)
				{
                     GIF_LOGO("gif_readextension ERR!\r\n");
                     return 1;
        }
	 			break;
			default:
				return 1;
		}
	}while(Introducer!=GIF_INTRO_TERMINATOR);//读到结束符了
	
	return 0;
}
//退出当前解码.
ATTRIBUTE_GIF_MEM void gif_quit(void)
{
	gifdecoding=0;
}
 
 
//解码一个gif文件
//本例子不能显示尺寸大与给定尺寸的gif图片!!!
//filename:带路径的gif文件名字
//x,y,width,height:显示坐标及区域大小.
uint8_t frame_count = 0;
ATTRIBUTE_GIF_MEM uint8_t gif_decode(uint32_t Gif_addr,uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
	uint8_t res=0;
	uint16_t dtime=0;//解码延时
	gif89a *mygif89a;
	uint32_t *gfile;
	gfile=&Gif_addr;
  // printf(" %s size =%d----------------------\r\n",__FUNCTION__,os_get_free_heap_size());
	mygif89a=(gif89a*)gif_malloc(sizeof(gif89a));
	if(mygif89a==NULL){
		GIF_LOGO("os_zalloc1 fail#1\r\n");
        return false;//申请内存失败   
    }        
	mygif89a->lzw=(LZW_INFO*)gif_malloc(sizeof(LZW_INFO));
	//	 printf(" %s size =%d----------------------\r\n",__FUNCTION__,os_get_free_heap_size());
	if(mygif89a->lzw==NULL){
		GIF_LOGO("os_zalloc1 fail#2\r\n");
		return false;//申请内存失败 
     
    }
	if(gif_check_head(gfile)){
		res=PIC_FORMAT_ERR;
		GIF_LOGO("gif_check_head ERR!\r\n");
	}
	if(gif_getinfo(gfile,mygif89a)){
		res=PIC_FORMAT_ERR;
		GIF_LOGO("gif_getinfo ERR!\r\n");
	}
	GIF_LOGO("gif_width=%d,gif_height=%d\r\n",mygif89a->gifLSD.width,mygif89a->gifLSD.height);
	if(mygif89a->gifLSD.width>width||mygif89a->gifLSD.height>height){
        res=PIC_SIZE_ERR;//尺寸太大.
        GIF_LOGO("PIC_SIZE_ERR ERR!\r\n");
    }else{
		//x=(width-mygif89a->gifLSD.width)/2+x;
		//y=(height-mygif89a->gifLSD.height)/2+y;
	}
	gif_file_height = mygif89a->gifLSD.height;
	gif_file_width = mygif89a->gifLSD.width;
	ram_buffer_high = GIF_RAM_BUFFER_SIZE/2/gif_file_width;
	GIF_LOGO("gif_file_width = %d ram_buffer_high =%d\r\n",gif_file_width,ram_buffer_high);
	gifdecoding=1;
	while(gifdecoding&&res==0)//解码循环
	{	      
//		#if (FREQCHIP_LOGO==1)
//		  x=100;y=100;
//		#endif
//		#if (KTXCHIP_LOGO==1)
//			x=30;y=0;
//		#endif
			
				res=gif_drawimage(gfile,mygif89a,x,y);//显示一张图片 x y 为显示的起始位置
        frame_count++;
        gif_display_defer(0, GIF_WIDTH_X, 0, GIF_HIGHT_Y, lcd_60x60_ram_buffer);
        
        GIF_LOGO("gc9c01_display1\r\n");
	    GIF_LOGO("gif_drawimage res =%x\r\n",res);
		    if(mygif89a->gifISD.flag&0x80)gif_recovergctbl(mygif89a);//恢复全局颜色表
		    if(mygif89a->delay){
		    dtime=mygif89a->delay;
		    GIF_LOGO("gif delay=%d\r\n",dtime);//8ms
		}
		else 
		{
//			dtime=10;//默认延时
//		  while((dtime--) && (gifdecoding)){
//			 
//			}
		}

		if(res==2)
		{
			res=0;
			break;
		}
	}
	gif_free(mygif89a->lzw);
	gif_free(mygif89a);
	gifdecoding=0;
	return res;
}


ATTRIBUTE_GIF_MEM void gif_lcd_display(uint16_t Xstar,uint16_t Xend,uint16_t Y,uint16_t color)
{
	uint16_t i=0;
	uint16_t color_data =color ;
	if(Y - ram_buffer_y >=(ram_buffer_high)){// ram Full
		LOG_LCD_UP;

    GIF_LOGO("X");

		LOG_LCD_DOWN;

		lcd_60x60_ram_buffer_add =0;
		ram_buffer_y = Y;
	}
	if(Xend == Xstar){
        lcd_60x60_ram_buffer[GIF_WIDTH_X*(Y)+Xstar] = color_data;//360*Y+Xstar
	}else{
		for(i = 0;i < (Xend - Xstar)+1;i++){
            lcd_60x60_ram_buffer[GIF_WIDTH_X*(Y)+Xstar+i] = color_data;
		}
	}
	
}
ATTRIBUTE_GIF_MEM static void gif_lcd_fill(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color)
{
    uint16_t i=0,j=0;
    uint16_t color_data = (color>>8)|(color<<8);
   // uint16_t color_data =color ;
    for(i = x0;i<x1;i++)
        for(j = y0;j < y1;j++)
            lcd_60x60_ram_buffer[GIF_WIDTH_X*j+i] = color_data;
}

uint16_t _x_s, _x_e, _y_s, _y_e, *disp_buff=NULL;
uint8_t defer_count = 0;
void gif_display_defer(uint16_t x_s, uint16_t x_e, uint16_t y_s, uint16_t y_e, uint16_t *buff)
{
    _x_s = x_s;
    _x_e = x_e;
    _y_s = y_s;
    _y_e = y_e;
    disp_buff = buff;
    defer_count = 3;
    //lv_img_set_src(gif_img_obj, &gif_img_param);//设置图片源
    //lv_img_set_src(g_page_param.gif_img, &gif_img_param);//设置图片源
	  GIF_LOGO("x_s:%d  x_e:%d y_s:%d y_e:%d\r\n",x_s,x_e,y_s,y_e);
}
ATTRIBUTE_GIF_MEM void start_gif_decoder_handler(void)
{
	if(get_st77903_send_state()&&disp_buff!=NULL)
	{
		 st77903_display((uint16_t *)disp_buff,0);
	}
		
}
void my_memset(uint16_t *dest, uint16_t set, int len)
{
	if (dest == NULL || len < 0)
	{
		return;
	}
	uint16_t *pdest = (uint16_t *)dest;
	while ((len--)>0)
	{
		*pdest++ = set;
	}
	
}
#if 0
ATTRIBUTE_GIF_MEM void start_gif_decoder_handler(void)
{
	if(defer_count) {
        defer_count--;
        if(defer_count == 0) {
						display_wait_transfer_done();
						display_set_window(_x_s,_x_e-1,_y_s,_y_e-1);
					    display_update(((_x_e-_x_s)*(_y_e-_y_s)),disp_buff, NULL);
        }
    }
 
}
#endif
#if 0
void read_gif_file(void)
{
#define file_path "D:\\gif_image\\xiaohuanren.gif"
    FILE* fp = NULL;
    uint32_t size;
   // fp = fopen("D:\\gif_image\\giftest1.gif", "rb"); //
    fp = fopen(file_path, "rb");
    if (fp != NULL)
    {

        //feof(file stream )文件指针到达文件末尾
        //while (!feof(fp)) //读文件
        //    printf("%x\n", fgetc(fp));
       //  把文件的位置指针移到文件尾
        fseek(fp, 0, SEEK_END);
        // 获取文件长度
        size = ftell(fp);
        printf("file len: %d byte\r\n", size);
       

        fclose(fp);
    }
    else
        printf("fail to open! \n");

    fp = fopen(file_path, "rb");
    if (fp != NULL)
    {
        uint32_t read_len = fread(&GIF_FILE_ADDR[0], 1, size, fp);
        printf("read len: %d byte\r\n", read_len);
        fclose(fp);
    }
    else
        printf("fail to open! \n");  
}
#endif

#if 0
static uint32_t* gfile;
static gif89a* mygif89a = NULL;

void gif_decoder_task_cb(void)
{
//    static uint8_t first = 0;
    uint32_t res = 0;
    uint16_t dtime = 0;//解码延时
    if (my_gif_handle.first == 0)
    {
				gif_param_init();
        my_gif_handle.first = 1;
    }
    res = gif_drawimage(gfile, mygif89a, my_gif_handle.x, my_gif_handle.y);//显示一张图片
    frame_count++;
    //gif_display_defer(0, GIF_WIDTH_X - 1, 0, GIF_HIGHT_Y - 1, lcd_60x60_ram_buffer);
    GIF_LOGO("gc9c01_display1\r\n");
    GIF_LOGO("gif_drawimage res =%x\r\n", res);
    if (mygif89a->gifISD.flag & 0x80)gif_recovergctbl(mygif89a);//恢复全局颜色表
    if (mygif89a->delay) {
        dtime = mygif89a->delay;
				//printf("dtime %d frame_count:%d \r\n ",dtime,frame_count);
        lv_timer_set_period(my_gif_handle.timer, mygif89a->delay);//设置解码定时器延时
    }
    if (res == 2)
    {
        //printf("\r\n\r\nres 2");
			  my_gif_handle.flash_addr = 0;
        my_gif_handle.first = 0;
			  frame_count=0;
    }
		switch(g_main_param.gif_num)
		{
		  case 1:
							lv_event_send(g_page_param.work1_gif_img, LV_EVENT_VALUE_CHANGED, &res);
			break;
			
			case 2:
				      lv_event_send(g_page_param.work2_gif_img, LV_EVENT_VALUE_CHANGED, &res);
			break;
			default:
				return ;
		}
    
    /*os_free(mygif89a->lzw);
    os_free(mygif89a);*/
}
void my_next_frame_task_cb(lv_timer_t* t)
{
    gif_decoder_task_cb();
}

void gif_param_init(void)
{
        uint8_t res;    
        my_gif_handle.flash_addr = 0;
        my_gif_handle.width = GIF_WIDTH_X;
        my_gif_handle.hight = GIF_HIGHT_Y;
        my_gif_handle.x = 0;
        my_gif_handle.y = 0;
				if (mygif89a == NULL) {
						mygif89a = (gif89a*)gif_malloc(sizeof(gif89a));
						if (mygif89a == NULL)return ;//申请内存失败
						co_printf("gif_malloc ok#1\r\n");
				}
				//	 printf(" %s size =%d----------------------\r\n",__FUNCTION__,os_get_free_heap_size());
				if (mygif89a->lzw == NULL) {
						mygif89a->lzw = (LZW_INFO*)gif_malloc(sizeof(LZW_INFO));
						if (mygif89a->lzw == NULL)return ;//申请内存失败 
						co_printf("gif_malloc ok#2\r\n");
				}
				printf(" %s size =%d----------------------\r\n",__FUNCTION__,os_get_free_heap_size());
        gfile = &my_gif_handle.flash_addr;
        // printf(" %s size =%d----------------------\r\n",__FUNCTION__,os_get_free_heap_size());

        if (gif_check_head(gfile)) {
            res = PIC_FORMAT_ERR;
            GIF_LOGO("gif_check_head ERR!\r\n");
        }
        if (gif_getinfo(gfile, mygif89a)) {
            res = PIC_FORMAT_ERR;
            GIF_LOGO("gif_getinfo ERR!\r\n");
        }
        GIF_LOGO("gif_width=%d,gif_height=%d\r\n", mygif89a->gifLSD.width, mygif89a->gifLSD.height);
        if (mygif89a->gifLSD.width > my_gif_handle.width || mygif89a->gifLSD.height > my_gif_handle.hight) {
            res = PIC_SIZE_ERR;//尺寸太大.
            GIF_LOGO("PIC_SIZE_ERR ERR!\r\n");
        }
        else {
            my_gif_handle.x = (my_gif_handle.width - mygif89a->gifLSD.width) / 2 + my_gif_handle.x;
            my_gif_handle.y = (my_gif_handle.hight - mygif89a->gifLSD.height) / 2 + my_gif_handle.y;
        }
        gif_file_height = mygif89a->gifLSD.height;
        gif_file_width = mygif89a->gifLSD.width;
				GIF_HIGHT_Y = mygif89a->gifLSD.height;
				GIF_WIDTH_X = mygif89a->gifLSD.width;
				ram_buffer_high = GIF_RAM_BUFFER_SIZE / 2 / gif_file_width;
				gif_img_param.header.w = mygif89a->gifLSD.width;
				gif_img_param.header.h = mygif89a->gifLSD.height;
				gif_img_param.data_size = (gif_img_param.header.w * gif_img_param.header.h) * LV_COLOR_SIZE / 8;
				switch(g_main_param.gif_num)
				{
					case 1: 
								 lv_img_set_src(g_page_param.work1_gif_img, &gif_img_param);//设置图片源
					break;
					case 2:
								 lv_img_set_src(g_page_param.work2_gif_img, &gif_img_param);//设置图片源
					break;
				}
				GIF_LOGO("gif_file_width = %d ram_buffer_high =%d\r\n", gif_file_width, ram_buffer_high);
}

void gif_frame_update_event(lv_event_t* e)
{
	  static uint8_t first_run=0;
    uint32_t* res = lv_event_get_param(e);
	  if(first_run==0)
		{
				lv_mygif_decoder_stop();
				lv_mygif_decoder_start();
				first_run = 1;
		}
	  switch(g_main_param.gif_num)
		{
		  case 1: 
 
							{
							  lv_img_set_src(g_page_param.work1_gif_img, &gif_img_param);//设置图片源
							}
							//printf("gif_frame_update_event %d\r\n",*res);
			break;
			case 2:
				      lv_img_set_src(g_page_param.work2_gif_img, &gif_img_param);//设置图片源
			break;
		}
    
}
#endif
void start_gif_decoder_init(void)
{
	gif_flash_addr=0;
	uint16_t *p_buf = (void *)GIF_PSRAM_ADDR;
	// c7c7c7
	#if (FREQCHIP_LOGO==1)
	uint8_t rgb888_buf[3]={0xff,0xff,0xff};//背景色
	#endif
	#if (KTXCHIP_LOGO==1)
	uint8_t rgb888_buf[3]={0xc7,0xc7,0xc7};//背景色
	#endif

	uint16_t rgb565_bg=gif_getrgb565(rgb888_buf);
	my_memset(p_buf,rgb565_bg,(GIF_WIDTH_X*GIF_HIGHT_Y));
	printf("gif_1\r\n");
	gif_display_defer(0, GIF_WIDTH_X, 0, GIF_HIGHT_Y, p_buf);
	gif_decode(gif_flash_addr,100,100,GIF_WIDTH_X,GIF_HIGHT_Y);
	printf("gif_2\r\n");
		
}

#if 0
void lv_mygif_decoder_init(void)
{
 
		lv_obj_add_event_cb(g_page_param.work1_gif_img , gif_frame_update_event, LV_EVENT_VALUE_CHANGED,NULL);
	  lv_obj_add_event_cb(g_page_param.work2_gif_img , gif_frame_update_event, LV_EVENT_VALUE_CHANGED,NULL);
		my_gif_handle.timer = lv_timer_create(my_next_frame_task_cb, 10, NULL);
		lv_timer_pause(my_gif_handle.timer);
	  
}

void lv_mygif_decoder_start(void)
{
			frame_count=0;
			if(my_gif_handle.decoder_sta==0)
			{
				my_gif_handle.flash_addr = 0;
				my_gif_handle.decoder_sta=1;
				my_memset((void *)gif_img_param.data,0x0000,GIF_WIDTH_X*GIF_HIGHT_Y);
				switch(g_main_param.gif_num)
				{
				  case 1:
						g_gif_paddr =  GIF_WORK1_ADDR;
					break;
					
					case 2:
						g_gif_paddr =  GIF_WORK1_ADDR;
					break;
				}
				lv_timer_resume(my_gif_handle.timer);
				lv_timer_reset(my_gif_handle.timer);
				
			}
			 
}

void lv_mygif_decoder_stop(void)
{
	if(my_gif_handle.decoder_sta==1)
	{
		lv_timer_pause(my_gif_handle.timer);
	  my_gif_handle.first = 0;
	  if(mygif89a->lzw!=NULL && mygif89a!=NULL)
		{
				gif_free(mygif89a->lzw);
				gif_free(mygif89a);
				mygif89a->lzw=NULL;
				mygif89a=NULL;
		}
		my_gif_handle.decoder_sta=0;
	}
		printf(" %s size =%d----------------------\r\n",__FUNCTION__,os_get_free_heap_size());
}
#endif

#endif


