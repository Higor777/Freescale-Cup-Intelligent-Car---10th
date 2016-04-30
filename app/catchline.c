#include "catchline.h"
#include "ccd.h"
#include "ctrl.h"
#include "led.h"
#define DiffAB(A,B) ((A)>(B)?((A)-(B)):((B)-(A)))

uint8_t Binary_Image[128]={0};
uint8_t Binary_Image_2[128]={0};
uint8_t LeftLine=16,RightLine=105,MiddleLine=64;
uint8_t Left_flag=0,Right_flag=0,Middle_flag=0; 
uint8_t RightAngle_flag2=0,RightAngle_flag2_Old=0;
int32_t CCD_View=0,CCD_HalfView=0,CCD_LeftView=0,CCD_RightView=0;
uint8_t Threshold=90;
uint8_t Begin_Pos=64;

uint8_t BlackLine=0;

uint8_t BlackLine_Width=3;
uint8_t Track_Width=100;


uint8_t RightAngle_Mode=0;
uint8_t RightAngle_Indicator=1;

uint8_t Block_flag=0;
uint8_t BlockRecord[3][2];

//==============================================================
//��������void SearchBeginPos(uint8_t* pos)
//��������������ɨ����ʼ��
//������pos ��ʼ���ַ
//���أ���
//==============================================================
void SearchBeginPos(uint8_t* pos)
{
	if(Middle_flag==1)
	{
		*pos=MiddleLine;
	}
	else
	{
		*pos=(RightLine+LeftLine)>>1;
	}
}

//==============================================================
//��������void Get_TrackWidth(uint8_t* trackwidth)
//����������������
//������trackwidth �߿��ַ
//���أ���
//==============================================================
void Get_TrackWidth(uint8_t* trackwidth)
{
	if(Left_flag==1&&Right_flag==1)
	{
		if(RightLine-LeftLine>80)
		{
		  *trackwidth=(RightLine-LeftLine);
		}
	}
}

//==============================================================
//��������void SearchBlock(void)
//���������������ֿ����������·��ʶ��
//��������
//���أ���
//==============================================================
void SearchBlock(void)
{
	uint8_t pBlockRecord=0;
	extern float DirectCtrlOut;
	for(uint8_t pos=0+5;pos<127-5;pos++)
	{
		if(Binary_Image[pos]==1)
		{
			uint8_t blackpixel=pos;
			
			BlockRecord[pBlockRecord][0]=pos;
			while(Binary_Image[blackpixel]==1 && blackpixel<127-5)
			{
				blackpixel++;
			}
			if(blackpixel!=127-5)
			{
				BlockRecord[pBlockRecord][1]=blackpixel-1;
			  pBlockRecord++;
			}
			pos=blackpixel;
			
		}
		if(pBlockRecord>2)
		{
			break;
		}
	}
	if(pBlockRecord==2)
	{
		if((BlockRecord[0][1]-BlockRecord[0][0])>(BlockRecord[1][1]-BlockRecord[1][0]+10))
		{
//      MiddleLine=(BlockRecord[0][1]+BlockRecord[0][0])>>1;
			DirectCtrlOut=2000;
		}
		else if((BlockRecord[0][1]-BlockRecord[0][0]+10)<(BlockRecord[1][1]-BlockRecord[1][0]))
		{
//			MiddleLine=(BlockRecord[1][1]+BlockRecord[1][0])>>1;
			DirectCtrlOut=-2000;
		}

		if(BlockRecord[1][0]-BlockRecord[0][1]<27&&BlockRecord[1][0]-BlockRecord[0][1]>7)
			Block_flag=1;
	}
}

