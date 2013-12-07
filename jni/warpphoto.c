#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <math.h>
#include <malloc.h>
#include <android/log.h>
#include <asm/io.h>
#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int *colorRed;
int *colorGreen;
int *colorBlue;
int *colorsP;//����ֵ

int *colorAlpha;
int _width,_height;
jintArray imagecolors;

//���ұ�
static long SinXDivX_Table_8[(2<<8)+1];
double MySin(double x){
	 double absx;
	 absx = x;
	 if(x<0) absx = -x;

     double x2 = absx*absx;
     double x3 = x2*absx;
	 if(absx<1)    return (1-2*x2+x3);
	 else if(absx<2) return (4-8*absx+5*x2-x3);
	 else return 0;
}
int getRedColor(int i,int j){

	//return *(colorRed+j*_width+i);
	return (*(colorsP+j*_width+i) >> 16)&0xFF;
}

int getGreenColor(int i,int j){
	//return *(colorGreen+j*_width+i);
	return (*(colorsP+j*_width+i) >> 8)&0xFF;
}

int getBlueColor(int i,int j){
	//return *(colorBlue+j*_width+i);
	return (*(colorsP+j*_width+i))&0xFF;
}

	/**
     * �õ�x*x+y*y��ֵ
     * @param x
     * @param y
     * @return
     */
 double hypotsq(double x,double y){
    	return x*x+y*y;
    }
 //����ӳ��
 //Ϊ��Ч��û�аѸú����ڵĴ����װ�ɺ��������Դ��뿴�����Ƚ϶�Ƚ���
void mapping(JNIEnv *env,jintArray image,int x,int y,double max_dist,
    		           double orig_x,double orig_y,double cur_x,double cur_y){
	      // if(mode == 120){
	        float u = 0,v = 0;
	    	float fu = x,fv = y;
	    	double dx = x - orig_x;
	    	double dy = y - orig_y;
	    	long i,j;
	    	int i1,j1,_i,_j,i2,j2;
	    	float du;
	   	    float dv;
	   	    int colorpix = 0;
	   	    int red,green,blue,alpha;
	   	    double max_dist_sq = max_dist*max_dist;
	   	    double mou_dx = cur_x - orig_x;
	   	    //����-------^_^
	   	    mou_dx = mou_dx/4;
	   	    //--------------
	   	    double mou_dy = cur_y - orig_y;
	   	    //����-------^_^
	   	    mou_dy = mou_dy/4;
	   	    //-----------
	    	if(dx > -max_dist&&dx<max_dist&&dy>-max_dist&&dy<max_dist){
	    		double rsq = hypotsq(dx, dy);
	    		if(rsq < max_dist_sq){
						//λ��
						double msq = hypotsq(dx - mou_dx,dy - mou_dy);
						double edge_dist = max_dist_sq - rsq;
						double a = edge_dist/(edge_dist + msq);
						a *=a;
						fu -= a*mou_dx; //ӳ���ĸ�������
						fv -= a*mou_dy; //ӳ���ĸ�������

					u = fu;
					v = fv;
					{
					u = u<(_width-1)?u:(_width-1);
					v = v<(_height-1)?v:(_height-1);

					u = u>0?u:0;
					v = v>0?v:0;
					}
	//		    	i = (int)u; //��ӦԴͼ��X������������
	//		    	j = (int)v; //��ӦԴͼ��Y������������
	//		    	du = u-i;//С���㲿��
	//		    	dv = v-j;//С���㲿��
					long intu = (long)(u*(1 << 16)); //�Ŵ�
					long intv = (long)(v*(1 << 16)); //�Ŵ�

					i = intu >> 16; //Ȼ������С�õ���������
					j = intv >> 16; //Ȼ������С�õ���������

					long idu = (intu & 0xFFFF)>> 8;  //�õ�С������
					long idv = (intv & 0xFFFF)>> 8;  //�õ�С������

					long _idu = (256-idu);     //�൱��û�Ŵ�ǰ��(1-du)֮���
					long _idv = (256-idv);

					i1 = (i+1)<(_width-1)?(i+1):(_width-1);//Math.min(i+1, width-1);
					j1 = (j+1)<(_height-1)?(j+1):(_height-1);//Math.min(j+1, height-1);
					//*******************************************˫���Բ�ֵ�㷨***************************************************//
					red = (_idu*_idv*getRedColor(i, j)+_idu*idv*getRedColor(i, j1)+
							idu*_idv*getRedColor(i1, j)+idu*idv*getRedColor(i1,j1))>>16;

					green = (_idu*_idv*getGreenColor(i, j)+_idu*idv*getGreenColor(i, j1)+
							idu*_idv*getGreenColor(i1, j)+idu*idv*getGreenColor(i1,j1))>>16;

					blue = (_idu*_idv*getBlueColor(i, j)+_idu*idv*getBlueColor(i, j1)+
							idu*_idv*getBlueColor(i1, j)+idu*idv*getBlueColor(i1,j1))>>16;

					//*******************************************���ξ����ֵ�㷨****************************************************//
					int red3,green3,blue3;
					i2 = (i+2)<(_width-1)?(i+2):(_width-1);
					j2 = (j+2)<(_height-1)?(j+2):(_height-1);
					_i = (i-1)>0?(i-1):0;
					_j = (j-1)>0?(j-1):0;
					long A[4] = {SinXDivX_Table_8[(1<<8)+idu],SinXDivX_Table_8[idu+0],SinXDivX_Table_8[(1<<8)-idu],SinXDivX_Table_8[(2<<8)-idu]};
					long C[4] = {SinXDivX_Table_8[(1<<8)+idv],
								   SinXDivX_Table_8[idv+0],
								   SinXDivX_Table_8[(1<<8)-idv],
								   SinXDivX_Table_8[(2<<8)-idv]};

					//��ɫ
					int redB[16] = {getRedColor(_i, _j),  getRedColor(_i,j), getRedColor(_i,j1),  getRedColor(_i,j2),
									   getRedColor(i,_j),    getRedColor(i,j),  getRedColor(i,j1),   getRedColor(i,j2),
									   getRedColor(i1,_j),   getRedColor(i1,j), getRedColor(i1,j1),  getRedColor(i1,j2),
									   getRedColor(i2,_j),   getRedColor(i2,j), getRedColor(i2,j1),  getRedColor(i2,j2)};

					double A_B[4] = {(A[0]*redB[0]+A[1]*redB[4]+A[2]*redB[8]+A[3]*redB[12]),
									  (A[0]*redB[1]+A[1]*redB[5]+A[2]*redB[9]+A[3]*redB[13]),
									  (A[0]*redB[2]+A[1]*redB[6]+A[2]*redB[10]+A[3]*redB[14]),
									  (A[0]*redB[3]+A[1]*redB[7]+A[2]*redB[11]+A[3]*redB[15])};
					red3 = (int)(A_B[0]*C[0]+A_B[1]*C[1]+A_B[2]*C[2]+A_B[3]*C[3])>>16;
					//��ɫ
					int greenB[16] = {getGreenColor(_i, _j),  getGreenColor(_i,j), getGreenColor(_i,j1),  getGreenColor(_i,j2),
										 getGreenColor(i,_j),    getGreenColor(i,j),  getGreenColor(i,j1),   getGreenColor(i,j2),
										 getGreenColor(i1,_j),   getGreenColor(i1,j), getGreenColor(i1,j1),  getGreenColor(i1,j2),
										 getGreenColor(i2,_j),   getGreenColor(i2,j), getGreenColor(i2,j1),  getGreenColor(i2,j2)};
					double gA_B[4] = {(A[0]*greenB[0]+A[1]*greenB[4]+A[2]*greenB[8]+A[3]*greenB[12]),
									  (A[0]*greenB[1]+A[1]*greenB[5]+A[2]*greenB[9]+A[3]*greenB[13]),
									  (A[0]*greenB[2]+A[1]*greenB[6]+A[2]*greenB[10]+A[3]*greenB[14]),
									  (A[0]*greenB[3]+A[1]*greenB[7]+A[2]*greenB[11]+A[3]*greenB[15])};
					green3 = (int)(gA_B[0]*C[0]+gA_B[1]*C[1]+gA_B[2]*C[2]+gA_B[3]*C[3])>>16;
					//��ɫ
					int blueB[16] = {getBlueColor(_i, _j),  getBlueColor(_i,j), getBlueColor(_i,j1),  getBlueColor(_i,j2),
										getBlueColor(i,_j),    getBlueColor(i,j),  getBlueColor(i,j1),   getBlueColor(i,j2),
										getBlueColor(i1,_j),   getBlueColor(i1,j), getBlueColor(i1,j1),  getBlueColor(i1,j2),
										getBlueColor(i2,_j),   getBlueColor(i2,j), getBlueColor(i2,j1),  getBlueColor(i2,j2)};
					double bA_B[4] = {(A[0]*blueB[0]+A[1]*blueB[4]+A[2]*blueB[8]+A[3]*blueB[12]),
									  (A[0]*blueB[1]+A[1]*blueB[5]+A[2]*blueB[9]+A[3]*blueB[13]),
									  (A[0]*blueB[2]+A[1]*blueB[6]+A[2]*blueB[10]+A[3]*blueB[14]),
									  (A[0]*blueB[3]+A[1]*blueB[7]+A[2]*blueB[11]+A[3]*blueB[15])};
					blue3 = (int)(bA_B[0]*C[0]+bA_B[1]*C[1]+bA_B[2]*C[2]+bA_B[3]*C[3])>>16;

					//**************************************************************************************************************//
					//**************************************************************************************************************//
					//Ϊ�˵õ������Ⱥ���������������������һ���жϣ�������ڻ�С��ĳ��ֵ����˫�����㷨������ֵ�����������ξ���㷨�õ���ֵ
					if(red3<0||green3<0||blue3<0||red3>255||green3>255||blue3>255){
						colorpix = (255 << 24)|(red << 16)|(green << 8)|blue;
					}else{
						colorpix = (255 << 24)|(red3 << 16)|(green3 << 8)|blue3;
					}
		    	//LOGI("c---> ca = %d",);
		    	(*env)->SetIntArrayRegion(env,image,(int) (y*_width + x),1,&colorpix);
	    		}
	    	}

}

