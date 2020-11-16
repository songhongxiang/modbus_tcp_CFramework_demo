#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include "modbus.h"
#include "config.h"
#include <windows.h>
#define SERVER_ID	1	//从端设备地址

using namespace std;

int main(void)
{
    int server_socket = -1;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    ctx = modbus_new_tcp("127.0.0.1",502);
    modbus_set_debug(ctx,FALSE);

    modbus_set_slave(ctx,SERVER_ID);

    //申请内存区用于存放寄存器数据
    mb_mapping = modbus_mapping_new(500,500,500,500);
    if(mb_mapping == NULL)
    {
        fprintf(stderr,"Failed mapping:%s\n",modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    //开始监听端口
    server_socket = modbus_tcp_listen(ctx,1);
    if(server_socket == -1)
    {
        fprintf(stderr,"Unable to listen TCP.\n");
        modbus_free(ctx);
        return -1;
    }


    // 开始接收数据
    modbus_tcp_accept(ctx,&server_socket);
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc;


    /*
     *  过程：
     * （1）服务端先发送1，如果发送成功，客户端会被激活，并将1反馈过来
     * （2）判断1是不是收到了，如果不是，则循环等待
     * （3）如果收到了，服务端+1，并发送
     *  >>>>>> 注意：所有的不成功都要做处理
    */

    uint16_t poll_heartbeat = 1;
    mb_mapping->tab_registers[1] = poll_heartbeat;
    modbus_reply(ctx,query,rc,mb_mapping); // 回复响应报文
    while(1)
    {
        rc = modbus_receive(ctx,query); //获取查询报文
        if(rc >= 0)
        {
            if(mb_mapping->tab_registers[0]==poll_heartbeat) //如果客户端心跳数与服务器不同，则循环等待
            {
                poll_heartbeat ++;
                if(poll_heartbeat > 10)
                    poll_heartbeat = 1;
                mb_mapping->tab_registers[1] = poll_heartbeat;
                cout<<"客户端："<<mb_mapping->tab_registers[0]<<endl; // 打印第一个保持寄存器
                cout<<"服务端："<<mb_mapping->tab_registers[1]<<endl;
                cout<<"------------------"<<endl;
            }
            modbus_reply(ctx,query,rc,mb_mapping); // 回复响应报文，需要一直回复，否则客户端将认为超时
        }
        else
        {
            // Connection closed by the client or error
            printf("Connection Closed.\n");

            //Close ctx
            modbus_close(ctx);

            //等待下一个客户端报文
            modbus_tcp_accept(ctx,&server_socket);
        }

    }

    printf("Quit the loop:%s\n",modbus_strerror(errno));

    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

	return 0;
}