//==============================================================
//��������void SearchLine(int16_t begin_pos)
//������������������
//��������
//���أ���
//==============================================================
void SearchLine(int16_t begin_pos)
{
	Left_flag=0;
	Right_flag=0;
	Middle_flag=0;
//	Block_flag=0;
	
	//Ѱ�������
	for(int16_t i=begin_pos-20; i>Invalid_Pixel_NUM; i--)
	{
		if(Binary_Image[i]==1&&Binary_Image[i-1]==0)
		{
			LeftLine=i;
			for(int16_t j=LeftLine-1; j>=Invalid_Pixel_NUM; j--)
			{
				if(Binary_Image[j]==1) break;
				else if((LeftLine-j)==BlackLine_Width)
				{
					Left_flag=1;
					break;
				}
			}
		}
		if(Left_flag==1) break;
	}
	
	//Ѱ���ұ���
	for(int16_t i=begin_pos+20;i<127-Invalid_Pixel_NUM;i++)
	{
		if(Binary_Image[i]==1&&Binary_Image[i+1]==0)
		{
			RightLine=i;
			for(int16_t j=RightLine+1; j<=127-Invalid_Pixel_NUM; j++)
			{
				if(Binary_Image[j]==1) break;
				else if((j-RightLine)==BlackLine_Width)
				{
					Right_flag=1;
					break;
				}
			}
		} 
		if(Right_flag==1) break;
	}

	
	//Ѱ������
	for(int16_t i=begin_pos-20; i<begin_pos+20; i++)
	{
		//�ҵ����������
		if(Binary_Image[i]==1&&Binary_Image[i+1]==0)
		{
			//ȷ����߰�ɫ����
			if((i-(begin_pos-20))>(BlackLine_Width<<1))
			{
				//ȷ���м����
				int16_t j=0;
				for(j=i+1; j<=begin_pos+20; j++)
				{
					if(Binary_Image[j]==1) break;
				}
				MiddleLine=(j+i)>>1;
				if((j-i-1)>=BlackLine_Width)
				{
					//ȷ���ұ߰�ɫ����
					for(int16_t k=j; k<=begin_pos+20; k++)
					{
						if(Binary_Image[k]==0) break;
						else if((k-j+1)==(BlackLine_Width<<1))
						{
							Middle_flag=1;
							break;
						}
					}
				}
			}
		}
	}
	
	//�жϵ�����Ч��
	if(Left_flag==1&&Right_flag==0)//����
	{
		if(LeftLine<(127-Track_Width-5)) Left_flag=2;
	}
	else if(Left_flag==0&&Right_flag==1)//����
	{
		if(RightLine>(Track_Width+5)) Right_flag=2;
	}
		

	
	//�����������
	Get_TrackWidth(&Track_Width);
	
	//�ж�ֱ�����ʶ
	uint8_t cnt=0;
//	extern uint8_t RightAngle_cnt;
	for(uint8_t i=0; i<128; i++)
	{
		if(Binary_Image[i]==0) cnt++;
	}
	if(cnt>90) BlackLine=1;
	else if((128-cnt)>80&&Left_flag==1&&Right_flag==1)
	{
		BlackLine=0;
	}
	
	
  if(Block_flag==0&&Middle_flag==0&&Left_flag==1&&Right_flag==1&&cnt>20)
	{
    SearchBlock();
	}
}

float Dfindgap=1500,Ddelay1=1500;

