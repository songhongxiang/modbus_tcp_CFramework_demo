#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<errno.h>
#include <iostream>
#include "modbus.h"
#include "config.h"


#define LOOP 1 //循环次数
#define SERVER_ID	1	//从端设备地址
#define ADDRESS_START 0  //测试寄存器起始地址
#define ADDRESS_END		99//测试寄存器结束地址



using namespace std;

int main(void)
{
    cout<<"start"<<endl;
	modbus_t *ctx;
    int rc;
    int addr; //地址
    int nb;// 长度

    uint16_t poll_heartbeat[2];
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


    // 读服务器心跳
    addr = 0;
    nb   = 1;
    rc   = modbus_read_registers(ctx, 0, 1, poll_heartbeat);
    if (rc == -1)
    {
        cout<<"读心跳失败"<<endl;
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }

    cout<<"服务器心跳为："<<poll_heartbeat[0]<<endl;

    // 写客户端心跳
    addr = 1;
    nb   = 1;
    slave_heartbear = 923;
    rc   =  modbus_write_register(ctx, addr, slave_heartbear);
    cout<<"客户端心跳写入成功!"<<endl;

    float poll_data;
    float salve_data = 22.22;
    uint16_t cache[2];

    // 读服务器浮点数据
    addr = 4;
    nb   = 2;
    rc   =  modbus_read_registers(ctx, addr, nb, cache);
    poll_data = modbus_get_float_badc(cache);
    cout <<"服务器浮点数为："<< poll_data<<endl;

    // 写客户端浮点数据
    addr = 6;
    nb   = 2;
    modbus_set_float_badc(salve_data,cache);
    rc   =  modbus_write_registers(ctx,addr,nb,cache);
    cout<<"客户端浮点数据写入成功!"<<endl;


    modbus_close(ctx);
    modbus_free(ctx);

	return 0;
}
