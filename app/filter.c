#include "filter.h"
#include "anglesensor.h"

AverFilter_Type AverFilter_Gyro_AR2={{0},0},AverFilter_Acc_Z={{0},0};


//==============================================================
//��������float AverFilter_getVal(AverFilter_Type *filter,int32_t current)
//����������ƽ��ֵ�˲�
//������filter �˲����ݵ�ַ current ��ǰ���� 
//���أ���
//==============================================================
float AverFilter_getVal(AverFilter_Type *filter,int32_t current)
{
	float sum=0.0;
	
	if(filter->pos>=FILTER_BUFSIZE)
	{
		filter->pos=0;
	}
	
	filter->buf[filter->pos]=current;
	(filter->pos)++;
	for(uint8_t i=0;i<FILTER_BUFSIZE;i++)
	{
		sum+=filter->buf[i];
	}
	
	return sum/FILTER_BUFSIZE;
}


LP1Filter_Type LP1Filter_Gyro_AR2={{0,0}},LP1Filter_Acc_Z={{0,0}};
LP1Filter_Type LP1Filter_CarSpeed={{0,0}};

//==============================================================
//��������float LP1Filter_getVal(LP1Filter_Type *filter,float current,float a)
//������������ͨ�˲�
//������filter ֮ǰ�˲����ݵ�ַ current ��ǰ���� a �˲�����
//���أ���
//==============================================================
float LP1Filter_getVal(LP1Filter_Type *filter,float current,float a)
{
	filter->data[0]=filter->data[1];
	filter->data[1]=current;
	filter->data[1]=a*(filter->data[1])+(1-a)*(filter->data[0]);
	
	return (filter->data[1]);
}


const float Ts=0.005;
//float p11=0.690047,p22=0.364945,p12=-0.315133,p21=-0.315133;
//float p11=0.7,p22=0.4,p12=-0.32,p21=-0.32;
float p11=7.9E+8,p22=2.5E+2,p12=-3.16E+5,p21=-3.16E+5;
float q_inc=0.001,q_gyro=0.001;
float r_meas=100;
float x1_angle=0,x2_drift=0;
extern float Last_Angle,Angle,Anglar_Rate;
extern float Gyro_Integ;

//==============================================================
//��������void KalmanFliter(float z_inc, float u_gyro)
//�����������������˲��ں�
//������z_inc Z����ٶ��ź� u_gyro �������ź�
//���أ���
//==============================================================
void KalmanFliter(float z_inc, float u_gyro)
{
    float x1_pre,x2_pre;
    float p11_pre,p22_pre,p12_pre;
    float y,s;
    float k1,k2;
	
		/******************************Ԥ��(ʱ�����)*****************************/
    //(1)Ԥ������״̬����ֵ
    x1_pre=x1_angle+Ts*(u_gyro-x2_drift);//x_k=Ak*x_k-1+Bk*u_k
    x2_pre=x2_drift;
    
		//(2)Ԥ���������Э����
    p11_pre=p11-Ts*(p12+p21)+Ts*Ts*p22+q_inc;
    p12_pre=p12-Ts*p22; 
    p22_pre=p22+q_gyro;
	
		/******************************����(��������)*****************************/
	  //(3)���㿨��������
    y=z_inc-x1_pre;			//��������ǵĲ����в�
    s=p11_pre+r_meas;		//����в��Э����
    k1=p11_pre/s;				//����������ſ���������
    k2=p12_pre/s;				//����Ư�����ſ���������
    
		//(4)��������״̬����ֵ
    x1_angle=x1_pre+y*k1;
    x2_drift=x2_pre+y*k2;
    
		//(5)�����������Э����
    p11=(1-k1)*p11_pre;
    p12=(1-k1)*p12_pre;
    p21=-k2*p11_pre+p12_pre;
    p22=-k2*p12_pre+p22_pre;
    
    Last_Angle=Angle;
    Angle=x1_angle;
    Anglar_Rate=(u_gyro-x2_drift);
    Gyro_Integ+=Ts*u_gyro;
}
