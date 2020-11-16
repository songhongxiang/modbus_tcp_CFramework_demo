#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include "modbus.h"
#include "config.h"
#define SERVER_ID	1	//�Ӷ��豸��ַ

using namespace std;

int main(void)
{
	modbus_t *ctx;
    int rc;
    int addr; //��ַ
    int nb;// ����

    uint16_t poll_heartbeat[1];
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


    // ��һ�ζ�����������,�ж�ͨ���Ƿ�����
    addr = 0;
    nb   = 1;
    poll_heartbeat[0] = 0;
    while(poll_heartbeat[0]!=1) //���������1,��ѭ���ȴ�
    {
        rc   = modbus_read_registers(ctx, 0, 1, poll_heartbeat);
        if (rc == -1)
        {
            cout<<"������ʧ��"<<endl;
            return -1;
        }
    }


    cout<<"����������Ϊ��"<<poll_heartbeat[0]<<endl;
    slave_heartbear = 1;
    while(1){

        // д�ͻ�������
        addr = 1;
        nb   = 1;
        uint16_t temp = slave_heartbear;
        rc   =  modbus_write_register(ctx, addr, temp);
        if (rc == -1)
        {
            cout<<"д����ʧ��"<<endl;
            return -1;
        }
        slave_heartbear++;
        if(slave_heartbear > 10)
            slave_heartbear = 1;

        while(poll_heartbeat[0]!=slave_heartbear) //���������slave_heartbear,��ѭ���ȴ�
        {
            rc   = modbus_read_registers(ctx, 0, 1, poll_heartbeat);
            if (rc == -1)
            {
                cout<<"������ʧ��"<<endl;
                return -1;
            }
            cout<<"����������Ϊ��"<<poll_heartbeat[0]<<endl;
        }
    }




    modbus_close(ctx);
    modbus_free(ctx);

	return 0;
}