JNIEXPORT jintArray JNICALL Java_com_example_warpphoto_WarpView_warpPhotoFromC
  (JNIEnv * env, jobject obj, jintArray imagearr, jint height, jint width, jdouble r,
		                jdouble orig_x, jdouble orig_y, jdouble cur_x, jdouble cur_y){
//��ʼ���ɫ����

	int len = (*env)->GetArrayLength(env,imagearr);

    int* color = (*env)->GetIntArrayElements(env,imagearr,0);

    int colors[len];

    int colorsOr[len];

    _width = width;
    _height = height;

    int i = 0;
	for(;i<len;i++){
		int colorpix = *(color+i);//��ǰ����ֵ
		colors[i] = colorpix;
	}
	colorsP = &colors[0];
	//Ť��ͼƬ��ӳ��

	int or_x = (orig_x-r)>0?(orig_x-r):0;//(int) Math.max((orig_x-r), 0);
	int or_y = (orig_y-r)>0?(orig_y-r):0;//(int) Math.max((orig_y-r), 0);

	int max_x = (orig_x+r)<width?(orig_x+r):width;//(int) Math.min((orig_x+r), width);
	int max_y = (orig_y+r)<height?(orig_y+r):height;//(int) Math.min((orig_y+r), height);
    int m = or_y;
	for(;m<max_y;m++){
		int n = or_x;
	        	for(;n<max_x;n++){
	        		mapping(env,imagearr,n, m,r,orig_x,orig_y,cur_x,cur_y);
	        }
     }
    return imagecolors;
}
//��ʼ�����ұ�
JNIEXPORT jint JNICALL Java_com_example_warpphoto_MainActivity_initArray
  (JNIEnv * env, jobject obj){
	    long k;
		for (k = 0;k<=(2<<8);++k){
		    SinXDivX_Table_8[k]=(long)(0.5+256*MySin(k*(1.0/(256))))*1;
		}
		return 0;
}
