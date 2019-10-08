//
// Created by along on 17-10-12.
// 获取计算机系统的基本信息
// 大小端判断   		bool isLittleEndian()
// CPU的位数/字长		int cpuWordSize()
// CPUID（略）       调用cpuid机器指令
// MAC（略）         调用驱动程序函数
//

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

/**
 * 定义宏：调用汇编语言中的cpuid以获取相关信息
 */
#define cpuid(in, a, b, c, d)  asm("cpuid": "=a" (a),\
 "=b" (b), "=c" (c), "=d" (d) : "a" (in))

/**
 * 判断是否为小端序
 * @return
 */
bool isLittleEndian(void) {
    int var = 1;
    char *p = (char *) &var;
    return *p == 1;
}

/**
 * 判断CPU的位长
 * 容易想到指针的字节数与CPU寄存器长度的关系
 * @return CPU寄存器位长
 */
int cpuWordSize(void) {
    return sizeof(void *);
}


void getMac(char ethname[], char mac_addr[]) {
    struct ifreq ifr_mac;

    //internet协议族的数据报类型套接口
    int sock_mac = socket(AF_INET, SOCK_STREAM, 0);

    memset(&ifr_mac, 0, sizeof(ifr_mac));

    //将设备名作为输入参数传入
    strcpy(ifr_mac.ifr_name, ethname);

    //执行取MAC地址操作
    if ((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0) {
        printf("mac ioctl error\n");
        return;
    }

    //取输出的MAC地址
    sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x",
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[0],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[1],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[2],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[3],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[5]);

    close(sock_mac);
}

/**
 * 获取制造商信息
 * ASCI码按低位到高位放在ebx,edx,ecx中
 * @param vendorID
 */
void getVenderID(char vendorID[], size_t size) {
    if (size < 13)
        return;
    unsigned cpuinfo[4];
    cpuid(0, cpuinfo[0], cpuinfo[1], cpuinfo[2], cpuinfo[3]);
    memset(vendorID, 0, size);
    memcpy(vendorID, &cpuinfo[1], 4);
    memcpy(vendorID + 4, &cpuinfo[3], 4);
    memcpy(vendorID + 8, &cpuinfo[2], 4);
}

/**
 * 返回处理器的相关信息
 * @return ProcessorInfo
 */
unsigned getProcessorInfo() {
    unsigned processorInfo;
    asm("cpuid": "=a" (processorInfo): "a" (1));
    return processorInfo;
}

int main() {
    printf("本机CPU为\"%s\"。\n",
           isLittleEndian() ? "小端序" : "大端序");
    printf("本机器是%d位机\n", 8 * cpuWordSize());

    char vendorID[20];
    getVenderID(vendorID, sizeof(vendorID));
    printf("处理器制造商信息:%s\n", vendorID);
    printf("处理器信息:%x", getProcessorInfo());

    char mac[30];
    getMac("enp6s0", mac);
    printf("MAC:%s", mac);

    return 0;
}