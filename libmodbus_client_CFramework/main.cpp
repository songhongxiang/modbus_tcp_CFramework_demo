#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include "modbus.h"
#include "config.h"
#define SERVER_ID	1	//从端设备地址

using namespace std;

int main(void)
{
	modbus_t *ctx;
    int rc;
    int addr; //地址
    int nb;// 长度

    uint16_t poll_heartbeat[1];
    uint16_t slave_heartbear;

    /*TCP*/
    ctx = modbus_new_tcp("127.0.0.1",502);	//创建TCP类型的容器
    modbus_set_debug(ctx, FALSE);				//设置debug模式

    rc = modbus_set_slave(ctx, SERVER_ID);
    if (rc == -1) {
        fprintf(stderr, "Invalid slave ID\n");
        modbus_free(ctx);
        return -1;
    }

    if (modbus_connect(ctx) == -1)
    {
        cout<<"连接失败"<<endl;
        //fprintf(stderr, "Connection failed:%d\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }


    // 第一次读服务器心跳,判断通信是否正常
    addr = 0;
    nb   = 1;
    poll_heartbeat[0] = 0;
    while(poll_heartbeat[0]!=1) //如果不等于1,则循环等待
    {
        rc   = modbus_read_registers(ctx, 0, 1, poll_heartbeat);
        if (rc == -1)
        {
            cout<<"读心跳失败"<<endl;
            return -1;
        }
    }


    cout<<"服务器心跳为："<<poll_heartbeat[0]<<endl;
    slave_heartbear = 1;
    while(1){

        // 写客户端心跳
        addr = 1;
        nb   = 1;
        uint16_t temp = slave_heartbear;
        rc   =  modbus_write_register(ctx, addr, temp);
        if (rc == -1)
        {
            cout<<"写心跳失败"<<endl;
            return -1;
        }
        slave_heartbear++;
        if(slave_heartbear > 10)
            slave_heartbear = 1;

        while(poll_heartbeat[0]!=slave_heartbear) //如果不等于slave_heartbear,则循环等待
        {
            rc   = modbus_read_registers(ctx, 0, 1, poll_heartbeat);
            if (rc == -1)
            {
                cout<<"读心跳失败"<<endl;
                return -1;
            }
            cout<<"服务器心跳为："<<poll_heartbeat[0]<<endl;
        }
    }




    modbus_close(ctx);
    modbus_free(ctx);

	return 0;
}
