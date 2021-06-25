#ifndef __BMP_H__
#define __BMP_H__

//#include"lpc_types.h"
#include "stdint.h"
typedef struct __BMP
{
	/*BMP�ļ�ͷ��14�ֽڣ�*/
	uint8_t  pic_head[2];						//λͼ�ļ������ͣ�����ΪBM(1-2�ֽڣ�
	uint16_t pic_size_l;			    	//λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ��3-6�ֽڣ���λ��ǰ��
	uint16_t pic_size_h;			    	//
	uint16_t pic_nc1;				    		//λͼ�ļ������֣�����Ϊ0(7-8�ֽڣ�
	uint16_t pic_nc2;				    		//λͼ�ļ������֣�����Ϊ0(9-10�ֽڣ�
	uint16_t pic_data_address_l;	  //λͼ���ݵ���ʼλ�ã��������λͼ��11-14�ֽڣ���λ��ǰ���ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ
	uint16_t pic_data_address_h;		//	
	/*λͼ��Ϣͷ��40�ֽڣ�*/
	uint16_t pic_message_head_len_l;//���ṹ��ռ���ֽ�����15-18�ֽڣ�
	uint16_t pic_message_head_len_h;//
	uint16_t pic_w_l;								//λͼ��ȣ�������Ϊ��λ��19-22�ֽڣ�
	uint16_t pic_w_h;				    		//
	uint16_t pic_h_l;				    		//λͼ�߶ȣ�������Ϊ��λ��23-26�ֽڣ�
	uint16_t pic_h_h;				    		//
	uint16_t pic_bit;				    		//Ŀ���豸�ļ��𣬱���Ϊ1(27-28�ֽڣ�
	uint16_t pic_dip;				    		//ÿ�����������λ����������1��˫ɫ������29-30�ֽڣ�4(16ɫ����8(256ɫ��16(�߲�ɫ)��24�����ɫ��֮һ 
	uint16_t pic_zip_l;			    		//λͼѹ�����ͣ�������0����ѹ��������31-34�ֽڣ�1(BI_RLE8ѹ�����ͣ���2(BI_RLE4ѹ�����ͣ�֮һ
	uint16_t pic_zip_h;			    		//
	uint16_t pic_data_size_l;		    //λͼ�Ĵ�С(���а�����Ϊ�˲���������4�ı�������ӵĿ��ֽ�)�����ֽ�Ϊ��λ��35-38�ֽڣ�
	uint16_t pic_data_size_h;		    //
	uint16_t pic_dipx_l;			    	//λͼˮƽ�ֱ��ʣ�ÿ����������39-42�ֽڣ�
	uint16_t pic_dipx_h;			    	//	
	uint16_t pic_dipy_l;			    	//λͼ��ֱ�ֱ��ʣ�ÿ����������43-46�ֽ�)
	uint16_t pic_dipy_h;			    	//
	uint16_t pic_color_index_l;	    //λͼʵ��ʹ�õ���ɫ���е���ɫ����47-50�ֽڣ�
	uint16_t pic_color_index_h;	    //
	uint16_t pic_other_l;			    	//λͼ��ʾ��������Ҫ����ɫ����51-54�ֽڣ�
	uint16_t pic_other_h;			    	//
	/*************��ɫ��*****************/
	//uint16_t pic_color_p01;		    	//28
	//uint16_t pic_color_p02;		    	//29
	//uint16_t pic_color_p03;		    	//30
	//uint16_t pic_color_p04;		    	//31
	//uint16_t pic_color_p05;		    	//32
	//uint16_t pic_color_p06;		    	//33
	//uint16_t pic_color_p07;		    	//34
	//uint16_t pic_color_p08;					//35			
}BMP_HEAD;

typedef struct BMP_point
{
	uint16_t x;
	uint16_t y;
	uint8_t  r;
	uint8_t  g;
	uint8_t  b;		
}BMP_POINT;


uint8_t is_bmp_file(uint8_t * filename);
uint32_t get_bmp_num(uint8_t *path);
void convert_bmp(uint8_t * dst, uint8_t * src,uint32_t width,uint32_t height);
void bmp_3to2(uint8_t *dst, uint8_t *src);


#endif
