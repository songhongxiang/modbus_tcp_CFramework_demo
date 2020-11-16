#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include "modbus.h"
#include "config.h"
#include <windows.h>
#define SERVER_ID	1	//�Ӷ��豸��ַ

using namespace std;

int main(void)
{
    int server_socket = -1;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    ctx = modbus_new_tcp("127.0.0.1",502);
    modbus_set_debug(ctx,FALSE);

    modbus_set_slave(ctx,SERVER_ID);

    //�����ڴ������ڴ�żĴ�������
    mb_mapping = modbus_mapping_new(500,500,500,500);
    if(mb_mapping == NULL)
    {
        fprintf(stderr,"Failed mapping:%s\n",modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    //��ʼ�����˿�
    server_socket = modbus_tcp_listen(ctx,1);
    if(server_socket == -1)
    {
        fprintf(stderr,"Unable to listen TCP.\n");
        modbus_free(ctx);
        return -1;
    }


    // ��ʼ��������
    modbus_tcp_accept(ctx,&server_socket);
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc;


    /*
     *  ���̣�
     * ��1��������ȷ���1��������ͳɹ����ͻ��˻ᱻ�������1��������
     * ��2���ж�1�ǲ����յ��ˣ�������ǣ���ѭ���ȴ�
     * ��3������յ��ˣ������+1��������
     *  >>>>>> ע�⣺���еĲ��ɹ���Ҫ������
    */

    uint16_t poll_heartbeat = 1;
    mb_mapping->tab_registers[1] = poll_heartbeat;
    modbus_reply(ctx,query,rc,mb_mapping); // �ظ���Ӧ����
    while(1)
    {
        rc = modbus_receive(ctx,query); //��ȡ��ѯ����
        if(rc >= 0)
        {
            if(mb_mapping->tab_registers[0]==poll_heartbeat) //����ͻ������������������ͬ����ѭ���ȴ�
            {
                poll_heartbeat ++;
                if(poll_heartbeat > 10)
                    poll_heartbeat = 1;
                mb_mapping->tab_registers[1] = poll_heartbeat;
                cout<<"�ͻ��ˣ�"<<mb_mapping->tab_registers[0]<<endl; // ��ӡ��һ�����ּĴ���
                cout<<"����ˣ�"<<mb_mapping->tab_registers[1]<<endl;
                cout<<"------------------"<<endl;
            }
            modbus_reply(ctx,query,rc,mb_mapping); // �ظ���Ӧ���ģ���Ҫһֱ�ظ�������ͻ��˽���Ϊ��ʱ
        }
        else
        {
            // Connection closed by the client or error
            printf("Connection Closed.\n");

            //Close ctx
            modbus_close(ctx);

            //�ȴ���һ���ͻ��˱���
            modbus_tcp_accept(ctx,&server_socket);
        }

    }

    printf("Quit the loop:%s\n",modbus_strerror(errno));

    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

	return 0;
}