//==============================================================
//��������void Rightangle_Recognition(void)
//����������ʶ��ֱ��
//��������
//���أ���
//==============================================================
void Rightangle_Recognition(void)
{
	static uint8_t blackline_mask=1;
	static int16_t delay_cnt=0;
	static uint8_t rightangle_dir=0;//ֱ�ǳ��ڷ���1Ϊ��2Ϊ��
	static RightAngle_State_Type rightangle_sta=RSTA_blackline;
	extern float Temp_Para;
	extern float CarSpeed;
	extern float ForceDirectCtrl_value;
	extern uint8_t ForceDirectCtrl_flag;
	
	if(RightAngle_Mode==0&&rightangle_sta!=RSTA_blackline)
	{
		rightangle_sta=RSTA_blackline;
		RightAngle_Indicator=1;
	}
	
	switch(rightangle_sta)
	{
		static uint8_t trackwidth=0;
		case RSTA_blackline:
		{
			//�ҵ����߽���ֱ��ģʽ
			if((BlackLine&blackline_mask)==1)
			{
				blackline_mask=(~BlackLine)&0x01;
				
				RightAngle_Mode=1;
				rightangle_sta=RSTA_findgap;
				delay_cnt=0;
				rightangle_dir=0;
				RightAngle_Indicator=0;
				trackwidth=0;
			}
			else if((BlackLine|blackline_mask)==0)
			{
				blackline_mask=(~BlackLine)&0x01;
			}
			break;
		}
		case RSTA_findgap:
		{
			//�ٴ��ҵ����������ʱ
			if((BlackLine&blackline_mask)==1)
			{
				blackline_mask=(~BlackLine)&0x01;
				
				delay_cnt=0;
			}
			else if((BlackLine|blackline_mask)==0)
			{
				blackline_mask=(~BlackLine)&0x01;
			}
			//Ѱ��ȱ��
			if(delay_cnt>Dfindgap)
			{
				uint8_t left_flag=0,right_flag=0;
				uint8_t leftline=0,rightline=127;
				for(int16_t i=64; i>Invalid_Pixel_NUM; i--)
				{
					//Ѱ�������
					if(Binary_Image_2[i]==1&&Binary_Image_2[i-1]==0)
					{
						leftline=i;
						for(int16_t j=i-1; j>=Invalid_Pixel_NUM; j--)
						{
							if(Binary_Image_2[j]==1) break;
							else if((i-j)==BlackLine_Width)
							{
								left_flag=1;
								break;
							}
						}
					}
					if(left_flag==1) break;
				}
				for(int16_t i=64;i<127-Invalid_Pixel_NUM;i++)
				{
					//Ѱ���ұ���
					if(Binary_Image_2[i]==1&&Binary_Image_2[i+1]==0)
					{
						rightline=i;
						for(int16_t j=i+1; j<=127-Invalid_Pixel_NUM; j++)
						{
							if(Binary_Image_2[j]==1) break;
							else if((j-i)==BlackLine_Width)
							{
								right_flag=1;
								break;
							}
						}
					} 
					if(right_flag==1) break;
				}
				//�����������
				if(left_flag==1&&right_flag==1)
				{
					trackwidth=rightline-leftline;
				}
				//������������
				if(trackwidth!=0)
				{
					if(left_flag==0&&right_flag==1)
					{
						if(rightline>trackwidth+5) rightangle_dir=1;
					}
					else if(left_flag==1&&right_flag==0)
					{
						if(leftline<(127-trackwidth)-5) rightangle_dir=2;
					}
				}
				if(rightangle_dir!=0)
				{
					
					rightangle_sta=RSTA_delay1;
					delay_cnt=0;
					//RightAngle_Indicator=1;
				}
			}
			//��ʱ����ʱ����δ�ҵ�ֱ��ȱ���˳�ֱ��ģʽ
			delay_cnt+=CarSpeed*10;
			if(delay_cnt>=10000)
			{
				
				RightAngle_Mode=0;
				//rightangle_sta=RSTA_blackline;
			}
			break;
		}
		case RSTA_turning:
		{
			if(rightangle_dir==1)
			{
				ForceDirectCtrl_value=CarSpeed*Temp_Para;
				ForceDirectCtrl_flag=1;
				if(Left_flag==1)
				{
					RightAngle_Mode=0;
					//rightangle_sta=RSTA_blackline;
				}
			}
			else if(rightangle_dir==2)
			{
				ForceDirectCtrl_value=-(CarSpeed*Temp_Para);
				ForceDirectCtrl_flag=1;
				if(Right_flag==1)
				{
					RightAngle_Mode=0;
					//rightangle_sta=RSTA_blackline;
				}
			}
			break;
		}
		case RSTA_delay1:
		{
			delay_cnt+=CarSpeed*10;
			if(delay_cnt>=Ddelay1)
			{
				rightangle_sta=RSTA_turning;
				//RightAngle_Indicator=1;
			}
			break;
		}
		case RSTA_delay2:
		{
			delay_cnt+=CarSpeed*100;
			if(delay_cnt>=0)
			{
				rightangle_sta=RSTA_turning;
			}
			break;
		}
	}
}

