#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<errno.h>
#include <iostream>
#include "modbus.h"
#include "config.h"


#define LOOP 1 //ѭ������
#define SERVER_ID	1	//�Ӷ��豸��ַ
#define ADDRESS_START 0  //���ԼĴ�����ʼ��ַ
#define ADDRESS_END		99//���ԼĴ���������ַ



using namespace std;

int main(void)
{
    cout<<"start"<<endl;
	modbus_t *ctx;
    int rc;
    int addr; //��ַ
    int nb;// ����

    uint16_t poll_heartbeat[2];
    uint16_t slave_heartbear;



    /*TCP*/
    ctx = modbus_new_tcp("127.0.0.1",502);	//����TCP���͵�����
    modbus_set_debug(ctx, FALSE);				//����debugģʽ

    rc = modbus_set_slave(ctx, SERVER_ID);
    if (rc == -1) {
        fprintf(stderr, "Invalid slave ID\n");
        modbus_free(ctx);
        return -1;
    }

    if (modbus_connect(ctx) == -1)
    {
        cout<<"����ʧ��"<<endl;
        //fprintf(stderr, "Connection failed:%d\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }


    // ������������
    addr = 0;
    nb   = 1;
    rc   = modbus_read_registers(ctx, 0, 1, poll_heartbeat);
    if (rc == -1)
    {
        cout<<"������ʧ��"<<endl;
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }

    cout<<"����������Ϊ��"<<poll_heartbeat[0]<<endl;

    // д�ͻ�������
    addr = 1;
    nb   = 1;
    slave_heartbear = 923;
    rc   =  modbus_write_register(ctx, addr, slave_heartbear);
    cout<<"�ͻ�������д��ɹ�!"<<endl;

    float poll_data;
    float salve_data = 22.22;
    uint16_t cache[2];

    // ����������������
    addr = 4;
    nb   = 2;
    rc   =  modbus_read_registers(ctx, addr, nb, cache);
    poll_data = modbus_get_float_badc(cache);
    cout <<"������������Ϊ��"<< poll_data<<endl;

    // д�ͻ��˸�������
    addr = 6;
    nb   = 2;
    modbus_set_float_badc(salve_data,cache);
    rc   =  modbus_write_registers(ctx,addr,nb,cache);
    cout<<"�ͻ��˸�������д��ɹ�!"<<endl;


    modbus_close(ctx);
    modbus_free(ctx);

	return 0;
}