//==============================================================
//��������uint8_t	ImageBinaryzation(uint8_t threshold, uint8_t* image, uint8_t* binary_image)
//����������ʶ��ֱ��
//������ threshold ��ֵ����ֵ image ͼ���ַ  binary_image ��ֵ��ͼ���ַ
//���أ�new_threshold �µĶ�ֵ����ֵ
//==============================================================
uint8_t	ImageBinaryzation(uint8_t threshold, uint8_t* image, uint8_t* binary_image)
{
	uint8_t len=128;
	uint8_t i=0;
	uint8_t average1_cnt=0,average2_cnt=0;
	uint16_t average1=0,average2=0;
	uint8_t new_threshold=threshold;
		
	uint8_t just_binaryzation=1;
	
	for(uint8_t iterations=0; iterations<10; iterations++)
	{
		average1_cnt=0,average2_cnt=0;
		average1=0,average2=0;
		
		//�õ���ֵ��ͼ��
		for(i=0; i<len; i++)
		{
			if(image[i]>new_threshold) binary_image[i]=1;
			else binary_image[i]=0;
		}
		
		if(just_binaryzation==0)
		{
		
		//��������ֵ
		for(i=0; i<len; i++)
		{
			if(binary_image[i])
			{
				average1+=image[i];
				average1_cnt++;
			}
			else
			{
				average2+=image[i];
				average2_cnt++;
			}
		}
		threshold=new_threshold;
		new_threshold=((average1/average1_cnt)+(average2/average2_cnt))>>1;
		if(DiffAB(new_threshold,threshold)<5)
		{
			//��ֵ������
			break;
		}
		else if(DiffAB(new_threshold,threshold)>40)
		{
			return threshold;
		}
		
		}
		return threshold;
	}
	for(i=0; i<len; i++)
	{
		if(image[i]>new_threshold) binary_image[i]=1;
		else binary_image[i]=0;
	}
	return new_threshold;
}


//==============================================================
//��������void ImageBinary(void)
//������������ε�����ֵ��
//������ ��
//���أ���
//==============================================================
void ImageBinary(void)
{
	uint8_t histogram[255]={0};
	uint8_t min_gray=255,max_gray=0;
	int32_t thres;
	
	for(int i=0;i<128;i++)
	{
		histogram[ImageData[i]]++;
		if(ImageData[i]<=min_gray)
		{
			min_gray=ImageData[i];
		}
		if(ImageData[i]>=max_gray)
		{
			max_gray=ImageData[i];
		}
	}
	//����10�μ��㶯̬��ֵ
	thres=(int32_t)(min_gray*0.3+max_gray*0.7);
	for(int iter=1;iter<=10;iter++)
	{
		int32_t l_sum=0,r_sum=0;
		int32_t l_cnt=0,r_cnt=0;
		
		for(int l=0;l<thres;l++)
		{
			l_sum+=histogram[l]*l;
			l_cnt+=histogram[l];
		}
		for(int r=thres;r<256;r++)
		{
			r_sum+=histogram[r]*r;
			r_cnt+=histogram[r];
		}
		thres=(l_sum/l_cnt+r_sum/r_cnt)>>1;
	}
	//��ֵ��
	for(int i=0;i<128;i++)
	{
		if(ImageData[i]<=thres)
		{//����
			Binary_Image[i]=0;
		}
		else
		{
			Binary_Image[i]=1;
		}
	}
}

